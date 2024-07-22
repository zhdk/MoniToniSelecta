/*
***********************************************************************************
@file         Deneyap_DerinlikOlcer.h
@mainpage     Deneyap ToF Range Finder Sensor VL53L0X Arduino library header file
@version      v1.0.2
@date         September 21, 2022
@brief        This file contains all function prototypes and macros
              for Deneyap ToF Range Finder Sensor VL53L0X Arduino library
***********************************************************************************
*/

#ifndef _Deneyap_DerinlikOlcer_H
#define _Deneyap_DerinlikOlcer_H

#include <Arduino.h>
#include <Wire.h>

#define IDENTIFICATION_MODEL_ID 0x00c0
#define IDENTIFICATION_REVISION_ID 0x00c2
#define PRE_RANGE_CONFIG_VCSEL_PERIOD 0x0050
#define FINAL_RANGE_CONFIG_VCSEL_PERIOD 0x0070
#define SYSRANGE_START 0x0000
#define RESULT_INTERRUPT_STATUS 0x0013
#define RESULT_RANGE_STATUS 0x0014
#define I2C_SLAVE_DEVICE_ADDRESS 0x008a
#define I2C_ADDR 0x0029
#define SYSTEM_RANGE_CONFIG 0x0009
#define VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV 0x0089
#define SYSRANGE_MODE_SINGLESHOT 0x0000
#define SYSRANGE_MODE_START_STOP 0x0001
#define SYSRANGE_MODE_BACKTOBACK 0x0002
#define SYSRANGE_MODE_TIMED 0x0004

#define DEVICEMODE_SINGLE_RANGING ((uint8_t)0)
#define DEVICEMODE_CONTINUOUS_RANGING ((uint8_t)1)
#define DEVICEMODE_CONTINUOUS_TIMED_RANGING ((uint8_t)3)
#define DEFAULT_MAX_LOOP 200

#define I2C_DevAddr 0x29

class TofRangeFinder {
public:
  void begin(uint8_t address, TwoWire &wirePort = Wire);
  float ReadDistance();

private:
  uint8_t _address;
  uint16_t _distance;
  void writeRegister(unsigned char Reg, unsigned char byte);
  uint8_t readRegister(unsigned char Reg);
  void readData(unsigned char Reg, unsigned char Num);

  unsigned char DevAddr;
  unsigned char Data[16];
  uint16_t distance;
};

#endif
