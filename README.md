# Tuya IoTOS Embedded Bluetooth LE Smart Hula Hoop

[English](./README.md) | [中文](./README_zh.md)

<br>

## Introduction

In this demo, we will show you how to retrofit an ordinary hula hoop and make it IoT-enabled. Based on the [Tuya IoT Platform](https://iot.tuya.com/), we use Tuya's Bluetooth LE module, SDK, and the Tuya Smart app to connect the hula hoop to the cloud. The smart hula hoop consists of two buttons, three indicators, and one segment LCD. It enables a bunch of features such as mode selection, workout data display, pairing request, network status indication, and device reset. This hula hoop can record time, number of rotations, and calories burned. We use a Hall effect sensor switch to detect the number of rotations. Since this hula hoop is connected to the cloud, you can view detailed tracking data, select workout mode, and set workout goals with the Tuya Smart app.

<br>

## Get started

### Set up development environment

- Install the integrated development environment (IDE) as per your chip platform.

- Find the download URL of the Tuya Bluetooth LE SDK Demo Project from the following table. Refer to the `README.md` file under each branch to import the project.

   | Chip platform | Model | Download URL |
   | :----------: | :------: | :----------------------------------------------------------: |
   | Nordic | nrf52832 | [tuya_ble_sdk_Demo_Project_nrf52832.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_nrf52832.git) |
   | Realtek | RTL8762C | [tuya_ble_sdk_Demo_Project_rtl8762c.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_rtl8762c.git) |
   | Telink | TLSR825x | [tuya_ble_sdk_Demo_Project_tlsr8253.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_tlsr8253.git) |
   | Silicon Labs | BG21 | Coming soon. |
   | Beken | BK3431Q | [tuya_ble_sdk_demo_project_bk3431q.git](https://github.com/TuyaInc/Tuya_ble_sdk_demo_project_bk3431q.git) |
   | Beken | BK3432 | [tuya_ble_sdk_Demo_Project_bk3432.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_bk3432.git) |
   | Cypress | Psoc63 | [tuya_ble_sdk_Demo_Project_PSoC63.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_PSoC63.git) |

<br>

### Compile and flash

- Edit code

   1. In `tuya_ble_app_demo.h`, specify the PID of the product you have created on the [Tuya IoT Platform](https://iot.tuya.com/).

      ```
       #define APP_PRODUCT_ID     "xxxxxxxx"
      ```

      Change `xxxxxxxx` to the PID.

   2. In `tuya_ble_app_demo.c`, specify the `authkey` and `UUID`.

      ```
       static const char auth_key_test[] = "yyyyyyyy";
       static const char device_id_test[] = "zzzzzzzz";
      ```

      Change `yyyyyyyy` to your authkey and `zzzzzzzz` to your UUID.

- Compile code

   Compile the edited code, download the code to the hardware, and run it. You may need to download the stack and bootloader depending on your chip models. Check the logs and use the third-party Bluetooth debugging app (such as LightBlue for iOS) to verify the Bluetooth broadcast.

<br>

### File introduction
```
├── src         /* Source code files */
|    ├── common
|    |    └── tuya_local_time.c                 /* Local schedule */
|    ├── sdk
|    |    └── tuya_uart_common_handler.c        /* Code for UART communication  */
|    ├── driver
|    |    ├── tuya_hall_sw.c                    /* Hall sensor driver */
|    |    ├── tuya_key.c                        /* Key driver */
|    |    ├── tuya_led.c                        /* Indicator driver */
|    |    └── tuya_seg_lcd.c                    /* Segment LCD driver */
|    ├── platform
|    |    ├── tuya_gpio.c                       /* GPIO driver */
|    |    └── tuya_timer.c                      /* Timer driver */
|    ├── tuya_ble_app_demo.c                    /* Entry file of application layer */
|    ├── tuya_hula_hoop_ble_proc.c              /* Network connection handler */
|    ├── tuya_hula_hoop_evt_user.c              /* User event handler */
|    ├── tuya_hula_hoop_evt_timer.c             /* Schedule event handler */
|    ├── tuya_hula_hoop_svc_basic.c             /* Basic features */
|    ├── tuya_hula_hoop_svc_data.c              /* Data services */
|    ├── tuya_hula_hoop_svc_disp.c              /* Data display *
|    └── tuya_hula_hoop.c                       /* Entry to the demo */
|
└── include     /* Header files */
     ├── common
     |    ├── tuya_common.h                     /* Common types and macros */
     |    └── tuya_local_time.h                 /* Local schedule */
     ├── sdk
     |    ├── custom_app_uart_common_handler.h  /* Code for UART communication */
     |    ├── custom_app_product_test.h         /* Implementation of custom production test items */
     |    └── custom_tuya_ble_config.h          /* Application configuration file */
     ├── driver
     |    ├── tuya_hall_sw.h                    /* Hall sensor driver */
     |    ├── tuya_key.h                        /* Key driver */
     |    ├── tuya_led.h                        /* Indicator driver */
     |    └── tuya_seg_lcd.h                    /* Segment LCD driver */
     ├── platform
     |    ├── tuya_gpio.h                       /* GPIO driver */
     |    └── tuya_timer.h                      /* Timer driver */
     ├── tuya_ble_app_demo.h                    /* Entry file of application layer */
     ├── tuya_hula_hoop_ble_proc.h              /* Network connection handler */
     ├── tuya_hula_hoop_evt_user.h              /* User event handler */
     ├── tuya_hula_hoop_evt_timer.h             /* Schedule event handler */
     ├── tuya_hula_hoop_svc_basic.h             /* Basic features */
     ├── tuya_hula_hoop_svc_data.h              /* Data services */
     ├── tuya_hula_hoop_svc_disp.h              /* Data display */
     └── tuya_hula_hoop.h                       /* Entry to the demo */
```

<br>

### Entry to application
Entry file: `/tuya_ble_app/tuya_ble_app_demo.c`

+ `void tuya_ble_app_init(void)` is executed to initialize Tuya IoTOS Embedded Bluetooth LE SDK. This function is executed only once.
+ `void app_exe()` is used to execute the application code. It is executed in a loop.

<br>

### Data point (DP)

| Function | tuya_ble_dp_data_report |
| :------: | :----------------------------------------------------------- |
| Function prototype | tuya_ble_status_t tuya_ble_dp_data_report(uint8_t *p_data,uint32_t len); |
| Feature overview | Reports DP data. |
| Parameters | `p_data [in]`: DP data. <br> `len[in]`: data length. It cannot exceed `TUYA_BLE_REPORT_MAX_DP_DATA_LEN`. |
| Return value | `TUYA_BLE_SUCCESS`: sent successfully. <br/>`TUYA_BLE_ERR_INVALID_PARAM`: invalid parameter. <br/>`TUYA_BLE_ERR_INVALID_STATE`: failed to send data due to the current Bluetooth connection, such as Bluetooth disconnected. <br/>`TUYA_BLE_ERR_NO_MEM`: failed to request memory allocation. <br/>`TUYA_BLE_ERR_INVALID_LENGTH`: data length error. <br/>`TUYA_BLE_ERR_NO_EVENT`: other errors. |
| Notes | `application` calls this function to send DP data to the mobile app. |

Parameter description:

The [Tuya IoT Platform](https://iot.tuya.com/) manages data through DPs. The data generated by any device is abstracted into a DP. DP data consists of four parts, as described below.

- `Dp_id`: the DP ID of a data point defined on the Tuya IoT platform. It is one byte.


- `Dp_type`: the data type. It is one byte.

   ​ `#define DT_RAW 0`       raw type.

   ​ `#define DT_BOOL 1`     Boolean type.

   ​ `#define DT_VALUE 2`   value type. The value range is specified when a DP of value type is created on the Tuya IoT Platform.

   ​ `#define DT_STRING 3`   string type.

   ​ `#define DT_ENUM 4 `     enum type.

   ​ `#define DT_BITMAP 5`  bitmap type.

- `Dp_len`: It can be one byte or two bytes. Currently, Bluetooth only supports one byte, so the data of a single DP can be up to 255 bytes.


- `Dp_data`: the DP data, with `dp_len` byte(s).


The data that the parameter `p_data` points to must be packaged in the following format for reporting.

| DP 1 data |         |        |         | — | DP n data |         |        |         |
| :---------: | :-----: | :----: | :-----: | :--- | :---------: | :-----: | :----: | :-----: |
| 1 | 2 | 3 | 4 | — | n | n+1 | n+2 | n+3 |
| Dp_id | Dp_type | Dp_len | Dp_data | — | Dp_id | Dp_type | Dp_len | Dp_data |

When this function is called, the maximum data length is `TUYA_BLE_REPORT_MAX_DP_DATA_LEN`, which is `255+3` currently.

<br>

### Pin configuration

   | Peripherals | I/O | Peripherals | I/O | Peripherals | I/O | Peripherals | I/O |
   | ---------- | ---- | -------------- | ---- | -------------- | ---- | -------------- | ---- |
   | Timer indicator | PD4 | Mode selection button | PB7 | Segment LCD SEG3 | PD3 | Segment LCD COM1 | PA1 |
   | Rotations indicator | PB6 | Reset button | PB1 | Segment LCD SEG4 | PC1 | Segment LCD COM2 | PC2 |
   | Calorie indicator | PD7 | Segment LCD SEG1 | PA0 | Segment LCD SEG5 | PC4 | Segment LCD COM3 | PC3 |
   | Hall sensor | PD2 | Segment LCD SEG2 | PC0 | Segment LCD SEG6 | PB5 | Segment LCD COM4 | PB4 |

<br>

## Reference

+ [BLE SDK Guide](https://developer.tuya.com/en/docs/iot/tuya-ble-sdk-user-guide?id=K9h5zc4e5djd9#title-13-The%20callback%20event%20of%20tuya%20ble%20sdk)
+ [BLE SDK Demo Overview](https://developer.tuya.com/en/docs/iot/tuya-ble-sdk-demo-instruction-manual?id=K9gq09szmvy2o)
+ [Tuya Project Hub](https://developer.tuya.com/demo)


<br>


## Technical support

You can get support from Tuya with the following methods:

- [Tuya Developer Platform](https://developer.tuya.com/en/)
- [Help Center](https://support.tuya.com/en/help)
- [Service & Support](https://service.console.tuya.com/8/3/list?source=support_center)

<br>
