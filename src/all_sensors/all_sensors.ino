#include "all_sensors.hpp"

#include <M5Unified.h>
#include <sstream>
#include <iomanip>
#include "M5AtomS3.h"
#include <Wire.h>


// ENV-III Sensor
SHT3X sht;
QMP6988 qmp;

// TVOC/eCO2 Sensor
Adafruit_SGP30 sgp;

// Grove Gas Sensor
int grove_gas_sensor_pin = G1;
float R0;

// Variables for all sensors
std::string sensor_names[] = { "SHT30", "QMP6988", "SGP30", "MQ9" };
long prev_millis = 0;
long millis_since_last_measurement[] = { 0, 0, 0, 0 };


/* 
 * This function will halt the program in case a fatal error occures.
 *
 * @param msg - the message printed to the serial monitor
 * @param display_msg_1 - the message printed to the first line of the AtomS3 display
 * @param display_msg_2 - the message printed to the second line of the AtomS3 display
 */
void throwError(std::string msg, std::string display_msg_1, std::string display_msg_2) {
  // Print error message to serial monitor
  Serial.println(msg.c_str());

  // Print error message to AtomS3 display
  AtomS3.Display.setTextDatum(middle_center);
  AtomS3.Display.setTextColor(RED);
  AtomS3.Display.setTextSize(1);
  AtomS3.Display.drawString("FATAL ERROR", AtomS3.Display.width() / 2, 15);
  AtomS3.Display.drawString(display_msg_1.c_str(), AtomS3.Display.width() / 2, AtomS3.Display.height() / 2 - 10);
  AtomS3.Display.drawString(display_msg_2.c_str(), AtomS3.Display.width() / 2, AtomS3.Display.height() / 2 + 10);

  while (true) delay(1);
}


/* 
 * This function determines the R0 value;
 * The R0 is needed to determine the ratio of gas in the air compared to a certain starting point.
 * This function should only be called once.
 *
 * @param pin - the pin the grove gas sensor is connected to
 * @param iterations - how many times the sensor measures
 * @returns the average R0 value that has been measured
 */
float calibrateGroveGasSensor(int pin, int iterations) {
  int sensor_value;
  float sensor_voltage;
  float avg_R0;
  float RS_air;
  
  // Calculate average sensor value
  for (int x = 0; x < iterations; x++)
    sensor_value = sensor_value + analogRead(pin);
    sensor_value = sensor_value / (float)iterations;

  // R0 calculation
  sensor_voltage = (sensor_value / 1024) * 5.0;
  RS_air = (5.0 - sensor_voltage) / sensor_voltage;
  avg_R0 = RS_air / 9.9;  // According to MQ9 datasheet table

  return avg_R0;
}


/*
 * This function will read the gas measurement;
 * This function returns the ratio of the current gas compared to the gas measured at the start (R0).
 *
 * @param pin - the pin the grove gas sensor is connected to
 * @param R0 - the R0 value measured at the start
 * @returns the measured ratio of RS_gas compared to R0
 */
float measureGroveGas(int pin, float R0) {
  int sensor_value;
  float sensor_voltage;
  float RS_gas;
  float ratio;

  sensor_value = analogRead(pin);
  sensor_voltage = (sensor_value / 1024) * 5.0;
  RS_gas = (5.0 - sensor_voltage) / sensor_voltage;
  ratio = RS_gas / R0;

  return ratio;
}


/*
 * This function will return a formatted string of the measurements of the SHT30 sensor.
 * This function should be called within an if-clause that checks if the SHT30 has updates.
 * @returns a formatted data string of the measurements
 */
const char* getSHT30dataStr() {
  std::string data = "----SHT30----\n";
  data += "Temperature: " + std::to_string(sht.cTemp) + " ºC\n";
  data += "Humidity: " + std::to_string(sht.humidity) + " % rH\n";
  data += "-------------\n\n";
  return data.c_str();
}


/*
 * This function will return a formatted string of the measurements of the QMP6988 sensor.
 * This function should be called within an if-clause that checks if the QMP6988 has updates.
 * @returns a formatted data string of the measurements
 */
