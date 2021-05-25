#pragma once
#include <ApfsLib/ApfsDir.h>
#include <ApfsLib/ApfsVolume.h>

class APFSWriter {
    uint64_t total_inode_count = 0;
    uint64_t count = 0;
    ApfsDir* dir = nullptr;
    ApfsVolume* volume = nullptr;
    std::string output_prefix;
    bool init_inode_count = false;

  public:
    APFSWriter(ApfsVolume* volume, bool init_inode_count, std::string output_prefix);
    ~APFSWriter();
    bool write_contents_of_tree(uint64_t inode);

  private:
    bool write_contents_of_tree_with_name(uint64_t inode, const std::string& name);
    void initialize_number_of_inodes(uint64_t inode);
    bool handle_symlink(uint64_t inode, const std::string& name);
    bool handle_directory(uint64_t inode, const std::string& name);
    bool handle_regular_file(uint64_t inode, const std::string& name);
};
