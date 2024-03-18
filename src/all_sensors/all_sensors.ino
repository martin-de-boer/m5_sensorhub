#include "M5AtomS3.h"
#include "Adafruit_SGP30.h"
#include "M5UnitENV.h"

// ENV-III Sensor
SHT3X sht30;
QMP6988 qmp;

// TVOC/eCO2 Sensor
Adafruit_SGP30 sgp;
int millis 

// CO/Gas Sensor
const int co_gas_sensor_pin = G2;
float R0;

void throwError(char msg[]) {
  Serial.println(msg);
  //while (true) delay(1);
}

float calibrateCOGasSensor(int pin, int iterations) {
  float sensor_voltage;
  float RS_air;
  float sensor_value;
  
  // Calculate average sensor value
  for(int x = 0; x < iterations; x++) 
    sensor_value = sensor_value + analogRead(pin); 
  sensor_value = sensor_value / (float)iterations;

  // R0 calculation
  sensor_voltage = (sensor_value / 1024) * 5.0; 
  RS_air = (5.0 - sensor_voltage) / sensor_voltage;
  avg_R0 = RS_air / 9.9; // According to MQ9 datasheet table 

  return avg_R0;
}

void setup() {
  Serial.begin(115200);

  // Configuring and initialising the M5 unit
  auto cfg = M5.config();
  AtomS3.begin(cfg);
  M5.begin();

  // Initializing ENV-III sensor
  if (!qmp.begin(&Wire, QMP6988_SLAVE_ADDRESS_L, 2, 1, 400000U)) 
    throwError("ENV-III sensor (QMP6988) is niet gevonden");

  if (!sht30.begin(&Wire, SHT3X_I2C_ADDR, 2, 1, 400000U))
    throwError("ENV-III sensor (SHT30) is niet gevonden");

  // Initializing SGP30
  if (!sgp.begin())
    throwError("Mini TVOC/eCO2 sensor (SGP30) is niet gevonden");

  // Atom display settings
  int textsize = AtomS3.Display.height() / 60;
  if (textsize == 0) textsize = 1;
  AtomS3.Display.setTextSize(textsize);

  // Calibration of CO/Gas sensor
  int iterations = 2000;
  R0 = calibrateCOGasSensor(co_gas_sensor_pin, iterations);
  
}

void loop() {
  // Print the measurements of the SHT30 in the serial monitor
  if (sht30.update()) {
    Serial.println("----SHT30----");
    Serial.print("Temperatuur: ");
    Serial.print(sht30.cTemp);
    Serial.println(" graden C");
    Serial.print("Luchtvochtigheid: ");
    Serial.print(sht30.humidity);
    Serial.println("% rH");
    Serial.println("-------------\r\n");

    // Throw an error if one of the temperature sensors differs
    if (abs(sht30.cTemp - qmp.cTemp) >= 3.0)
      throwError("FOUT: Een van de temperatuursensoren wijkt sterk af");
      
  }

  // Print the measurements of the QMP6988 in the serial monitor
  if (qmp.update()) {
    Serial.println("-----QMP-----");
    Serial.print("Temperatuur: ");
    Serial.print(qmp.cTemp);
    Serial.println(" graden C");
    Serial.print("Luchtdruk: ");
    Serial.print(qmp.pressure);
    Serial.println(" Pa");
    Serial.print("Altitude (ongeveer): ");
    Serial.print(qmp.altitude);
    Serial.println(" m boven zeeniveau");
    Serial.println("-------------\r\n");

    // Throw an error if one of the temperature sensors differs
    if (abs(sht30.cTemp - qmp.cTemp) >= 3.0)
      throwError("FOUT: Een van de temperatuursensoren wijkt sterk af");
  }

  // Print the measurements of the SGP30 in the serial monitor
  if (sgp.IAQmeasure()) {
    Serial.println("-----SGP-----");
    Serial.print("TVOC: ");
    Serial.print(sgp.TVOC);
    Serial.println(" ppb");
    Serial.print("eCO2: ");
    Serial.print(sgp.eCO2);
    Serial.println(" ppm");
    Serial.println("-------------\r\n");
  }

  delay(1000);
}