
#include "secrets.h"

String BLOGs = "https://engineering-news.org/kllfusion01/infusions/blog/blog.php?readmore=5";
String A_Reads = " A_Reads "; // feed by Ains()
String Tnows = " Tnows "; // feed by nows()
String MQTTs = " MQTTs "; // feed by web_wifi
String MQTTA0 = ""; 
String MQTTina = "";

String msg = "";
String BootLog = "__BootLog__";
char BootLogc[210]; // used for 4 chunks

char MQTTc[250]; // from 200 to 250 as INA219 data got cut short
char Tnowc[100];
String REMOTE_CMD="";
bool REMOTE_IN = false;
bool MQTTFAST = false;
// _______________________________________________ get analog in data

float sensorA0val = 0.0;  
float sensorA1val = 0.0;  
float sensorA2val = 0.0;  
float sensorT0val = 0.0;


#ifdef useACS712
float Coffset = 1.650;
float Cfix = 8.190; // aka 5.0/3.3 / 0.185 Vdivider and hall factor
float Ctune = 1.0;  //
float ACS712amp = 0.0;
String ACS712s = "";
// add filter
float Ma = 0.15;
float Mb = 1.0 - Ma;
float A0signal = 0.0;
boolean Finit = true;

#endif

char thisA_Reads[70]; // ________________________________________________ filled by JobNsec used in HTML

float refmax = 4095.0; //1023.0; // 4095.0

float in_pct(int ain) {
  float ainr = ain * 100.0f / refmax; 
  return ainr;
}

float in_volt(int ain) {
  float ainr = ain * 3.3f / refmax; 
  return ainr;
}

#ifdef ESP32

#ifdef useESP32s
// NODE MCU ESP32s
#define A0pin 36 // __________ ADC1_0 pin 3 left
#define A1pin 39 // __________ ADC1_3 pin 4 left
#define A2pin 34 // __________ ADC1_6 pin 5 left

#else

#include <Esp.h>

#include "driver/temperature_sensor.h"

temperature_sensor_handle_t temp_handle = NULL;
temperature_sensor_config_t temp_sensor = {
    .range_min = 20,
    .range_max = 100
};

#define LEDC_CHANNEL_0 0 // _________________________________________ use first channel of 16 channels (started from zero)
#define LEDC_TIMER_12_BIT 12 // _____________________________________ use 12 bit precission for LEDC timer
#define LEDC_BASE_FREQ  5000 // _____________________________________ use 5000 Hz as a LEDC base frequency
#define LED_PIN 5 // ________________________________________________ fade LED PIN (replace with LED_BUILTIN constant for built-in LED) my board left pin 5 is GPIO05

#define A0pin 1 // __________ GPIO1 ADC1_0 pin 4 right
#define A1pin 2 // __________ GPIO2 ADC1_1 pin 5 right
#define A2pin 3 // __________ GPIO3 ADC1_2 pin 4 left

#endif

#else
//PICO W

#define A0pin 26 // __________ ADC0 (31)
#define A1pin 27 // __________ ADC1 (32)
#define A2pin 28 // __________ ADC2 (34)

// [BOOTSEL] press release reboot
boolean BOOTSELwaspressed = false;

void BOOTSEL_REBOOT() {  // PICO board without [RESET] can use [BOOTSEL] button

  if ( BOOTSEL ) {
    Serial.println("BOOTSEL PRESSED");
    BOOTSELwaspressed = true;
  } else {
    if (BOOTSELwaspressed) {
      Serial.println("BOOTSEL RELEASED");
      rp2040.reboot();  // untested for PICO 2W
    }
    BOOTSELwaspressed = false;
  }

}
#endif

bool thisLED_status = 0;

void LED_toggle() {
  thisLED_status = !thisLED_status;
  digitalWrite(LED_BUILTIN, thisLED_status);
  Serial.println("gpio LED_TOGGLE");
  }

