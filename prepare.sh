#!/bin/sh

sed -i '' '1s/^/#include <cstddef>\n/' lib/apfs-fuse/ApfsLib/DiskStruct.h
