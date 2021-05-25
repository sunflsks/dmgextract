cmake_minimum_required(VERSION 3.10)

project(dmgextract)

set (CMAKE_CXX_STANDARD 17)

add_library(lzfse
        lib/lzfse/src/lzfse.h
        lib/lzfse/src/lzfse_decode.c
        lib/lzfse/src/lzfse_decode_base.c
        lib/lzfse/src/lzfse_encode.c
        lib/lzfse/src/lzfse_encode_base.c
        lib/lzfse/src/lzfse_encode_tables.h
        lib/lzfse/src/lzfse_fse.c
        lib/lzfse/src/lzfse_fse.h
        lib/lzfse/src/lzfse_internal.h
        lib/lzfse/src/lzfse_tunables.h
        lib/lzfse/src/lzvn_decode_base.c
        lib/lzfse/src/lzvn_decode_base.h
        lib/lzfse/src/lzvn_encode_base.c
        lib/lzfse/src/lzvn_encode_base.h
)

add_library(apfs
        lib/ApfsLib/Aes.cpp
        lib/ApfsLib/Aes.h
        lib/ApfsLib/AesXts.cpp
        lib/ApfsLib/AesXts.h
        lib/ApfsLib/ApfsContainer.cpp
        lib/ApfsLib/ApfsContainer.h
        lib/ApfsLib/ApfsDir.cpp
        lib/ApfsLib/ApfsDir.h
        lib/ApfsLib/ApfsNodeMapper.cpp
        lib/ApfsLib/ApfsNodeMapper.h
        lib/ApfsLib/ApfsNodeMapperBTree.cpp
        lib/ApfsLib/ApfsNodeMapperBTree.h
        lib/ApfsLib/ApfsVolume.cpp
        lib/ApfsLib/ApfsVolume.h
        lib/ApfsLib/BlockDumper.cpp
        lib/ApfsLib/BlockDumper.h
        lib/ApfsLib/BTree.cpp
        lib/ApfsLib/BTree.h
        lib/ApfsLib/CheckPointMap.cpp
        lib/ApfsLib/CheckPointMap.h
        lib/ApfsLib/Crc32.cpp
        lib/ApfsLib/Crc32.h
        lib/ApfsLib/Crypto.cpp
        lib/ApfsLib/Crypto.h
        lib/ApfsLib/Decmpfs.cpp
        lib/ApfsLib/Decmpfs.h
        lib/ApfsLib/Des.cpp
        lib/ApfsLib/Des.h
        lib/ApfsLib/Device.cpp
        lib/ApfsLib/Device.h
        lib/ApfsLib/DeviceDMG.cpp
        lib/ApfsLib/DeviceDMG.h
        lib/ApfsLib/DeviceLinux.cpp
        lib/ApfsLib/DeviceLinux.h
        lib/ApfsLib/DeviceMac.cpp
        lib/ApfsLib/DeviceMac.h
        lib/ApfsLib/DeviceSparseImage.cpp
        lib/ApfsLib/DeviceSparseImage.h
        lib/ApfsLib/DeviceWinFile.cpp
        lib/ApfsLib/DeviceWinFile.h
        lib/ApfsLib/DeviceWinPhys.cpp
        lib/ApfsLib/DeviceWinPhys.h
        lib/ApfsLib/DiskImageFile.cpp
        lib/ApfsLib/DiskImageFile.h
        lib/ApfsLib/DiskStruct.h
        lib/ApfsLib/Endian.h
        lib/ApfsLib/Global.h
        lib/ApfsLib/GptPartitionMap.cpp
        lib/ApfsLib/GptPartitionMap.h
        lib/ApfsLib/KeyMgmt.cpp
        lib/ApfsLib/KeyMgmt.h
        lib/ApfsLib/PList.cpp
        lib/ApfsLib/PList.h
        lib/ApfsLib/Sha1.cpp
        lib/ApfsLib/Sha1.h
        lib/ApfsLib/Sha256.cpp
        lib/ApfsLib/Sha256.h
        lib/ApfsLib/TripleDes.cpp
        lib/ApfsLib/TripleDes.h
        lib/ApfsLib/Util.cpp
        lib/ApfsLib/Util.h
        lib/ApfsLib/Unicode.cpp
        lib/ApfsLib/Unicode.h
)

include_directories(lib lib/lzfse/src)

add_executable(dmgextract src/apfs.cpp src/main.cpp)
target_link_libraries(dmgextract z bz2 apfs lzfse)
