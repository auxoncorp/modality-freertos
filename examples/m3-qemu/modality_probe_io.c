#include "FreeRTOS.h"

#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

#include "modality_probe_io.h"

static Socket_t xSenderSocket = FREERTOS_INVALID_SOCKET;
static struct freertos_sockaddr xSenderAddress = {0};

static Socket_t xReceiverSocket = FREERTOS_INVALID_SOCKET;
static struct freertos_sockaddr xReceiverAddress = {0};

static int32_t init_sender(void)
{
    int32_t ret = 0;

    if(xSenderAddress.sin_port == 0)
    {
        xSenderAddress.sin_port = FreeRTOS_htons(configCOLLECTOR_PORT);
        xSenderAddress.sin_addr = FreeRTOS_inet_addr_quick(
                configCOLLECTOR_SERVER_ADDR0,
                configCOLLECTOR_SERVER_ADDR1,
                configCOLLECTOR_SERVER_ADDR2,
                configCOLLECTOR_SERVER_ADDR3);
    }

    if(xSenderSocket == FREERTOS_INVALID_SOCKET)
    {
        xSenderSocket = FreeRTOS_socket(
                FREERTOS_AF_INET,
                FREERTOS_SOCK_DGRAM,
                FREERTOS_IPPROTO_UDP);
        if(xSenderSocket == FREERTOS_INVALID_SOCKET)
        {
            FreeRTOS_debug_printf( ( "Failed to create Modality trace IO sender socket\n" ) );
            ret = -1;
        }
        else
        {
            FreeRTOS_debug_printf( ( "Created Modality trace IO sender socket\n" ) );
        }
    }

    return ret;
}

static int32_t init_recvr(void)
{
    int32_t ret = 0;
    BaseType_t status;
    const TickType_t timeout = 0;

    if(xReceiverAddress.sin_port == 0)
    {
        xReceiverAddress.sin_port = FreeRTOS_htons(configCONTROL_PLANE_PORT);
        xReceiverAddress.sin_addr = FreeRTOS_inet_addr_quick(
                configIP_ADDR0,
                configIP_ADDR1,
                configIP_ADDR2,
                configIP_ADDR3);
    }

    if(xReceiverSocket == FREERTOS_INVALID_SOCKET)
    {
        xReceiverSocket = FreeRTOS_socket(
                FREERTOS_AF_INET,
                FREERTOS_SOCK_DGRAM,
                FREERTOS_IPPROTO_UDP);
        if(xReceiverSocket == FREERTOS_INVALID_SOCKET)
        {
            FreeRTOS_debug_printf( ( "Failed to create Modality trace IO receiver socket\n" ) );
            ret = -1;
        }
        else
        {
            FreeRTOS_debug_printf( ( "Created Modality trace IO receiver socket\n" ) );

            status = FreeRTOS_setsockopt(xReceiverSocket, 0, FREERTOS_SO_RCVTIMEO, &timeout, 0);
            configASSERT(status == 0);

            status = FreeRTOS_bind(xReceiverSocket, &xReceiverAddress, sizeof(xReceiverAddress));
            configASSERT(status == 0);
        }
    }

    return ret;
}

int32_t trace_io_read(void* data, uint32_t size, int32_t* bytes_read)
{
    int32_t ret = 0;
    struct freertos_sockaddr client;
    socklen_t client_size;

    ret = init_recvr();
    configASSERT(ret == 0);

    configASSERT(data != NULL);
    configASSERT(size != 0);
    configASSERT(bytes_read != NULL);

    if(xReceiverSocket != FREERTOS_INVALID_SOCKET)
    {
        ret = FreeRTOS_recvfrom(xReceiverSocket, data, (size_t) size, 0, &client, &client_size);

        if(ret == FREERTOS_SOCKET_ERROR)
        {
            ret = -1;
        }
        else
        {
            *bytes_read = ret;
            ret = 0;
        }
    }

    return ret;
}

int32_t trace_io_write(void* data, uint32_t size, int32_t* bytes_written)
{
    int32_t ret = 0;

    configASSERT(data != NULL);
    configASSERT(size != 0);
    configASSERT(bytes_written != NULL);

    ret = init_sender();
    configASSERT(ret == 0);

    if(xSenderSocket != FREERTOS_INVALID_SOCKET)
    {
        ret = FreeRTOS_sendto(xSenderSocket, data, size, 0, &xSenderAddress, sizeof(xSenderAddress));
        if(ret < 0)
        {
            ret = -1;
        }
        else
        {
            *bytes_written = ret;
            ret = 0;
        }
    }

    return ret;
}
