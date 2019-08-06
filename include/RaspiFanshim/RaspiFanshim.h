/***************************************************************************************************

  Raspberry Pi Fanshim Library

  Original Author : Florian Bernd

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.

***************************************************************************************************/

/**
 * @file
 * @brief   Provides functions to control the pimoroni `Fan SHIM` device on a Raspberry Pi.
 */

#ifndef RASPI_FANSHIM_H
#define RASPI_FANSHIM_H

#include <stdbool.h>
#include <stdint.h>
#include <RaspiFanshimExportConfig.h>

/* ============================================================================================== */
/* Exported functions                                                                             */
/* ============================================================================================== */

/* ---------------------------------------------------------------------------------------------- */
/* Initialization                                                                                 */
/* ---------------------------------------------------------------------------------------------- */

/**
 *  @brief  Initializes the `Fan SHIM` device.
 * 
 *  This function sets the GPIO pins `14`, `15`, and `18` to `OUTPUT` mode.
 */
RASPI_FANSHIM_EXPORT void RaspiFanshimInit(void);

/* ---------------------------------------------------------------------------------------------- */
/* FAN control                                                                                    */
/* ---------------------------------------------------------------------------------------------- */

/**
 * @brief   Queries the current status of the fan.
 * 
 * @return  `True`, if the fan is currently running or `false`, if not.
 */
RASPI_FANSHIM_EXPORT bool RaspiFanshimIsFanEnabled(void);

/**
 * @brief   Enables or disables the fan.
 * 
 * @param   enabled Pass `true` to enable the fan, `false` to disable it.
 */
RASPI_FANSHIM_EXPORT void RaspiFanshimEnableFan(bool enabled);

/**
 * @brief   Toggles the fan.
 */
RASPI_FANSHIM_EXPORT void RaspiFanshimToggleFan(void);

/* ---------------------------------------------------------------------------------------------- */
/* LED control                                                                                    */
/* ---------------------------------------------------------------------------------------------- */

/**
 * @brief   Updates the color of the `Fan SHIM`s integrated `APA102` LED.
 * 
 * @param   r           The red color component.
 * @param   g           The green color component.
 * @param   b           The blue color component.
 * @param   brightness  The LED brightness (0..31).
 */
RASPI_FANSHIM_EXPORT void RaspiFanshimUpdateLED(uint8_t r, uint8_t g, uint8_t b, 
    uint8_t brightness);

/* ---------------------------------------------------------------------------------------------- */

/* ============================================================================================== */

#endif /* RASPI_FANSHIM_H */
