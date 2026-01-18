/*
 * port/pico/wh_transport_spi.h
 *
 * RP2350 (Pico 2) SPI Slave Transport Header
 */

#ifndef WH_TRANSPORT_SPI_PICO_H
#define WH_TRANSPORT_SPI_PICO_H

#include <stdint.h>
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "wolfhsm/wh_comm.h"

/* 
 * Buffer size must accommodate the largest expected message.
 * Matching the QNX side (2048) for safety.
 */
#define PICO_SPI_BUFFER_SIZE 2048

typedef struct {
    spi_inst_t* spi;        /* e.g. spi0 or spi1 */
    uint32_t pin_rx;        /* GPIO number for RX (MISO or MOSI depending on wiring) */
    uint32_t pin_tx;        /* GPIO number for TX */
    uint32_t pin_sck;       /* GPIO number for SCK */
    uint32_t pin_csn;       /* GPIO number for CSn */
} whPicoSpiConfig;

typedef struct {
    spi_inst_t* spi;
    uint8_t buffer[PICO_SPI_BUFFER_SIZE];
    uint16_t buffer_len;
    /* State tracking if needed */
} whPicoSpiServerContext;

/* Callback structure initializer */
#define WH_PICO_SPI_SERVER_CB { \
    .Init = wh_PicoSpi_Init, \
    .Send = wh_PicoSpi_Send, \
    .Recv = wh_PicoSpi_Recv, \
    .Cleanup = wh_PicoSpi_Cleanup \
}

/* Function Prototypes */
int wh_PicoSpi_Init(void* context, const void* config, whCommSetConnectedCb connectcb, void* connectcb_arg);
int wh_PicoSpi_Send(void* context, uint16_t size, const void* data);
int wh_PicoSpi_Recv(void* context, uint16_t *out_size, void* data);
int wh_PicoSpi_Cleanup(void* context);

#endif /* WH_TRANSPORT_SPI_PICO_H */
