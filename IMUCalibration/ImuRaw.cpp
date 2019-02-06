/*
 * ImuRaw.cpp
 *
 *  Created on: 30 ene. 2019
 *      Author: racarla
 */

#include "ImuRaw.hpp"

ImuRaw::~ImuRaw(){}

bool ImuRaw::attachInterface(IAcc *iacc, IGyro *igyro, IMag *imag){
  if (iacc == nullptr || igyro == nullptr || imag == nullptr) return false;
  acc = iacc;
  gyro = igyro;
  mag = imag;
  return true;
}

bool ImuRaw::getDataImuRaw(int16_t (&data)[9]){
  if (acc == nullptr || gyro == nullptr || mag == nullptr) return false;
  acc->getDataAccRaw(accdata);
  gyro->getDataGyroRaw(gyrdata);
  mag->getDataMagRaw(magdata);
  for(int i = 0; i < 3; i++) data[i] = accdata[i];
  for(int i = 0; i < 3; i++) data[i+3] = gyrdata[i];
  for(int i = 0; i < 3; i++) data[i+6] = magdata[i];
  return true;
}

bool ImuRaw::getDataAccRaw(int16_t (&data)[3]){
  if (acc == nullptr) return false;
  return acc->getDataAccRaw(data);
}

bool ImuRaw::getDataGyroRaw(int16_t (&data)[3]){
  if (gyro == nullptr) return false;
  return gyro->getDataGyroRaw(data);
}

bool ImuRaw::getDataMagRaw(int16_t (&data)[3]){
  if (mag == nullptr) return false;
  return mag->getDataMagRaw(data);
}

bool ImuRaw::isDataReady(){
    return isDataAccReady() || isDataGyroReady() || isDataMagReady();
}

bool ImuRaw::isDataAccReady(){
  if (acc == nullptr) return false;
  return acc->isDataAccReady();
}

bool ImuRaw::isDataGyroReady(){
  if (gyro == nullptr) return false;
  return gyro->isDataGyroReady();
}

bool ImuRaw::isDataMagReady(){
  if (mag == nullptr) return false;
  return mag->isDataMagReady();
}
