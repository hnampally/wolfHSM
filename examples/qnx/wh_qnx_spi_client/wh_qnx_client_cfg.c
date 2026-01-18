/*
 * wolfHSM Client QNX Example
 */

#include "wh_qnx_cfg.h"
#include "wh_qnx_client_cfg.h"

#include "wolfhsm/wh_client.h"
#include "wolfhsm/wh_error.h"

#include "port/qnx/wh_transport_spi.h"

#include <string.h>

whQnxSpiClientContext          tccSpi;

whQnxSpiConfig          spiConfig;

whCommClientConfig c_comm;

whTransportClientCb spiCb = WH_QNX_SPI_CLIENT_CB;
/* client configuration setup example for transport */
int wh_QnxClient_ExampleSpiConfig(void* conf)
{
    //whClientConfig* c_conf = (whClientConfig*)conf;

    memset(&tccSpi, 0, sizeof(whQnxSpiClientContext));
    memset(&c_comm, 0, sizeof(whCommClientConfig));

    /* Default QNX SPI configuration */
    spiConfig.device_path = "/dev/io-spi/spi0/dev0";
    spiConfig.device_id   = 0;
    spiConfig.clock_rate  = 1000000;
    spiConfig.mode        = 0;
    spiConfig.loopback_mode = WH_QNX_SPI_LOOPBACK_NONE;

    c_comm.transport_cb      = &spiCb;
    c_comm.transport_context = (void*)&tccSpi;
    c_comm.transport_config  = (void*)&spiConfig;
    c_comm.client_id         = WH_QNX_CLIENT_ID;

    ((whClientConfig*)conf)->comm = &c_comm;

    return 0;
}
