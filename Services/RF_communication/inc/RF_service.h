/*--------------------------------------------------------------------------
 * RF_service.h
 *
 * Keil C51 header file for the Nordic VLSI nRF9E5 433-968MHz RF transceiver
 * with embedded 8051 compatible microcontroller.
 *
 * Radio communication service header
 *
 *------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RF_SERVICE_H
#define __RF_SERVICE_H

/* Includes ------------------------------------------------------------------*/
#include "common_conf.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void radio_disable(void);
extern void radio_enable_receiver(void);
extern void radio_enable_transmitter(void);
extern Bool is_radio_data_available(void);

extern FunctionState radio_configuration(const swap_data *const data_ptr);
extern FunctionState radio_data_extraction(const swap_data *const data_ptr);
extern FunctionState radio_data_transmission(const swap_data *const data_ptr);

extern FunctionState radio_resolve_remote_address(uint8_t *const addrPtr, const uint8_t addrSize, const uint8_t addrId);
extern FunctionState radio_rev_resolve_remote_address(const uint8_t *const addrPtr, const uint8_t addrSize, uint8_t *addrId);

/* External variables --------------------------------------------------------*/


#endif /* __RF_SERVICE_H */
/***************************** END OF FILE *****************************/
