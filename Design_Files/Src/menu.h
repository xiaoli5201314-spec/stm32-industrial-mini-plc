/** \file menu.h ***********************************************************
 * 
 *             Project: MAXREFDES61#
 *            Filename: menu.h
 *         Description: This module contains all the functions used to
 *                      generate the menus and menu options used to run the
 *                      MAXREFDES61 example firmware.
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

#ifndef MENU_H_
#define MENU_H_

#include "stm32f4xx_hal.h"
#include "stdio.h"

#define MAIN_MENU 0                 //!< // Menu state machine state
#define WAIT_KEYPRESS 1             //!< // Menu state machine state
#define MENU_SAMPLE_SPEED 2			//!< // Menu state machine state
#define MENU_SAMPLE_SIZE 3			//!< // Menu state machine state
#define START_CONTINUOUS_SAMPLING 4		//!< // Menu state machine state
#define START_BLOCK_SAMPLING 5			//!< // Menu state machine state
#define MENU_CHANNEL 6
#define BASE_FUNCTION_STATE 10      //!< // Menu state machine state

#define KEYPRESS_ARROW_UP 240       //!< Assign up-arrow an extended ascii code which won't be used elsewhere
#define KEYPRESS_ARROW_DOWN 241     //!< Assign up-arrow an extended ascii code which won't be used elsewhere
#define KEYPRESS_ARROW_LEFT 242     //!< Assign up-arrow an extended ascii code which won't be used elsewhere
#define KEYPRESS_ARROW_RIGHT 243    //!< Assign up-arrow an extended ascii code which won't be used elsewhere
#define KEYPRESS_END 244            //!< Assign up-arrow an extended ascii code which won't be used elsewhere

// Function Prototypes
void menu_cls(void);
void menu_print_maxim_banner(void);
void menu_print_maxim_banner_big(void);
void menu_print_prompt(void);
uint8_t menu_retrieve_keypress(uint32_t nUartAddress);
void menu_print_main_menu(void);
void menu_print_channel_menu(void);
void menu_print_sample_rate_menu(void);
void menu_print_sample_size_menu(void);

#endif /* MENU_H_ */
