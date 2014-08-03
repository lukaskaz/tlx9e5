/*--------------------------------------------------------------------------
 * MISC_services.h
 *
 * Keil C51 header file for the Nordic VLSI nRF9E5 433-968MHz RF transceiver
 * with embedded 8051 compatible microcontroller.
 *
 * Miscellaneous services header
 *
 *------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MISC_SERVICES_H
#define __MISC_SERVICES_H

/* Includes ------------------------------------------------------------------*/
#include "common_conf.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void set_timeout_ms(const uint16_t delay);
extern void delay_ms(const uint16_t delay);
extern Bool is_timeout_over(void);
extern uint8_t data_checksum_calculate(const swap_data *const data_ptr, uint8_t bytesForChecksum);

/* External variables --------------------------------------------------------*/


#endif /* __MISC_SERVICES_H */
/***************************** END OF FILE *****************************/
