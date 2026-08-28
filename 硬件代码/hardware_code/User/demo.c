#include "./SYSTEM/sys/sys.h"
#include "./BSP/ATK_IDM750C/atk_idm750c.h"
#include "./BSP/ATK_IDM750C/atk_idm750c_uart.h"
#include "string.h"
#include "demo.h"
#include "./BSP/ATK_MS6050/atk_ms6050.h"
#include "./BSP/ATK_MS6050/eMPL/inv_mpu.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/key/key.h"
#include "./BSP/lcd/lcd.h"
#include "./BSP/led/led.h"
#include "./BSP/ATK_MO1218/atk_mo1218.h"
#include "./BSP/ATK_MO1218/atk_mo1218_bin_msg.h"
#include "./BSP/ATK_MO1218/atk_mo1218_nmea_msg.h"
#include "./BSP/ATK_MO1218/atk_mo1218_uart.h"
#include "./BSP/DHT11/dht11.h"
#include "./BSP/Switch/switch.h"
#include "./BSP/Beep/beep.h"
#include "./BSP/watersensor/watersensor.h"
#include "./BSP/fuelgassensor/fuelgassensor.h"
#include "./BSP/smokesensor/smokesensor.h"
#include "./BSP/adc_dma/adc_dma.h"

void demo_run(void)
{
	  int i;
		uint8_t ret;
		uint8_t *buf;
		uint8_t t = 0;
		uint8_t temperature;
		uint8_t humidity;
		atk_mo1218_time_t utc;
		atk_mo1218_position_t position;
		int16_t altitude;
		uint16_t speed;
		int a;
		a=1;
		float pit, rol, yaw;
		int16_t acc_x, acc_y, acc_z;
		int16_t gyr_x, gyr_y, gyr_z;
		int16_t temp;
		usart_init(115200); /* 初始化串口1 */
	  Switches_Init();    /*初始化位移传感器 */
	  Buzzer_Init();      /* 初始化蜂鸣器 */
		watersensor_Init();
		fuelgassensor_Init();
		smokesensor_Init();
		adc_dma_enable(ADC_DMA_BUF_SIZE);   						/* 启动ADC DMA采集 */
	  dht11_init();/* 初始化dht11*/
	  /*if (dht11_check != 0)
		{
			  printf("dht11 init failed!\r\n");
			  while (1)
				{
					  LED0_TOGGLE();
            delay_ms(200);
				}
		}*/
	
		/* 初始化ATK-MS6050 */
    ret = atk_ms6050_init();
    if (ret != 0)
    {
        printf("ATK-MS6050 init failed!\r\n");
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
		
    /* 初始化ATK-MS6050 DMP */
    ret = atk_ms6050_dmp_init();
    if (ret != 0)
    {
        printf("ATK-MS6050 DMP init failed!\r\n");
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
		
		/* 初始化ATK-MO1218*/
    ret = atk_mo1218_init(38400);
    if (ret != 0)
    {
        printf("ATK-MO1218 init failed!\r\n");
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }

		/* 初始化ATK-IDM750C*/
    ret = atk_idm750c_init(115200);
    if (ret != 0)
    {
        printf("ATK-IDM750C init failed!\r\n");
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
		
    printf("Wait for all hardware modules to start, wait 10s....\r\n");
		
		// ATK-IDM750C AT指令测试 //
    ret  = atk_idm750c_at_test();
    // ATK-IDM750C 工作模式设置 //
    ret += atk_idm750c_query_workmode(ATK_IDM750C_WORKMODE_HTTP);
		ret += atk_idm750c_http_request_mode();
		char *actual_http_address = "http://your.actual.address";
		ret += atk_idm750c_http_url_address(actual_http_address);  // 传递实际的字符串变量
		ret += atk_idm750c_http_request_timeout();
		ret += atk_idm750c_http_request_header_information();  
    ret += atk_idm750c_enter_transfermode(); //ATK-IDM750C 进入透传状态 //

    if (ret != 0)
    {
        printf("**************************************************************************\r\n");
        printf("ATK-IDM750C Configuration Failed ...\r\n");
        printf("请按照以下步骤进行检查:\r\n");
        printf("1.使用电脑上位机配置软件检查DTU能否单独正常工作\r\n");
        printf("2.检查DTU串口参数与STM32通讯的串口参数是否一致\r\n");
        printf("3.检查DTU与STM32串口的接线是否正确\r\n");
        printf("4.检查DTU供电是否正常，DTU推荐使用12V/1A电源供电，不要使用USB的5V给模块供电！！\r\n");
        printf("**************************************************************************\r\n\r\n");
        
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
    atk_idm750c_uart_rx_restart();
		
		while (1)
    {	
			char json_data[512];  // 定义json_data
			ret = atk_mo1218_update(&utc, &position, &altitude, &speed, NULL, NULL, NULL, 5000);
			/* 获取ATK-MS6050 DMP处理后的数据 */
			ret  = atk_ms6050_dmp_get_data(&pit, &rol, &yaw);
			/* 获取ATK-MS6050加速度值 */
			ret += atk_ms6050_get_accelerometer(&acc_x, &acc_y, &acc_z);
			/* 获取ATK-MS6050陀螺仪值 */
			ret += atk_ms6050_get_gyroscope(&gyr_x, &gyr_y, &gyr_z);
			/* 获取ATK-MS6050温度值 */
			ret += atk_ms6050_get_temperature(&temp);
			char utc_time_str[50];
			sprintf(utc_time_str, "%d-%d-%d %d:%d:%d", utc.year, utc.month, utc.day, utc.hour, utc.minute, utc.second);
			float longitude = position.longitude.degree / 100000.0f;  // 将整数转换为浮点数
			float latitude = position.latitude.degree / 100000.0f;    // 将整数转换为浮点数
			
			/* 每100ms读取一次温湿度数据	*/
			if (t % 10 == 0)  
			{
            dht11_read_data(&temperature, &humidity);             /* 读取温湿度 */
            printf("temperature: %d\r\n", temperature);                     /* 打印温度 */
            printf("humidity: %d\r\n", humidity);                        /* 打印湿度 */
       }

        delay_ms(10);
        t++;

      if (t == 20)
      {
            t = 0;
      }
			
			//压力传感器
			uint16_t value_AD1 = 0;
			int PRESS_AO1 = 0;
			int VOLTAGE_AO1 = 0;
					
			value_AD1 = Get_Adc_Average(0);	                   // 调用函数获取通道1的AD值
			VOLTAGE_AO1 = map(value_AD1, 0, 4095, 0, 3300);    // AD值换算
			//压力数据校验
			if (VOLTAGE_AO1 < VOLTAGE_MIN) 
			{
					PRESS_AO1 = 0;
			} 
			else if (VOLTAGE_AO1 > VOLTAGE_MAX) 
			{
					PRESS_AO1 = PRESS_MAX;
			} 
			else 
			{
					PRESS_AO1 = map(VOLTAGE_AO1, VOLTAGE_MIN, VOLTAGE_MAX, PRESS_MIN, PRESS_MAX);
			}	
			
			/* dtu上传jason数据 */
			sprintf(json_data,
        "{\n"
				"    \"freightId\": %d,\n"
        "    \"longitude\": %.5f,\n"
        "    \"latitude\": %.5f,\n"
        "    \"altitude\": %d,\n"
        "    \"speed\": %u,\n"
        "    \"pit\": %.2f,\n"
				"    \"rol\": %.2f,\n"
				"    \"yaw\": %.2f,\n"
				"    \"acc_x\": %d,\n"
				"    \"acc_y\": %d,\n"
				"    \"acc_z\": %d,\n"
				"    \"gyr_x\": %d,\n"
				"    \"gyr_y\": %d,\n"
				"    \"gyr_z\": %d,\n"
				"    \"temp\": %d,\n"
				"    \"humidity\": %d,\n"
				"    \"press\": %1d,\n"
				"    \"temperature\": %d\n"
				"}\n",
				a,
        longitude,
        latitude,
        altitude,
        speed,
				pit, 
				rol, 
				yaw, 
				acc_x, 
				acc_y, 
				acc_z, 
				gyr_x, 
				gyr_y, 
				gyr_z, 
				temp,
				humidity,
				PRESS_AO1,
				temperature);	
        /* 串口打印数据 */
                /* UTC */
                printf("UTC Time: %04d-%02d-%02d %02d:%02d:%02d.%03d\r\n", utc.year, utc.month, utc.day, utc.hour, utc.minute, utc.second, utc.millisecond);
                
                /* 经纬度（放大了100000倍数） */
                printf("Position: %d.%d'%s %d.%d'%s\r\n", position.longitude.degree / 100000, position.longitude.degree % 100000, (position.longitude.indicator == ATK_MO1218_LONGITUDE_EAST) ? "E" : "W", position.latitude.degree / 100000, position.latitude.degree % 100000, (position.latitude.indicator == ATK_MO1218_LATITUDE_NORTH) ? "N" : "S");
                
                /* 海拔高度（放大了10倍） */
                printf("Altitude: %d.%dm\r\n", altitude / 10, altitude % 10);
                
                /* 速度（放大了10倍） */
                printf("Speed: %d.%dKm/H\r\n", speed / 10, speed % 10);
							  printf("pit: %.2f, rol: %.2f, yaw: %.2f, ", pit, rol, yaw);
                printf("acc_x: %d, acc_y: %d, acc_z: %d, ", acc_x, acc_y, acc_z);
                printf("gyr_x: %d, gyr_y: %d, gyr_z: %d, ", gyr_x, gyr_y, gyr_z);
                printf("temp: %d\r\n", temp);
								printf("1通道AD值 = %d,电压 = %d mv,压力 = %d g\r\n", value_AD1, VOLTAGE_AO1, PRESS_AO1);
								
				/* 位移传感器报警 */
				for ( i = 0; i < 4; i++ )
        {
            if (checkswitch1() == 0)//开关1
            {
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
							delay_ms(100);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
							delay_ms(100);
							printf("Switch1 on!\r\n");
							atk_idm750c_uart_printf("%s\r\n", "0");
            }
						if (checkswitch2() == 0)//开关2
            {
              HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
							delay_ms(100);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
							delay_ms(100);
							printf("Switch2 on!\r\n");
							 atk_idm750c_uart_printf("%s\r\n", "1");
            }
						if (checkswitch3() == 0)//开关3
            {
              HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
							delay_ms(100); 
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
							delay_ms(100);
							printf("Switch3 on!\r\n");
							 atk_idm750c_uart_printf("%s\r\n", "2");
            }
						if (checkswitch4() == 0)//开关4
            {
              HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
							delay_ms(100);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
							delay_ms(100);
							printf("Switch4 on!\r\n"); 
							atk_idm750c_uart_printf("%s\r\n", "3");
            }
						if (watersensor() == 0)//水浸
            {
              HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
							delay_ms(100);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
							delay_ms(100);
							printf("watersensor!\r\n");
							atk_idm750c_uart_printf("%s\r\n", "4");
            }
						if (smokesensor() == 0)//烟雾传感器
            {
              HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
							delay_ms(100);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
							delay_ms(100);
							printf("smokesensor!\r\n");
							atk_idm750c_uart_printf("%s\r\n", "5");
            }
						if (fuelgassensor() == 0)//燃气传感器
            {
              HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
							delay_ms(100);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
							delay_ms(100);
							printf("fuelgassensor!\r\n");
							atk_idm750c_uart_printf("%s\r\n", "6");
            }
        }
				
				atk_idm750c_uart_printf("%s\r\n", json_data);
				printf("111");
				buf = atk_idm750c_uart_rx_get_frame();
				if (buf != NULL)
					{
						printf("%s", buf);
						atk_idm750c_uart_rx_restart();
					}       
					LED1_TOGGLE();
					delay_ms(200);
		}
}
