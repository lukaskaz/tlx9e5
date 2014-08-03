/*--------------------------------------------------------------------------
 * IRQ_service.h
 *
 * Keil C51 header file for the Nordic VLSI nRF9E5 433-968MHz RF transceiver
 * with embedded 8051 compatible microcontroller.
 *
 * IRQ service header
 *
 *------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IRQ_SERVICE_H
#define __IRQ_SERVICE_H

/* Includes ------------------------------------------------------------------*/
#include "common_conf.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* External variables --------------------------------------------------------*/
extern volatile uint32_t run_time_ms;
extern volatile uint16_t sys_tick_ms;

extern volatile uint8_t  uart_data_sent;
extern volatile uint8_t  uart_data_received;
extern volatile uint8_t  spi_data_ready;
extern volatile uint8_t  radio_data_ready;

#endif /* __IRQ_SERVICE_H */
/***************************** END OF FILE *****************************/
