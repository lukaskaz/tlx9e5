/****************************************Copyright (c)****************************************************
**                                      
**--------------File Info---------------------------------------------------------------------------------
** File name:               IRQ_service.c
** Descriptions:            IRQ service routines with 'on IRQ event' access and invoked based on
                            interrupt vector (code memory pool in range 0x0000 to 0x006B has to be 
                            considered during program and xdata allocation)
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
#include "IRQ_service.h"
#include <reg9e5.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint32_t run_time_ms = 0;
volatile uint16_t sys_tick_ms = 0;
volatile uint8_t  uart_data_sent     = FALSE;
volatile uint8_t  uart_data_received = FALSE;
volatile uint8_t  spi_data_ready     = FALSE;
volatile uint8_t  radio_data_ready   = FALSE;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : Ext0_ISR
* Description    : Routine maintain External0 interrupt event
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void Ext0_ISR(void) interrupt 0 using 1
{

}


/*******************************************************************************
* Function Name  : Timer0_ISR
* Description    : Routine maintain Timer0 interrupt event
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void Timer0_ISR(void) interrupt 1 using 1
{
    TL0 = (uint8_t)(TIME_BASE_INTERVAL>>0);
    TH0 = (uint8_t)(TIME_BASE_INTERVAL>>8);

    run_time_ms += 1;
    if(sys_tick_ms) {
        sys_tick_ms -= 1;
    }
}


/*******************************************************************************
* Function Name  : Ext1_ISR
* Description    : Routine maintain External1 interrupt event
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void Ext1_ISR(void) interrupt 2 using 1
{

}


/*******************************************************************************
* Function Name  : Timer1_ISR
* Description    : Routine maintain Timer1 interrupt event
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void Timer1_ISR(void) interrupt 3 using 1
{

}


/*******************************************************************************
* Function Name  : Serial_ISR
* Description    : Routine maintain UART communication interrupt event(RI/TI)
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void Serial_ISR(void) interrupt 4 using 1
{
    if(TI != 0) {
        TI = 0;
        uart_data_sent = TRUE;
    }
    if(RI != 0) {
        RI = 0;
        uart_data_received = TRUE;
    }
}


/*******************************************************************************
* Function Name  : Timer2_ISR
* Description    : Routine maintain Timer2 interrupt event
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void Timer2_ISR(void) interrupt 5 using 1
{

}


/*******************************************************************************
* Function Name  : SPI_Data_Ready_ISR
* Description    : Routine maintain SPI interrupt event
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void SPI_Data_Ready_ISR(void) interrupt 9 using 1
{
    EXIF &= ~SPI_READY;
    spi_data_ready = TRUE;
}


/*******************************************************************************
* Function Name  : Radio_Data_Ready_ISR
* Description    : Routine maintain radio DR interrupt event
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void Radio_Data_Ready_ISR(void) interrupt 10 using 1
{
    EXIF &= ~RF_DATA_READY;
    radio_data_ready = TRUE;
}


/***************************** END OF FILE *****************************/
