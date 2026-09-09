/** \file maximDeviceSpecificUtilities.h ****************************************
 *
 *             Project: MAXREFDES61#
 *            Filename: maximDeviceSpecificUtilities.h
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

#ifndef MAXIMDEVICESPECIFICUTILITIES_H_
#define MAXIMDEVICESPECIFICUTILITIES_H_

#include "stm32f4xx_hal.h"

uint32_t StartSampling(uint8_t uchADCChannel, uint32_t unSampleSize, int nSampleRate, uint16_t *auSamples);
void ContinuousSampling(uint8_t uchADCChannel);
void ADCInit(void);


#endif /* MAXIMDEVICESPECIFICUTILITIES_H_ */
