//-----------------------------------------------------------------------------
// Copyright (c) 2018 Semiconductor Components Industries LLC
// (d/b/a "ON Semiconductor").  All rights reserved.
// This software and/or documentation is licensed by ON Semiconductor under
// limited terms and conditions.  The terms and conditions pertaining to the
// software and/or documentation are available at
// http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf ("ON Semiconductor Standard
// Terms and Conditions of Sale, Section 8 Software") and if applicable the
// software license agreement.  Do not use this software and/or documentation
// unless you have carefully read and you agree to the limited terms and
// conditions.  By using this software and/or documentation, you agree to the
// limited terms and conditions.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <BDK.h>

#include <BSEC_ENV.h>
#include <timermath.h>
#include <ansi_color.h>
#include <bsec_integration.h>
#include <I2CEeprom.h>
#include <math.h>

//#define APP_USE_ANSI_COLORS


/* Delays program execution for given amount of time. */
void App_BsecSleep(uint32_t t_ms)
{
    HAL_Delay(t_ms);
}

void App_BsecOutputReady(bsec_env_output_struct *output)
{
//    printf("[2J"); // clear screen
    printf("BSEC output:\r\n");

    printf("  timestamp = %lu ms\r\n", (uint32_t)(output->timestamp / 1000000));

#ifdef APP_USE_ANSI_COLORS
    printf("  iaq = " COLORIZE("%.0f", GREEN) " (%d)\r\n", output->iaq,
            output->iaq_accuracy);

    printf("  temperature = " COLORIZE("%.2f °C", RED) " (%d)\r\n",
            output->temperature, output->temperature_accuracy);

    printf("  humidity = " COLORIZE("%.2f %%", CYAN) " (%d)\r\n",
            output->humidity, output->humidity_accuracy);
#else
    printf("  iaq = %.0f (%d)\r\n", output->iaq, output->iaq_accuracy);

    printf("  temperature = %.2f °C (%d)\r\n",
            output->temperature, output->temperature_accuracy);

    printf("  humidity = %.2f %% (%d)\r\n",
            output->humidity, output->humidity_accuracy);

#endif /* APP_USE_ANSI_COLORS */
    printf("  raw_pressure = %f Pa\r\n", output->raw_pressure);

    float bar = (output->raw_pressure) * 1e-5; // ALTITUDE
    float altitude = -8005 * log(bar); // ALTITUDE
    printf("  altitude = %f mts\r\n", altitude); // ALTITUDE

    printf("  co2_equivalent = %.2f ppm (%d)\r\n", output->co2_equivalent,
            output->co2_accuracy);

    printf("  breath_voc_equivalent = %.2f ppm (%d)\r\n",
            output->breath_voc_equivalent,
            output->breath_voc_accuracy);

    printf("\r\n\n");
}

/* Retrieves current system time stamp.
 *
 * The system uses 32-bit counter incremented every ms.
 * Therefore some additional logic was added to handle counter overflow and
 * expands the time stamp into 64-bit value.
 */
int64_t App_BsecGetTimestampUs()
{
    static int64_t timestamp = 0;
    static struct tm_math math;
    static uint32_t last = 0;

    uint32_t now, diff;

    if (timestamp == 0)
    {
        tm_initialize(&math, UINT32_MAX);
        last = 0;
    }

    now = HAL_Time();
    diff = tm_get_diff(&math, now, last);
    last = now;

    timestamp += diff;

    return timestamp * 1000;
}

uint32_t App_BsecStateLoadFromEEPROM(uint8_t *state_buffer, uint32_t buffer_len)
{
    int32_t retval;
    uint8_t magic[APP_BSEC_EEPROM_MAGIC_SIZE] = APP_BSEC_EEPROM_MAGIC;
    uint8_t magic_read[APP_BSEC_EEPROM_MAGIC_SIZE] = APP_BSEC_EEPROM_MAGIC;
    I2CEeprom m24rf64;

    I2CEeprom_Initialize(APP_BSEC_EEPROM_I2C_ADDR, 4, &m24rf64);

    retval = I2CEeprom_Read(APP_BSEC_EEPROM_ADDR, magic_read,
            APP_BSEC_EEPROM_MAGIC_SIZE, &m24rf64);
    if (retval == I2C_EEPROM_OK)
    {
        if (memcmp(magic, magic_read, APP_BSEC_EEPROM_MAGIC_SIZE - 1) == 0)
        {
            uint32_t saved_size = magic_read[APP_BSEC_EEPROM_MAGIC_SIZE - 1];

            if (saved_size != 0 && saved_size <= buffer_len)
            {
                retval = I2CEeprom_Read(
                        APP_BSEC_EEPROM_ADDR + APP_BSEC_EEPROM_MAGIC_SIZE,
                        state_buffer, saved_size, &m24rf64);
                if (retval == I2C_EEPROM_OK)
                {
                    printf("BSEC: Loaded BSEC state from EEPROM memory.\r\n");
                    return saved_size;
                }
            }
        }
    }

    printf("BSEC: Failed to load BSEC state from EEPROM memory.\r\n");
    return 0;
}

void App_BsecStateSaveToEEPROM(uint8_t *state_buffer, uint32_t size)
{
    int32_t retval;
    uint8_t magic[APP_BSEC_EEPROM_MAGIC_SIZE] = APP_BSEC_EEPROM_MAGIC;
    magic[APP_BSEC_EEPROM_MAGIC_SIZE - 1] = size;
    I2CEeprom m24rf64;

    I2CEeprom_Initialize(APP_BSEC_EEPROM_I2C_ADDR, 4, &m24rf64);

    retval = I2CEeprom_Write(APP_BSEC_EEPROM_ADDR, magic,
            APP_BSEC_EEPROM_MAGIC_SIZE, &m24rf64);
    if (retval == I2C_EEPROM_OK)
    {
        retval = I2CEeprom_Write(
                APP_BSEC_EEPROM_ADDR + APP_BSEC_EEPROM_MAGIC_SIZE, state_buffer,
                size, &m24rf64);
    }

    if (retval == I2C_EEPROM_OK)
    {
        printf("Saved BSEC state to EEPROM.\r\n");
    }
    else
    {
        printf(
                "BSEC: Error while saving BSEC state to EEPROM. (errcode=%ld)\r\n",
                retval);
    }
}
