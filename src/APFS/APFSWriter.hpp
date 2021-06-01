#pragma once
#include <ApfsLib/ApfsDir.h>
#include <ApfsLib/ApfsVolume.h>

class APFSWriter {
    uint64_t total_object_count = 0;
    uint64_t count = 0;
    ApfsDir* dir = nullptr;
    ApfsVolume* volume = nullptr;
    std::string output_prefix;

  public:
    APFSWriter(ApfsVolume* volume, std::string output_prefix, const apfs_superblock_t& superblock);
    ~APFSWriter();
    bool write_contents_of_tree(uint64_t inode);

  private:
    bool write_contents_of_tree_with_name(uint64_t inode, const std::string& name);
    bool handle_symlink(uint64_t inode, const std::string& name);
    bool handle_directory(uint64_t inode, const std::string& name);
    bool handle_regular_file(uint64_t inode, std::string name);
    bool handle_compressed_file(uint64_t inode, const std::string& name);
};
