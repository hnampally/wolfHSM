cmake_minimum_required(VERSION 3.10)

# Project Name
project(wh_qnx_spi_client C ASM)

# Define default paths
if(NOT DEFINED WOLFSSL_DIR)
    get_filename_component(WOLFSSL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../wolfssl" ABSOLUTE)
endif()

if(NOT DEFINED WOLFHSM_DIR)
    get_filename_component(WOLFHSM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)
endif()

set(WOLFHSM_PORT_DIR "${WOLFHSM_DIR}/port/posix")
set(WOLFHSM_PORT_QNX_DIR "${WOLFHSM_DIR}/port/qnx")
set(WOLFHSM_DEMO_CLIENT_DIR "${WOLFHSM_DIR}/examples/demo/client")
set(CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}") # Config files are in current dir

# QNX Specific Flags check
if("${CMAKE_SYSTEM_NAME}" STREQUAL "QNX")
    add_definitions(-D_QNX_SOURCE)
endif()

# Includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${WOLFSSL_DIR}
    ${WOLFHSM_DIR}
    ${WOLFHSM_PORT_DIR}
    ${WOLFHSM_DEMO_CLIENT_DIR}
)

# Defines
add_definitions(-D_POSIX_C_SOURCE=200809L)
add_definitions(-DWOLFSSL_USER_SETTINGS -DWOLFHSM_CFG)

# Debug configuration
if(DEBUG)
    add_definitions(-DWOLFHSM_CFG_DEBUG)
    add_compile_options(-ggdb -g3)
endif()

if(DEBUG_VERBOSE)
    add_definitions(-DWOLFHSM_CFG_DEBUG -DWOLFHSM_CFG_DEBUG_VERBOSE)
    add_compile_options(-ggdb -g3)
endif()

# DMA configuration
if(DMA)
    add_definitions(-DWC_USE_DEVID=0x57444D41 -DWC_NO_DEFAULT_DEVID -DWOLFHSM_CFG_DMA)
else()
    add_definitions(-DWC_USE_DEVID=0x5748534D)
endif()

# Compiler options
add_compile_options(-Werror -Wall -Wextra -ffunction-sections -fdata-sections)
set(CMAKE_C_STANDARD 90)

# Linker options
if(APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-dead_strip")
else()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
endif()

# Use ASAN
if(ASAN)
    add_compile_options(-fsanitize=address)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

# Source Files
file(GLOB SRC_WOLFCRYPT "${WOLFSSL_DIR}/wolfcrypt/src/*.c")
file(GLOB SRC_WOLFCRYPT_TEST "${WOLFSSL_DIR}/wolfcrypt/test/*.c")
file(GLOB SRC_WOLFCRYPT_BENCH "${WOLFSSL_DIR}/wolfcrypt/benchmark/*.c")
file(GLOB SRC_WOLFSSL "${WOLFSSL_DIR}/src/*.c")
file(GLOB SRC_WOLFHSM "${WOLFHSM_DIR}/src/*.c")
# file(GLOB SRC_PORT "${WOLFHSM_PORT_DIR}/*.c")
set(SRC_PORT "${WOLFHSM_PORT_DIR}/posix_flash_file.c"
             "${WOLFHSM_PORT_DIR}/posix_log_file.c"
             "${WOLFHSM_PORT_DIR}/posix_time.c")
file(GLOB SRC_PORT_QNX "${WOLFHSM_PORT_QNX_DIR}/*.c")
file(GLOB SRC_PROJECT "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
file(GLOB SRC_DEMO_CLIENT "${WOLFHSM_DEMO_CLIENT_DIR}/*.c")

# Combine Sources
if(NOT NOCRYPTO)
    set(SOURCES 
        ${SRC_WOLFCRYPT} 
        ${SRC_WOLFCRYPT_TEST} 
        ${SRC_WOLFCRYPT_BENCH} 
        ${SRC_WOLFSSL}
    )
else()
    add_definitions(-DWOLFHSM_CFG_NO_CRYPTO)
endif()

list(APPEND SOURCES
    ${SRC_WOLFHSM}
    ${SRC_PORT}
    ${SRC_PORT_QNX}
    ${SRC_PROJECT}
    ${SRC_DEMO_CLIENT}
)

# Create Executable
add_executable(wh_qnx_spi_client ${SOURCES})

# Link Libraries
target_link_libraries(wh_qnx_spi_client m)
if("${CMAKE_SYSTEM_NAME}" STREQUAL "QNX" OR "${CMAKE_SYSTEM_NAME}" STREQUAL "SunOS")
    target_link_libraries(wh_qnx_spi_client socket)
endif()
