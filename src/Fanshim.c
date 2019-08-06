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

#include <wiringPi.h>
#include <RaspiAPA102/APA102.h>
#include <RaspiFanshim/Fanshim.h>

/* ============================================================================================== */
/* Statics                                                                                        */
/* ============================================================================================== */

static RaspiAPA102Device G_FANSHIM_LED;

/* ============================================================================================== */
/* Constants                                                                                      */
/* ============================================================================================== */

#define RASPI_FANSHIM_PIN_FAN       18
#define RASPI_FANSHIM_PIN_SPI_SCLK  14
#define RASPI_FANSHIM_PIN_SPI_MOSI  15

/* ============================================================================================== */
/* Exported functions                                                                             */
/* ============================================================================================== */

/* ---------------------------------------------------------------------------------------------- */
/* Initialization                                                                                 */
/* ---------------------------------------------------------------------------------------------- */

void RaspiFanshimInit(void)
{
    wiringPiSetupGpio();
    pinMode(RASPI_FANSHIM_PIN_FAN, OUTPUT);

    RaspiAPA102DeviceInitSoftware(&G_FANSHIM_LED, RASPI_FANSHIM_PIN_SPI_SCLK, 
        RASPI_FANSHIM_PIN_SPI_MOSI, -1);
}

/* ---------------------------------------------------------------------------------------------- */
/* FAN control                                                                                    */
/* ---------------------------------------------------------------------------------------------- */

bool RaspiFanshimIsFanEnabled(void)
{
    const int value = digitalRead(RASPI_FANSHIM_PIN_FAN);

    return (value == LOW) ? false : true;
}

void RaspiFanshimEnableFan(bool enabled)
{
    digitalWrite(RASPI_FANSHIM_PIN_FAN, enabled ? HIGH : LOW);
}

void RaspiFanshimToggleFan(void)
{
    RaspiFanshimEnableFan(!RaspiFanshimIsFanEnabled());
}

/* ---------------------------------------------------------------------------------------------- */
/* LED control                                                                                    */
/* ---------------------------------------------------------------------------------------------- */

void RaspiFanshimUpdateLED(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness)
{
    const RaspiAPA102ColorQuad color = RASPI_APA102_COLOR_QUAD_INITIALIZER(r, g, b, brightness);

    RaspiAPA102DeviceUpdate(&G_FANSHIM_LED, &color, 1);
}

/* ---------------------------------------------------------------------------------------------- */

/***************************************************************************************************/
