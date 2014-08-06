/*--------------------------------------------------------------------------
 * UART_service.h
 *
 * Keil C51 header file for the Nordic VLSI nRF9E5 433-968MHz RF transceiver
 * with embedded 8051 compatible microcontroller.
 *
 * UART module<->master communication service header
 *
 *------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_SERVICE_H
#define __UART_SERVICE_H

/* Includes ------------------------------------------------------------------*/
#include "common_conf.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void uart_disable_receiver(void);
extern void uart_enable_receiver(void);

extern Bool is_uart_data_available(void);
extern void uart_data_extraction(swap_data *data_ptr);
extern FunctionState uart_data_transmission(swap_data *data_ptr);

/* External variables --------------------------------------------------------*/


#endif /* __UART_SERVICE_H */
/***************************** END OF FILE *****************************/
