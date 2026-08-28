#include "./BSP/DHT11/dht11.h"
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "demo.h"
#include "./BSP/Switch/switch.h"
#include "./BSP/Beep/beep.h"
#include "./BSP/watersensor/watersensor.h"
#include "./BSP/ATK_IDM750C/atk_idm750c.h"
#include "./BSP/ATK_IDM750C/atk_idm750c_uart.h"
#include "./BSP/adc/adc.h"
#include "./BSP/adc_dma/adc_dma.h"



int main(void)
{
    uint16_t adcx;
    float voldust;
		float dust;
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9); /* 设置时钟, 72Mhz */
    delay_init(72);                     /* 延时初始化 */
    usart_init(115200);                 /* 串口初始化为115200 */
    led_init();                         /* 初始化LED */
		adc_init();                         /* 初始化ADC */
		adc_nch_dma_init((uint32_t)&g_adc_dma_buf); 		/* 初始化ADC DMA采集 */
    //demo_run();                         /* 运行主程序 */		


		//初始化PA0口，我用的是PA0来开启粉尘检测，PA1来读取电压
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOA_CLK_ENABLE(); 
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; 
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
		
		
		while (1)
    {
				//开启粉尘检测
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);  
				delay_us(280);
				//这里唯一用到ADC测量
				adcx = adc_get_result(ADC_ADCX_CHY);            
				delay_us(19);   
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
				delay_us(9680);     
			
				//数据转换
				voldust = (float)adcx * (3.3 / 4096*2);                  
        adcx = voldust;       
				printf("Voltage: %fV\n", voldust);
        LED0_TOGGLE();
        delay_ms(1000);
				printf("111\n");	
				dust=(voldust-0.5)/6;
			
			//数据判断
			if (voldust < 0.9)
        {
            if (dust < 0)
            {
                printf("dust:0.0mg/m^3\n");
                printf("dust density low!");//这个信息后面做的可以上传4G
            }
            else
            {
                printf("dust:%.3fmg/m^3\n", dust);
            }
        }    
        else if (voldust < 4.0)
        {
					dust=dust-0.06;
					if (voldust > 3)
            {
                printf("dust density too high!");//这个信息后面做的可以上传4G
                printf("dust:%.3fmg/m^3\n", dust);
                //蜂鸣器报警
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
                delay_ms(100);
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
                delay_ms(100);
            }
            else
            {
                printf("dust:%.3fmg/m^3\n", dust);
            }    
        }
        else
        {
            printf("adc measurement Wrong!");
        } 
    }  
}
