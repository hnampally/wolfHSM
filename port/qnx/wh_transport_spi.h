/*
 * port/qnx/wh_transport_spi.h
 *
 * QNX SPI Transport Header
 */

#ifndef WH_TRANSPORT_SPI_H
#define WH_TRANSPORT_SPI_H

#include <stdint.h>
#include "wolfhsm/wh_comm.h"

/* 
 * Must be larger than WOLFHSM_CFG_COMM_DATA_LEN + sizeof(whCommHeader) 
 * 1024 + 8 = 1032. We use 2048 for safety.
 */
#define SPI_BUFFER_SIZE 2048

typedef struct {
    const char* device_path; /* e.g. "/dev/spi0" */
    uint32_t device_id;      /* spi device id */
    uint32_t clock_rate;
    uint32_t mode;           /* SPI mode flags */
    int      loopback_mode;  /* 0=None, 1=Client, 2=Server */
} whQnxSpiConfig;

#define WH_QNX_SPI_LOOPBACK_NONE   0
#define WH_QNX_SPI_LOOPBACK_CLIENT 1
#define WH_QNX_SPI_LOOPBACK_SERVER 2

typedef struct {
    int fd;
    uint32_t device_id;
    uint8_t buffer[SPI_BUFFER_SIZE];
    uint16_t buffer_offset;
    int is_loopback;
    int loopback_read_fd;
    int loopback_write_fd;
} whQnxSpiClientContext;

/* Callback structure initializer */
#define WH_QNX_SPI_CLIENT_CB { \
    .Init = wh_QnxSpi_Init, \
    .Send = wh_QnxSpi_Send, \
    .Recv = wh_QnxSpi_Recv, \
    .Cleanup = wh_QnxSpi_Cleanup \
}

typedef whQnxSpiClientContext whQnxSpiServerContext;

#define WH_QNX_SPI_SERVER_CB { \
    .Init = wh_QnxSpi_Init, \
    .Send = wh_QnxSpi_Send, \
    .Recv = wh_QnxSpi_Recv, \
    .Cleanup = wh_QnxSpi_Cleanup \
}

/* Function Prototypes */
int wh_QnxSpi_Init(void* context, const void* config, whCommSetConnectedCb connectcb, void* connectcb_arg);
int wh_QnxSpi_Send(void* context, uint16_t size, const void* data);
int wh_QnxSpi_Recv(void* context, uint16_t *out_size, void* data);
int wh_QnxSpi_Cleanup(void* context);

#endif /* WH_TRANSPORT_SPI_H */
