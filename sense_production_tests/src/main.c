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
#include <BDK.h>
#include <BSP_Components.h>
#include <BLE_Components.h>
#include <ansi_color.h>
#include <SEGGER_RTT.h>

#include <stdio.h>
#include <math.h>

#define AUDIO_DMIC0_GAIN                0x800
#define AUDIO_DMIC1_GAIN                0x800
#define AUDIO_OD_GAIN                   0x800

#define AUDIO_CONFIG                    (OD_AUDIOSLOWCLK            | \
                                         DMIC_AUDIOCLK              | \
                                         DECIMATE_BY_64             | \
                                         OD_UNDERRUN_PROTECT_ENABLE | \
                                         OD_DATA_MSB_ALIGNED        | \
                                         DMIC0_DATA_LSB_ALIGNED     | \
                                         DMIC1_DATA_LSB_ALIGNED     | \
                                         OD_DMA_REQ_DISABLE         | \
                                         DMIC0_DMA_REQ_DISABLE      | \
                                         DMIC1_DMA_REQ_DISABLE      | \
                                         OD_INT_GEN_DISABLE         | \
                                         DMIC0_INT_GEN_ENABLE       | \
                                         DMIC1_INT_GEN_DISABLE      | \
                                         OD_DISABLE                 | \
                                         DMIC0_ENABLE               | \
                                         DMIC1_DISABLE)

#define EEPROM_I2C_ADDR       (0x50)
#define EEPROM_PAGE_SIZE      (4)

/* All data written to RX characteristic will be echoed back as a notification
 * send from TX characteristic. */
void BLE_CustomServiceEcho(struct BLE_ICS_RxIndData *ind);

/* Prints message into serial terminal when button is pressed / released. */
void PB_TransitionEvent(void *arg);

/* Copies measurement data for printing of status messages. */
void BME680_Callback(struct BME680_ENV_Data *output);

/* Copies measurement data for printing of status messages. */
void BHY_OrientationCallback(bhy_data_generic_t *data, bhy_virtual_sensor_t sensor);


int32_t noa1305_status = -1;

int32_t bme680_status = -1;
struct BME680_ENV_Data bme680_output = { 0 };

int32_t bhy_status = -1;
bhy_data_vector_t bhy_orientation;

int32_t eeprom_status = -1;
I2CEeprom eeprom;
const uint8_t eeprom_ref[]  = "This is just a test";
const uint8_t eeprom_ref_length = sizeof(eeprom_ref);

int32_t dmic_value = 0;
int32_t dmic_max = 0;
int32_t dmic_min = INT32_MAX;

float tmax = 0;
float hmax = 0;
float altitude;
float bar;

