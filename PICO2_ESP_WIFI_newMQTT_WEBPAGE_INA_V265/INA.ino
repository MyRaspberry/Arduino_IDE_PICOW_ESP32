#include "secrets.h"
// LIB installed on Arduin IDE
// Adafruit INA219 v 1.2.3 ( with all dependencies )
/* 
 here for PICO W default wires used:
 wire SDA PICO W GP4 (pin 6)
 wire SCL PICO W GP5 (pin 7)

 here for ESP32S3 default wires used:
 wire SDA ESP32S3 8 (pin 12)
 wire SCL ESP32S3 9 (pin 15)

 here for ESP32S default wires used:
 wire SDA ESP32S GP21 (pin 6 right)
 wire SCL ESP32S GP22 (pin 3 right)

*/
#include <Wire.h>

#include <Adafruit_INA219.h>
Adafruit_INA219 ina219(0x40);  // default 0x40 (0x41)...

#include <Adafruit_INA260.h>
Adafruit_INA260 ina260  = Adafruit_INA260();  // ? default 0x40

float shuntvoltage = 0;
float busvoltage = 0;
float current_mA = 0;
float loadvoltage = 0;
float power_mW = 0;

float busvoltage_mV = 0;  // INA260

bool INA_OK = false;

void make_INA219(){
  //Serial.println("make_INA219");
  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  //power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  power_mW = loadvoltage * current_mA; // calc NOT measure
  if ( false ) {
    Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
    Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
    Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
    Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
    //Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
    Serial.println("");
  }
  if ( INA_OK ) {
    MQTTina  = "";
    MQTTina += ", \"VDC\": ";
    MQTTina += String(loadvoltage,3);
    MQTTina += ", \"ADC\": ";
    MQTTina += String(current_mA/1000.0,3);
    MQTTina += ", \"POW\": ";
    MQTTina += String(power_mW/1000.0,3);
  } else {
    MQTTina  = ", \"VDC\": -0.007, \"ADC\":  -0.007, \"POW\": -0.007";
  }
}

void setup_INA219() {
  // Initialize the INA219.
  if (! ina219.begin()) {
      msg="gpio Failed to find INA219 chip";
    } else {
      INA_OK= true;
      msg="gpio Measuring voltage, current, and power with INA219 board";
    }
    Serial.println(msg);
    BootLog += msg + "\n";
}

// for INA260 board and other adafruit lib

void make_INA260(){
  //Serial.println("make_INA260");
  busvoltage_mV = ina260.readBusVoltage();
  current_mA = ina260.readCurrent();
  power_mW = ina260.readPower();
  // power_mW = busvoltage_mV * current_mA ; // calc NOT measure !! uW
  if ( false ) {
    Serial.print("Bus Voltage:   "); Serial.print(busvoltage_mV); Serial.println(" mV");
    Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
    Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
    Serial.println("");
  }
  if ( INA_OK ) {
    MQTTina  = "";
    MQTTina += ", \"VDC\": ";
    MQTTina += String(busvoltage_mV/1000.0,3);
    
    MQTTina += ", \"ADC\": ";
    MQTTina += String(current_mA/1000.0,3);

    MQTTina += ", \"POW\": ";
    MQTTina += String(power_mW/1000.0,3);

  } else {
    MQTTina  = ", \"VDC\": -0.007, \"ADC\":  -0.007, \"POW\": -0.007";
  }
}

void setup_INA260() {
  // Initialize the INA260.
  if (! ina260.begin()) {
    msg="gpio Failed to find INA260 chip";
  } else {
    INA_OK = true;
    msg="gpio Measuring voltage, current, and power with INA260 board";
  }
  Serial.println(msg);
  BootLog += msg + "\n";
}
