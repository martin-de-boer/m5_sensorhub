#include "Adafruit_SGP30.h"
#include "M5AtomS3.h"

Adafruit_SGP30 sgp;

void throwError(char msg[]) {
  Serial.println(msg);
  //while (true) delay(1);
}

void setup() {
  Serial.begin(115200);
  if (!sgp.begin())
    throwError("Mini TVOC/eCO2 sensor (SGP30) is niet gevonden");

  delay(1000);
}

void loop() {
  if (!sgp.IAQmeasure()) throwError("Meting gefaald");

  Serial.print("TVOC: ");
  Serial.print(sgp.TVOC);
  Serial.println(" ppb");
  Serial.print("eCO2: ");
  Serial.print(sgp.eCO2);
  Serial.println(" ppm");

  delay(1000);
}
