#include "stm32f1xx_hal.h"

/* stm32f1xx_hal_spi.h 使用规则*/

/**
       The SPI HAL driver can be used as follows:

      (#) Declare a SPI_HandleTypeDef handle structure, for example:
          SPI_HandleTypeDef  hspi;

      (#)Initialize the SPI low level resources by implementing the HAL_SPI_MspInit() API:
          (##) Enable the SPIx interface clock
          (##) SPI pins configuration
              (+++) Enable the clock for the SPI GPIOs
              (+++) Configure these SPI pins as alternate function push-pull
          (##) NVIC configuration if you need to use interrupt process
              (+++) Configure the SPIx interrupt priority
              (+++) Enable the NVIC SPI IRQ handle
          (##) DMA Configuration if you need to use DMA process
              (+++) Declare a DMA_HandleTypeDef handle structure for the transmit or receive Channel
              (+++) Enable the DMAx clock
              (+++) Configure the DMA handle parameters
              (+++) Configure the DMA Tx or Rx Channel
              (+++) Associate the initilalized hdma_tx(or _rx) handle to the hspi DMA Tx (or Rx) handle
              (+++) Configure the priority and enable the NVIC for the transfer complete interrupt on the DMA Tx or Rx Channel

      (#) Program the Mode, BidirectionalMode , Data size, Baudrate Prescaler, NSS
          management, Clock polarity and phase, FirstBit and CRC configuration in the hspi Init structure.

      (#) Initialize the SPI registers by calling the HAL_SPI_Init() API:
          (++) This API configures also the low level Hardware GPIO, CLOCK, CORTEX...etc)
              by calling the customized HAL_SPI_MspInit() API.
     [..]
       Circular mode restriction:
      (#) The DMA circular mode cannot be used when the SPI is configured in these modes:
          (##) Master 2Lines RxOnly
          (##) Master 1Line Rx
      (#) The CRC feature is not managed when the DMA circular mode is enabled
      (#) When the SPI DMA Pause/Stop features are used, we must use the following APIs
          the HAL_SPI_DMAPause()/ HAL_SPI_DMAStop() only under the SPI callbacks
     [..]
       Master Receive mode restriction:
      (#) In Master unidirectional receive-only mode (MSTR =1, BIDIMODE=0, RXONLY=0) or
          bidirectional receive mode (MSTR=1, BIDIMODE=1, BIDIOE=0), to ensure that the SPI
          does not initiate a new transfer the following procedure has to be respected:
          (##) HAL_SPI_DeInit()
          (##) HAL_SPI_Init()
 */

SPI_HandleTypeDef hspi2;

/* 第一步： 配置SPI IO及clock等 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *spiHandle)
{
  /* 在stm32f1xx_hal_spi.c中HAL_SPI_Init()会调用HAL_SPI_MspInit()这个API 需要实现这个函数的方法*/
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (spiHandle->Instance == SPI2)
  {
    /* SPI2 clock enable */
    __HAL_RCC_SPI2_CLK_ENABLE();

    /* SPI2 GPIO enable*/
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /** SPI2 GPIO Configuration   
    *  PB12     ------> SPI2_NSS
	  *  PB13     ------> SPI2_SCK
	  *  PB14     ------> SPI2_MISO
	  *  PB15     ------> SPI2_MOSI 
	  */

    /* SPI2_SCK and SPI2_MOSI */
    GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_15; //PB13 PB15
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

#if 1
    /* SPI2_MISO */
    GPIO_InitStruct.Pin = GPIO_PIN_14; //PB14
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* SPI2_NSS */
    GPIO_InitStruct.Pin = GPIO_PIN_12; //PB14
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif
  }
}

/* NSS soft ctrl */
void SPI2_NSS_Set(void)
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
}

void SPI2_NSS_Clear(void)
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
}

/* 第二步： 初始化SPI2函数 */
void MY_SPI2_Init(void)
{

  HAL_StatusTypeDef status = HAL_ERROR;

  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_16BIT;   //一个数据8位
  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH; //时钟空闲为高电平
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;      //第二个跳变采样数据
  hspi2.Init.NSS = SPI_NSS_SOFT;              //软件设置CS
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;                 //高位在前
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;                 //禁用TI模式,不支持
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE; //禁用CRC校验
  hspi2.Init.CRCPolynomial = 10;

  status = HAL_SPI_Init(&hspi2);
  if (status != HAL_OK)
  {
    //Error_Handler();
  }
}

#define LED_PIN GPIO_PIN_13
#define LED_GPIO_PORT GPIOC
#define LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()

int main(void)
{
  HAL_StatusTypeDef errorcode = HAL_OK;

  HAL_Init();

  LED_GPIO_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct;

  GPIO_InitStruct.Pin = LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);

  MY_SPI2_Init();

  uint16_t buf = 0x505a;

  SPI2_NSS_Set();
  errorcode = HAL_SPI_Transmit_IT(&hspi2, (uint8_t *)&buf, 1);
  SPI2_NSS_Clear();

  if (errorcode != HAL_OK)
  {
    while (1)
    {
      HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_SET);

      HAL_Delay(50);
      HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_RESET);
      HAL_Delay(50);
    }
  }
  else
  {
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_SET);
  }
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  while (1)
  {
  }
}

void BusFault_Handler(void)
{
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}
