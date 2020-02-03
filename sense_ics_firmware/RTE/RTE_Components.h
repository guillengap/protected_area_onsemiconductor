/*
 * Auto generated Run-Time-Environment Component Configuration File
 *      *** Do not modify ! ***
 *
 * Project: sense_ics_firmware
 * RTE configuration: sense_ics_firmware.rteconfig
*/
#ifndef RTE_COMPONENTS_H
#define RTE_COMPONENTS_H

/*
 * Define the Device Header File:
*/
#define CMSIS_device_header "rsl10.h"

/* ONSemiconductor.BDK::BLE.Peripheral Server.BASS */
#define RTE_BDK_BLE_PERIPHERAL_SERVER_BASS
/* ONSemiconductor.BDK::BLE.Peripheral Server.ICS */
#define RTE_BDK_BLE_PERIPHERAL_SERVER_ICS
/* ONSemiconductor.BDK::BLE.Peripheral Server.Peripheral Server */
#define RTE_BDK_BLE_PERIPHERAL_SERVER
/* ONSemiconductor.BDK::BLE.Peripheral Server.Template */
#define RTE_BDK_BLE_PERIPHERAL_SERVER
/* ONSemiconductor.RSL10-SENSE-GEVK::Board Support.ICS Protocol.ALS Node */
#define RTE_ICS_PROTOCOL_NODE_ALS HB_GEVB
/* ONSemiconductor.RSL10-SENSE-GEVK::Board Support.ICS Protocol.AO Node */
#define RTE_ICS_PROTOCOL_NODE_AO HB_GEVB
/* ONSemiconductor.RSL10-SENSE-GEVK::Board Support.ICS Protocol.ENV Node.BSEC */
#define RTE_ICS_PROTOCOL_NODE_ENV HB_GEVB
/* ONSemiconductor.RSL10-SENSE-GEVK::Board Support.ICS Protocol.System Node */
#define RTE_ICS_PROTOCOL_NODE_SYS HB_GEVK
/* ONSemiconductor.RSL10-SENSE-GEVK::Board Support.Libraries.BHI160_NDOF */
#define RTE_HB_BOARD_SUPPORT_LIBRARIES_BHI160_NDOF
/* ONSemiconductor.RSL10-SENSE-GEVK::Board Support.Libraries.BME680_BSEC */
#define RTE_HB_BOARD_SUPPORT_LIBRARIES_BME680_BSEC
/* ONSemiconductor.RSL10-SENSE-GEVK::Board Support.Libraries.Button */
#define RTE_HB_BOARD_SUPPORT_LIBRARIES_BUTTON
/* ONSemiconductor.RSL10-SENSE-GEVK::Board Support.Libraries.I2C Eeprom */
#define RTE_BOARD_SUPPORT_LIBRARIES_I2C_EEPROM
/* ONSemiconductor.RSL10-SENSE-GEVK::Board Support.Libraries.LED */
#define RTE_HB_BOARD_SUPPORT_LIBRARIES_LED
/* ONSemiconductor.RSL10-SENSE-GEVK::Board Support.Libraries.NOA1305_ALS */
#define RTE_HB_BOARD_SUPPORT_LIBRARIES_NOA1305_ALS
/* ONSemiconductor.RSL10-SENSE-GEVK::Board Support.Pinmap */
#define RTE_BDK_BOARD_SUPPORT HB_GEVB
/* ONSemiconductor::Components.Ambient Light Sensor.NOA1305 */
#define RTE_COMPONENTS_NOA1305
/* ONSemiconductor::Components.Environmental Sensor.BME680 */
#define RTE_COMPONENTS_BME680
/* ONSemiconductor::Components.Environmental Sensor.BSEC.Normal */
#define RTE_COMPONENTS_BSEC NORMAL_VERSION
/* ONSemiconductor::Components.Motion Sensor.BHI160 Firmware.di03 BMI160 BMM150 */
#define RTE_COMPONENTS_BHI160_FIRMWARE_DI03_BMI160_BMM150
/* ONSemiconductor::Components.Motion Sensor.BHy1 Driver */
#define RTE_COMPONENTS_BHY1
/* ONSemiconductor::Device.BDK.AES */
#define RTE_DEVICE_BDK_AES
/* ONSemiconductor::Device.BDK.Event Callback */
#define RTE_DEVICE_BDK_EVENT_CALLBACK
/* ONSemiconductor::Device.BDK.HAL */
#define RTE_DEVICE_BDK_HAL
/* ONSemiconductor::Device.BDK.Output Redirection.SEGGER RTT */
#define RTE_DEVICE_BDK_OUTPUT_REDIRECTION SEGGER_RTT
/* ONSemiconductor::Device.BDK.Scheduling */
#define RTE_DEVICE_BDK_SCHEDULING
/* ONSemiconductor::Device.BDK.Software Timer */
#define RTE_DEVICE_BDK_SOFTWARE_TIMER

#endif /* RTE_COMPONENTS_H */
