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
//
// Application specific integration functions for BSEC library.
//
//-----------------------------------------------------------------------------
#ifndef BSEC_INTEGRATION_H__
#define BSEC_INTEGRATION_H__

#ifdef __cplusplus
extern "C"
{
#endif


#include <BSEC_ENV.h>

/* Defines the sample rate of sensor outputs.
 *
 * The standard BSEC sample rates are:
 *
 * * BSEC_SAMPLE_RATE_LP - Low power mode with sample period of 3 seconds.
 * * BSEC_SAMPLE_RATE_ULP - Ultra low power mode with sample period of 5 minutes.
 */
#define APP_BSEC_SAMPLE_RATE            BSEC_SAMPLE_RATE_LP

/* Static temperature offset that will be subtracted from all temperature
 * measurements. It compensates for other heat sources located near BME680.
 */
#define APP_BSEC_TEMPERATURE_OFFSET     (0.0f)

/* Total number of desired virtual outputs.
 *
 * This value should match with the number of declared sensors in the
 * APP_BSEC_REQUESTED_SENSORS below.
 */
#define APP_BSEC_REQUESTED_SENSOR_COUNT (7)

/* 7-bit I2C address of the EEPROM memory. */
#define APP_BSEC_EEPROM_I2C_ADDR       (0x50)

/* EEPROM memory page size. */
#define APP_BSEC_EEPROM_PAGE_SIZE      (4)

/* Start address where BSEC state will be saved to EEPROM.
 * Needs to be adjusted based on EEPROM memory size.
 */
#define APP_BSEC_EEPROM_ADDR           (8192-256)

/* Header that will be written before BSEC state to identify the block of
 * memory.
 *
 * This sequence is checked when attempting to load the state from EEPROM.
 *
 * Last byte will contain the length of saved BSEC state in bytes.
 */
#define APP_BSEC_EEPROM_MAGIC          {'C', 'S', 'N', ' ', 'B', 'S', 'E', 'C', 0}

#define APP_BSEC_EEPROM_MAGIC_SIZE     (9)

/* Initialize value for BSEC requested sensor outputs.
 *
 * It consists of desired virtual sensor outputs and their sample rates.
 */
#define APP_BSEC_REQUESTED_SENSORS { \
    { APP_BSEC_SAMPLE_RATE, BSEC_OUTPUT_IAQ }, \
    { APP_BSEC_SAMPLE_RATE, BSEC_OUTPUT_STATIC_IAQ }, \
    { APP_BSEC_SAMPLE_RATE, BSEC_OUTPUT_CO2_EQUIVALENT }, \
    { APP_BSEC_SAMPLE_RATE, BSEC_OUTPUT_BREATH_VOC_EQUIVALENT }, \
    { APP_BSEC_SAMPLE_RATE, BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE }, \
    { APP_BSEC_SAMPLE_RATE, BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY }, \
    { APP_BSEC_SAMPLE_RATE, BSEC_OUTPUT_RAW_PRESSURE }, \
}

/* Initializer value for BSEC init structure used to initialize the BSEC library.
 */
#define APP_BSEC_INIT_STRUCT_VALUE { \
    .temperature_offset = APP_BSEC_TEMPERATURE_OFFSET, \
    .sleep = App_BsecSleep, \
    .config_load = NULL, \
    .state_load = NULL, \
    .requested_virtual_sensors = APP_BSEC_REQUESTED_SENSORS, \
    .n_requested_virtual_sensors = APP_BSEC_REQUESTED_SENSOR_COUNT, \
}

/* Initializer value for BSEC process structure that defines runtime behavior.
 *
 */
#define APP_BSEC_PROCESS_STRUCT_VALUE { \
    .sleep = App_BsecSleep, \
    .get_timestamp_us = App_BsecGetTimestampUs, \
    .output_ready = App_BsecOutputReady, \
    .save_state = NULL, \
    .save_interval = 10, \
}


extern void App_BsecSleep(uint32_t t_ms);

extern int64_t App_BsecGetTimestampUs();

extern void App_BsecOutputReady(bsec_env_output_struct *output);

extern uint32_t App_BsecStateLoadFromEEPROM(uint8_t *state_buffer,
        uint32_t buffer_len);

extern void App_BsecStateSaveToEEPROM(uint8_t *state_buffer, uint32_t size);


#ifdef __cplusplus
}
#endif

#endif /* BSEC_INTEGRATION_H__ */
