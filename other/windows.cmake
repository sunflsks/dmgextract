set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_AR x86_64-w64-mingw32-ar)
set(CMAKE_LINKER x86_64-w64-mingw32-ld)
set(CMAKE_OBJCOPY x86_64-w64-mingw32-objcopy)
set(CMAKE_RANLIB x86_64-w64-mingw32-ranlib)
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
