#ifndef ALL_SENSORS_HPP
  #define ALL_SENSORS_HPP
#endif


#include <string>
#include "Adafruit_SGP30.h"
#include "M5UnitENV.h"


// Sensors
extern SHT3X sht30;
extern QMP6988 qmp;
extern Adafruit_SGP30 sgp;

// Gas sensor variables
extern int grove_gas_sensor_pin;
extern float R0;

// Variables for all sensors
extern std::string sensor_names[4];
extern long prev_millis;
extern long millis_since_last_measurement[4];

// Functions
void throwError(std::string msg, std::string display_msg_1, std::string display_msg_2);
float calibrateGroveGasSensor(int pin, int iterations);
float measureGroveGas(int pin, float R0);
const char* getSHT30dataStr();
const char* getQMP7988dataStr();
const char* getSGP30dataStr();
const char* getMQ9dataStr(float ratio);

