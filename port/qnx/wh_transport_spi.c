/*
 * port/qnx/wh_transport_spi.c
 *
 * QNX SPI Transport Implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <devctl.h>
#include <hw/io-spi.h>

#include "wolfhsm/wh_error.h"
#include "wh_transport_spi.h"

#define DUMMY_BYTE 0xFF
#define FIFO_C2S "/tmp/wh_spi_c2s"
#define FIFO_S2C "/tmp/wh_spi_s2c"

int wh_QnxSpi_Init(void* context, const void* config, whCommSetConnectedCb connectcb, void* connectcb_arg)
{
    whQnxSpiClientContext* ctx = (whQnxSpiClientContext*)context;
    const whQnxSpiConfig* cfg = (const whQnxSpiConfig*)config;
    
    if (ctx == NULL || cfg == NULL) {
        return WH_ERROR_BADARGS;
    }

    ctx->device_id = cfg->device_id;
    ctx->buffer_offset = 0;
    ctx->is_loopback = (cfg->loopback_mode != WH_QNX_SPI_LOOPBACK_NONE);

    if (ctx->is_loopback) {
        /* Create FIFOs if they don't exist */
        mkfifo(FIFO_C2S, 0666);
        mkfifo(FIFO_S2C, 0666);

        if (cfg->loopback_mode == WH_QNX_SPI_LOOPBACK_CLIENT) {
            printf("Opening Loopback Client... C2S(WR) S2C(RD)\n");
            ctx->loopback_write_fd = open(FIFO_C2S, O_WRONLY);
            ctx->loopback_read_fd  = open(FIFO_S2C, O_RDONLY);
        } else if (cfg->loopback_mode == WH_QNX_SPI_LOOPBACK_SERVER) {
            printf("Opening Loopback Server... C2S(RD) S2C(WR)\n");
            ctx->loopback_read_fd  = open(FIFO_C2S, O_RDONLY);
            ctx->loopback_write_fd = open(FIFO_S2C, O_WRONLY);
        }
        
        if (ctx->loopback_read_fd < 0 || ctx->loopback_write_fd < 0) {
            perror("Failed to open loopback FIFOs");
            return WH_ERROR_ABORTED;
        }
        ctx->fd = -1; /* unused in loopback */
    } else {
        /* Open the SPI driver */
        printf("Opening SPI Device: %s\n", cfg->device_path);        
        ctx->fd = open(cfg->device_path, O_RDWR);
        if (ctx->fd < 0) {
            perror("Failed to open SPI device");
            return WH_ERROR_ABORTED;
        }
        printf("SPI Device opened successfully\n");
    }

    /* 
     * Optional: Configure SPI Device 
     * struct spi_cfg defined in hw/spi-master.h
     * This depends on if the driver requires runtime config or boot time.
     * We assume basic connectivity is established.
     */
    
    if (connectcb) {
        connectcb(ctx, WH_COMM_CONNECTED);
    }
    (void)connectcb_arg;

    return 0;
}

int wh_QnxSpi_Send(void* context, uint16_t size, const void* data)
{
    whQnxSpiClientContext* ctx = (whQnxSpiClientContext*)context;
    spi_xchng_t* msg;
    uint8_t* raw_buf;
    int rc;

    if (ctx == NULL || data == NULL || size == 0) {
        return 0; /* Nothing to send */
    }

    if (ctx->is_loopback) {
        ssize_t written = write(ctx->loopback_write_fd, data, size);
        if (written != (ssize_t)size) {
            return WH_ERROR_ABORTED;
        }
        return 0;
    }
    printf("SPI Send %u bytes\n", (unsigned int)size);

    if (sizeof(spi_xchng_t) + size > SPI_BUFFER_SIZE) {
        printf("SPI Send failed: size %u exceeds buffer %u\n", (unsigned int)size, SPI_BUFFER_SIZE);
        return WH_ERROR_ABORTED;
    }
    
    raw_buf = ctx->buffer;
    memset(raw_buf, 0, sizeof(spi_xchng_t) + size);
    
    msg = (spi_xchng_t*)raw_buf;
    msg->nbytes = size;
    
    /* Copy data to Tx buffer */
    memcpy(msg->data, data, size);
    printf("Performing SPI Data Exchange (size=%d)\n", (int)size);
    rc = devctl(ctx->fd, DCMD_SPI_DATA_XCHNG, msg, sizeof(spi_xchng_t) + size, NULL);
    printf("SPI Data Exchange completed (rc=%d)\n", rc);
    
    if (rc != EOK) {
        return WH_ERROR_ABORTED;
    }
    
    return 0;
}

