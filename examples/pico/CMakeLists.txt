cmake_minimum_required(VERSION 3.13)

# Initialize Pico SDK
include(pico_sdk_import.cmake)

project(wh_pico_server C CXX)

pico_sdk_init()

if (NOT DEFINED WOLFHSM_ROOT)
    set(WOLFHSM_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
endif()

# WolfSSL Path - Assume sibling to WolfHSM or inside
if (EXISTS "${WOLFHSM_ROOT}/wolfssl")
    set(WOLFSSL_ROOT "${WOLFHSM_ROOT}/wolfssl")
else()
    set(WOLFSSL_ROOT "${WOLFHSM_ROOT}/../wolfssl")
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${WOLFHSM_ROOT}
    ${WOLFSSL_ROOT}
    ${WOLFHSM_ROOT}/port/pico
)

# Define sources
set(WOLFHSM_SOURCES
    ${WOLFHSM_ROOT}/src/wh_server.c
    ${WOLFHSM_ROOT}/src/wh_comm.c
    ${WOLFHSM_ROOT}/src/wh_log.c
    ${WOLFHSM_ROOT}/src/wh_utils.c
    ${WOLFHSM_ROOT}/src/wh_message_comm.c
    ${WOLFHSM_ROOT}/src/wh_message_nvm.c
    ${WOLFHSM_ROOT}/src/wh_message_counter.c
    ${WOLFHSM_ROOT}/src/wh_message_customcb.c
    # Add dummies or full implementation if needed. 
    # For basic Init/Echo with NO_CRYPTO + NULL NVM, these should suffice 
    # if weak linking or no calls are made.
    # However, wh_server may reference symbols.
    ${WOLFHSM_ROOT}/src/wh_server_nvm.c
    ${WOLFHSM_ROOT}/src/wh_nvm.c
    ${WOLFHSM_ROOT}/src/wh_server_crypto.c
    ${WOLFHSM_ROOT}/src/wh_crypto.c
    ${WOLFHSM_ROOT}/src/wh_server_keystore.c
    ${WOLFHSM_ROOT}/src/wh_server_counter.c
    ${WOLFHSM_ROOT}/src/wh_server_customcb.c
)

add_executable(pico_spi_server
    pico_spi_server.c
    ${WOLFHSM_ROOT}/port/pico/wh_transport_spi.c
    ${WOLFHSM_SOURCES}
)

# Configs
target_compile_definitions(pico_spi_server PRIVATE
    WOLFHSM_CFG_NO_CRYPTO
    WOLFHSM_CFG_NO_SYS_TIME
    WOLFHSM_CFG_ENABLE_SERVER
    WOLFHSM_CFG_COMM_DATA_LEN=1024
    SPI_BUFFER_SIZE=2048
)

target_link_libraries(pico_spi_server
    pico_stdlib
    hardware_spi
    hardware_gpio
)

pico_add_extra_outputs(pico_spi_server)
