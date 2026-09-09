/** \file maximDeviceSpecificUtilities.c ****************************************
 *
 *             Project: MAXREFDES61#
 *            Filename: maximDeviceSpecificUtilities.c
 *         Description: This module is an embedded controller driver for the MAXREFDES61#.
 *                      It contains high level functions:
 *                      StartSampling(...)
 *                      ContinuousSampling()
 *                      ADCInit()
 *
 *                      This driver can be dropped into a user's application as a starting
 *                      point for development of an end application
 *
*    Revision History:
 *\n                    06-12-14    Rev 01.00    GL    Initial release.
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
 *\n    uint8_t (array)         auchPmodBuffer[16]
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

#include "maximDeviceSpecificUtilities.h"
#include "stm32f4xx_hal.h"
#include "tim.h"
#include "usart.h"
#include "spi.h"
#include "stdio.h"
#include "string.h"
#include "stm32f4xx_hal_dma.h"
#include "utilities.h"

uint16_t g_auWriteBuffer[2], g_auReadBuffer[2];
char sTerminalMessage[100];
uint16_t *g_auSamples;
uint32_t g_unSampleSize=65535;
uint16_t g_auCount=0;
uint8_t g_uchReadADCStop;

uint32_t StartSampling(uint8_t uchADCChannel, uint32_t unSampleSize, int nSampleRate, uint16_t *auSamples)
/**
* \brief       Receive a block of samples at a constant rate
* \par         Details
*							This function is used to receive a block of samples at a constant
* \n					sampling rate.  The size of the block is defined in uint32_t unSampleSize.
* \n					The sampling rate is defined in int nSampleRate. The Sampled data
* \n					will be stored in an array *auSamples
* \n					50ksps, 20ksps, 10ksps, and 1ksps rates controlled by the timer
*
* \param[in]   uchADCChannel     - Channel number
* \param[in]   unSampleSize      - Sample size
* \param[in]   nSampleRate       - Sample rate
* \param[out]  *auSamples        - The array to store the data samples
*
* \retval      uint32_t  			 - Number of samples received
*/
{
	g_uchReadADCStop=0;
	g_auSamples=auSamples;
	g_unSampleSize=unSampleSize-1;	// -1 because g_auCount counts from 0
	g_auCount=0;

	g_auWriteBuffer[0] = (0x80 + (uchADCChannel<<4))<<8;	//SPI read command for the selected ADC channel
	g_auWriteBuffer[1] = 0;
	
	if(nSampleRate==0)      //Timer setup for 1ksps
	{
		htim1.Init.Prescaler = 0; //Timer clock = 50MHz
		htim1.Init.Period = 50000-1;  //Period = 50MHz/50000
	}
	else if(nSampleRate==1) //Timer setup for 10ksps
	{
		htim1.Init.Prescaler = 0; 
		htim1.Init.Period = 5000-1;  //Period = 50MHz/5000
	}
	else if(nSampleRate==2) //Timer setup for 25ksps
	{
		htim1.Init.Prescaler = 0; 
		htim1.Init.Period = 2000-1;  //Period = 50MHz/2000
	}
	else if(nSampleRate==3) //Timer setup for 50ksps
	{
		htim1.Init.Prescaler = 0; 
		htim1.Init.Period = 1000-1;  //Period = 50MHz/1000
	}
	
	__HAL_SPI_RESET_CRC(&hspi1);	//resets the SPI1 peripheral
	__HAL_SPI_ENABLE(&hspi1);			//enable the SPI1 peripheral

	g_uchReadADCStop=0;	//block sampling is done when g_uchReadADCStop becomes 1
	HAL_TIM_Base_Init(&htim1);	//initializes timer with the above settings
	HAL_TIM_Base_Start_IT(&htim1);	//starts the timer and enables interrupt
	
	/*
	 * wait until the sampling is done
	 */
	while(g_uchReadADCStop==0)
	{
	}
	return unSampleSize;
}

//Timer's interrupt event handler
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//Low-level commands are used to optimize the sampling rate
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);	//Set CS pin low
	SPI1->DR = g_auWriteBuffer[0];	// write the first word of the SPI command to the SPI DR register
	while(!__HAL_SPI_GET_FLAG(&hspi1,SPI_FLAG_TXE));	//wait until the TX empty flag asserts
	SPI1->DR = g_auWriteBuffer[1];	// write the second word of the SPI command to the SPI DR register
	while(!__HAL_SPI_GET_FLAG(&hspi1,SPI_FLAG_RXNE));	//wait until the RX not empty flag asserts
	g_auSamples[g_auCount]=SPI1->DR;	//read the DR register
	while(!__HAL_SPI_GET_FLAG(&hspi1,SPI_FLAG_RXNE)); //wait until the RX not empty flag asserts
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);		//Set CS pin high
	g_auSamples[g_auCount]=SPI1->DR;  //read the DR register for ADC data
	if(g_auCount==g_unSampleSize)
	{
		g_uchReadADCStop=1;
		HAL_TIM_Base_Stop_IT(&htim1);	//stops the timer
	}
	/*if(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_RXNE))
	{
		if(getchar()==0x1B)
		{
			g_uchReadADCStop=1;
			HAL_TIM_Base_Stop_IT(&htim1);	//stops the timer
		}
	}*/
	g_auCount++;
}

