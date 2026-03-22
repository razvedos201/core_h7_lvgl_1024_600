#include "GT911.h"

extern I2C_HandleTypeDef hi2c4;

void InitGT911Pin(void)
{/*
	InitGPIO(GT911_GPIOPort, GT911_INT, OUT);
	InitGPIO(GT911_GPIOPort, GT911_RST, OUT);
	
	SetGPIO(GT911_GPIOPort, GT911_INT, 1);
	SetGPIO(GT911_GPIOPort, GT911_RST, 1);*/
}

void GT911_Reset(void)
{
	uint32_t i;
	GPIO_InitTypeDef GPIO_InitStruct;
	if(GT911_DEFAULT_ID == 0x28){
		HAL_GPIO_WritePin(GT911_RST_GPIO_Port, GT911_RST_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GT911_INT_GPIO_Port, GT911_INT_Pin, GPIO_PIN_RESET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(GT911_INT_GPIO_Port, GT911_INT_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(GT911_RST_GPIO_Port, GT911_RST_Pin, GPIO_PIN_SET);
		HAL_Delay(10);

		GPIO_InitStruct.Pin = GT911_INT_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GT911_INT_GPIO_Port, &GPIO_InitStruct);
	}
	if(GT911_DEFAULT_ID == 0xBA){
		HAL_GPIO_WritePin(GT911_RST_GPIO_Port, GT911_RST_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GT911_INT_GPIO_Port, GT911_INT_Pin, GPIO_PIN_RESET);
		HAL_Delay(1);
		HAL_GPIO_WritePin(GT911_RST_GPIO_Port, GT911_RST_Pin, GPIO_PIN_SET);
		HAL_Delay(5);

		GPIO_InitStruct.Pin = GT911_INT_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GT911_INT_GPIO_Port, &GPIO_InitStruct);
	}

	HAL_Delay(10);
}
void INT_LS(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GT911_INT_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GT911_INT_GPIO_Port, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GT911_INT_GPIO_Port, GT911_INT_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GT911_INT_GPIO_Port, GT911_INT_Pin, GPIO_PIN_SET);

	GPIO_InitStruct.Pin = GT911_INT_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GT911_INT_GPIO_Port, &GPIO_InitStruct);
}

void GT911_CalculateCheckSum(GT911ConfInfo_TypeDef * GT911ConfInfo)
{
	GT911ConfInfo->ConfigChksum = 0;
	for(uint8_t i = 0 ; i < 184 ; i++){
		GT911ConfInfo->ConfigChksum += *((uint8_t*)GT911ConfInfo + i);
	}
	GT911ConfInfo->ConfigChksum = (~GT911ConfInfo->ConfigChksum) + 1;
}

void GT911_Read(uint16_t RegisterAddress, uint8_t* ReceivedBuffer, uint8_t Size)
{
	HAL_StatusTypeDef result;
	result = HAL_I2C_Mem_Read(&hi2c4, GT911_DEFAULT_ID, RegisterAddress, 2, ReceivedBuffer, Size, 50);
  //I2CRead(GT911_I2C, GT911_DEFAULT_ID, (uint8_t*)&RegisterAddress, 2, ReceivedBuffer, Size);
}

void GT911_Write(uint16_t RegisterAddress, uint8_t* TransmitedBuffer, uint8_t Size)
{
	HAL_StatusTypeDef result;
	result = HAL_I2C_Mem_Write(&hi2c4, GT911_DEFAULT_ID, RegisterAddress, 2, TransmitedBuffer, Size, 50);
  //I2CWrite(GT911_I2C, GT911_DEFAULT_ID, (uint8_t*)&RegisterAddress, 2, TransmitedBuffer, Size);
}

void GT911_Init(void)
{
	//InitGT911Pin();

	GT911ConfInfo_TypeDef GT911ConfInfo = {
		0x00, 0x00, 0x04, 0x58, 0x02, 0x0A, 0x0C, 0x20, 0x01, 0x08, 0x28, 0x05, 0x50, // 0x8047 - 0x8053
		0x3C, 0x0F, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x8054 - 0x8060
		0x00, 0x89, 0x2A, 0x0B, 0x2D, 0x2B, 0x0F, 0x0A, 0x00, 0x00, 0x01, 0xA9, 0x03, // 0x8061 - 0x806D
		0x2D, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, // 0x806E - 0x807A
		0x59, 0x94, 0xC5, 0x02, 0x07, 0x00, 0x00, 0x04, 0x93, 0x24, 0x00, 0x7D, 0x2C, // 0x807B - 0x8087
		0x00, 0x6B, 0x36, 0x00, 0x5D, 0x42, 0x00, 0x53, 0x50, 0x00, 0x53, 0x00, 0x00, // 0x8088	- 0x8094
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x8095 - 0x80A1
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x80A2 - 0x80AD
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, // 0x80AE - 0x80BA
		0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, // 0x80BB - 0x80C7
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x80C8 - 0x80D4
		0x02, 0x04, 0x06, 0x08, 0x0A, 0x0F, 0x10, 0x12, 0x16, 0x18, 0x1C, 0x1D, 0x1E, // 0x80D5 - 0x80E1
		0x1F, 0x20, 0x21, 0x22, 0x24, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, // 0x80E2 - 0x80EE
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x80EF - 0x80FB
		0x00, 0x00, 0xD6, 0x01 }; // 0x80FC - 0x8100
	
	GT911ConfInfo.XOutputMax_H = 0x4;   /* 1024 = 0x0400 */
	GT911ConfInfo.XOutputMax_L = 0x00;

	GT911ConfInfo.YOutputMax_H = 0x2;   /* 600 = 0x0258 */
	GT911ConfInfo.YOutputMax_L = 0x58;
	
	GT911ConfInfo.TouchNumber = 5;
		
	GT911ConfInfo.LargeTouch = 40;
		
	//GT911ConfInfo.ModuleSwitch[0] = 0xcc;
	GT911ConfInfo.ModuleSwitch[0] = 0x3D;
		
	GT911_CalculateCheckSum(&GT911ConfInfo);

	GT911_Reset();

	GT911_Write(CONFIG_VERSION, (uint8_t*)&GT911ConfInfo, 184);

	uint8_t cmd = 0x00;

	GT911_Write(COMMAND_ADDRESS, &cmd, 1);
/*
	  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	  SYSCFG->EXTICR[4 >> 2] |= (((uint32_t)TOUCH_IRQ_GPIO_Port) & 0xFF00) >> 10;
	  EXTI->IMR  |= TOUCH_IRQ_Pin;
	  EXTI->FTSR |= TOUCH_IRQ_Pin;
*/
	  NVIC_SetPriority (EXTI3_IRQn, 6);
	  NVIC_EnableIRQ(EXTI3_IRQn);

		HAL_Delay(1);
		INT_LS();
}

uint8_t GT911_ReadTouch(GT911Touch_TypeDef * GT911Touch)
{
	uint8_t status;

	GT911_Read(STATUS_ADDRESS, &status, 1);

	if ((status & 0x80) != 0)
	{
		for (uint8_t i = 0; i < (status & 0xF); i++)
		{
			GT911_Read(POINT1_X_ADDRESS+i*8,(uint8_t*)&GT911Touch[i],sizeof(GT911Touch[i]));
		}
		uint8_t reset = 0;
		GT911_Write(STATUS_ADDRESS, &reset, 1);
	}

	return (status & 0xF);
}
