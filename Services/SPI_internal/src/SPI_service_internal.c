/****************************************Copyright (c)****************************************************
**                                      
**--------------File Info---------------------------------------------------------------------------------
** File name:               SPI_service_internal.c
** Descriptions:            SPI service function to maintain SPI on-chip transfers with RF/ADC moduls
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
#include "SPI_service_internal.h"
#include <reg9e5.h>

#include "irq_service.h"
/* Private typedef -----------------------------------------------------------*/
typedef enum
{
    SPI_STATE_DISABLED           = 0,
    SPI_STATE_ENABLED_EEPROM     = 1,
    SPI_STATE_ENABLED_RADIO_ADC  = 2,
}spi_state;

typedef enum
{
    SPI_READ = 0,
    SPI_WRITE,
}spi_transmission;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const uint8_t SPI_command[] = {STA, WRC, RRC, WTA, RTA, WTP, RTP, RRP};
const uint8_t SPI_transmission_len[] =
    {STA_LEN, WRC_LEN, RRC_LEN, WTA_LEN, RTA_LEN, WTP_LEN, RTP_LEN, RRP_LEN};

/* Private function prototypes -----------------------------------------------*/
static void spi_enable_for_radio(void);
static void spi_disable(void);
static void spi_wait_transfer_done(void);

static FunctionState spi_service(const swap_data *const data_ptr, const spi_transmission mode);

/* Private functions ---------------------------------------------------------*/
FunctionState spi_data_extraction(const swap_data *const data_ptr)
{
    return spi_service(data_ptr, SPI_READ);
}

FunctionState spi_data_transmission(const swap_data *const data_ptr)
{
    return spi_service(data_ptr, SPI_WRITE);
}

/*******************************************************************************
* Function Name  : SPI_Service
* Description    : Routine handles in/out SPI transfers between RF/ADC modules
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
FunctionState spi_service(const swap_data *const data_ptr, const spi_transmission mode)
{
    FunctionState return_status = FAIL;
    uint8_t *data_input = &data_ptr->iodata.byte[0];
    int8_t transmission_length = SPI_transmission_len[data_ptr->data_kind];

    spi_enable_for_radio();

    SPI_DATA = SPI_command[data_ptr->data_kind];
    spi_wait_transfer_done();

    if(mode == SPI_READ)
    {
        if(transmission_length == 0) {
            *data_input = SPI_DATA;
        }
        else {
            while(transmission_length-- > 0)
            {
                SPI_DATA = 0x00;
                spi_wait_transfer_done();
                *data_input++ = SPI_DATA;
            }
        }

        return_status = SUCCESS;
    }
    else if(mode == SPI_WRITE)
    {
        while(transmission_length-- > 0)
        {
            SPI_DATA = *data_input++;
            spi_wait_transfer_done();
        }

        return_status = SUCCESS;
    }
    else
    {
        //undefined action
    }

    spi_disable();
    return return_status;
}

void spi_enable_for_radio()
{
    SPI_CTRL = SPI_STATE_ENABLED_RADIO_ADC;
    RACSN = RESET;
}

void spi_disable()
{
    RACSN = SET;
    SPI_CTRL = SPI_STATE_DISABLED;
}

void spi_wait_transfer_done()
{
    while(spi_data_ready != TRUE);
    spi_data_ready = FALSE;
}


/***************************** END OF FILE *****************************/
