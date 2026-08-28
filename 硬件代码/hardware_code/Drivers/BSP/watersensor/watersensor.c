#include "./BSP/watersensor/watersensor.h"
#include "./SYSTEM/delay/delay.h"

// 定义开关状态
#define WATERSENSOR_OPEN   0
#define WATERSENSOR_CLOSED 1

void watersensor_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

int watersensor(void) {
    GPIO_PinState switchState = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);
    
    if(switchState == GPIO_PIN_SET) {
        return WATERSENSOR_CLOSED; // 如果开关闭合，返回1
    } else {
        return WATERSENSOR_OPEN; // 如果开关断开，返回0
    }
}