int main(void)
{
    int32_t retval = 0;

    /* Initialize BDK library, set system clock (default 8MHz). */
    BDK_InitializeFreq(HAL_CLK_CONF_8MHZ);
    HAL_I2C_SetBusSpeed(HAL_I2C_BUS_SPEED_FAST);

    /* Initialize all LEDs */
    LED_Initialize(LED_RED);
    LED_Initialize(LED_GREEN);
    LED_Initialize(LED_BLUE);

    /* Test LEDs are working */
    LED_On(LED_RED);
    HAL_Delay(500);

    LED_Off(LED_RED);
    LED_On(LED_GREEN);
    HAL_Delay(500);

    LED_Off(LED_GREEN);
    LED_On(LED_BLUE);

    /* Initialize BLE stack */
    BDK_BLE_Initialize();
    BDK_BLE_SetLocalName("RSL10-SENSE-TESTER");
    BLE_ICS_Initialize(NULL);

    /* Initialize Button to call callback function when pressed or released. */
    BTN_Initialize(BTN0);
    BTN_Initialize(BTN1);

    /* AttachScheduled -> Callback will be scheduled and called by Kernel Scheduler. */
    /* AttachInt -> Callback will be called directly from interrupt routine. */
    BTN_AttachScheduled(BTN_EVENT_TRANSITION, &PB_TransitionEvent, (void*)BTN0, BTN0);
    BTN_AttachScheduled(BTN_EVENT_TRANSITION, &PB_TransitionEvent, (void*)BTN1, BTN1);

    /** Initialize NOA1305. */
    retval = NOA1305_ALS_Initialize();
    if (retval == NOA1305_OK)
    {
        retval = NOA1305_ALS_StartContinuous(0, NULL);
        if (retval == NOA1305_OK)
        {
            noa1305_status = 0;
        }
        else
        {
            noa1305_status = 2;
        }
    }
    else
    {
        noa1305_status = 1;
    }

    /** Initialize BME680 */
    retval = BME680_ENV_Initialize();
    if (retval == 0)
    {
        retval = BME680_ENV_StartPeriodic(BME680_Callback, 3000);
        if (retval == 0)
        {
            bme680_status = 0;
        }
        else
        {
            bme680_status = 2;
        }
    }
    else
    {
        bme680_status = 1;
    }

    /** Initialize BHI160 + load BMM150 RAM patch */
    retval = BHI160_NDOF_Initialize();
    if (retval == BHY_SUCCESS)
    {
        retval = BHI160_NDOF_EnableSensor(BHI160_NDOF_S_ORIENTATION, BHY_OrientationCallback, 1);
        if (retval == BHY_SUCCESS)
        {
            bhy_status = 0;
        }
        else
        {
            bhy_status = 2;
        }
    }
    else
    {
        bhy_status = 1;
    }

    /** DO R/W test for N24RF64 EEPROM memory. */
    retval = I2CEeprom_Initialize(EEPROM_I2C_ADDR, EEPROM_PAGE_SIZE, &eeprom);
    if (retval == I2C_EEPROM_OK)
    {
        retval = I2CEeprom_Write(0, (uint8_t*)eeprom_ref, eeprom_ref_length, &eeprom);
        if (retval == I2C_EEPROM_OK)
        {
            uint8_t eeprom_readback[sizeof(eeprom_ref)];

            retval = I2CEeprom_Read(0, eeprom_readback, eeprom_ref_length, &eeprom);
            if (retval == I2C_EEPROM_OK)
            {
                if (memcmp(eeprom_ref, eeprom_readback, eeprom_ref_length) == 0)
                {
                    memset(eeprom_readback, 0, eeprom_ref_length);
                    I2CEeprom_Write(0, eeprom_readback, eeprom_ref_length, &eeprom);

                    eeprom_status = 0;
                }
                else
                {
                    eeprom_status = 4;
                }
            }
            else
            {
                eeprom_status = 3;
            }
        }
        else
        {
            eeprom_status = 2;
        }
    }
    else
    {
        eeprom_status = 1;
    }

    /** Configure DMIC input to test INMP522 microphone. */

    /* Configure AUDIOCLK to 2 MHz and AUDIOSLOWCLK to 1 MHz. */
    CLK->DIV_CFG1 &= ~(AUDIOCLK_PRESCALE_64 | AUDIOSLOWCLK_PRESCALE_4);
    CLK->DIV_CFG1 |= AUDIOCLK_PRESCALE_4 | AUDIOSLOWCLK_PRESCALE_2;

    /* Configure OD, DMIC0 and DMIC1 */
    Sys_Audio_Set_Config(AUDIO_CONFIG);

    Sys_Audio_Set_DMICConfig(DMIC0_DCRM_CUTOFF_20HZ | DMIC1_DCRM_CUTOFF_20HZ |
                             DMIC1_DELAY_DISABLE | DMIC0_FALLING_EDGE |
                             DMIC1_RISING_EDGE, 0);

    Sys_Audio_DMICDIOConfig(DIO_6X_DRIVE | DIO_LPF_DISABLE | DIO_NO_PULL,
                            10, 6, DIO_MODE_AUDIOCLK);

    /* Configure Gains for DMIC0, DMIC1 and OD */
    AUDIO->DMIC0_GAIN = AUDIO_DMIC0_GAIN;
    NVIC_EnableIRQ(DMIC_OUT_OD_IN_IRQn);

    LED_Off(LED_BLUE);
    printf("APP: Entering main loop.\r\n");

    uint32_t timestamp = 0;
    while (1)
    {
        /* Execute any events that have occurred & refresh Watchdog timer. */
        BDK_Schedule();

        /* Print status information every second */
        if (HAL_TIME_ELAPSED_SINCE(timestamp) >= 1000)
        {
            timestamp = HAL_Time();

            printf(RTT_CTRL_CLEAR "Test status:\r\n");

            printf("NOA1305 initialization: %s\r\n", noa1305_status == 0 ? COLORIZE("OK", GREEN) : COLORIZE("ERROR", RED));
            if (noa1305_status == 0)
            {
                uint32_t lux = 0;
                NOA1305_ALS_ReadLux(&lux);
                printf("NOA1305 measured value: " COLORIZE("%lu", YELLOW) " lux\r\n\n", lux);
            }

            printf("BME680 initialization: %s\r\n", bme680_status == 0 ? COLORIZE("OK", GREEN) : COLORIZE("ERROR", RED));
            if (bme680_status == 0)
            {
                printf("BME680 temperature: " COLORIZE("%.2f", YELLOW) " °C\r\n", ((float)bme680_output.temperature) / 100.0f);
                printf("Maximum temperature: " COLORIZE("%.2f", YELLOW) " °C\r\n", tmax / 100.0f);
                printf("BME680 humidity: " COLORIZE("%.2f", YELLOW) " %%\r\n", ((float)bme680_output.humidity) / 1000.0f);
                printf("Maximum humidity: " COLORIZE("%.2f", YELLOW) " %%\r\n", hmax / 1000.0f);
                printf("BME680 pressure: " COLORIZE("%.2f", YELLOW) " Pa\r\n", ((float)bme680_output.pressure));
                bar = bme680_output.pressure * 1e-5;
                altitude = -8005 * log(bar);
                printf("Altitude: " COLORIZE("%.2f", YELLOW) " mts\r\n\n", altitude);
            }

            if (bme680_output.temperature > tmax) {
            	tmax = bme680_output.temperature;
            	//printf("t max: " COLORIZE("%.2f", YELLOW) " °C\r\n", tmax / 100.0f);
            	}

            if (bme680_output.humidity > hmax) {
            	hmax = bme680_output.humidity;
            	//printf("h max: " COLORIZE("%.2f", YELLOW) " %%\r\n", hmax / 1000.0f);
            	}

            printf("BHI160 (+BMM150) initialization: %s\r\n", bhy_status == 0 ? COLORIZE("OK", GREEN) : COLORIZE("ERROR", RED) );
            if (bhy_status == 0)
            {
                float h = bhy_orientation.x / 32768.0f * 360.0f;
                float p = bhy_orientation.y / 32768.0f * 360.0f;
                float y = bhy_orientation.z / 32768.0f * 360.0f;

                printf("BHI160 heading=" COLORIZE("%.2f°", YELLOW) ", ", h);
                printf("pitch=" COLORIZE("%.2f°", YELLOW) ", ", p);
                printf("yaw=" COLORIZE("%.2f°", YELLOW) "\r\n\n", y);
            }

            printf("N24RF64 read back test: %s\r\n\n", eeprom_status == 0 ? COLORIZE("OK", GREEN) : COLORIZE("ERROR", RED));

            printf("INMP522: Use J-Scope to see waveform\r\n");
            printf("INMP522: dmic_min=" COLORIZE("%ld", YELLOW) ", dmic_max=" COLORIZE("%ld", YELLOW) "\r\n", dmic_min, dmic_max);
        }

        SYS_WAIT_FOR_INTERRUPT;
    }

    return 0;
}

