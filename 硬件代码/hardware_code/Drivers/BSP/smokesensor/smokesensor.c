#include "./BSP/smokesensor/smokesensor.h"
#include "./SYSTEM/delay/delay.h"

// 定义开关状态
#define SMOKESENSOR_OPEN   0
#define SMOKESENSOR_CLOSED 1

void smokesensor_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

int smokesensor(void) {
    GPIO_PinState switchState = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
    
    if(switchState == GPIO_PIN_SET) {
        return SMOKESENSOR_CLOSED; // 如果开关闭合，返回1
    } else {
        return SMOKESENSOR_OPEN; // 如果开关断开，返回0
    }
}
