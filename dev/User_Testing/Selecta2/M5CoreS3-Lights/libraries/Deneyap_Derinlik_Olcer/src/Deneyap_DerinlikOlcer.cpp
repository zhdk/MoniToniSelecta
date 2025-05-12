/*
***************************************************************************************
@file         Deneyap_DerinlikOlcer.cpp
@mainpage     Deneyap ToF Range Finder Sensor VL53L0X Arduino library source file
@maintainer   RFtek Electronics <techsupport@rftek.com.tr>
@version      v1.0.2
@date         September 21, 2022
@brief        Includes functions to control Deneyap ToF Range Finder Sensor VL53L0X
              Arduino library

Library includes:
--> Configuration functions
--> Data manipulation functions
--> I2C communication functions
**************************************************************************************
*/

#include "Deneyap_DerinlikOlcer.h"

/**
 * @brief   I2C initialization and read check
 * @param   adress: Device adress
 * @retval  None
 **/
void TofRangeFinder::begin(uint8_t address, TwoWire &wirePort) {
  _address = address;
  Wire.begin();
  //uint8_t x = readRegister(IDENTIFICATION_MODEL_ID);
  //  Serial.print("Device ID: "); Serial.println(x,HEX);
  //  Serial.println("");
  //  if(x != 0xEE) {
  //    Serial.print("Read Chip ID fail!");
  //  }

  writeRegister(0x88, 0x00);
  writeRegister(0x80, 0x01);
  writeRegister(0xFF, 0x01);
  writeRegister(0x00, 0x00);
  readRegister(0x91);
  writeRegister(0x91, 0x3c);
  writeRegister(0x00, 0x01);
  writeRegister(0xFF, 0x00);
  writeRegister(0x80, 0x00);

  writeRegister(0x80, 0x01);
  writeRegister(0xFF, 0x01);
  writeRegister(0x00, 0x00);
  writeRegister(0x91, 0x3c);
  writeRegister(0x00, 0x01);
  writeRegister(0xFF, 0x00);
  writeRegister(0x80, 0x00);

  writeRegister(SYSRANGE_START, SYSRANGE_MODE_BACKTOBACK);
  //return true;
}

/**
 * @brief
 * @param
 * @retval
 **/
void TofRangeFinder::writeRegister(unsigned char Reg, unsigned char byte) {
  Wire.beginTransmission(_address);
  Wire.write(Reg);
  Wire.write((uint8_t)byte);
  Wire.endTransmission();
}

/**
 * @brief
 * @param
 * @retval
 **/
void TofRangeFinder::readData(unsigned char Reg, unsigned char Num) {
  Wire.beginTransmission(_address);
  Wire.write((uint8_t)Reg);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)_address, (uint8_t)Num);

  for (int i = 0; i < Num; i++) {
    Data[i] = Wire.read();
    delay(1);
  }
}

/**
 * @brief
 * @param
 * @retval
 **/
uint8_t TofRangeFinder::readRegister(unsigned char Reg) {

  Wire.beginTransmission(_address);
  Wire.write((uint8_t)Reg);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)_address, (uint8_t)1);

  return Wire.read();
}

/**
 * @brief
 * @param
 * @retval
 **/
float TofRangeFinder::ReadDistance() {
  readData(RESULT_RANGE_STATUS, 12);
  distance = ((Data[10] & 0xFF) << 8) |
             (Data[11] & 0xFF);
  writeRegister(SYSTEM_RANGE_CONFIG, 1);
  if (distance == 20)
    distance = _distance;
  else
    _distance = distance;
  return distance / 40;
}