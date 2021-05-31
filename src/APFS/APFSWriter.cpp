#include "APFSWriter.hpp"
#include "../utils.hpp"
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
#define S_IFLNK 0120000 /* Symbolic link.  */
#define S_ISLNK(m) (((m)&S_IFMT) == S_IFLNK)
#endif

#define BUFFER_SIZE 4096
#define APFS_ROOT_INODE 2

static bool is_inode_compressed(ApfsDir::Inode inode) {
    return (inode.bsd_flags & APFS_UF_COMPRESSED) != 0;
}

APFSWriter::APFSWriter(ApfsVolume* volume,
                       std::string output_prefix,
                       const apfs_superblock_t& superblock) {
    this->volume = volume;
    this->output_prefix = output_prefix + "/";
    total_object_count = superblock.apfs_num_files + superblock.apfs_num_directories +
                         superblock.apfs_num_symlinks + superblock.apfs_num_other_fsobjects;
    dir = new ApfsDir(*volume);
}

APFSWriter::~APFSWriter() {
    delete dir;
    Utilities::print_progress(count, total_object_count, true);
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
            Utilities::print_progress(count, total_object_count, false);
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
        } else {
            fprintf(stderr, "Unknown object type: mode is %d\n", mode);
        }
    }

    return true;
}

bool APFSWriter::handle_regular_file(uint64_t inode, const std::string& name) {
    ApfsDir::Inode inodeobj;
    dir->GetInode(inodeobj, inode);

    if (is_inode_compressed(inodeobj)) {
        return handle_compressed_file(inode, name);
    }

    mode_t mode = inodeobj.mode;
    std::vector<uint8_t> file_contents(4096);

    std::ofstream output(name, std::ios::binary);
    if (!output.good()) {
        std::error_code ec(errno, std::system_category());
        throw std::filesystem::filesystem_error("Unable to open output " + name, ec);
        return false;
    }

    uint64_t size = inodeobj.ds_size;
    uint64_t curpos = 0;
    file_contents.resize(size);

    for (curpos = 0; curpos < size / BUFFER_SIZE; curpos++) {
        dir->ReadFile(file_contents.data(), inodeobj.private_id, curpos * BUFFER_SIZE, BUFFER_SIZE);
        output.write((char*)file_contents.data(), BUFFER_SIZE);
        if (!output.good()) {
            std::error_code ec(errno, std::system_category());
            throw std::filesystem::filesystem_error("Unable to write to output " + name, ec);
            return false;
        }
    }

    if (size % BUFFER_SIZE) {
        dir->ReadFile(
          file_contents.data(), inodeobj.private_id, curpos * BUFFER_SIZE, size % BUFFER_SIZE);
        output.write((char*)file_contents.data(), size % BUFFER_SIZE);
        if (!output.good()) {
            std::error_code ec(errno, std::system_category());
            throw std::filesystem::filesystem_error("Unable to write to output " + name, ec);
            return false;
        }
    }

    output.close();

    chmod(name.c_str(), mode & 07777);
    return true;
}

bool APFSWriter::handle_compressed_file(uint64_t inode, const std::string& name) {
    std::vector<uint8_t> compressed(4096);
    std::vector<uint8_t> file_contents(4096);

    bool rc = dir->GetAttribute(compressed, inode, "com.apple.decmpfs");
    if (!rc) {
        fprintf(stderr,
                "File %s seems to be compressed, but has no com.apple.decmpfs attribute. "
                "Weird! Not writing this file out.\n",
                name.c_str());
        return false;
    }

    DecompressFile(*dir, inode, file_contents, compressed);

    std::ofstream output(name, std::ios::binary);
    if (!output.good()) {
        std::error_code ec(errno, std::system_category());
        throw std::filesystem::filesystem_error("Unable to open output " + name, ec);
        return false;
    }

    output.write((char*)file_contents.data(), file_contents.size());
    if (!output.good()) {
        std::error_code ec(errno, std::system_category());
        throw std::filesystem::filesystem_error("Unable to write to output " + name, ec);
        return false;
    }

    output.close();
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
