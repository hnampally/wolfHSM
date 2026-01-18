/*
 * examples/pico/pico_spi_server.c
 *
 * RP2350 (Pico 2) SPI WolfHSM Server
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

/* WolfHSM Includes */
#include "wolfhsm/wh_error.h"
#include "wolfhsm/wh_server.h"
#include "wolfhsm/wh_comm.h"

/* Port Include via relative path */
#include "../../port/pico/wh_transport_spi.h"

/* 
 * Pin Configuration (Using SPI0 default pins)
 * Wiring must match Master requirements
 */
#define SPI_PORT spi0
#define PIN_RX   16 /* MISO / RX */
#define PIN_CSN  17 /* CSn */
#define PIN_SCK  18 /* SCK */
#define PIN_TX   19 /* MOSI / TX */

/* Global contexts to ensure persistence */
static whServerContext      g_server;
static whPicoSpiServerContext g_transport_ctx;

int main() {
    stdio_init_all();
    
    /* Small delay for serial to come up */
    sleep_ms(2000);
    printf("\n=== Pico 2 WolfHSM SPI Server ===\n");

    /* 1. Setup Transport Configuration */
    whPicoSpiConfig spi_cfg = {
        .spi = SPI_PORT,
        .pin_rx = PIN_RX,
        .pin_tx = PIN_TX,
        .pin_sck = PIN_SCK,
        .pin_csn = PIN_CSN
    };

    /* 2. Determine Transport Callback */
    static const whTransportServerCb trans_cb = WH_PICO_SPI_SERVER_CB;
    
    /* 3. Setup Communication Config */
    whCommServerConfig comm_cfg = {
        .transport_cb = &trans_cb,
        .transport_context = &g_transport_ctx,
        .transport_config = &spi_cfg,
        .server_id = 1 /* Server ID */
    };
    
    /* 4. Setup Server Config */
    /* Minimal config for INIT/ECHO. No NVM/Crypto for this simple example. */
    whServerConfig server_cfg = {
        .comm_config = &comm_cfg,
        .nvm = NULL
#ifndef WOLFHSM_CFG_NO_CRYPTO
        , .crypto = NULL
#endif
#ifdef WOLFHSM_CFG_LOGGING
        , .logConfig = NULL
#endif
    };

    /* 5. Initialize Server */
    int rc = wh_Server_Init(&g_server, &server_cfg);
    if (rc != WH_ERROR_OK) {
        printf("Server Init Failed: %d\n", rc);
        while(1) tight_loop_contents();
    }

    printf("Server Initialized. Waiting for requests...\n");

    /* 6. Main Processing Loop */
    while(1) {
        /* Process one message if available */
        rc = wh_Server_HandleRequestMessage(&g_server);
        
        if (rc == WH_ERROR_OK) {
            printf("Message Initialized/Handled\n");
        } else if (rc == WH_ERROR_NOTREADY) {
            /* No request waiting, just idle */
            /* Maybe small sleep to yield? */
            // sleep_us(100); 
            tight_loop_contents();
        } else {
            /* Basic error logging */
            printf("Error handling message: %d\n", rc);
        }
    }

    return 0;
}
