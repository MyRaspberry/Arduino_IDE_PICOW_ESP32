// file: secrets.h

#define use4RELAY  // disable with //
// currently can only select 1 INA at x40
//#define use_INA_219
//#define use_INA_260
//#define useACS712
#define ACS712useFilter true
#define ACS712diag true //false // every second print and now also send to mqtt /log

#define SECRET_SSID "<SSID>"
#define SECRET_PASSWORD "<PASSWORD>"

#define LOCAL_TZ 7 // Time zone in Boston (GMT-4), Thailand 7 

//#define usePICOW2  // _________________________________________________ enable THIS to burn PICO2W D217

#define useESP32s  // _________________________________________________ for Node32s boards.. i use board: NodeMCU-32S


#define PROJECT "PICO2_ESP_WIFI_newMQTT_WEBPAGE_INA"
#define REV "v2.6.5"

#define use_boot_ms  // print boot timing

// Web-Server
#define useFIXIP true //false   // new disable switch

#ifdef ESP32

#ifdef useESP32s
#define FIXIP {192,168,1,214}
#else
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

#define useFakeAuth false  // false true
#define SECRET_URLHASH "/AaAweNoaS8FR7F75YNUmowR7ivxtPR0WG57Cqb4bN1c=/"
// http://192.168.1.215:1234/AaAweNoaS8FR7F75YNUmowR7ivxtPR0WG57Cqb4bN1c=/

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
#define sTOPIC "ESP32S/D214/status"
#define bTOPIC "ESP32S/D214/log"
#define dTOPIC "ESP32S/D214/data"
#define rTOPIC "ESP32S/D214/set" //'LED TOGGLE' R0 R1 R2 R3
#define fTOPIC "ESP32S/D214/fdata" //___________ 1 sec reads A0
#define MQTT_CLIENTID "ESP32s D214"
#define MQTT_STATUS "ESP32s ArduinoIDE"
#else // ESP32S3
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
#define sTOPIC "PICOW/D217/status"
#define bTOPIC "PICOW/D217/log"
#define dTOPIC "PICOW/D217/data"
#define rTOPIC "PICOW/D217/set" //'LED TOGGLE' R0 R1 R2 R3
#define fTOPIC "PICOW/D217/fdata" //___________ 1 sec reads A0
#define MQTT_CLIENTID "PICO2W D217"
#define MQTT_STATUS "PICO 2W ArduinoIDE"
#else
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
#define HTML_TITLE "ESP32"
#define HTML_HEADER "ESPRESSIF ESP32"
#else
#define HTML_TITLE "ESP32S3 N16R8"
#define HTML_HEADER "ESPRESSIF ESP32-S3"
#endif

#else

#ifdef usePICOW2
#define HTML_TITLE "PICO 2W"
#define HTML_HEADER "Raspberry Pi Pico 2W"

#else
#define HTML_TITLE "PICO W"
#define HTML_HEADER "Raspberry Pi Pico W"

#endif

#endif
