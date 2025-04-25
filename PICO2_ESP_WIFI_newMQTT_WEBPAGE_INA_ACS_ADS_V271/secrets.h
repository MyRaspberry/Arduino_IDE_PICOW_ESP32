// file: secrets.h
#define MY_PROJECT "SITE: __________ , DEVICE: D___"  // D2xx is IP .2xx and /D2xx/ in topic..( unique for ONE site / LAN )

#define use4RELAY   // now with EEPROM memory

// I2S: currently can only select 1 INA at default x40
//#define use_INA_219
//#define use_INA_260

// ACS and ADS overwrite A0 input so also can NOT use both
//#define use_ADS1115  // at default x48

//#define useACSxxx
#ifdef useACSxxx
#define useACS712
//#define useACS758
#endif

#define SECRET_SSID "<SSID>"
#define SECRET_PASSWORD "<PASSWORD>"

#define LOCAL_TZ 7 // Time zone in Boston (GMT-4), Thailand 7 

//#define usePICOW2  // _________________________________________________ enable THIS to burn PICO2W D217

//#define useESP32s  // _________________________________________________ for Node32s boards.. i use board: NodeMCU-32S


#define PROJECT "PICO2_ESP_WIFI_newMQTT_WEBPAGE_INA_ACS_ADS"
#define REV "v2.7.1"

#define use_boot_ms  // print boot timing

// Web-Server
#define useFIXIP true //false   // new disable switch

#ifdef ESP32
//https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/adc.html
#define useESP_CAL_VOLT // use the esp calibarted millivolt function for Ain

#ifdef useESP32s
#define FIXIP {192,168,1,214}
#else // ESP32S3
#define FIXIP {192,168,1,216}
#endif

#else

#ifdef usePICOW2
#define FIXIP {192,168,1,217}
#else // PICO w
#define FIXIP {192,168,1,215}
#endif

#endif

#define PORT 1234

// __________________________________________________________________ V271 with web-page login
#define OPERATIONPW "admin"
#define Login_Diag false //true

// MQTT select remote broker TLS or local broker NO TLS // select only ONE
//#define useREMOTE_BROKER_HIVEMQ  // deselect by //

#ifdef useREMOTE_BROKER_HIVEMQ
#else
#define useLOCAL_BROKER
#endif

#ifdef useREMOTE_BROKER_HIVEMQ
// REMOTE BROKER
#define SECRET_MQTTBROKER "<xxx.hivemq.cloud>"
#define SECRET_MQTTUSER "<user>"
#define SECRET_MQTTPW "<password>"
#define MQTT_port 8883
#define MQTT_SECURE true
#endif

// __________________________________________________________________ LOCAL NO TLS

#ifdef useLOCAL_BROKER
// RPI4 Mosquitto ( with MQTT_REMOTE=False )
#define SECRET_MQTTBROKER "192.168.1.104"  // 192,168,1,104 if use IPAddress mqtt_broker(192,168,1,104);
#define SECRET_MQTTUSER "uPICOW"
#define SECRET_MQTTPW "pPICOW"
#define MQTT_port 1883
#define MQTT_SECURE false
#endif

#ifdef ESP32

#ifdef useESP32s
#define WIFI_HOSTNAME "ESP32S_D214"
#define sTOPIC "ESP32S/D214/status"
#define bTOPIC "ESP32S/D214/log"
#define dTOPIC "ESP32S/D214/data"
#define rTOPIC "ESP32S/D214/set" //'LED TOGGLE' R0 R1 R2 R3
#define fTOPIC "ESP32S/D214/fdata" //___________ 1 sec reads A0
#define MQTT_CLIENTID "ESP32s D214"
#define MQTT_STATUS "ESP32s ArduinoIDE"
#else // ESP32S3
#define WIFI_HOSTNAME "ESP32S3_D216"
#define sTOPIC "ESP32S3/D216/status"
#define bTOPIC "ESP32S3/D216/log"
#define dTOPIC "ESP32S3/D216/data"
#define rTOPIC "ESP32S3/D216/set" //'LED TOGGLE' R0 R1 R2 R3
#define fTOPIC "ESP32S3/D216/fdata" //___________ 1 sec reads A0
#define MQTT_CLIENTID "ESP32S3 D216"
#define MQTT_STATUS "ESP32S3 ArduinoIDE"
#endif

#else

#ifdef usePICOW2
#define WIFI_HOSTNAME "PICOW_D217"
#define sTOPIC "PICOW/D217/status"
#define bTOPIC "PICOW/D217/log"
#define dTOPIC "PICOW/D217/data"
#define rTOPIC "PICOW/D217/set" //'LED TOGGLE' R0 R1 R2 R3
#define fTOPIC "PICOW/D217/fdata" //___________ 1 sec reads A0
#define MQTT_CLIENTID "PICO2W D217"
#define MQTT_STATUS "PICO 2W ArduinoIDE"
#else
#define WIFI_HOSTNAME "PICOW_D215"
#define sTOPIC "PICOW/D215/status"
#define bTOPIC "PICOW/D215/log"
#define dTOPIC "PICOW/D215/data"
#define rTOPIC "PICOW/D215/set" //'LED TOGGLE' R0 R1 R2 R3
#define fTOPIC "PICOW/D215/fdata" //___________ 1 sec reads A0
#define MQTT_CLIENTID "PICOW D215"
#define MQTT_STATUS "PICO W ArduinoIDE"
#endif

#endif
// device HOME page auto adjust on device
#ifdef ESP32

#ifdef useESP32s
#define HTML_HEADER "ESPRESSIF ESP32"
#else
#define HTML_HEADER "ESPRESSIF ESP32-S3"
#endif

#else

#ifdef usePICOW2
#define HTML_HEADER "Raspberry Pi Pico 2W"

#else
#define HTML_HEADER "Raspberry Pi Pico W"

#endif

#endif
