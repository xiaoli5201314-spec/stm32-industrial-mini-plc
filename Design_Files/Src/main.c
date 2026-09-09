/** \file main.c ******************************************************
 *
 *             Project: maxrefdes61#
 *            Filename: main.c
 *         Description: This module contains the Main application for the
 *                      maxrefdes61 customized firmware
 *
 *    Revision History:
 *\n                    6/11/2014    Rev 01.00    GL    Initial release.
 *
 *  --------------------------------------------------------------------
 *
 *  This code follows the following naming conventions:
 *
 *\n    char                    chPmodValue
 *\n    char (array)            sPmodString[16]
 *\n    float                   fPmodValue
 *\n    int                     nPmodValue
 *\n    int (array)             anPmodValue[16]
 *\n    uint16_t                uPmodValue
 *\n    uint16_t (array)        auPmodValue[16]
 *\n    uint8_t                 uchPmodValue
 *\n    uint8_t (array)					auchPmodBuffer[16]
 *\n    unsigned int            unPmodValue
 *\n    int *                   punPmodValue
 *
 *  ------------------------------------------------------------------------- */
/*
 * Copyright (C) 2012 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED PRODUCTS BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated Products
 * shall not be used except as stated in the Maxim Integrated Products
 * Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products retains all ownership rights.
 *
 ***************************************************************************/
/*!\mainpage Main Page
 *
 * \section intro_sec Introduction
 *
 * This is the code documentation for the MAXREFDES61# subsystem reference design.
 * \n
 * \n The Files page contains the File List page and the Globals page.
 * \n
 * \n The Globals page contains the Functions, Variables, and Macros sub-pages.
 *
 * \image html MAXREFDES61_Block_Diagram.bmp "MAXREFDES61# System Block Diagram"
 * \n
 * \image html MAXREFDES61_firmware_Flowchart.bmp "MAXREFDES61# Firmware Flowchart"
 *
 */ 


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN 0 */
#include "maximDeviceSpecificUtilities.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "menu.h"
#include "utilities.h"

#define MAJOR_REVISION 01
#define MINOR_REVISION 00

uint16_t g_auADCData[65536] __attribute__((at(0x20000000)));	//ADC sample storage

/* USER CODE END 0 */

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);