const char* getQMP6988dataStr() {
  std::string data = "-----QMP-----\n";
  data += "Temperature: " + std::to_string(qmp.cTemp) + " ºC\n";
  data += "Air pressure: " + std::to_string(qmp.pressure) + " Pa\n";
  data += "Altitude: about " + std::to_string(qmp.altitude) + " m above sea level\n";
  data += "-------------\n\n";
  return data.c_str();
}


/*
 * This function will return a formatted string of the measurements of the SGP30 sensor.
 * This function should be called within an if-clause that checks if the SGP30 has updates.
 * @returns a formatted data string of the measurements
 */
const char* getSGP30dataStr() {
  std::string data = "-----SGP-----\n";
  data += "TVOC: " + std::to_string(sgp.TVOC) + " ppb\n";
  data += "eCO2: " + std::to_string(sgp.eCO2) + " ppm\n";
  data += "-------------\n\n";
  return data.c_str();
}


/*
 * This function will return a formatted string of the measurements of the MQ9 sensor.
 * The ratio can be determined using the measureGroveGas(int, float) function.
 * @param ratio - the measured ratio of RS_gas compared to R0
 * @returns a formatted data string of the measurements
 */
const char* getMQ9dataStr(float ratio) {
  std::string data = "-----MQ9-----\n";
  data += "Ratio: " + std::to_string(ratio) + "\n";
  data += "-------------\n\n";
  return data.c_str();
}


/*
 * Displays two lines at the bottom of the AtomS3 screen.
 * 
 * @param str1 - the string to display on the first line, via std::string.c_str()
 * @param str2 - the string to display on the second line, via std::string.c_str()
 * @param alignment - the anchor of the text (display centered via top_center and left-aligned via top_left)
 * @param color - the color of the text
 * @param y1 - alternate y-position for the first line
 * @param y2 - alternate y-position for the second line
 * @param text_size - optional scaling multiplier for the font size
 */
void displayTwoLineString(const char* str1, const char* str2, uint8_t alignment, int color, int y1=75, int y2=95, int text_size=1.0f) {
  AtomS3.Display.setTextDatum(alignment);
  AtomS3.Display.setTextColor(color);
  AtomS3.Display.setTextSize(text_size);

  // Display the text on the left side if the alignment is top_left
  if (alignment == top_left) {    
    AtomS3.Display.drawString(str1, 2, y1);
    AtomS3.Display.drawString(str2, 2, y2);
  }

  // Display the text in the middle of the screen if the alignment is top_center
  if (alignment == top_center) {    
    AtomS3.Display.drawString(str1, AtomS3.Display.width() / 2, y1);
    AtomS3.Display.drawString(str2, AtomS3.Display.width() / 2, y2);
  }
}


/*
 * Default setup function needed in every Arduino program
 * The setup function will initialize the serial connection, the sensors and the M5 unit
 * Recommended place to measure the R0 using the calibrateGroveGasSensor() function
 */
void setup() {

  // Start serial connection
  Serial.begin(115200);
  Serial.println("Setting up...");

  // Configuring and initialising the M5 unit
  auto cfg = M5.config();
  AtomS3.begin(cfg);
  M5.begin();

  // Atom display settings
  AtomS3.Display.setFont(&fonts::FreeSansBold9pt7b);

  // Initializing the grove gas sensor
  Wire.begin()

  // Initializing the QMP6988 from the ENV-III sensor
  int fail_count = 0;
  while (!qmp.begin(&Wire, QMP6988_SLAVE_ADDRESS_L, 2, 1, 400000U)) {
    Serial.println("Reattempting connection with QMP6988...");

    // Throw an error if connection fails 10 times
    fail_count++;
    if (fail_count > 10)
      throwError("ENV-III sensor (QMP6988) was not found", "QMP6988", "not found");
  }

  // Initializing the SHT30 from the ENV-III sensor
  fail_count = 0;
  while (!sht.begin(&Wire, SHT3X_I2C_ADDR, 2, 1, 400000U)) { 
    Serial.println("Reattempting connection with SHT30...");

    // Throw an error if connection fails 10 times
    fail_count++;
    if (fail_count > 10) 
      throwError("ENV-III sensor (SHT30) was not found", "SHT30", "not found");
  }

  // Initializing SGP30
  fail_count = 0;
  while (!sgp.begin()) {
    Serial.println("Reattempting connection with SGP30...");

    // Throw an error if connection fails 10 times
    fail_count++;
    if (fail_count > 10) 
      throwError("Mini TVOC/eCO2 sensor (SGP30) was not found", "SGP30", "not found");
  }

  // Calibration of CO/Gas sensor
  int iterations = 2000;
  R0 = calibrateGroveGasSensor(grove_gas_sensor_pin, iterations);

  Serial.println("Setup complete.");
}


