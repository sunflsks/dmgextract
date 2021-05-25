cmake_minimum_required(VERSION 3.10)

project(dmgextract)
set(APFS_FUSE_DIR lib/apfs-fuse)

set (CMAKE_CXX_STANDARD 17)

add_library(lzfse
        ${APFS_FUSE_DIR}/3rdparty/lzfse/src/lzfse.h
        ${APFS_FUSE_DIR}/3rdparty/lzfse/src/lzfse_decode.c
        ${APFS_FUSE_DIR}/3rdparty/lzfse/src/lzfse_decode_base.c
        ${APFS_FUSE_DIR}/3rdparty/lzfse/src/lzfse_encode.c
        ${APFS_FUSE_DIR}/3rdparty/lzfse/src/lzfse_encode_base.c
        ${APFS_FUSE_DIR}/3rdparty/lzfse/src/lzfse_encode_tables.h
        ${APFS_FUSE_DIR}/3rdparty/lzfse/src/lzfse_fse.c
        ${APFS_FUSE_DIR}/3rdparty/lzfse/src/lzfse_fse.h
        ${APFS_FUSE_DIR}/3rdparty/lzfse/src/lzfse_internal.h
        ${APFS_FUSE_DIR}/3rdparty/lzfse/src/lzfse_tunables.h
        ${APFS_FUSE_DIR}/3rdparty/lzfse/src/lzvn_decode_base.c
        ${APFS_FUSE_DIR}/3rdparty/lzfse/src/lzvn_decode_base.h
        ${APFS_FUSE_DIR}/3rdparty/lzfse/src/lzvn_encode_base.c
        ${APFS_FUSE_DIR}/3rdparty/lzfse/src/lzvn_encode_base.h
)

add_library(apfs
        ${APFS_FUSE_DIR}/ApfsLib/Aes.cpp
        ${APFS_FUSE_DIR}/ApfsLib/Aes.h
        ${APFS_FUSE_DIR}/ApfsLib/AesXts.cpp
        ${APFS_FUSE_DIR}/ApfsLib/AesXts.h
        ${APFS_FUSE_DIR}/ApfsLib/ApfsContainer.cpp
        ${APFS_FUSE_DIR}/ApfsLib/ApfsContainer.h
        ${APFS_FUSE_DIR}/ApfsLib/ApfsDir.cpp
        ${APFS_FUSE_DIR}/ApfsLib/ApfsDir.h
        ${APFS_FUSE_DIR}/ApfsLib/ApfsNodeMapper.cpp
        ${APFS_FUSE_DIR}/ApfsLib/ApfsNodeMapper.h
        ${APFS_FUSE_DIR}/ApfsLib/ApfsNodeMapperBTree.cpp
        ${APFS_FUSE_DIR}/ApfsLib/ApfsNodeMapperBTree.h
        ${APFS_FUSE_DIR}/ApfsLib/ApfsVolume.cpp
        ${APFS_FUSE_DIR}/ApfsLib/ApfsVolume.h
        ${APFS_FUSE_DIR}/ApfsLib/BlockDumper.cpp
        ${APFS_FUSE_DIR}/ApfsLib/BlockDumper.h
        ${APFS_FUSE_DIR}/ApfsLib/BTree.cpp
        ${APFS_FUSE_DIR}/ApfsLib/BTree.h
        ${APFS_FUSE_DIR}/ApfsLib/CheckPointMap.cpp
        ${APFS_FUSE_DIR}/ApfsLib/CheckPointMap.h
        ${APFS_FUSE_DIR}/ApfsLib/Crc32.cpp
        ${APFS_FUSE_DIR}/ApfsLib/Crc32.h
        ${APFS_FUSE_DIR}/ApfsLib/Crypto.cpp
        ${APFS_FUSE_DIR}/ApfsLib/Crypto.h
        ${APFS_FUSE_DIR}/ApfsLib/Decmpfs.cpp
        ${APFS_FUSE_DIR}/ApfsLib/Decmpfs.h
        ${APFS_FUSE_DIR}/ApfsLib/Des.cpp
        ${APFS_FUSE_DIR}/ApfsLib/Des.h
        ${APFS_FUSE_DIR}/ApfsLib/Device.cpp
        ${APFS_FUSE_DIR}/ApfsLib/Device.h
        ${APFS_FUSE_DIR}/ApfsLib/DeviceDMG.cpp
        ${APFS_FUSE_DIR}/ApfsLib/DeviceDMG.h
        ${APFS_FUSE_DIR}/ApfsLib/DeviceLinux.cpp
        ${APFS_FUSE_DIR}/ApfsLib/DeviceLinux.h
        ${APFS_FUSE_DIR}/ApfsLib/DeviceMac.cpp
        ${APFS_FUSE_DIR}/ApfsLib/DeviceMac.h
        ${APFS_FUSE_DIR}/ApfsLib/DeviceSparseImage.cpp
        ${APFS_FUSE_DIR}/ApfsLib/DeviceSparseImage.h
        ${APFS_FUSE_DIR}/ApfsLib/DeviceWinFile.cpp
        ${APFS_FUSE_DIR}/ApfsLib/DeviceWinFile.h
        ${APFS_FUSE_DIR}/ApfsLib/DeviceWinPhys.cpp
        ${APFS_FUSE_DIR}/ApfsLib/DeviceWinPhys.h
        ${APFS_FUSE_DIR}/ApfsLib/DiskImageFile.cpp
        ${APFS_FUSE_DIR}/ApfsLib/DiskImageFile.h
        ${APFS_FUSE_DIR}/ApfsLib/DiskStruct.h
        ${APFS_FUSE_DIR}/ApfsLib/Endian.h
        ${APFS_FUSE_DIR}/ApfsLib/Global.h
        ${APFS_FUSE_DIR}/ApfsLib/GptPartitionMap.cpp
        ${APFS_FUSE_DIR}/ApfsLib/GptPartitionMap.h
        ${APFS_FUSE_DIR}/ApfsLib/KeyMgmt.cpp
        ${APFS_FUSE_DIR}/ApfsLib/KeyMgmt.h
        ${APFS_FUSE_DIR}/ApfsLib/PList.cpp
        ${APFS_FUSE_DIR}/ApfsLib/PList.h
        ${APFS_FUSE_DIR}/ApfsLib/Sha1.cpp
        ${APFS_FUSE_DIR}/ApfsLib/Sha1.h
        ${APFS_FUSE_DIR}/ApfsLib/Sha256.cpp
        ${APFS_FUSE_DIR}/ApfsLib/Sha256.h
        ${APFS_FUSE_DIR}/ApfsLib/TripleDes.cpp
        ${APFS_FUSE_DIR}/ApfsLib/TripleDes.h
        ${APFS_FUSE_DIR}/ApfsLib/Util.cpp
        ${APFS_FUSE_DIR}/ApfsLib/Util.h
        ${APFS_FUSE_DIR}/ApfsLib/Unicode.cpp
        ${APFS_FUSE_DIR}/ApfsLib/Unicode.h
)

include_directories(${APFS_FUSE_DIR} ${APFS_FUSE_DIR}/3rdparty/lzfse/src)

add_executable(dmgextract src/apfs.cpp src/main.cpp)
target_link_libraries(dmgextract z bz2 apfs lzfse)
