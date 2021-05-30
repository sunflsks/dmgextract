#include "APFSHandler.hpp"
#include "../utils.hpp"
#include "APFSWriter.hpp"

#define APFS_ROOT_INODE 2

APFSHandler::APFSHandler(std::string device_path, std::string output_directory) {
    this->device_path = device_path;
    this->output_directory = output_directory;
}

bool APFSHandler::init() {
    device = Device::OpenDevice(device_path.c_str());

    if (!device) {
        Utilities::print(Utilities::MSG_STATUS_ERROR, "Unable to open device %s.\n", device_path);
        return false;
    }

    size = device->GetSize();
    if (size == 0) {
        Utilities::print(Utilities::MSG_STATUS_ERROR,
                         "Device should not have a size of 0. It's probably invalid.\n");
        return false;
    }

    Utilities::print(
      Utilities::MSG_STATUS_SUCCESS, "Found device %" PRIu64 " MB large\n", size / (1024 * 1024));

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
            return false;
        }
    }

    container = new ApfsContainer(device, offset, size);
    if (!container->Init()) {
        Utilities::print(Utilities::MSG_STATUS_ERROR, "Could not initialize the APFS container.\n");
        return false;
    }

    volcnt = container->GetVolumeCnt();
    if (volcnt <= 0) {
        Utilities::print(Utilities::MSG_STATUS_ERROR, "Volume count should not be zero.\n");
        return false;
    }

    Utilities::print(
      Utilities::MSG_STATUS_SUCCESS, "Found APFS filesystem! Volume count: %d\n", volcnt);
    return true;
}

bool APFSHandler::write() {
    // Iterate through the volumes, extracting each one to a specific dir.
    for (int i = 0; i < volcnt; i++) {
        container->GetVolumeInfo(i, superblock);
        if ((superblock.apfs_fs_flags & APFS_FS_CRYPTOFLAGS) == 0) {
            Utilities::print(Utilities::MSG_STATUS_ERROR,
                             "Filevault is not supported yet. Exiting.\n");
            return false;
        }

        auto volume = std::unique_ptr<ApfsVolume>(container->GetVolume(i));
        APFSWriter writer(
          volume.get(), output_directory + "/Volume " + std::to_string(i) + "/out", superblock);
        if (!writer.write_contents_of_tree(APFS_ROOT_INODE)) {
            Utilities::print(Utilities::MSG_STATUS_ERROR, "Error while writing volume %d.\n", i);
            return false;
        }
    }

    return true;
}

APFSHandler::~APFSHandler() {
    delete container;
}