void ContinuousSampling(uint8_t uchADCChannel)
/**
* \brief       Continuously reads the ADC and displays the data on the Terminal
* \par         Details
*              This function reads the ADC every half of a second and displays the data
* \n           to the Terminal.  Press the ESC key to return to the main menu.
*
* \param[in]   uchADCChannel     - Channel number
*
* \retval      None
*/
{
	uint8_t uchInput=0;
	uint8_t firstsample=1;

	printf("Press the ESC key to stop sampling\n\n\r");
	printf("\nChannel %d\n\r", uchADCChannel);

	g_auWriteBuffer[1]=0;
	
	while(uchInput!=0x1B)
	{
		g_auWriteBuffer[0] = (0x80 + (uchADCChannel<<4))<<8;	//SPI read command for the selected ADC channel
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		HAL_SPI_TransmitReceive(&hspi1, (uint8_t*) g_auWriteBuffer, (uint8_t*) g_auReadBuffer, 2, 10000);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);

		if(firstsample==1)   // Skip very first sample
		{
			firstsample=0;
		}
		else
		{
			printf("%i\n\r", g_auReadBuffer[1]);
		}

		Delay(ONE_SECOND/2);	//500ms delay

		if(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_RXNE))
			uchInput=getchar();
	}
}

void ADCInit(void)
/**
* \brief       Configure ADC input range for each input channel
* \par         Details
*              This function configures the ADC inputs
* \n           Commented examples are provided to easily change the input range of each channel.
*
* \param       None
*
* \retval      None
*/
{
	// MAX1301 ADC Input Range Configuration
    // Initialize the MAX1301 for AIN0=+/-10V, AIN1=+/-10V, AIN2=0 to 5V, AIN3=0 to 10V
	//
	// +/-10V input Votage Equation => -12.288 + (375uV x Code) = Input Voltage
	// 4-20mA/0V to 5V input input Votage Equation => (93.75uV x Code) = Input Voltage
	// 0 to 10V input input Votage Equation => (187.5uV x Code) = Input Voltage
	//
	// The higher four bits is the channel,  0x87 where 8 is AIN0
	// The lower four bits is the input range, 0x87 where 7 is -10V to 10V
	// Input Range Values are:
	// 0xX3 =>   0V to  5V
	// 0xX4 =>  -5V to +5V
	// 0xX6 =>   0V to 10V
	// 0xX7 => -10V to 10V
	// 6VREF -10V to 10V AIN0 => 0x87
	// 6VREF -10V to 10V AIN1 => 0x97
	// 3VREF/2 0V to  5V AIN2 => 0xA3
	// 3VREF   0V to 10V AIN3 => 0xB6

// Channel 0
//					g_auWriteBuffer[0]=0x8700;   // -10V to 10V AIN0 => 0x87
	g_auWriteBuffer[0]=0x8600;   //   0V to 10V AIN0 => 0x86
//          g_auWriteBuffer[0]=0x8400;   //  -5V to  5V AIN0 => 0x84
//          g_auWriteBuffer[0]=0x8300;   //   0V to  5V AIN0 => 0x83
	g_auWriteBuffer[1]=0;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi1, (uint8_t*) g_auWriteBuffer, (uint8_t*) g_auReadBuffer, 4, 10000);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	
// Channel 1
//	g_auWriteBuffer[0]=0x9700;   // -10V to 10V AIN1 => 0x97
	g_auWriteBuffer[0]=0x9600;   //   0V to 10V AIN1 => 0x96
//	g_auWriteBuffer[0]=0x9400;   //  -5V to  5V AIN1 => 0x94
//	g_auWriteBuffer[0]=0x9300;   //   0V to  5V AIN1 => 0x93
	g_auWriteBuffer[1]=0;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi1, (uint8_t*) g_auWriteBuffer, (uint8_t*) g_auReadBuffer, 4, 10000);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);

// Channel 2
//	g_auWriteBuffer[0]=0xA300;   //    0V to 5V AIN2 => 0xA3
	g_auWriteBuffer[0]=0xA700;   // -10V to 10V AIN2 => 0xA7
//	g_auWriteBuffer[0]=0xA600;   //   0V to 10V AIN2 => 0xA6
//	g_auWriteBuffer[0]=0xA400;   //  -5V to  5V AIN2 => 0xA4
	g_auWriteBuffer[1]=0;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi1, (uint8_t*) g_auWriteBuffer, (uint8_t*) g_auReadBuffer, 4, 10000);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);

// Channel 3
//	g_auWriteBuffer[0]=0xB600;  	//   0V to 10V AIN3 => 0xB6
	g_auWriteBuffer[0]=0xB700;   // -10V to 10V AIN3 => 0xB7
//	g_auWriteBuffer[0]=0xB400;   //  -5V to  5V AIN3 => 0xB4
//	g_auWriteBuffer[0]=0xB300;   //   0V to  5V AIN3 => 0xB3
	g_auWriteBuffer[1]=0;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi1, (uint8_t*) g_auWriteBuffer, (uint8_t*) g_auReadBuffer, 4, 10000);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
}
