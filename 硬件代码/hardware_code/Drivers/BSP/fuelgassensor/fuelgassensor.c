#include "./BSP/fuelgassensor/fuelgassensor.h"
#include "./SYSTEM/delay/delay.h"

// 定义开关状态
#define FUELGASSENSOR_OPEN   0
#define FUELGASSENSOR_CLOSED 1

void fuelgassensor_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOD_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

int fuelgassensor(void) {
    GPIO_PinState switchState = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_11);
    
    if(switchState == GPIO_PIN_SET) {
        return FUELGASSENSOR_CLOSED; // 如果开关闭合，返回1
    } else {
        return FUELGASSENSOR_OPEN; // 如果开关断开，返回0
    }
}
