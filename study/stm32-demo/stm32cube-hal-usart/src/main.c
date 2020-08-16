#include "stm32f1xx_hal.h"
#include "string.h"

/* USART1 PA9 - TX，  PA10 - RX */

uint8_t rData[255];
uint8_t tData[255];

uint8_t message = 0xf5;

UART_HandleTypeDef uart1;

int main(void) {
  GPIO_InitTypeDef GPIO_InitStruct;

  HAL_Init();
  // SystemClock_Config();


    /* 第一步： 配置USART1 IO及clock等 */

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /* USART1 GPIO Tx Rx */
    GPIO_InitStruct.Pin = GPIO_PIN_9; // tx  pa9, rx  pa10
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10; // tx  pa9, rx  pa10
    GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    __HAL_RCC_USART1_CLK_ENABLE(); //__HAL_RCC_USART1_CLK_DISABLE

  /* 第二步： HAL_USART_Init */
  uart1.Instance = USART1;
  uart1.Init.BaudRate = 9600;
  uart1.Init.WordLength = UART_WORDLENGTH_8B;
  uart1.Init.StopBits = UART_STOPBITS_1;
  uart1.Init.Parity = UART_PARITY_NONE;
  uart1.Init.Mode = UART_MODE_TX_RX;
  uart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uart1.Init.OverSampling = UART_OVERSAMPLING_16;  


  if (HAL_UART_Init(&uart1) != HAL_OK) {
    //_Error_Handler(__FILE__, __LINE__);
  }

  HAL_UART_Transmit(&uart1, &message, 2, 0xff);
  //HAL_UART_Transmit_DMA(&uart1, message, 11);

  //HAL_UART_Receive(&uart1, rData, 255, 1);

/*
  while(1) {
    if (HAL_UART_Receive(&uart1, rData, 255, 10)  == HAL_OK) {
      if (HAL_UART_Transmit(&uart1, tData, 255, 10) == HAL_OK) {

      }
    }
    HAL_Delay(10);
  }
*/
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart) {

  if (huart->Instance == USART1) {



    //HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    //HAL_NVIC_EnableIRQ(USART1_IRQn);
    //HAL_UART_IRQHandler(&uart1);

    /* USER CODE BEGIN USART1_MspInit 1 */
    /* 开启接收中断，并初始化串口接收指针 */
    //HAL_UART_Receive_IT(&uart1, rData, 255);
    /* USER CODE BEGIN USART1_MspInit 1 */

  }

}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

}

void SysTick_Handler(void)
{
    HAL_IncTick();
}

