# m5_sensorhub
A program that will measure various quantities (like temperature, CO and gas) using the M5AtomS3 PoE version.

## Components
For this application connect the following components:
- [M5Atom PoE version](https://docs.m5stack.com/en/atom/atom_poe)
- [AtomS3 display for M5Atom modules](https://docs.m5stack.com/en/core/AtomS3)
- An I2C hub to connect all the sensors, for example [this one](https://store-usa.arduino.cc/products/grove-i2c-hub-6-port)
- [Mini TVOC/eCO2 Gas Unit](https://docs.m5stack.com/en/unit/tvoc) developed for M5Stack. Equipped with the _SGP30_-sensor
- [ENV. III Sensor Unit](https://docs.m5stack.com/en/unit/envIII) developed for M5Stack. Equipped with the _QMP6988_-sensor and _SHT30_-sensor
- [Grove Gas sensor MQ9](https://wiki.seeedstudio.com/Grove-Gas_Sensor-MQ9/) developed for Arduino. Equipped with the _MQ9_-sensor
- 4 I2C cables

### Wiring up
- Put the AtomS3 display on the Atom PoE module.
- Connect the I2C hub to the Atom PoE module via an I2C cable.
- Now connect the 3 different sensor modules to the I2C hub via I2C cables.

![I2C Circuit Connection](https://github.com/martin-de-boer/m5_sensorhub/assets/67046979/f5e79a2d-a6f5-4f59-9392-6dc2f7acf914)


## Installation
To install the board in the Arduino IDE, follow this tutorial: [Quick Start AtomS3 in Arduino IDE](https://docs.m5stack.com/en/quick_start/atoms3/arduino)  
Also install the following libraries in the Arduino IDE (using the _Sketch_ &rarr; _Include Library_ &rarr; _Manage Libraries_ menu):
- M5AtomS3
- Adafruit SGP30 Sensor
- M5Unit-ENV

![The Manage Libraries menu](https://github.com/martin-de-boer/m5_sensorhub/assets/67046979/2ca77eae-380b-415b-9827-7aabb3d10e09)

## Docs
See the [documentation](https://martin-de-boer.github.io/m5_sensorhub/) for more information.