/*
 * Default loop function needed in every Arduino program
 * The loop function will read all sensors and update the AtomS3 display with sensor data every second
 * It will also check if certain sensors are not working or if another problem arises
 */
void loop() {

  // Update time variables; check if sensors are working
  for (int i = 0; i < 4; i++) {
    millis_since_last_measurement[i] += millis() - prev_millis;
    prev_millis = millis();

    // Throw an error if a sensor did not update for 30 seconds
    std::string error_string = "ERROR: Sensor " + sensor_names[i] + " is not working!";
    if (millis_since_last_measurement[i] >= 5000) throwError(error_string, sensor_names[i], "not working");
  }


  // Print the measurements of the SHT30 in the serial monitor
  if (sht.update()) {
    Serial.print(getSHT30dataStr());
    millis_since_last_measurement[0] = 0;
  }

  // Print the measurements of the QMP6988 in the serial monitor
  if (qmp.update()) {
    Serial.print(getQMP6988dataStr());
    millis_since_last_measurement[1] = 0;
  }

  // Print the measurements of the SGP30 in the serial monitor
  if (sgp.IAQmeasure()) {
    Serial.print(getSGP30dataStr());
    millis_since_last_measurement[2] = 0;
  }

  // Print the measurements of the MQ9 Grove Gas sensor in the serial monitor
  Serial.print(getMQ9dataStr(measureGroveGas(grove_gas_sensor_pin, 2000)));
  millis_since_last_measurement[3] = 0;

  // Print data to AtomS3 screen
  AtomS3.Display.clear();
  AtomS3.Display.setTextDatum(top_left);
  AtomS3.Display.setTextSize(2);
  AtomS3.Display.setTextColor(GREEN);

  // Print temperature (from the SHT30 because that sensor is the most reliable) rounded to one decimal place
  std::stringstream temp_stream;
  temp_stream << std::fixed << std::setprecision(1) << sht.cTemp;
  AtomS3.Display.drawString((temp_stream.str() + " 'C").c_str(), 2, 2);

  // Print humidity rounded to one decimal place
  std::stringstream hum_stream;
  hum_stream << std::fixed << std::setprecision(1) << sht.humidity;
  AtomS3.Display.drawString((hum_stream.str() + " %").c_str(), 2, 35);

  // Check for problems, otherwise print eCO2 and TVOC values
  AtomS3.Display.setTextSize(1);
  if (sgp.TVOC > 2000 || sgp.eCO2 > 3000) 
    displayTwoLineString("Harmful air", "Evacuate!", top_center, RED);
  else if (sgp.TVOC > 400 || sgp.eCO2 > 1200)
    displayTwoLineString("Air may be", "harmful", top_center, YELLOW);
  else if (abs(sht.cTemp - qmp.cTemp) >= 2.0)
    displayTwoLineString("Temperature", "not accurate", top_center, YELLOW);
  else
    displayTwoLineString(
      ("eCO2: " + std::to_string(sgp.eCO2) + " ppm").c_str(), 
      ("TVOC: " + std::to_string(sgp.TVOC) + " ppb").c_str(), 
      top_left, GREEN
    );

  delay(1000);
}