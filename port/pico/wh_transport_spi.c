/*
 * port/pico/wh_transport_spi.c
 *
 * RP2350 (Pico 2) SPI Slave Transport Implementation
 */

#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "wolfhsm/wh_error.h"
#include "wh_transport_spi.h"

int wh_PicoSpi_Init(void* context, const void* config, whCommSetConnectedCb connectcb, void* connectcb_arg)
{
    whPicoSpiServerContext* ctx = (whPicoSpiServerContext*)context;
    const whPicoSpiConfig* cfg = (const whPicoSpiConfig*)config;

    if (ctx == NULL || cfg == NULL) {
        return WH_ERROR_BADARGS;
    }

    ctx->spi = cfg->spi;
    memset(ctx->buffer, 0, sizeof(ctx->buffer));
    ctx->buffer_len = 0;

    /* Initialize SPI instance as Slave */
    /* Note: Baud rate is ignored in slave mode, but we pass a valid value */
    spi_init(ctx->spi, 1000 * 1000); 
    spi_set_slave(ctx->spi, true);

    /* Configure GPIOs */
    gpio_set_function(cfg->pin_rx, GPIO_FUNC_SPI);
    gpio_set_function(cfg->pin_tx, GPIO_FUNC_SPI);
    gpio_set_function(cfg->pin_sck, GPIO_FUNC_SPI);
    gpio_set_function(cfg->pin_csn, GPIO_FUNC_SPI);
    
    /* 
     * Optional: Set data format if needed. 
     * Default is 8 data bits, CPOL=0, CPHA=0, which usually matches.
     * spi_set_format(ctx->spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
     */

    if (connectcb) {
        connectcb(ctx, WH_COMM_CONNECTED);
    }
    (void)connectcb_arg;

    return 0;
}

int wh_PicoSpi_Send(void* context, uint16_t size, const void* data)
{
    whPicoSpiServerContext* ctx = (whPicoSpiServerContext*)context;
    
    if (ctx == NULL || data == NULL || size == 0) {
        return 0;
    }

    /* 
     * In SPI Slave mode, "Send" means filling the TX FIFO so that 
     * when the Master clocks data out, this data is sent.
     * The Master is expected to send dummy bytes to retrieve this.
     */
    
    if (spi_write_blocking(ctx->spi, (const uint8_t*)data, size) != size) {
        return WH_ERROR_ABORTED;
    }

    return 0;
}

int wh_PicoSpi_Recv(void* context, uint16_t *out_size, void* data)
{
    whPicoSpiServerContext* ctx = (whPicoSpiServerContext*)context;
    size_t bytes_read = 0;
    uint16_t max_len;

    if (ctx == NULL || data == NULL || out_size == NULL) {
        return WH_ERROR_BADARGS;
    }

    /* Check if anything is available in the RX FIFO */
    if (!spi_is_readable(ctx->spi)) {
        *out_size = 0;
        return WH_ERROR_NOTREADY;
    }

    max_len = *out_size;
    if (max_len > PICO_SPI_BUFFER_SIZE) max_len = PICO_SPI_BUFFER_SIZE;

    /* 
     * Read all available data. 
     * Since SPI separates transactions by CS, we assume a continuous stream 
     * in the FIFO belongs to one packet.
     * 
     * We loop while readable. 
     */
    uint8_t* dst = (uint8_t*)data;
    while (spi_is_readable(ctx->spi) && bytes_read < max_len) {
        spi_read_blocking(ctx->spi, 0, &dst[bytes_read], 1);
        bytes_read++;
        
        /* 
         * Small busy wait to see if more data arrives in this burst? 
         * Depending on clock speed, checking spi_is_readable might be too fast.
         * But usually for SPI at high speeds, the FIFO fills up.
         */
    }

    *out_size = (uint16_t)bytes_read;
    return 0;
}

int wh_PicoSpi_Cleanup(void* context)
{
    whPicoSpiServerContext* ctx = (whPicoSpiServerContext*)context;
    if (ctx) {
        spi_deinit(ctx->spi);
    }
    return 0;
}
