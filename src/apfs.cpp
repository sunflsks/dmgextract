#include "apfs.hpp"
#include <ApfsLib/ApfsDir.h>
#include <ApfsLib/Decmpfs.h>
#include <cinttypes>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

#ifdef WIN32
#include <windows.h>
#define     S_IFLNK       0120000 /* Symbolic link.  */
#define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#endif

#define APFS_ROOT_INODE 2

static bool is_inode_compressed(ApfsDir::Inode inode) {
    return (inode.bsd_flags & APFS_UF_COMPRESSED) != 0;
}

APFSWriter::APFSWriter(ApfsVolume* volume,
                       std::string output_prefix,
                       const apfs_superblock_t& superblock) {
    this->volume = volume;
    this->output_prefix = output_prefix + "/";
    total_object_count =
      superblock.apfs_num_files + superblock.apfs_num_directories + superblock.apfs_num_symlinks;
    dir = new ApfsDir(*volume);
}

APFSWriter::~APFSWriter() {
    delete dir;
    printf("Wrote %" PRIu64 "/%" PRIu64 " objects\n", count, total_object_count);
}

bool APFSWriter::write_contents_of_tree(uint64_t inode) {
    return write_contents_of_tree_with_name(inode, output_prefix);
}

bool APFSWriter::write_contents_of_tree_with_name(uint64_t inode, const std::string& out) {
    std::vector<ApfsDir::DirRec> dir_list;
    dir->ListDirectory(dir_list, inode);

    for (size_t i = 0; i < dir_list.size(); i++) {
        // do not keep calling printf
        if (count % 50 == 0) {
            printf("Wrote %" PRIu64 "/%" PRIu64 " objects\r", count, total_object_count);
            fflush(stdout);
        }
        count++;
        mode_t mode = (dir_list[i].flags & DREC_TYPE_MASK) << 12;
        if (S_ISDIR(mode)) {
            handle_directory(dir_list[i].file_id, out + "/" + dir_list[i].name);
        } else if (S_ISREG(mode)) {
            handle_regular_file(dir_list[i].file_id, out + "/" + dir_list[i].name);
        } else if (S_ISLNK(mode)) {
            handle_symlink(dir_list[i].file_id, out + "/" + dir_list[i].name);
        }
    }

    // reset dir for the next volume
    return 0;
}

bool APFSWriter::handle_regular_file(uint64_t inode, const std::string& name) {
    ApfsDir::Inode inodeobj;
    dir->GetInode(inodeobj, inode);

    mode_t mode = inodeobj.mode;
    std::vector<uint8_t> file_contents(4096);

    if (inodeobj.bsd_flags & APFS_UF_COMPRESSED) {
        std::vector<uint8_t> compressed(4096);
        bool rc = dir->GetAttribute(compressed, inode, "com.apple.decmpfs");
        if (!rc) {
            fprintf(stderr,
                    "File %s seems to be compressed, but has no com.apple.decmpfs attribute. "
                    "Weird! Not writing this file out.\n");
            return false;
        }
        DecompressFile(*dir, inode, file_contents, compressed);
    }

    else {
        uint64_t size = inodeobj.ds_size;
        file_contents.resize(size);
        dir->ReadFile(file_contents.data(), inodeobj.private_id, 0, size);
    }

    std::ofstream output(name, std::ios::binary);
    output.write((char*)file_contents.data(), file_contents.size());
    output.close();
    chmod(name.c_str(), mode & 07777);
    return true;
}

bool APFSWriter::handle_directory(uint64_t inode, const std::string& name) {
    std::filesystem::create_directories(name);
    write_contents_of_tree_with_name(inode, name);
    return 0;
}

bool APFSWriter::handle_symlink(uint64_t inode, const std::string& name) {
    std::vector<uint8_t> buffer;
    bool rc = dir->GetAttribute(buffer, inode, "com.apple.fs.symlink");
    if (!rc) {
        fprintf(stderr, "Unable to find target for symlink %s\n", name.c_str());
        return false;
    }

#ifdef WIN32
    CreateSymbolicLinkA(name.c_str(), NULL, 0x02);
#else
    symlink(reinterpret_cast<const char*>(buffer.data()), name.c_str());
#endif
    return true;
}
