/***************************************************************************************************

  Raspberry Pi Fanshim Service

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

#include <stdio.h>
#include <unistd.h>
#include <RaspiAPA102/RaspiAPA102.h>
#include <RaspiFanshim/RaspiFanshim.h>

/* ============================================================================================== */
/* Internal Functions                                                                             */
/* ============================================================================================== */

/**
 * @brief   Interpolates two color values using the given ratio.
 * 
 * @param   color_from  The start color.
 * @param   color_to    The target color.
 * @param   ratio       The current ratio (between `0.0f` and `1.0`f).
 * 
 * @return  The interpolated color. 
 */
static RaspiAPA102ColorQuad InterpolateColor(const RaspiAPA102ColorQuad color_from, 
    const RaspiAPA102ColorQuad color_to, double ratio)
{
    const uint8_t br_f = RASPI_APA102_COLOR_QUAD_BRIGHTNESS(color_from); 
    const uint8_t br_t = RASPI_APA102_COLOR_QUAD_BRIGHTNESS(color_to); 

    const uint8_t br_n = br_f + (br_t - br_f) * ratio; 
    const uint8_t  r_n = color_from.r + (color_to.r - color_from.r) * ratio; 
    const uint8_t  g_n = color_from.g + (color_to.g - color_from.g) * ratio;  
    const uint8_t  b_n = color_from.b + (color_to.b - color_from.b) * ratio;   

    const RaspiAPA102ColorQuad result = RASPI_APA102_COLOR_QUAD_INITIALIZER(r_n, g_n, b_n, br_n);  
    return result;
}

/**
 * @brief   Returns the current CPU temperature.
 * 
 * @return  The current CPU temperature.
 */
static float ReadCPUTemperature()
{
    FILE* file = fopen ("/sys/class/thermal/thermal_zone0/temp", "r");
    if (file == NULL)
    {
        return 0;
    }

    double result;
    fscanf(file, "%lf", &result);
    fclose(file);
    
    return result / 1000;
}

/**
 * @brief   x
 * 
 * @return  x 
 */
static float CalcPercentualRatioInRange(double lo, double hi, double value)
{
    if (value < lo)
    {
        return 0.0f;
    } 
    if (value > hi)
    {
        return 1.0f;
    } 
    
    return (value - lo) / (hi - lo);
}

/* ============================================================================================== */
/* Entry Point                                                                                    */
/* ============================================================================================== */

void main(void)
{
    /* ////////////////////////////////////////////////////////////////////////////////////////// */

    // The update/check interval in miliseconds. Choose higher values for faster reaction time, or
    // lower values for reduced CPU usage.
    const int update_interval = 2500;
    // The required amount of consecutive temperature reads above the upper threshold for the fan  
    // to start
    const int required_measure_count = 3;
    // The temperature threshold for the fan to start
    const double threshold_temp_hi = 65.0f;
    // The temperature threshold for the fan to stop 
    const double threshold_temp_lo = 40.0f;
    // The low temperature color
    const RaspiAPA102ColorQuad color_temp_lo = 
        RASPI_APA102_COLOR_QUAD_INITIALIZER(0, 255, 0, 16);
    // The high temperature color
    const RaspiAPA102ColorQuad color_temp_hi = 
        RASPI_APA102_COLOR_QUAD_INITIALIZER(255, 0, 0, 16);
    // The number of color interpolation steps. Choose higher values for smoother animation, or
    // lower values for reduced CPU usage.
    const int color_interpolation_steps = 10;
    
    /* ////////////////////////////////////////////////////////////////////////////////////////// */

    RaspiFanshimInit();

    const int delay = (int)((float)update_interval / color_interpolation_steps);

    int measure_count = 0;
    RaspiAPA102ColorQuad color_last = InterpolateColor(color_temp_lo, color_temp_hi, 0.0f);
    while (true)
    {
        const double temp = ReadCPUTemperature();
        printf ("The temperature is %6.3f C, measure count is %d\n", temp, measure_count);

        if (temp >= threshold_temp_hi && !RaspiFanshimIsFanEnabled())
        {
            if (++measure_count == required_measure_count)
            {
                RaspiFanshimEnableFan(true);
            }
        } 
        else
        {
            measure_count = 0;  
        }
        
        if (temp <= threshold_temp_lo && RaspiFanshimIsFanEnabled())
        {
            RaspiFanshimEnableFan(false);
        }

        printf ("New measure count is %d, fan enabled %d\n", measure_count, 
            RaspiFanshimIsFanEnabled());

        const double ratio = CalcPercentualRatioInRange(threshold_temp_lo, threshold_temp_hi, temp);
        
        printf ("The temperature is %6.3f C, ratio is %2.2f\n", temp, ratio);

        const RaspiAPA102ColorQuad color_fade_to = InterpolateColor(color_temp_lo, color_temp_hi, 
            ratio); 

        RaspiAPA102ColorQuad color_fade_from = color_last;
        for (int i = 1; i <= color_interpolation_steps; ++i)
        {
            const double ratio = (double)i / color_interpolation_steps;
            const RaspiAPA102ColorQuad color = InterpolateColor(color_last, color_fade_to, ratio);

            if (RASPI_APA102_COLOR_QUAD_R(color) != RASPI_APA102_COLOR_QUAD_R(color_fade_from) || 
                RASPI_APA102_COLOR_QUAD_G(color) != RASPI_APA102_COLOR_QUAD_G(color_fade_from) || 
                RASPI_APA102_COLOR_QUAD_B(color) != RASPI_APA102_COLOR_QUAD_B(color_fade_from))
            {
                RaspiFanshimUpdateLED(RASPI_APA102_COLOR_QUAD_R(color),RASPI_APA102_COLOR_QUAD_G(color),
                    RASPI_APA102_COLOR_QUAD_B(color), RASPI_APA102_COLOR_QUAD_BRIGHTNESS(color));
                color_fade_from = color;

                printf ("Color is %02d %02d %02d, Target color is %02d %02d %02d\n", 
                    RASPI_APA102_COLOR_QUAD_R(color),
                    RASPI_APA102_COLOR_QUAD_G(color),
                    RASPI_APA102_COLOR_QUAD_B(color), 
                    RASPI_APA102_COLOR_QUAD_R(color_fade_to),
                    RASPI_APA102_COLOR_QUAD_G(color_fade_to),
                    RASPI_APA102_COLOR_QUAD_B(color_fade_to));
            }

            usleep(1000 * delay);
        }

        color_last = color_fade_to;
    }
}

/* ============================================================================================== */