void Ains() {
  // change to Volt for A0 A1 A2
  sensorA0val = in_volt(analogRead(A0pin));
  sensorA1val = in_volt(analogRead(A1pin));
  sensorA2val = in_volt(analogRead(A2pin));
#ifdef ESP32

#ifdef useESP32s
  // get ESP32s temp?
  sensorT0val = -0.007;
#else  // ESP32S3
  //ESP_ERROR_CHECK (temperature_sensor_enable(temp_handle));
  ESP_ERROR_CHECK (temperature_sensor_get_celsius (temp_handle, &sensorT0val));
  //ESP_ERROR_CHECK (temperature_sensor_disable (temp_handle));
#endif

#else // PICO
  BOOTSEL_REBOOT();
  sensorT0val = analogReadTemp();
#endif


#ifdef useACS712
// here use the A0 sensorA0val (Volt) and calc about the ACS AMP  ( for 5Amp version 0.185 V/A)
// Coffset = 1.650; // Cfix = 8.190; Ctune = 1.0; ACS712amp = 0.0; ACS712s = ""; // add filter  float Ma = 0.3; float Mb = 1.0 - Ma; float A0signal = 0.0; boolean useFilter = true; boolean Finit = true;

  //sensorA0val = 1.65;  // _____________________________ simulate input test if no hardware available

  if ( Finit ) {
    Finit = false;
    A0signal = sensorA0val;  // ________________________ init to avoid the first 15 sec SWING IN
  }
  if ( ACS712useFilter ) {
    A0signal = Ma * sensorA0val + Mb * A0signal;
  } else {
    A0signal = sensorA0val;
  }
  ACS712amp = ( A0signal - Coffset ) * Cfix * Ctune;

  if ( ACS712diag ) {  // diagnostic print
    // now make diagnostic print a mqtt dataset
    ACS712s = "{ \"input\": "+String(sensorA0val,3)+", \"Ma\": "+String(Ma,3)+", \"Mb\": "+String(Mb,3)+", \"Filter\": "+String(ACS712useFilter)+", \"A0signal\": "+String(A0signal,3)+", \"Coffset\": "+String(Coffset,3)+", \"Cfix\": "+String(Cfix,3)+", \"Ctune\": "+String(Ctune,3)+", \"ACS712amp\": " + String(ACS712amp,3)+" }";
    Serial.println(ACS712s);
    BootLog = ACS712s;
    send_MQTT_LOG();
  }
  sensorA0val = ACS712amp;     // ________________________  temporary overwrite the measured Volt with the ACS712 calculation and it goes all the way to broker and database without any code

#endif


  //snprintf(thisA_Reads,100, " ___ Job15sec: Ain: A0: %.2f A1: %.2f A2: %.2f [pct] T0: %.2f [degC] DO5: %.2f [pct] ", in_pct(sensorA0val), in_pct(sensorA1val), in_pct(sensorA2val), sensorT0val, DO5val );
  // global A_Reads feed here ( with the 15 sec job timing ) and used by web_wifi
  A_Reads  = " Ain: A0: ";
  A_Reads += String(sensorA0val,3);
  A_Reads += " A1: ";
  A_Reads += String(sensorA1val,3);
  A_Reads += " A2: ";
  A_Reads += String(sensorA2val,3);
  A_Reads += " [volt] T0: ";
  A_Reads += String(sensorT0val,1); // already float
  A_Reads += " [degC] ";
#ifdef use4RELAY
  board_DOUT_stat(); // also make the relay status report string
#endif
}

#include <time.h>

int TZs = LOCAL_TZ*3600; // ____________________________________________________ Time zone see secrets.h

// use : nows(); Serial.println(Tnows);
void nows() { 
  time_t now = time(nullptr);
  now += TZs; // ____________________________________________________ add TZs
  struct tm ti;
  gmtime_r(&now, &ti);
  sprintf(Tnowc, "%d-%02d-%02d %02d:%02d:%02d",ti.tm_year+1900,ti.tm_mon+1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec );
  Tnows = String(Tnowc);
}

void setup_IO() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // _______________________________ later operated by webpage and local MQTT
  LED_toggle(); // _________________________________________________ see LED change to know board working
#ifdef use4RELAY
  Relayboard_board_setup(); // see DOUT_4_RELAY.ino
#endif

  analogReadResolution(12); //now the 4095 works
#ifdef ESP32

#ifdef useESP32s
  // not use temperature
#else // ESP32S3
  ESP_ERROR_CHECK (temperature_sensor_install (&temp_sensor, &temp_handle));
  ESP_ERROR_CHECK (temperature_sensor_enable(temp_handle));
#endif

#endif
}

