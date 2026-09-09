/** \file menu.c ***********************************************************
 * 
 *             Project: MAXREFDES61#
 *            Filename: menu.c
 *         Description: This module contains all the functions used to
 *                      generate the menus and menu options used to run the
 *                      MAXREFDES61# example firmware.
 * 
 *    Revision History:
 *\n                    06-13-14    Rev 01.00    GL    Initial Release
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

#include "stdio.h"
#include "menu.h"

void menu_cls()
/**
* \brief       Function to clear the screen via Hyperterminal
*
* \param       None
*
* \retval      None
*/
{
	// The following code will cause a clear screen event on Hyperterminal and many other terminal emulators
	printf("\033[2J");
}


void menu_print_maxim_banner()
/**
* \brief       Print standard Maxim banner at top of Hyperterminal screen
*
* \param       None
*
* \retval      None
*/
{
	printf( "\r\n");
	printf( "///////////////////////////////////////////////////////////////////\r\n");
	printf( "//                        _____    _____                         //\r\n");
	printf( "//                       /  __ \\  / __  \\                        //\r\n");
	printf( "//                       | |  \\/ / /  | |                        //\r\n");
	printf( "//                       | |    / /   | |                        //\r\n");
	printf( "//                       | |   /  \\   | |                        //\r\n");
	printf( "//                       | |  / /\\ \\  | |                        //\r\n");
	printf( "//                       |_| /_/  \\_\\ |_|                        //\r\n");
	printf( "//                                                               //\r\n");
	printf( "///////////////////////////////////////////////////////////////////\r\n");
	printf("\r\n");
}


void menu_print_maxim_banner_big()
/**
* \brief       Print large Maxim banner at top of Hyperterminal screen
*
* \param       None
*
* \retval      None
*/
{
	printf( "\r\n");
	printf( "///////////////////////////////////////////////////////////////////\r\n");
	printf( "//                          ###   ###                            //\r\n");
	printf( "//                      ##             ##                        //\r\n");
	printf( "//                                                               //\r\n");
	printf( "//                   ##   _____    _____   ##                    //\r\n");
	printf( "//                       /  __ \\  / __  \\                        //\r\n");
	printf( "//                  ##   | |  \\/ / /  | |   ##                   //\r\n");
	printf( "//                       | |    / /   | |                        //\r\n");
	printf( "//                       | |   /  \\   | |                        //\r\n");
	printf( "//                  ##   | |  / /\\ \\  | |   ##                   //\r\n");
	printf( "//                       |_| /_/  \\_\\ |_|                        //\r\n");
	printf( "//                   ##                    ##                    //\r\n");
	printf( "//                                                               //\r\n");
	printf( "//                      ##             ##                        //\r\n");
	printf( "//                          ###   ###                            //\r\n");
	printf( "//                                                               //\r\n");
	printf( "//                M A X I M   I N T E G R A T E D                //\r\n");
	printf( "//                                                               //\r\n");
	printf( "///////////////////////////////////////////////////////////////////\r\n");

	printf("\r\n\r\n");
}

void menu_print_prompt()
/**
* \brief       Print a standard prompt for keyboard input "  > "
*
* \param       None
*
* \retval      None
*/
{
	printf("\r\n>> ");
	fflush(stdout);
}


void menu_print_line()
/**
* \brief       Print one line of dashes across the screen via Hyperterminal
*
* \param       None
*
* \retval      None
*/
{
	printf("----------------------------------------------------\r\n\r\n");
}


uint8_t menu_retrieve_keypress(uint32_t nUartAddress)
/**
* \brief       Get a single keypress via Hyperterminal.
* \par         Details
*              Returns ascii character corresponding to keypress 
*
* \param[in]   nUartAddress        - address of the UART peripheral in MicroBlaze memory map
*
* \retval      Character, partially decoded.
*/
{
	uint8_t uchInput;

	uchInput = getchar();

	if(uchInput==27)  // Escape sequence (likely an arrow key)
	{
		uchInput = getchar();
		if(uchInput==91)  // Left bracket (part #2 of the 3 part escape sequence)
		{
			uchInput = getchar();
			if(uchInput==65)
				uchInput = KEYPRESS_ARROW_UP-10;
			if(uchInput==66)
				uchInput = KEYPRESS_ARROW_DOWN-10;
			if(uchInput==67)
				uchInput = KEYPRESS_ARROW_RIGHT-10;
			if(uchInput==68)
				uchInput = KEYPRESS_ARROW_LEFT-10;
			if(uchInput==75)
				uchInput = KEYPRESS_END - 10;
		}
	}
	else if(uchInput>=48 && uchInput <=57) // digits 0..9
		uchInput -=48; // subtract 48 to get the real number converted from the ASCII digit
	else if(uchInput>=97 && uchInput<=122)
		uchInput -=32;  // convert lowercase to uppercase

	return(uchInput);
}

void menu_print_main_menu()
/**
* \brief       Print the main menu listing choice of module to test
* \par         Details.
*
* \retval      None
*/
{
	menu_print_maxim_banner();

	printf("Press a number to select sampling mode:\r\n");

	printf("{0} Continuous Sampling \n\r"\
		"{1} Block Sampling\n\r");
	menu_print_prompt();
}

void menu_print_channel_menu()
{
	menu_print_maxim_banner();

	printf("Press a number to select analog input channel:\r\n");

	printf("{0} Channel 0 \n\r"\
		"{1} Channel 1\n\r"\
		"{2} Channel 2\n\r"\
		"{3} Channel 3\n\r");
	menu_print_prompt();
}

void menu_print_sample_rate_menu()
/**
* \brief       Print a menu listing sample speed choices
* \par         Details
*
* \retval      None
*/
{
	menu_print_maxim_banner();

	printf("Press a number to select the sample rate:\r\n");

	printf("{0} 1ksps\n\r"\
		"{1} 10ksps\n\r"\
		"{2} 20ksps\n\r"\
		"{3} 50ksps\n\r");
	menu_print_prompt();
}

void menu_print_sample_size_menu()
/**
* \brief       Print a menu listing sample size choices
* \par         Details
*
* \retval      None
*/
{
	menu_print_maxim_banner();

	printf("Press a key/number to select the sample size:\r\n");

	printf("{0} 8192\n\r"\
		"{1} 16384\n\r"\
		"{2} 32768\n\r"\
		"{3} 65536\n\r");
	menu_print_prompt();
}
