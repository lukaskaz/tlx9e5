/****************************************Copyright (c)****************************************************
**                                      
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            Main function for transciever prior loop
**
**--------------------------------------------------------------------------------------------------------
** Created by:              LucasK
** Created date:            2012-04-18
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
#include <reg9e5.h>

#include "global_init.h"
#include "RF_service.h"
#include "UART_service.h"
/* Defined constatnts --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* External variables --------------------------------------------------------*/

/*******************************************************************************
* Function Name  : main
* Description    : Main loop
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
int main(void)
{
    swap_data data_buffer = {0};
    Bool waitOpProcessing = FALSE;

    global_init(&data_buffer);
    while(1)
    {
        if(waitOpProcessing != TRUE) {
            waitOpProcessing = TRUE;
            uart_enable_receiver();
            radio_enable_receiver();
        }
        if(is_radio_data_available() == TRUE) {
            waitOpProcessing = FALSE;
            uart_disable_receiver();
            radio_data_extraction(&data_buffer);
        }
        if(is_uart_data_available() == TRUE) {
            waitOpProcessing = FALSE;
            radio_disable();
            uart_data_extraction(&data_buffer);
        }
    }

    return 0;
}


/*********************************************************************************************************
      END OF FILE
*********************************************************************************************************/
