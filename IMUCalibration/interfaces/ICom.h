#ifndef ICOM_H
#define ICOM_H

class ICom {
public:
  virtual bool readCOM(uint8_t deviceId, uint8_t address, uint8_t *data, uint8_t size) = 0;
  virtual bool writeCOM(uint8_t deviceId, uint8_t address, uint8_t *data, uint8_t size) = 0;
};

#endif
