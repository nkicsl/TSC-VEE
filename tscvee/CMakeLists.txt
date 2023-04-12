cmake_minimum_required(VERSION 3.10)

SET(CMAKE_BUILD_TYPE "Debug")

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")

SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

project(tsc-vee)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(CROSS_COMPILE 0)

if(CROSS_COMPILE) 
  
set(CMAKE_SYSTEM_NAME Linux)
set(TOOLCHAIN_DIR "/home/jian/optee/toolchains/aarch64")

set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/aarch64-linux-gnu-g++)
set(CMAKE_C_COMPILER   ${TOOLCHAIN_DIR}/bin/aarch64-linux-gnu-gcc)
set(GNU_FLAGS "-O2")
set(CMAKE_CXX_FLAGS "${GNU_FLAGS} ")
set(CMAKE_C_FLAGS "${GNU_FLAGS}  ")

SET(CMAKE_FIND_ROOT_PATH  ${TOOLCHAIN_DIR}
 ${TOOLCHAIN_DIR}/aarch64-linux-gnu/include
 ${TOOLCHAIN_DIR}/aarch64-linux-gnu/lib
 ${TOOLCHAIN_DIR}/aarch64-linux-gnu/lib64 )
  
ENDIF(CROSS_COMPILE)

include_directories(include stc)

set(HELP_OBJS src/cJSON.c src/sha3.c src/uint256.c)

add_executable(tsc-vee ${HELP_OBJS} src/main.c)
