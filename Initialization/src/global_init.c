/****************************************Copyright (c)****************************************************
**                                      
**--------------File Info---------------------------------------------------------------------------------
** File name:               global_init.c
** Descriptions:            Set of functions initializing separate modules
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
#include "global_init.h"
#include <reg9e5.h>

#include "SPI_service_internal.h"
#include "RF_service.h"
#include "UART_service.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
    RF_PWR_OUTPUT_MINUS10dB  = 0<<2,
    RF_PWR_OUTPUT_MINUS02dB  = 1<<2,
    RF_PWR_OUTPUT_PLUS06dB   = 2<<2,
    RF_PWR_OUTPUT_PLUS10dB   = 3<<2,
}RF_PWR_OUTPUT_Selection;

typedef enum{
    RF_RXADDR_WIDTH_UNDEF    = 0<<0,
    RF_RXADDR_WIDTH_1BYTE    = 1<<0,
    RF_RXADDR_WIDTH_2BYTE    = 2<<0,
    RF_RXADDR_WIDTH_3BYTE    = 3<<0,
    RF_RXADDR_WIDTH_4BYTE    = 4<<0,
}RF_RXADDR_WIDTH_Selection;

typedef enum
{
    RF_TXADDR_WIDTH_UNDEF    = 0,
    RF_TXADDR_WIDTH_1BYTE    = 1<<4,
    RF_TXADDR_WIDTH_2BYTE    = 2<<4,
    RF_TXADDR_WIDTH_3BYTE    = 3<<4,
    RF_TXADDR_WIDTH_4BYTE    = 4<<4,
}RF_TXADDR_WIDTH_Selection;

typedef enum
{
    RF_PLL_BAND_433MHz       = 0<<1,
    RF_PLL_BAND_868MHz       = 1<<1,
}RF_PLL_BAND_Selection;

typedef enum
{
    RF_CRC_MODE_8BIT         = 0<<7,
    RF_CRC_MODE_16BIT        = 1<<7,
}RF_CRC_MODE_Selection;

typedef enum
{
    SPI_CLK_UNDEF            = 0,
    SPI_CLK_CPU_1_2          = 1,
    SPI_CLK_CPU_1_4          = 2,
    SPI_CLK_CPU_1_8          = 3,
    SPI_CLK_CPU_1_16         = 4,
    SPI_CLK_CPU_1_32         = 5,
    SPI_CLK_CPU_1_64         = 6,
}SPI_Clock;

typedef enum
{
    CPU_PRESCOSCILLATOR      = 0<<2,
    CPU_EXTOSCILLATOR        = 1<<2,
}CPU_OSCILLATOR_Selection;

typedef enum
{
    CPU_EXTOSCILLATOR_FREQ_4MHz      = 0<<3,
    CPU_EXTOSCILLATOR_FREQ_8MHz      = 1<<3,
    CPU_EXTOSCILLATOR_FREQ_12MHz     = 2<<3,
    CPU_EXTOSCILLATOR_FREQ_16MHz     = 3<<3,
    CPU_EXTOSCILLATOR_FREQ_20MHz     = 4<<3,
}CPU_EXTOSCILLATOR_FREQ_Selection;

typedef enum
{
    CPU_PRESCOSCILLATOR_FREQ_4MHz    = 0<<0,
    CPU_PRESCOSCILLATOR_FREQ_2MHz    = 1<<0,
    CPU_PRESCOSCILLATOR_FREQ_1MHz    = 2<<0,
    CPU_PRESCOSCILLATOR_FREQ_500kHz  = 3<<0,
}CPU_PRESCOSCILLATOR_FREQ_Selection;


/* Private define ------------------------------------------------------------*/
//configuration bytes
    //bytes [5-8]
#define RF_BASE_ADDRESS         RF_BASE_NODE_ADDRESS

    //byte 0
#define RF_CHANNEL_NO           267U
#define RF_CHANNEL_NO_LOW       ((uint8_t)RF_CHANNEL_NO)

    //byte 1