int wh_QnxSpi_Recv(void* context, uint16_t *out_size, void* data)
{
    whQnxSpiClientContext* ctx = (whQnxSpiClientContext*)context;
    spi_xchng_t* msg;
    uint8_t* raw_buf;
    int rc;
    uint16_t bytes_to_read = 1; /* Polling byte by byte? Efficient? No. */

    if (ctx == NULL || data == NULL || out_size == NULL || bytes_to_read == 0) {
        return WH_ERROR_BADARGS;
    }

    if (*out_size > 0) bytes_to_read = *out_size;
    if (ctx->is_loopback) {
        ssize_t n = read(ctx->loopback_read_fd, data, bytes_to_read);
        if (n > 0) {
            *out_size = (uint16_t)n;
            return 0;
        } else if (n == 0) {
            /* EOF - Writer closed? */
            *out_size = 0;
            return WH_ERROR_NOTREADY; 
        } else {
            /* Error or EAGAIN */
            *out_size = 0;
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return WH_ERROR_NOTREADY;
            }
            return WH_ERROR_ABORTED;
        }
    }
    
    if (sizeof(spi_xchng_t) + bytes_to_read > SPI_BUFFER_SIZE) {
        printf("SPI Recv failed: size %u exceeds buffer %u\n", (unsigned int)bytes_to_read, SPI_BUFFER_SIZE);
        return WH_ERROR_ABORTED;
    }

    raw_buf = ctx->buffer;
    msg = (spi_xchng_t*)raw_buf;
    msg->nbytes = bytes_to_read;
    
    /* Set Tx to DUMMY */
    memset(msg->data, DUMMY_BYTE, bytes_to_read);
    printf("Performing SPI Data Exchange for Recv (size=%d)\n", (int)bytes_to_read);    
    rc = devctl(ctx->fd, DCMD_SPI_DATA_XCHNG, msg, sizeof(spi_xchng_t) + bytes_to_read, NULL);
    
    if (rc == EOK) {
         memcpy(data, msg->data, bytes_to_read);
         *out_size = bytes_to_read;
         printf("SPI Recv completed, received %u bytes\n", (unsigned int)*out_size);
    } else {
        *out_size = 0;
         /* If EAGAIN, return WH_ERROR_NOTREADY */
         printf("SPI Recv failed (rc=%d)\n", rc);
         if (rc == EAGAIN) {
             /* No free() needed */
             printf("SPI Recv not ready\n");
             return WH_ERROR_NOTREADY;
         }
    }
    
    if (rc != EOK && rc != EAGAIN) {
        return WH_ERROR_ABORTED;
    }

    return 0;
}

int wh_QnxSpi_Cleanup(void* context)
{
    whQnxSpiClientContext* ctx = (whQnxSpiClientContext*)context;
    if (ctx) {
        if (ctx->is_loopback) {
            if (ctx->loopback_read_fd >= 0) close(ctx->loopback_read_fd);
            if (ctx->loopback_write_fd >= 0) close(ctx->loopback_write_fd);
        } else {
            if (ctx->fd >= 0) {
                close(ctx->fd);
                ctx->fd = -1;
            }
        }
    }
    return 0;
}
