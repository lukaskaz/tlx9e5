/****************************************Copyright (c)****************************************************
**                                      
**--------------File Info---------------------------------------------------------------------------------
** File name:               MISC_service.c
** Descriptions:            Auxiliary services for support regular module functionalities
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
#include "MISC_services.h"
#include <reg9e5.h>

#include "IRQ_service.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : set_timeout_ms
* Description    : Routine allows to set timout limit determined in microseconds
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void set_timeout_ms(const uint16_t delay)
{
    ET0  = 0;
    sys_tick_ms = delay;
    ET0  = 1;
}

/*******************************************************************************
* Function Name  : is_timeout_over
* Description    : Routine allows to get information if timeout is pending
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
Bool is_timeout_over()
{
    if(sys_tick_ms) {
        return FALSE;
    }
    return TRUE;
}

/*******************************************************************************
* Function Name  : delay_ms
* Description    : Routine provides delay determined in microsecond base
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void delay_ms(const uint16_t delay)
{
    set_timeout_ms(delay);
    while(sys_tick_ms);
}

/*******************************************************************************
* Function Name  : data_checksum_calculate
* Description    : Routine provides checksum calculating algorithm
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
uint8_t data_checksum_calculate(const swap_data *const data_ptr, uint8_t bytesForChecksum)
{
    uint8_t checksum = 0;

    do {
        checksum += data_ptr->iodata.byte[--bytesForChecksum];
    } while( bytesForChecksum != 0);

    checksum = (uint8_t)((checksum^0xFF) + 1);

    return checksum;
}


/***************************** END OF FILE *****************************/
