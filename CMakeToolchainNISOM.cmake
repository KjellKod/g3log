
set( CMAKE_SYSTEM_NAME Linux )
set( CMAKE_SYSTEM_PROCESSOR ARM )

set(XC_ROOT "C:/Program Files (x86)/National Instruments/Eclipse/14.0/arm/sysroots/i686-nilrtsdk-mingw32")
set(XC_BINARIES "${XC_ROOT}/usr/bin/armv7a-vfp-neon-nilrt-linux-gnueabi")

# set(CMAKE_SYSROOT "C:/Program Files (x86)/National Instruments/Eclipse/14.0/arm/sysroots/armv7a-vfp-neon-nilrt-linux-gnueabi")
# set(CMAKE_STAGING_PREFIX /home/devel/stage)

set(CMAKE_C_COMPILER "${XC_BINARIES}/arm-nilrt-linux-gnueabi-gcc.exe")
set(CMAKE_CXX_COMPILER "${XC_BINARIES}/arm-nilrt-linux-gnueabi-g++.exe")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

