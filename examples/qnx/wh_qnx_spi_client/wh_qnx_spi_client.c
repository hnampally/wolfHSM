/*
 * wolfHSM Client QNX SPI Example
 */

#include <stdint.h>
#include <stdio.h>  /* For printf */
#include <string.h> /* For memset, memcpy */
#include <unistd.h> /* for read */
#include <time.h>   /* For nanosleep */

#include "wolfhsm/wh_error.h"
#include "wolfhsm/wh_comm.h"
#include "wolfhsm/wh_utils.h"
#include "wolfhsm/wh_message.h"
#include "wolfhsm/wh_client.h"
#include "wolfhsm/wh_client_crypto.h"

#include "examples/demo/client/wh_demo_client_all.h"
#include "wh_qnx_cfg.h"
#include "wh_qnx_client_cfg.h"

#ifndef WOLFHSM_CFG_NO_CRYPTO
/* included to print out the version of wolfSSL linked with */
#include "wolfssl/version.h"
#endif

/** Local declarations */
static void _sleepMs(long milliseconds);
static int  wh_ClientTask(void* cf, int test);


static void _sleepMs(long milliseconds)
{
    struct timespec req;
    req.tv_sec  = milliseconds / 1000;
    req.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&req, NULL);
}

enum {
    REPEAT_COUNT = 20,
    REQ_SIZE     = 32,
    RESP_SIZE    = WOLFHSM_CFG_COMM_DATA_LEN,
    ONE_MS       = 1,
};

static int wh_ClientTask(void* cf, int test)
{
    static whClientContext client[1];
    whClientConfig* config = (whClientConfig*)cf;
    int             ret    = 0;
    int             counter = 1;

    uint8_t  tx_req[REQ_SIZE] = {0};
    uint16_t tx_req_len       = 0;

    uint8_t  rx_resp[RESP_SIZE] = {0};
    uint16_t rx_resp_len        = 0;

    if (config == NULL) {
        return -1;
    }

    ret = wh_Client_Init(client, config);
    if (ret != 0) {
        perror("Init error:");
        return -1;
    }
    printf("Client initialized successfully\n");
    if (ret == 0) {
        ret = wh_Client_CommInit(client, NULL, NULL);
        if (ret != 0) {
            WOLFHSM_CFG_PRINTF("Failed to initialize client communication\n");
            return -1;
        }
    }
    printf("Client communication initialized successfully\n");
    WOLFHSM_CFG_PRINTF("Client connecting to server...\n");
    if (ret == 0 && test) {
        WOLFHSM_CFG_PRINTF("not Running client demos...\n");
        // return wh_DemoClient_All(client);
    }
    
    WOLFHSM_CFG_PRINTF("Client connected. Running echo test...\n");

    for (counter = 0; counter < REPEAT_COUNT; counter++) {
        sprintf((char*)tx_req, "Request:%u", counter);
        tx_req_len = strlen((char*)tx_req);
        do {
            ret = wh_Client_EchoRequest(client, tx_req_len, tx_req);
            if (ret != WH_ERROR_NOTREADY) {
                if (ret != 0) {
                    WOLFHSM_CFG_PRINTF("wh_Client_EchoRequest failed with ret=%d\n", ret);
                }
            }
            _sleepMs(ONE_MS);
        } while (ret == WH_ERROR_NOTREADY);

        if (ret != 0) {
            WOLFHSM_CFG_PRINTF("Client had failure. Exiting\n");
            break;
        }

        rx_resp_len = 0;
        memset(rx_resp, 0, sizeof(rx_resp));

        do {
            ret = wh_Client_EchoResponse(client, &rx_resp_len, rx_resp);
            _sleepMs(ONE_MS);
        } while (ret == WH_ERROR_NOTREADY);

        if (ret != 0) {
            WOLFHSM_CFG_PRINTF("Client had failure. Exiting\n");
            break;
        }
    }
#if defined(DWOLFHSM_CFG_NO_CRYPTO)
    /* Context 1: Client Local Crypto */
    WC_RNG  rng[1];
    uint8_t buffer[128] = {0};
    wc_InitRng_ex(rng, NULL, INVALID_DEVID);
    wc_RNG_GenerateBlock(rng, buffer, sizeof(buffer));
    wc_FreeRng(rng);
    wh_Utils_Hexdump("Context 1: Client Local RNG:\n", buffer, sizeof(buffer));

    /* Context 2: Client Remote Crypto */
    memset(buffer, 0, sizeof(buffer));
    wc_InitRng_ex(rng, NULL, WH_DEV_ID);
    wc_RNG_GenerateBlock(rng, buffer, sizeof(buffer));
    wc_FreeRng(rng);
    wh_Utils_Hexdump("Context 2: Client Remote RNG:\n", buffer, sizeof(buffer));
#endif

    (void)wh_Client_CommClose(client);
    (void)wh_Client_Cleanup(client);
    WOLFHSM_CFG_PRINTF("Client disconnected\n");

    return ret;
}

void Usage(const char* exeName)
{
    WOLFHSM_CFG_PRINTF("Usage: %s --test\n", exeName);
}

whClientConfig c_conf[1];
int main(int argc, char** argv)
{
    int            test = 0; /* flag if running wolfcrypt test */
    int            i;

    (void)argc;
    (void)argv;

    memset(c_conf, 0, sizeof(whClientConfig));
    WOLFHSM_CFG_PRINTF("Example wolfHSM QNX client ");
#ifndef WOLFHSM_CFG_NO_CRYPTO
    WOLFHSM_CFG_PRINTF("built with wolfSSL version %s\n", LIBWOLFSSL_VERSION_STRING);
#else
    WOLFHSM_CFG_PRINTF("built with WOLFHSM_CFG_NO_CRYPTO\n");
#endif

    /* Parse command-line arguments */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--test") == 0) {
            test = 1;
        }
        else {
            WOLFHSM_CFG_PRINTF("Invalid argument: %s\n", argv[i]);
            Usage(argv[0]);
            return -1;
        }
    }

    WOLFHSM_CFG_PRINTF("Using SPI transport\n");
    wh_QnxClient_ExampleSpiConfig(c_conf);

    return wh_ClientTask(c_conf, test);
}
