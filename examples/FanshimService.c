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
#include <math.h>
#include <unistd.h>
#include <RaspiAPA102/APA102.h>
#include <RaspiAPA102/ColorConversion.h>
#include <RaspiFanshim/Fanshim.h>

/* ============================================================================================== */
/* Internal Functions                                                                             */
/* ============================================================================================== */

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

/**
 * @brief   Updates the `Fanshim` LED color to the given `hue` value and brightness.
 * 
 * @param   hue         The hue color component.
 * @param   brightness  The LED brightness.
 */
static void UpdateFanshimLED(double hue, uint8_t brightness)
{
    const RaspiAPA102HSV color_hsv = 
    {
        hue,
        1.0f,
        1.0f
    };    
    const RaspiAPA102RGB color = RaspiAPA102HSV2RGB(color_hsv);
    RaspiFanshimUpdateLED(floor(color.r * 255), floor(color.g * 255), floor(color.b * 255), 
        brightness);
}

/* ============================================================================================== */
/* Entry Point                                                                                    */
/* ============================================================================================== */

int main(void)
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
    const double threshold_temp_lo = 50.0f;
    // The LED brightness
    const uint8_t led_brightness = 8;
    // The low temperature color
    const RaspiAPA102RGB color_temp_lo_rgb = 
    {
          0 / 255, 
        255 / 255,
          0 / 255
    };
    // The high temperature color
    const RaspiAPA102RGB color_temp_hi_rgb = 
    {
        255 / 255, 
          0 / 255,
          0 / 255
    };
    // The number of animation steps. Choose higher values for smoother animation, or lower values 
    // for reduced CPU usage.
    const int animation_steps = 10;
    
    /* ////////////////////////////////////////////////////////////////////////////////////////// */

    const RaspiAPA102HSV color_temp_lo_hsv = RaspiAPA102RGB2HSV(color_temp_lo_rgb);
    const RaspiAPA102HSV color_temp_hi_hsv = RaspiAPA102RGB2HSV(color_temp_hi_rgb);

    const int delay = (int)((float)update_interval / animation_steps);
    const double hue_base = color_temp_lo_hsv.h;
    const double hue_delta_total = color_temp_hi_hsv.h - color_temp_lo_hsv.h;

    RaspiFanshimInit(); 

    // Initial color update
    const double temp = ReadCPUTemperature();
    const double factor = CalcPercentualRatioInRange(threshold_temp_lo, threshold_temp_hi, temp);
    double hue_value_last = hue_delta_total * factor;
    UpdateFanshimLED(hue_base + hue_value_last, led_brightness);

    int measure_count = 0;
    while (true)
    {
        const double temp = ReadCPUTemperature();

        // Fan logic
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

        // LED logic
        const double factor = CalcPercentualRatioInRange(threshold_temp_lo, threshold_temp_hi, 
            temp);
        const double hue_value_target = hue_delta_total * factor;
        const double hue_delta_local  = hue_value_target - hue_value_last;

        if (fabs(hue_value_target - hue_value_last) < 1.0f)
        {
            usleep(1000 * delay * animation_steps);
            continue;
        }

        double hue_value_check = hue_value_last;
        for (int i = 1; i <= animation_steps; ++i)
        {
            const double factor = (double)i / animation_steps;
            const double hue_value_current = hue_delta_local * factor;

            if (fabs(hue_value_check - hue_value_current) > 1.0f)
            {
                UpdateFanshimLED(hue_base + hue_value_last + hue_value_current, led_brightness);
                hue_value_check = hue_value_current;
            }

            usleep(1000 * delay);
        }

        hue_value_last = hue_value_target;
    }

    return 0;
}

/* ============================================================================================== */