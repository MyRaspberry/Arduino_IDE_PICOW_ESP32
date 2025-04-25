//__________________________________________________________________________________________
// ___ USER_SPACE ___ here document what you need to remember
// ___ Arduino IDE settings 
// selected board :
// selected switch :
// date time flash :
// to board?label :
//__________________________________________________________________________________________
// you might need to install some libraries:
// PubSubClient by Nick O'Leary v2.8
// Adafruit ADS1X15 v 2.5.0
// Adafruit INA219 v 1.2.3
// Adafruit INA260 v 1.5.2

//__________________________________________________________________________________________
// KLL restart PICO W with Arduino IDE 2.3.4 ( 2 / 2025 ) / 2.3.5 (4 / 2025 ) / 2.3.6 (4 / 2025 )

// PICO W    https://engineering-news.org/kllfusion01/infusions/blog/blog.php?readmore=4
// PICO 2W   https://engineering-news.org/kllfusion01/infusions/blog/blog.php?readmore=6
// ESP32S3   https://engineering-news.org/kllfusion01/infusions/blog/blog.php?readmore=14
// ESP32s    https://engineering-news.org/kllfusion01/infusions/blog/blog.php?readmore=7
// INA       https://engineering-news.org/kllfusion01/infusions/blog/blog.php?readmore=9
// ACS712    https://engineering-news.org/kllfusion01/infusions/blog/blog.php?readmore=12
// ADS1115   https://engineering-news.org/kllfusion01/infusions/blog/blog.php?readmore=13

// RPI4      https://engineering-news.org/kllfusion01/infusions/blog/blog.php?readmore=8


// try new webpage like latest Micropython version
// incl FakeAuth feature (v 1.1.0)
// test set |Tools| Cpu speed to 200 MHz OK ? cpu gets hotter ?

// test on ESP32-S3 Devkitc1 N16R8
// use ifdef esp32 for special esp32 code
// that is now a combi project for PICO W and ESP32S3 boards
// add a switch for the PICO 2W .217 /D217/ and more info use // to disable this
// add switch for ESP32s old boards like my NODE MCU ESP32S  [use right button at upload]

// 4-relay board as option
// INA219 as option
// INA260 as option
// ACS712 as option ( optional print help tuning )
// ACS758 for 3v3 signal and 60A measurements 

// newMQTT // for ESP32S3 use internal MQTT lib
// if need TLS to broker use <WiFiClientSecure.h> 

// print boot time from millis option

// have PICO [BOOTSEL] botton do reboot 

// try print more board info if ESP32
// https://embedded-things.blogspot.com/2021/10/install-arduino-esp32-200-on-arduino.html

// _________________________________________________________ add info to the 4 board ligic:
// it is like:
// -1-
// the code is for PICO W
// and works if you select arduino board PICO xW
// -2-
// if you select arduino board ESPxxx
//
// the compiler switch
// #ifdef ESP32   ( defined by ardunio and YOUR board selection )
// take the code for ESP32S3
// -3-
// unless you enable
// #useESP32s
// then take the code for the old ESP32s used
// -4-
// if you arduino board select
// PICO 2 W
// the same code is used as for PICO W
//
// unless you enable
// #usePICOW2
// then it will be given other IP and topic and also different web-page content...
//
// all switches are used in different places in the code
// ( but that is what they are for )
// because without there you would have to make manual changes
// if you use different board and want run it together with other boards.. ( like 4 boards need 4 IP / TOPIC ...)
//
// -5- that also means that
// #useESP32s
// #usePICOW2
//
// can both be enabled as they never work together...
//
// * and if you burn PICO xW it will be .217
// * if burn a ESP32S it will be OK
// * if burn a ESP32S3 it will fail from the esp32s code
//
// yes, same confusing like
// M&M with that many colors!
