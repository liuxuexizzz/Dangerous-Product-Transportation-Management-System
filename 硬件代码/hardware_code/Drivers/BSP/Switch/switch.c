#include "./SYSTEM/sys/sys.h"
#include "./BSP/Switch/switch.h"
#include "./BSP/Beep/beep.h"

void Switches_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOF_CLK_ENABLE(); 
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;

    //(PF0, PF1, PF2, PF3)
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
}

int IsAnySwitchPressed(void)
{
    if (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_0) == GPIO_PIN_RESET ||
        HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_1) == GPIO_PIN_RESET ||
        HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_2) == GPIO_PIN_RESET ||
        HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_3) == GPIO_PIN_RESET) 
		{
        return 1; 
    }
		else 
		{
        return 0; 
    }
}

int checkswitch1(void)
{
		if (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_0) == GPIO_PIN_RESET)
		{
		return 1;
		}
		else
		{
		return 0;
		}	
}

int checkswitch2(void)
{
		if (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_1) == GPIO_PIN_RESET)
		{
		return 1;
		}
		else
		{
		return 0;
		}	
}

int checkswitch3(void)
{
		if (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_2) == GPIO_PIN_RESET)
		{
		return 1;
		}
		else
		{
		return 0;
		}	
}

int checkswitch4(void)
{
		if (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_3) == GPIO_PIN_RESET)
		{
		return 1;
		}
		else
		{
		return 0;
		}	
}


