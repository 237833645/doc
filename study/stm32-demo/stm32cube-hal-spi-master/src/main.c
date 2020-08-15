#include "stm32f1xx_hal.h"

/* NSS soft ctrl */
void SPI2_NSS_Set(void) {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
}

void SPI2_NSS_Clear(void) {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
}

int main(void) {
  HAL_StatusTypeDef errorcode = HAL_OK;
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  SPI_HandleTypeDef hspi = {0};

  HAL_Init();

  /* 第一步： 配置SPI IO及clock等 */
  /* SPI2 clock enable */
  __HAL_RCC_SPI2_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* SPI2_SCK and SPI2_M  */
  GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_15; // PB13~PB15
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* SPI2_MISO */
  GPIO_InitStruct.Pin = GPIO_PIN_14; //PB14
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* SPI2_NSS */
  GPIO_InitStruct.Pin = GPIO_PIN_12; //PB12
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* 第二步： HAL_SPI_Init */
  hspi.Instance = SPI2;
  hspi.Init.Mode = SPI_MODE_MASTER;
  hspi.Init.Direction = SPI_DIRECTION_2LINES;
  hspi.Init.DataSize = SPI_DATASIZE_16BIT;   //一个数据8位
  hspi.Init.CLKPolarity = SPI_POLARITY_HIGH; //时钟空闲为高电平
  hspi.Init.CLKPhase = SPI_PHASE_2EDGE;      //第二个跳变采样数据
  hspi.Init.NSS = SPI_NSS_SOFT;              //软件设置CS
  hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi.Init.FirstBit = SPI_FIRSTBIT_MSB; //高位在前
  hspi.Init.TIMode = SPI_TIMODE_DISABLE; //禁用TI模式,不支持
  hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE; //禁用CRC校验
  hspi.Init.CRCPolynomial = 10;

  errorcode = HAL_SPI_Init(&hspi);
  if (errorcode != HAL_OK) {
    // Error_Handler();
  }

  /* 第三步： 传输数据 */
  uint16_t buf = 0x505a;

  SPI2_NSS_Set();
  errorcode = HAL_SPI_Transmit(&hspi, (uint8_t *)&buf, 1, 1);
  SPI2_NSS_Clear();
  if (errorcode != HAL_OK) {
  }
}
