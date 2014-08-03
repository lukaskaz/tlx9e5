/*--------------------------------------------------------------------------
 * common_conf.h
 *
 * Keil C51 header file for the Nordic VLSI nRF9E5 433-968MHz RF transceiver
 * with embedded 8051 compatible microcontroller.
 *
 * Common data header
 *
 *------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMMON_CONF_H
#define __COMMON_CONF_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>


/* Exported constants --------------------------------------------------------*/
#define CONTROLLER              0
#define REMOTE                  1

#define RADIO_ADDRESS_SIZE      4U
#define DATA_PAYLOAD_SIZE       10U
#define DATA_FRAME_SIZE         (DATA_PAYLOAD_SIZE + 6U)
#define TIME_BASE_INTERVAL      ( (uint16_t)(-3990) )

/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef enum
{
    MOD_FUNC_UNDEFINED = 0,
    MOD_FUNC_RADIO_SEND,
    MOD_FUNC_RADIO_RECEIVE,
}module_function;

typedef enum
{
    DATA_R_STATUS,
    DATA_W_RF_CONFIG,
    DATA_R_RF_CONFIG,
    DATA_W_RF_TXADDR,
    DATA_R_RF_TXADDR,
    DATA_W_RF_TXPAYLOAD,
    DATA_R_RF_TXPAYLOAD,
    DATA_R_RF_RXPAYLOAD,

    DATA_UART_INPUT,
    DATA_RADIO_INPUT,
    DATA_UART_OUTPUT,
    DATA_RADIO_OUTPUT,
    DATA_RELEASED,
}DATA_Type;

typedef union
{
    DATA_Type data_kind;
    struct
    {
        DATA_Type data_kind;
        uint8_t byte[DATA_FRAME_SIZE];
    }iodata;

    struct
    {
        DATA_Type data_kind;
        uint8_t status;
    }radio_status;

    struct
    {
        DATA_Type data_kind;
        uint8_t ch_no;
        uint8_t freq_pll;
        uint8_t rxtx_afw;
        uint8_t rx_pw;
        uint8_t tx_pw;
        uint8_t rxaddr[RADIO_ADDRESS_SIZE];
        uint8_t crc_cpu;
    }radio_config;
     
    struct
    {
        DATA_Type data_kind;
        uint8_t txaddr[RADIO_ADDRESS_SIZE];
    }radio_txaddress;

    struct
    {
        DATA_Type data_kind;
        uint8_t base_addr[RADIO_ADDRESS_SIZE];
        uint8_t payload[DATA_PAYLOAD_SIZE];
        uint8_t checksum;
        uint8_t response;
    }radio_payload;

    struct
    {
        DATA_Type data_kind;
        uint8_t uart_sender_addr;
        uint8_t function;
        uint8_t radio_target_addr;
        uint8_t payload_bytes_nb;
        uint8_t payload[DATA_PAYLOAD_SIZE];
        uint8_t checksum;
        uint8_t response;
    }uart_payload;
}swap_data;


/* Exported functions ------------------------------------------------------- */
/* External variables --------------------------------------------------------*/


#endif /* __COMMON_CONF_H */
/***************************** END OF FILE *****************************/