void BLE_CustomServiceEcho(struct BLE_ICS_RxIndData *ind)
{
    BLE_ICS_Notify(ind->data, ind->data_len);
}

void PB_TransitionEvent(void *arg)
{
    ButtonName btn = (ButtonName)arg;

    switch (btn)
    {
    case BTN0:
        LED_Toggle(LED_RED);
        break;
    case BTN1:
        LED_Toggle(LED_GREEN);
        break;
    default:
        return;
    }

    /* Read current Push Button state. */
    bool button_pressed = BTN_Read(btn);

    /* Print current button state to serial terminal. */
    printf("PB: Button %s state: %s [%lu ms]\r\n", (btn == BTN0) ? "0" : "1",
            (button_pressed == BTN_PRESSED) ? "PRESSED" : "RELEASED",
            HAL_Time());
}

void BME680_Callback(struct BME680_ENV_Data *output)
{
    memcpy(&bme680_output, output, sizeof(struct BME680_ENV_Data));
}

void BHY_OrientationCallback(bhy_data_generic_t *data, bhy_virtual_sensor_t sensor)
{
    memcpy(&bhy_orientation, &data->data_vector, sizeof(bhy_data_vector_t));
}

void DMIC_OUT_OD_IN_IRQHandler(void)
{
    dmic_value = (int32_t)AUDIO->DMIC0_DATA;

    if (dmic_max < dmic_value)
    {
        dmic_max = dmic_value;
    }
    else
    {
        if (dmic_min > dmic_value)
        {
            dmic_min = dmic_value;
        }
    }
}

