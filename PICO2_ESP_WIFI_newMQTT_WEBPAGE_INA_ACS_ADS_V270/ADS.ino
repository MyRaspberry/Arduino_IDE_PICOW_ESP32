// LIB installed on Arduin IDE
// Adafruit ADS1X15 v 2.5.0 ( with all dependencies )
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

#ifdef use_ADS1115

bool ADS_OK = false;

#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
//Adafruit_ADS1015 ads;     /* Use this for the 12-bit version */

int16_t result01;
float A01mV;
int16_t result23;
float A23mV;
  /* Be sure to update this value based on the IC and the gain settings! */
  //float   multiplier = 3.0F;    /* ADS1015 @ +/- 6.144V gain (12-bit results) */
float multiplier = 0.1875F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */


void setup_ADS1115() {

  msg = "gpio I2C ADS1115 A01 Getting differential reading from AIN0 (P) and AIN1 (N) show at A0 in [mV]\n";
  msg+= "gpio I2C ADS1115 A23 Getting differential reading from AIN3 (P) and AIN4 (N) show at A1 in [mV]\n";
  //Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");
  msg+= "gpio I2C ADC Range: +/- 6.144V (1 bit = 0.1875mV/ADS1115)\n";

  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  msg += "gpio try connect ADS1115 expect SDA on ";
  msg += SDA;
  msg += ", and SCL on ";
  msg += SCL;
  msg += " \n";
  if (!ads.begin()) {
    msg+= "gpio Failed to initialize ADS1115 on 0x48\n";
    //while (1);
  } else {
    msg+= "gpio ADS1115 connected\n";
    ADS_OK = true;
  }
  Serial.println(msg);
  BootLog += msg + "\n";
}


void calc_ADS1115() {
  if ( ADS_OK ) {
    result01 = ads.readADC_Differential_0_1();
    result23 = ads.readADC_Differential_2_3();
    A01mV = result01 * multiplier;
    A23mV = result23 * multiplier;

    //Serial.print("A01: "); Serial.print(A01mV,3); Serial.println(" mV"); Serial.print(" , A23: "); Serial.print(A23mV,3); Serial.println(" mV");
    // temporary overwrite A0 A1
    sensorA0val = A01mV;
    sensorA1val = A23mV;
  } else { // _____________________________ we want show that this is not A0 A1 instead it's BAD ADS data ( as you enabled ADS )
    sensorA0val = -0.007;
    sensorA1val = -0.007;
  }

}


#endif