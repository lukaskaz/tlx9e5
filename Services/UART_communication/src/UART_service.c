/****************************************Copyright (c)****************************************************
**                                      
**--------------File Info---------------------------------------------------------------------------------
** File name:               UART_service.c
** Descriptions:            UART service functions to maintain UART transmissions beetwen
**                          this module and master CPU
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
#include "UART_service.h"
#include <reg9e5.h>

#include "irq_service.h"
#include "RF_service.h"
#include "MISC_services.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
    UART_TRANSMIT = 0,
    UART_RESPONSE,
}uart_transmission;

typedef enum
{
    UART_FRAME_NORMAL = 0,
    UART_FRAME_RESPONSE,
    UART_FRAME_OTHER,
}uart_frame_type;

typedef enum
{
    UART_RES_TRANSFER_ERROR = 0,
    UART_RES_OPERATION_ERROR,
    UART_RES_OPERATION_COMPLETE,
    UART_RES_OPERATION_UNSUPPORTED,
}uart_response;

/* Private define ------------------------------------------------------------*/
#define UART_BYTES_FOR_TRANSMIT          (DATA_FRAME_SIZE - 1U)
#define UART_BYTES_FOR_RECEPTION         (DATA_FRAME_SIZE - 1U)
#define UART_BYTES_FOR_CHECKSUM_CALC     (UART_BYTES_FOR_TRANSMIT - 1U)

#define UART_BASE_ADDRESS                10U
#define UART_MASTER_ADDRESS              5U
#define UART_END_TX_SIGNATURE            255U
#define UART_FRAME_RECEPTION_TIMEOUT_MS  10U

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static FunctionState uart_data_response(const swap_data *const data_ptr);
static FunctionState uart_service(const swap_data *const data_ptr, const uart_transmission mode);

static void uart_data_send(const uint8_t txData);
static uint8_t uart_data_receive();

static FunctionState uart_data_checksum_verify(const swap_data *const data_ptr);

/* Private functions ---------------------------------------------------------*/
void uart_disable_receiver()
{
    REN = 0;
}

void uart_enable_receiver()
{
    REN = 1;
}

Bool is_uart_data_available()
{
    return uart_data_received;
}

FunctionState uart_data_transmission(swap_data *data_ptr)
{
    if(data_ptr->uart_payload.data_kind == DATA_RADIO_OUTPUT) {
        uint8_t radioAddressSignature = -1;

        radio_rev_resolve_remote_address(data_ptr->radio_payload.base_addr, RADIO_ADDRESS_SIZE, &radioAddressSignature);

        data_ptr->uart_payload.uart_sender_addr  = UART_BASE_ADDRESS;
        data_ptr->uart_payload.function          = MOD_FUNC_RADIO_RECEIVE;
        data_ptr->uart_payload.radio_target_addr = radioAddressSignature;
        data_ptr->uart_payload.payload_bytes_nb  = DATA_PAYLOAD_SIZE;
        data_ptr->uart_payload.checksum          = data_checksum_calculate(data_ptr, UART_BYTES_FOR_CHECKSUM_CALC);

        return uart_service(data_ptr, UART_TRANSMIT);
    }

    return FAIL;
}

FunctionState uart_data_extraction(swap_data *data_ptr)
{
    FunctionState return_status = FAIL;

    set_timeout_ms(UART_FRAME_RECEPTION_TIMEOUT_MS);
    if(uart_data_receive() == UART_BASE_ADDRESS)
    {
        uint8_t i = 0, frame_type = UART_FRAME_NORMAL;

        RB8 = 0;
        SM2 = 0;
        if(uart_data_receive() == UART_FRAME_NORMAL) {
            for(i=0; RB8==0 && i<UART_BYTES_FOR_RECEPTION && is_timeout_over() == FALSE; i++) {
                data_ptr->iodata.byte[i] = uart_data_receive();
            }
        }
        else {
            frame_type = UART_FRAME_OTHER;
        }
        SM2 = 1;

        uart_disable_receiver();

        if(frame_type == UART_FRAME_NORMAL) {
            //processing normal frame, check if frame content is valid
            if(uart_data_checksum_verify(data_ptr) == SUCCESS) {
                //process frame defined function
                if(data_ptr->uart_payload.function == MOD_FUNC_RADIO_SEND) {
                    radio_resolve_remote_address(data_ptr->radio_txaddress.txaddr, RADIO_ADDRESS_SIZE, data_ptr->uart_payload.radio_target_addr);

                    data_ptr->data_kind = DATA_RADIO_INPUT;
                    if(radio_data_transmission(data_ptr) == SUCCESS) {
                        data_ptr->uart_payload.response = UART_RES_OPERATION_COMPLETE;
                        return_status = SUCCESS;
                    }
                    else {
                        //radio transmission process failed
                        data_ptr->uart_payload.response = UART_RES_OPERATION_ERROR;
                    }
                }
                else {
                    //requested function is not supported yet
                    data_ptr->uart_payload.response = UART_RES_OPERATION_UNSUPPORTED;
                }
            }
            else {
                //checksum validation failed
                data_ptr->uart_payload.response = UART_RES_TRANSFER_ERROR;
            }
        }
        else {
            //frame type is not supported yet
            data_ptr->uart_payload.response = UART_RES_OPERATION_UNSUPPORTED;
        }

        uart_data_response(data_ptr);
    }

    return return_status;
}

FunctionState uart_data_response(const swap_data *const data_ptr)
{
    return uart_service(data_ptr, UART_RESPONSE);
}

FunctionState uart_service(const swap_data *const data_ptr, const uart_transmission mode)
{
    FunctionState return_status = FAIL;

    TB8  = 1;
    uart_data_send(UART_MASTER_ADDRESS);
    TB8  = 0;

    if(mode == UART_TRANSMIT)
    {
        uint8_t i = 0;

        uart_data_send(UART_FRAME_NORMAL);
        for(i=0; i<UART_BYTES_FOR_TRANSMIT; i++) {
            uart_data_send(data_ptr->iodata.byte[i]);
        }

        return_status = SUCCESS;
    }
    else if(mode == UART_RESPONSE)
    {
        uart_data_send(UART_FRAME_RESPONSE);
        uart_data_send(data_ptr->uart_payload.response);

        return_status = SUCCESS;
    }
    else {
        //no action defined
    }

    TB8  = 1;
    uart_data_send(UART_END_TX_SIGNATURE);
    TB8  = 0;

    return return_status;
}

FunctionState uart_data_checksum_verify(const swap_data *const data_ptr)
{
    if( data_ptr->uart_payload.checksum == data_checksum_calculate(data_ptr, UART_BYTES_FOR_CHECKSUM_CALC) ) {
        return SUCCESS;
    }

    return FAIL;
}

uint8_t uart_data_receive()
{
    uint8_t rxData = 0;

    while(uart_data_received != TRUE && is_timeout_over() != TRUE);
    uart_data_received = FALSE;
    rxData = SBUF;

    return rxData;
}

void uart_data_send(const uint8_t txData)
{
    SBUF = txData;
    uart_data_sent = FALSE;
    while(uart_data_sent != TRUE);
}


/***************************** END OF FILE *****************************/
