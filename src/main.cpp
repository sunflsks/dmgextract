#include "apfs.hpp"
#include <ApfsLib/ApfsContainer.h>
#include <ApfsLib/ApfsDir.h>
#include <ApfsLib/ApfsVolume.h>
#include <ApfsLib/GptPartitionMap.h>
#include <cassert>
#include <cinttypes>
#include <filesystem>
#include <iostream>
#include <memory>

// The inode for '/' on all APFS filesystems.
#define APFS_ROOT_INODE 2

void usage(const char* name) {
    fprintf(stderr, "Usage: %s apfs_filesystem[.dmg] extractdir\n", name);
}

int main(int argc, char** argv) {
    uint64_t offset = 0;
    uint64_t size = 0;
    int volcnt;
    apfs_superblock_t superblock;

    if (argc < 3) {
        usage(argv[0]);
        return 1;
    }

    if (std::filesystem::exists(argv[2])) {
        fprintf(
          stderr,
          "Will not write to an existing directory/file. Please delete/move it and try again.\n");
        return 2;
    }

    std::unique_ptr<Device> device(Device::OpenDevice(argv[1]));

    if (!device) {
        std::cerr << "Unable to open device " + std::string(argv[1]) << std::endl;
        return ENOENT;
    }

    size = device->GetSize();
    printf("Found device %" PRIu64 " MB large\n", size / (1024 * 1024));
    if (size == 0) {
        fprintf(stderr, "Device should not have a size of 0. It's probably invalid.\n");
        return EBADMSG;
    }

    // If a disk image, open it up and find the offset.
    GptPartitionMap gpt;
    bool rc = gpt.LoadAndVerify(*device);
    if (rc) {
        printf("Found GPT table. Looking for APFS partition\n");
        int partid = gpt.FindFirstAPFSPartition();
        if (partid != -1) {
            gpt.GetPartitionOffsetAndSize(partid, offset, size);
        } else {
            fprintf(stderr, "Could not find an APFS partion\n");
            return ENOENT;
        }
    }

    auto container = std::make_unique<ApfsContainer>(device.get(), offset, size);

    if (!container->Init()) {
        fprintf(stderr, "Could not initialize the APFS container.\n");
        return EBADMSG;
    }

    volcnt = container->GetVolumeCnt();
    printf("Found APFS filesystem! Volume count: %d\nExtracting\n", volcnt);

    // Iterate through the volumes, extracting each one to a specific dir.
    for (int i = 0; i < volcnt; i++) {
        container->GetVolumeInfo(i, superblock);
        if ((superblock.apfs_fs_flags & APFS_FS_CRYPTOFLAGS) == 0) {
            fprintf(stderr, "Filevault is not supported yet. Exiting.\n");
            return ENOTSUP;
        }

        auto volume = std::unique_ptr<ApfsVolume>(container->GetVolume(i));
        APFSWriter writer(
          volume.get(), std::string(argv[2]) + "/Volume " + std::to_string(i) + "/out", superblock);
        writer.write_contents_of_tree(APFS_ROOT_INODE);
    }
}
