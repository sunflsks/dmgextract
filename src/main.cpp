#include "APFS/APFSHandler.hpp"
#include "APFS/APFSWriter.hpp"
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

bool dmgextract_verbose = false;

// The inode for '/' on all APFS filesystems.
#define APFS_ROOT_INODE 2

void usage(const char* name) {
    fprintf(stderr, "Usage: %s -i filesystem[.dmg] -o extractdir [-v]\n", name);
}

int main(int argc, char** argv) {
    const char* device_name = nullptr;
    const char* output_dir = nullptr;
    char opt;

#ifdef WIN32
    Utilities::print(Utilities::MSG_STATUS_WARNING,
                     "Windows detected. Please run as administrator or with developer mode enabled "
                     "for symlink support.\n");
#endif // WIN32

    while ((opt = getopt(argc, argv, "i:o:v")) != -1) {
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
                dmgextract_verbose = true;
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

    if (std::filesystem::exists(output_dir)) {
        Utilities::print(
          Utilities::MSG_STATUS_ERROR,
          "Will not write to an existing directory/file. Please delete/move it and try"
          "again.\n");
        return 2;
    }

    APFSHandler handler(device_name, output_dir);
    if (handler.init()) {
        return !handler.write();
    }

    Utilities::print(Utilities::MSG_STATUS_WARNING, "Not APFS. Trying HFS+ instead...\n");
    // hfs crap here
}
