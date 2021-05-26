#include "APFSWriter.hpp"
#include "utils.hpp"
#include <ApfsLib/ApfsContainer.h>
#include <ApfsLib/ApfsDir.h>
#include <ApfsLib/ApfsVolume.h>
#include <ApfsLib/GptPartitionMap.h>
#include <cassert>
#include <cinttypes>
#include <filesystem>
#include <getopt.h>
#include <iostream>
#include <memory>

// The inode for '/' on all APFS filesystems.
#define APFS_ROOT_INODE 2

void usage(const char* name) {
    fprintf(stderr, "Usage: %s -i filesystem[.dmg] -o extractdir [-v]\n", name);
}

int main(int argc, char** argv) {
    uint64_t offset = 0;
    uint64_t size = 0;
    bool vflag = false;
    int volcnt;
    const char* device_name = nullptr;
    const char* output_dir = nullptr;
    char opt;

    while ((opt = getopt(argc, argv, "i:o:v:")) != -1) {
        switch (opt) {
            case 'i': {
                device_name = optarg;
                break;
            }

            case 'o': {
                output_dir = optarg;
                break;
            }

            case 'v': {
                vflag = true;
                break;
            }

            case '?': {
                usage(argv[0]);
                return 1;
            }

            default: {
                usage(argv[0]);
                return 1;
            }
        }
    }

    if (device_name == nullptr || output_dir == nullptr) {
        usage(argv[0]);
        return 1;
    }

    apfs_superblock_t superblock;

    if (std::filesystem::exists(output_dir)) {
        Utilities::print(
          Utilities::MSG_STATUS_ERROR,
          "Will not write to an existing directory/file. Please delete/move it and try again.\n");
        return 2;
    }

    std::unique_ptr<Device> device(Device::OpenDevice(device_name));

    if (!device) {
        Utilities::print(Utilities::MSG_STATUS_ERROR, "Unable to open device %s.\n", device_name);
        return ENOENT;
    }

    size = device->GetSize();
    Utilities::print(
      Utilities::MSG_STATUS_SUCCESS, "Found device %" PRIu64 " MB large\n", size / (1024 * 1024));
    if (size == 0) {
        Utilities::print(Utilities::MSG_STATUS_ERROR,
                         "Device should not have a size of 0. It's probably invalid.\n");
        return EBADMSG;
    }

    // If a disk image, open it up and find the offset.
    GptPartitionMap gpt;
    bool rc = gpt.LoadAndVerify(*device);
    if (rc) {
        Utilities::print(Utilities::MSG_STATUS_SUCCESS,
                         "Found GPT table. Looking for APFS partition...\n");
        int partid = gpt.FindFirstAPFSPartition();
        if (partid != -1) {
            gpt.GetPartitionOffsetAndSize(partid, offset, size);
        } else {
            Utilities::print(Utilities::MSG_STATUS_ERROR, "Could not find an APFS partition.\n");
            return ENOENT;
        }
    }

    auto container = std::make_unique<ApfsContainer>(device.get(), offset, size);

    if (!container->Init()) {
        Utilities::print(Utilities::MSG_STATUS_ERROR, "Could not initialize the APFS container.\n");
        return EBADMSG;
    }

    volcnt = container->GetVolumeCnt();
    if (volcnt <= 0) {
        Utilities::print(Utilities::MSG_STATUS_ERROR, "Volume count should not be zero.\n");
        return EBADMSG;
    }
    Utilities::print(Utilities::MSG_STATUS_SUCCESS,
                     "Found APFS filesystem! Volume count: %d\nExtracting\n",
                     volcnt);

    // Iterate through the volumes, extracting each one to a specific dir.
    for (int i = 0; i < volcnt; i++) {
        container->GetVolumeInfo(i, superblock);
        if ((superblock.apfs_fs_flags & APFS_FS_CRYPTOFLAGS) == 0) {
            Utilities::print(Utilities::MSG_STATUS_ERROR,
                             "Filevault is not supported yet. Exiting.\n");
            return ENOTSUP;
        }

        auto volume = std::unique_ptr<ApfsVolume>(container->GetVolume(i));
        APFSWriter writer(volume.get(),
                          std::string(output_dir) + "/Volume " + std::to_string(i) + "/out",
                          superblock);
        writer.write_contents_of_tree(APFS_ROOT_INODE);
    }
}
