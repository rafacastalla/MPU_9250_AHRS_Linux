#include <stdint.h>

#define Interface(name, abbr, dof, type)  \
class I##abbr { \
  public: \
    virtual bool getData##abbr(type data[dof]) = 0; \
};

//#include "ISensor.h"
//Interface(Magnetometer, Mag, 3, int16_t) // @suppress("Class has a virtual method and non-virtual destructor")

// Ejemplo para ver el resultado preprocesado: g++ IName.cpp -E -o IName.txt
// IName hace referencia al nombre del archivo
// Visualizar el archivo para ver el resultado

/*
#if !defined(Sensor)
#define Sensor Accelerometer
#endif

#if Sensor == Accelerometer
#define INAME IAcc
#define ABBR Acc
#elif Sensor == Gyroscope
#define INAME IGyro
#define ABBR Gyro
#elif Sensor == Magnetometer
#define INAME IMag
#define ABBR Mag
#elif Sensor == Barometer
#define INAME IBaro
#define ABBR Baro
#elif Sensor == IMU
#define INAME IIMU
#define ABBR Imu
#endif

#if !defined(DOF)
#define DOF 3
#endif
*/
