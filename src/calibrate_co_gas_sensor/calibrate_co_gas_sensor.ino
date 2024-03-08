const int sensor_pin = G2;

void setup() { 
  Serial.begin(9600); 
} 
  
void loop() { 
  float sensor_voltage;  
  float RS_air; //  Rs in clean air 
  float R0;  // R0 in 1000 ppm LPG 
  float sensorValue; 
 
  // Calculate average sensor value
  for(int x = 0 ; x < 100 ; x++) 
  { 
    sensorValue = sensorValue + analogRead(sensor_pin); 
  } 
  sensorValue = sensorValue / 100.0; 
 
  sensor_voltage = (sensorValue / 1024) * 5.0; 
  RS_air = (5.0 - sensor_voltage) / sensor_voltage;
  R0 = RS_air / 9.9; // According to MQ9 datasheet table 
   
  Serial.print("Sensor voltage: "); 
  Serial.print(sensor_voltage); 
  Serial.println("V");
  
  Serial.print("R0: "); 
  Serial.println(R0); 
  delay(1000); 
}