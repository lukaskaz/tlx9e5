/****************************************Copyright (c)****************************************************
**                                      
**--------------File Info---------------------------------------------------------------------------------
** File name:               RF_service.c
** Descriptions:            RF service function to maintain Radio wireless transmissions beetwen nodes
**
**--------------------------------------------------------------------------------------------------------
** Created by:              LucasK
** Created date:            2012-05-03 
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             LukasK
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "RF_service.h"
#include <reg9e5.h>

#include "irq_service.h"
#include "SPI_service_internal.h"
#include "UART_service.h"
#include "MISC_services.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
    RADIO_RECEIVE = 0,
    RADIO_TRANSMIT,
    RADIO_CONFIGURE,
}radio_transmission;

typedef enum
{
    RADIO_RES_UNDEFINED = 0,
    RADIO_RES_ERROR,
    RADIO_RES_COMPLETE,
}radio_response;

typedef enum
{
    RADIO_ADDR_BASE = 0,
    RADIO_ADDR_NODE_1,
    RADIO_ADDR_NODE_MAX,
}radio_addresses;

/* Private define ------------------------------------------------------------*/
#define RADIO_MAX_RETRANSMISSIONS        10U
#define RADIO_MAX_RECEIVE_RETRIES        20U

#define RADIO_BYTES_FOR_CHECKSUM_CALC    (DATA_FRAME_SIZE - 2U)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if CONTROLLER
uint32_t radioNodeAddresses[] = {0xC0300C01, 0xC0300C03};
#elif REMOTE
uint32_t radioNodeAddresses[] = {0xC0300C03, 0xC0300C01};
#endif

/* Private function prototypes -----------------------------------------------*/
static void radio_enable_programming(void);
static FunctionState radio_service(const swap_data *const data_ptr, const radio_transmission mode);

static void radio_data_receive(const swap_data *const data_ptr);
static void radio_data_sent(const swap_data *const data_ptr);

static FunctionState radio_confirmation_receive(const swap_data *const data_ptr);
static void radio_confirmation_sent(const swap_data *const data_ptr);
static void radio_wait_transfer_done(void);
static void radio_wait_timeout_transfer_done(void);
static FunctionState radio_data_checksum_verify(swap_data *data_ptr);

/* Private functions ---------------------------------------------------------*/
Bool is_radio_data_available()
{
    return radio_data_ready;
}

FunctionState radio_configuration(const swap_data *const data_ptr)
{
    return radio_service(data_ptr, RADIO_CONFIGURE);
}

FunctionState radio_data_extraction(const swap_data *const data_ptr)
{
    if(radio_service(data_ptr, RADIO_RECEIVE) == SUCCESS &&
       uart_data_transmission(data_ptr) == SUCCESS        ) {
        return SUCCESS;
    }

    return FAIL;
}

FunctionState radio_data_transmission(const swap_data *const data_ptr)
{
    return radio_service(data_ptr, RADIO_TRANSMIT);
}

FunctionState radio_resolve_remote_address(uint8_t *const addrPtr, const uint8_t addrSize, const uint8_t addrId)
{
    if(addrId < RADIO_ADDR_NODE_MAX && addrSize == RADIO_ADDRESS_SIZE)
    {
        *(addrPtr+0) = (uint8_t)(radioNodeAddresses[addrId]>>0);
        *(addrPtr+1) = (uint8_t)(radioNodeAddresses[addrId]>>8);
        *(addrPtr+2) = (uint8_t)(radioNodeAddresses[addrId]>>16);
        *(addrPtr+3) = (uint8_t)(radioNodeAddresses[addrId]>>24);

        return SUCCESS;
    }

    return FAIL;
}

FunctionState radio_rev_resolve_remote_address(const uint8_t *const addrPtr, const uint8_t addrSize, uint8_t *addrId)
{
    if(addrSize == RADIO_ADDRESS_SIZE)
    {
        uint8_t i = 0;
        uint32_t remoteAddress = ((uint32_t)(*addrPtr)<<0)|((uint32_t)(*(addrPtr+1))<<8)|
                                 ((uint32_t)(*(addrPtr+2))<<16)|((uint32_t)(*(addrPtr+3))<<24);

        for(i=0; i<sizeof(radioNodeAddresses)/sizeof(radioNodeAddresses[0]); i++) {
            if(radioNodeAddresses[i] == remoteAddress) {
                *addrId = i;
                return SUCCESS;
            }
        }
    }

    return FAIL;
}

void radio_enable_transmitter()
{
    TXEN   = SET;
    TRX_CE = SET;
}

void radio_enable_receiver()
{
    TXEN   = RESET;
    TRX_CE = SET;
}

void radio_enable_programming()
{
    TXEN   = SET;
    TRX_CE = RESET;
}

