#include "M5AtomS3.h"
#include "M5UnitENV.h"

// ENV-III Sensor
SHT3X sht30;
QMP6988 qmp;

// TVOC/eCO2 Sensor
Adafruit_SGP30 sgp;

void throwError(char msg[]) {
  Serial.println(msg);
  //while (true) delay(1);
}

void setup() {
  Serial.begin(115200);

  auto cfg = M5.config();
  AtomS3.begin(cfg);
  M5.begin();

  // Initializing ENV-III sensor
  if (!qmp.begin(&Wire, QMP6988_SLAVE_ADDRESS_L, 2, 1, 400000U)) 
    throwError("ENV-III sensor (QMP6988) is niet gevonden");

  if (!sht30.begin(&Wire, SHT3X_I2C_ADDR, 2, 1, 400000U))
    throwError("ENV-III sensor (SHT30) is niet gevonden");

  // Initializing 

  // Atom display settings
  int textsize = AtomS3.Display.height() / 60;
  if (textsize == 0) textsize = 1;
  AtomS3.Display.setTextSize(textsize);
}

void loop() {
  if (sht30.update()) {
    Serial.println("----SHT30----");
    Serial.print("Temperatuur: ");
    Serial.print(sht30.cTemp);
    Serial.println(" graden C");
    Serial.print("Luchtvochtigheid: ");
    Serial.print(sht30.humidity);
    Serial.println("% rH");
    Serial.println("-------------\r\n");

    if (abs(sht30.cTemp - qmp.cTemp) >= 3.0)
      throwError("FOUT: Een van de temperatuursensoren wijkt sterk af");
      
  }

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

    if (abs(sht30.cTemp - qmp.cTemp) >= 3.0)
      throwError("FOUT: Een van de temperatuursensoren wijkt sterk af");
  }

  delay(1000);
}