void setup(void) {
  setup_IO();
  Serial.begin(115200); // __________________________________________ talk to serial
  while (!Serial && (millis() < 10000));
  Serial.println();

#ifdef ESP32

#ifdef useESP32s
  msg="ESP32 on Arduino IDE 2\nmodel: ";
  msg += ESP.getChipModel();
#else // ESP32S3
  msg="ESP32S3 on Arduino IDE 2\nmodel: ";
  msg += ESP.getChipModel();
#endif

#else // for PICO still need to find out if chip & wifi model info available 

#ifdef usePICOW2
  msg="PICO 2W on Arduino IDE 2";
#else
  msg="PICO W on Arduino IDE 2";
#endif

#endif
  Serial.println(msg);
  BootLog = msg + "\n";
  msg = String(PROJECT) + " revision " + String(REV); 
  Serial.println(msg);
  BootLog += msg + "\n";

#ifdef use_boot_ms
  msg="uptime [s]: " + String(millis()/1000.0 , 2);
  Serial.println(msg);
  BootLog += msg + "\n";
#endif

#ifdef use_INA_219
  setup_INA219();
#endif
#ifdef use_INA_260
  setup_INA260();
#endif

#ifdef useACS712
  msg="ACS712 enabled";
  Serial.println(msg);
  BootLog += msg + "\n";
#endif

  setup_WIFI(); // __________________________________________________ see web_wifi.ino

#ifdef use_boot_ms
  msg="uptime [s]: " + String(millis()/1000.0 , 2);
  Serial.println(msg);
  BootLog += msg + "\n";
#endif

  setup_MQTT(); // ____________________________________________________ see web_wifi.ino

#ifdef use_boot_ms
  msg="END SETUP\nuptime [s]: " + String(millis()/1000.0 , 2);
  Serial.println(msg);
  BootLog += msg + "\n";
#endif

  send_MQTT_BootLog(); // ___________________________________________ now after setup want send BootLog to MQTT
}

int loopc = 0;
const int loopclim = 1000; // _______________________________________ cycles until we check time
int thissec = 0;
int thismin = 0;

void loopC () {
  loopc += 1;
  if ( loopc >= loopclim ) {
    loopc = 0;
    time_t now = time(nullptr);
    struct tm ti;
    gmtime_r(&now, &ti);
    int nowsec = ti.tm_sec;
    if (nowsec != thissec) {
      thissec = nowsec;
      thismin = ti.tm_min;
      Serial.print(".");
      //Serial.println(thissec); // _________________________________ other 1 sec JOBs
      Ains();

#ifdef use_INA_219
      make_INA219(); // _____________________________________ see INA.ino
#endif
#ifdef use_INA_260
      make_INA260(); // _____________________________________ see INA.ino
#endif

      run_WIFI(); // ________________________________________________ moved from main
      run_MQTT(); // ________________________________________________ also from web_wifi.ino ( for subscription ... )
#ifdef use4RELAY
      RelayOp_pending();  // see DOUT_4_RELAY.ino
#endif
      if ( MQTTFAST) {
        send_MQTT_FAST();
      }
    }
  }
}

int loopNs = 0;
int loopNc = 0;
int loopNslim = 15; // ______________________________________________ make a 15 sec tick

void JobNsec() { // _________________________________________________ and do some analog reading
  //Serial.println(); // ______________________________________________ after the "." need a LF
  //Ains(); // ________________________________________________________ do the readings see pico_w_io.ino AND PID and store to global
  //Serial.println(A_Reads);
}

void loopNsec() {
  if (loopNs != thissec) {
    loopNs = thissec;
    loopNc += 1;
    if ( loopNc >= loopNslim ) {
      loopNc = 0;
      JobNsec();
    }
  }
}

int loop1minc = 0;

void Job1min(bool PRINT=false){
  nows();
  if ( PRINT ) {
    Serial.println();
    Serial.print("___ loop1min ");
    Serial.println(Tnows); // set by nows();
  }
  WIFI_connect(); // check if WIFI needs reconnect
  make_MQTT(); // only makes the string
  send_MQTT();
}

void loop1min() { // ________________________________________________ dt is 1min
  if ( loop1minc != thismin && thissec == 0) { // ___________________ avoid the first min print/mqtt at xx sec after start
    loop1minc = thismin;
    Job1min();
  }
}

int loop1M = 0;
unsigned long lastTms = millis();
unsigned long nowms=lastTms;
bool showms = false; //true; //false; 
bool showT = false; // true;

void loopM() {
  loop1M += 1;
  if ( loop1M >= 1000000 ) { // ________________________________________ about 11 sec ? now 22 sec / without MDNBS.update now 9 sec
    loop1M = 0;
    if ( showms ) {
      nowms = millis();
      Serial.printf("\n___ loop1M dt : %.2f sec",(nowms - lastTms)/1000.0);
      lastTms = nowms;
    }
    if ( showT ) {
      Serial.println(); // _____________________________________________ after the "." need a LF
      Serial.print("___ loop1M: ");
      nows();
      Serial.println(Tnows); // set by nows();
    }
  }
}

void loop(void) {
  loopC(); // __________________________________________________________ Mloop counter and one sec JOB ... print '.' update wifi and mqtt, do Ains() 
  //loopNsec(); // _____________________________________________________ unused 15 sec JOB,
  loop1min(); // _______________________________________________________ MQTT
  loopM(); //___________________________________________________________ report dt for 1M loops ( disabled )
}

// check 1Mloop PICO W ( 133MHz)
// loop1M dt : 0.73
// full code / last check
// loop1M dt : 0.96

// check ESP32S3 (240MHz)
//___ loop1M dt : 0.42 sec