#define RF_AUTO_RETRAN          (1<<5)
     
#define RF_RX_RED_PWR           (1<<4)
#define RF_PA_PWR               RF_PWR_OUTPUT_PLUS10dB
 
#define RF_HFREQ_PLL            RF_PLL_BAND_868MHz
#define RF_CHANNEL_NO_HIGH      ((uint8_t)(RF_CHANNEL_NO>>8))

    //byte 2
#define RF_RXADDR_WIDTH         RF_RXADDR_WIDTH_4BYTE
#define RF_TXADDR_WIDTH         RF_TXADDR_WIDTH_4BYTE

    //byte 3
#define RF_RXPAYLOAD_WIDTH      RRP_LEN

    //byte 4
#define RF_TXPAYLOAD_WIDTH      WTP_LEN

    //byte 9
#define RF_CRC_MODE             RF_CRC_MODE_16BIT
#define RF_CRC_EN               (1<<6)
#define CPU_XOF                 CPU_EXTOSCILLATOR_FREQ_16MHz  //value must be fit excatly to external osc freq
#define CPU_XO_DIRECT           CPU_EXTOSCILLATOR
#define CPU_UP_CLK_FREQ         CPU_PRESCOSCILLATOR_FREQ_4MHz


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void CPU_initialization(void);
static void SPI_initialization(void);
static void RF_initialization(swap_data *data_ptr);

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Global initialization
* Description    : Included functions are to be initializing separate modules
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void global_init(swap_data *data_ptr)
{
    CPU_initialization();
    SPI_initialization();
    RF_initialization(data_ptr);

    uart_enable_receiver();
    radio_enable_receiver();
}


static void CPU_initialization(void)
{
    TL0 = (uint8_t)(TIME_BASE_INTERVAL>>0);
    TH0 = (uint8_t)(TIME_BASE_INTERVAL>>8);

    TMOD = (0<<3)|(0<<2)|(0<<1)|(1<<0);
    PT0 = 1;
    TF0 = 0;
    ET0 = 1;
    TR0 = 1;

    T2CON = (0<<7)|(0<<6)|(1<<5)|(1<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0);
    RCAP2L = 0xFC;      // baud rate set to 125000bd
    RCAP2H = 0xFF;      // br = 16000000/(32 * (0x10000 - 0xFFFC))
    TR2 = 1;

    P0_DIR |= 0x06;
    P0_ALT |= 0x06;
    SCON = (1<<7)|(1<<6)|(1<<5)|(0<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0);
    ES = 1;

    EX3 = 1;
    EX4 = 1;

    CKCON |= (1<<3);
    EA  = 1;

    CKLFCON = CPU_XOF|CPU_XO_DIRECT|CPU_UP_CLK_FREQ;
}

static void SPI_initialization(void)
{
    //SPI_CTRL = SPI_STATE_DISABLED;
    SPICLK   = SPI_CLK_CPU_1_2;
}

static void RF_initialization(swap_data *data_ptr)
{
    data_ptr->radio_config.ch_no    = RF_CHANNEL_NO_LOW;
    data_ptr->radio_config.freq_pll = RF_PA_PWR|RF_HFREQ_PLL|RF_CHANNEL_NO_HIGH;

    data_ptr->radio_config.rxtx_afw = RF_TXADDR_WIDTH|RF_RXADDR_WIDTH;

    data_ptr->radio_config.rx_pw    = RF_RXPAYLOAD_WIDTH;
    data_ptr->radio_config.tx_pw    = RF_TXPAYLOAD_WIDTH;

    data_ptr->radio_config.crc_cpu  = RF_CRC_MODE|RF_CRC_EN|CPU_XOF|CPU_XO_DIRECT|CPU_UP_CLK_FREQ;
    data_ptr->data_kind = DATA_W_RF_CONFIG;

    radio_configuration(data_ptr);
}


/***************************** END OF FILE *****************************/
