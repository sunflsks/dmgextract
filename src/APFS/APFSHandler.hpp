#pragma once
#include <ApfsLib/ApfsContainer.h>
#include <ApfsLib/ApfsDir.h>
#include <ApfsLib/ApfsVolume.h>
#include <ApfsLib/GptPartitionMap.h>
#include <cinttypes>

class APFSHandler {
    uint64_t offset = 0;
    uint64_t size = 0;
    int volcnt = 0;
    std::string device_path;
    std::string output_directory;
    Device* device = nullptr;
    ApfsContainer* container = nullptr;
    apfs_superblock_t superblock;

  public:
    APFSHandler(const std::string& device_path, const std::string& output_directory);
    ~APFSHandler();
    bool init();
    bool write();
};
