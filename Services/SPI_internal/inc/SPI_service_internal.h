/*--------------------------------------------------------------------------
 * SPI_service_internal.h
 *
 * Keil C51 header file for the Nordic VLSI nRF9E5 433-968MHz RF transceiver
 * with embedded 8051 compatible microcontroller.
 *
 * SPI service header
 *
 *------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_INTERN_H
#define __SPI_INTERN_H

/* Includes ------------------------------------------------------------------*/
#include "common_conf.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern FunctionState spi_data_extraction(const swap_data *const data_ptr);
extern FunctionState spi_data_transmission(const swap_data *const data_ptr);

/* External variables --------------------------------------------------------*/


#endif /* __SPI_INTERN_H */
/***************************** END OF FILE *****************************/
