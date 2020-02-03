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
#include <BSP_Components.h>

#include <SoftwareTimer.h>
#include <bsec_integration.h>


struct SwTimer bsec_timer;
bsec_env_process_struct bsec_process_params = APP_BSEC_PROCESS_STRUCT_VALUE;


int main(void)
{
    BDK_Initialize();

    LED_Initialize(LED_RED);
    LED_Initialize(LED_GREEN);
    LED_Initialize(LED_BLUE);

    BTN_Initialize(BTN0);

    printf("\r\nAPP: BME680_BSEC example.\r\n");

    /* Clear saved BSEC state if button is pressed after reset. */
    if (BTN_Read(BTN0) == BTN_PRESSED)
    {
        I2CEeprom m24rf64;
        uint8_t buffer[APP_BSEC_EEPROM_MAGIC_SIZE] = { 0 };

        I2CEeprom_Initialize(APP_BSEC_EEPROM_I2C_ADDR,
                APP_BSEC_EEPROM_PAGE_SIZE, &m24rf64);

        I2CEeprom_Write(APP_BSEC_EEPROM_ADDR, buffer,
                APP_BSEC_EEPROM_MAGIC_SIZE, &m24rf64);

        printf("APP: Deleted saved BSEC state.\r\n");
    }

    /* Initialize BSEC library */
    {
        bsec_env_return_val retval;
        bsec_env_init_struct bsec_init_params = APP_BSEC_INIT_STRUCT_VALUE;

        retval = BSEC_ENV_Initialize(&bsec_init_params);

        ASSERT_DEBUG(retval.bme680_status == BME680_OK);
        ASSERT_DEBUG(retval.bsec_status == BSEC_OK);
    }

    LED_On(LED_GREEN);
    HAL_Delay(500);
    LED_Off(LED_GREEN);

    SwTimer_Initialize(&bsec_timer);
    SwTimer_ExpireInMs(&bsec_timer, 1);

    printf("APP: Entering main loop.\r\n");
    while (1)
    {
        /* Execute any events that occurred and refresh Watchdog. */
        BDK_Schedule();

        if (SwTimer_IsExpired(&bsec_timer) == true)
        {
            int64_t next_call;

            next_call = BSEC_ENV_Process(&bsec_process_params);
            SwTimer_ExpireInMs(&bsec_timer, next_call);

            printf("BSEC: Next process call in: %lu ms.\r\n",
                    (uint32_t) next_call);

            LED_Toggle(LED_GREEN);
        }

        SYS_WAIT_FOR_INTERRUPT;
    }

    return 0;
}