int main(void)
{

  /* USER CODE BEGIN 1 */
	
	uint8_t uchInput=0;		//UART input storage
	int nMenuState=0; // Go to Channel Select Menu First
	int nSelectedMode;
	int nSelectedChannel;
	uint32_t unNumberOfSamplesReceived;
	uint32_t unSampleSize = 512;
	int nSampleRate = 0;
	uint8_t uchFirstEntry=1;
	
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
	
	Delay(ONE_SECOND/2);	//wait a moment for the oscillator to become active.
  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();

  /* USER CODE BEGIN 2 */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);	//set CS pin high
	ADCInit();
	// Clear the Screen, and then display the big Maxim banner for about 2 seconds
	menu_cls();
	menu_print_maxim_banner_big();
	printf("Maxim MAXREFDES61#\r\n");
	printf("Revision v%d.%d\r\n",MAJOR_REVISION,MINOR_REVISION);

	Delay(2*ONE_SECOND);	//2 second pause
  /* USER CODE END 2 */

  /* USER CODE BEGIN 3 */

  /* Infinite loop */
  while (1)
  {
		switch(nMenuState)
		{
			case MAIN_MENU:
				menu_cls();
				menu_print_main_menu();
				do{
					if(uchFirstEntry==1)
					{
						if(!__HAL_UART_GET_FLAG(&huart1,UART_FLAG_RXNE))
						{
							Delay(ONE_SECOND);	//1 sec pause
						}
						if(!__HAL_UART_GET_FLAG(&huart1,UART_FLAG_RXNE))
						{
							Delay(ONE_SECOND);	//1 sec pause
							nMenuState = MAIN_MENU;
							break;
						}
					}
					uchFirstEntry=0;
					uchInput = getchar();
					printf("%c\r\n",uchInput);
					if(uchInput<='1')
					{
						nMenuState = MENU_CHANNEL;
						if(uchInput=='0')
						{
							nSelectedMode = START_CONTINUOUS_SAMPLING;
						}
						else
						{
							nSelectedMode = MENU_SAMPLE_SPEED;
						}
					}
				}while(uchInput>'1' || uchInput<'0');
				break;

			case MENU_CHANNEL:
				menu_cls();
				menu_print_channel_menu();
				do{
					uchInput = getchar();
					printf("%c\r\n",uchInput);
					nMenuState = nSelectedMode;
					nSelectedChannel=uchInput-48;
				}while(uchInput>'3' || uchInput<'0');
				break;

			case MENU_SAMPLE_SPEED:
				menu_cls();
				menu_print_sample_rate_menu();
				do{
					uchInput = getchar();
					printf("%c\r\n",uchInput);
					if(uchInput<='3')
					{
						nSampleRate=uchInput-48;
					}
				}while(uchInput>'3' || uchInput<'0');
				nMenuState = MENU_SAMPLE_SIZE;
				break;

			case MENU_SAMPLE_SIZE:
				menu_cls();
				menu_print_sample_size_menu();
				do{
					uchInput = getchar();
					printf("%c\r\n",uchInput);
					if(uchInput<='3')
					{
						if(uchInput=='0')
							unSampleSize=8192;
						else if(uchInput=='1')
							unSampleSize=16384;
						else if(uchInput=='2')
							unSampleSize=32768;
						else
							unSampleSize=65536;
					}
				}while(uchInput>'3' || uchInput<'0');
				nMenuState = START_BLOCK_SAMPLING;
				break;

			case START_CONTINUOUS_SAMPLING:
				menu_cls();
				ContinuousSampling(nSelectedChannel);
				nMenuState=MAIN_MENU;
				break;

			case START_BLOCK_SAMPLING:
				menu_cls();
				printf("Press the ESC key to stop sampling\n\r\n");
				printf("Sampling...\r\n");
			
				unNumberOfSamplesReceived=StartSampling(nSelectedChannel, unSampleSize, nSampleRate, g_auADCData);
				printf("Done sampling.  Data are stored in the \"Samples\" array\r\n");
				printf("Do you want to read the sampled data? (Y/N) \r\n>>");
				do{

					uchInput = getchar();
					printf("%c\r\n",uchInput);
					if(uchInput=='n' || uchInput=='N')
					{
						nMenuState=MAIN_MENU;
						break;
					}
					
					uint32_t i;
					menu_cls();
					printf("Press the ESC key to stop reading back the data\n\r\n");
					Delay(3*ONE_SECOND);
					printf("Channel %i\r\n", nSelectedChannel);
					for(i=0; i<unNumberOfSamplesReceived; i++)
					{
						if(uchInput==0x1B)
						{
							nMenuState=MAIN_MENU;
							break;
						}
						if(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_RXNE))
						{
							if(getchar()==0x1B)
							{
								nMenuState=MAIN_MENU;
								break;
							}
						}
						printf("%i\r\n", g_auADCData[i]);
					}
						

					if(i!=unNumberOfSamplesReceived)	//if the Escape key is pressed before the for loop is finished
						break;	//go back to the main menu
					printf("End of samples.  Press any key to go back to the main menu\r\n");
					nMenuState=MAIN_MENU;
					uchInput=getchar();
					break;
				}while(uchInput!='n' && uchInput!='y' && uchInput!='Y' && uchInput!='N');

				break;

			default:
				printf("Invalid Keypress, please try again\r\n");
				Delay(3*ONE_SECOND);
				nMenuState = MAIN_MENU;
				break;
		}
	}
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
static void SystemClock_Config(void)
{

  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