void radio_disable()
{
    TXEN   = RESET;
    TRX_CE = RESET;
}
/*******************************************************************************
* Function Name  : RF_Service
* Description    : Routine handles in/out RF transfers to/from outer RF nodes
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
FunctionState radio_service(const swap_data *const data_ptr, const radio_transmission mode)
{
    FunctionState return_status = FAIL;

    if(mode == RADIO_RECEIVE)
    {
        radio_data_receive(data_ptr);
        radio_confirmation_sent(data_ptr);
        radio_disable();

        data_ptr->data_kind = DATA_RADIO_OUTPUT;
        return_status = SUCCESS;
    }
    else if(mode == RADIO_TRANSMIT)
    {
        uint8_t transmission_counter = 0;

        if(data_ptr->data_kind == DATA_RADIO_INPUT)
        {
            swap_data data_temp = {0};

            while(transmission_counter < RADIO_MAX_RETRANSMISSIONS)
            {
                data_temp = *data_ptr;

                radio_data_sent(&data_temp);
                if(radio_confirmation_receive(&data_temp) == SUCCESS)
                {
                    data_ptr->data_kind = DATA_RELEASED;
                    return_status = SUCCESS;
                    break;
                }
                else
                {
                    transmission_counter++;
                }
            }

            radio_disable();
        }
    }
    else if(mode == RADIO_CONFIGURE)
    {
        radio_enable_programming();
        radio_resolve_remote_address(data_ptr->radio_config.rxaddr, RADIO_ADDRESS_SIZE, RADIO_ADDR_BASE);
        spi_data_transmission(data_ptr);

        data_ptr->data_kind = DATA_RELEASED;
        return_status = SUCCESS;
    }
    else
    {
        //undefined action
    }

    return return_status;
}


void radio_data_receive(const swap_data *const data_ptr)
{
    uint8_t Carrier_presence = 0;

    radio_enable_receiver();
    radio_wait_transfer_done();

    do{
        Carrier_presence = CD;
    }while(Carrier_presence);

    data_ptr->data_kind = DATA_R_RF_RXPAYLOAD;
    spi_data_extraction(data_ptr);
}

void radio_data_sent(const swap_data *const data_ptr)
{
    uint8_t Carrier_presence = 0;

    radio_enable_receiver();
    do{
        Carrier_presence = CD;
    }while(Carrier_presence);

    radio_enable_programming();

    data_ptr->data_kind = DATA_W_RF_TXADDR;
    spi_data_transmission(data_ptr);

    data_ptr->data_kind = DATA_W_RF_TXPAYLOAD;
    radio_resolve_remote_address(data_ptr->radio_payload.base_addr, RADIO_ADDRESS_SIZE, RADIO_ADDR_BASE);
    data_ptr->radio_payload.checksum = data_checksum_calculate(data_ptr, RADIO_BYTES_FOR_CHECKSUM_CALC);
    spi_data_transmission(data_ptr);

    radio_enable_transmitter();
    radio_wait_transfer_done();
}

FunctionState radio_confirmation_receive(const swap_data *const data_ptr)
{
    data_ptr->radio_payload.response = RADIO_RES_UNDEFINED;

    radio_enable_receiver();
    radio_wait_timeout_transfer_done();

    data_ptr->data_kind = DATA_R_RF_RXPAYLOAD;
    spi_data_extraction(data_ptr);

    if(data_ptr->radio_payload.response == RADIO_RES_COMPLETE)
    {
        return SUCCESS;
    }

    return FAIL;
}

void radio_confirmation_sent(const swap_data *const data_ptr)
{
    radio_enable_programming();

    data_ptr->data_kind = DATA_W_RF_TXADDR;
    spi_data_transmission(data_ptr);

    data_ptr->data_kind = DATA_W_RF_TXPAYLOAD;
    //radio_resolve_remote_address(data_ptr->radio_payload.base_addr, DATA_ADDRESS_SIZE, RADIO_ADDR_BASE);

    if(radio_data_checksum_verify(data_ptr) != SUCCESS) {
        data_ptr->radio_payload.response = RADIO_RES_ERROR;
    }
    else {
        data_ptr->radio_payload.response = RADIO_RES_COMPLETE;
    }
    spi_data_transmission(data_ptr);

    radio_enable_transmitter();
    radio_wait_transfer_done();
}

void radio_wait_transfer_done()
{
    while(radio_data_ready != TRUE);
    radio_data_ready = FALSE;
}

void radio_wait_timeout_transfer_done()
{
    uint8_t i = 0;

    while(radio_data_ready != TRUE && i < RADIO_MAX_RECEIVE_RETRIES){
        delay_ms(1);
        i++;
    }
    radio_data_ready = FALSE;
}

FunctionState radio_data_checksum_verify(const swap_data *const data_ptr)
{
    if( data_ptr->radio_payload.checksum == data_checksum_calculate(data_ptr, RADIO_BYTES_FOR_CHECKSUM_CALC) ) {
        return SUCCESS;
    }

    return FAIL;
}


/***************************** END OF FILE *****************************/
