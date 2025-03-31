// web_wifi.ino
#include "secrets.h" // ______________________________________ here find the router login SSID PASSWORD ...

#include <WiFi.h>
#include <WebServer.h>

// 
#if MQTT_SECURE
#include <WiFiClientSecure.h>
WiFiClientSecure wifiClient;
#else
#include <WiFiClient.h>
WiFiClient wifiClient;
#endif

#ifdef ESP32
#include <MQTT.h>  // __________________________________________ ESP32 internal MQTT with TLS
MQTTClient mqttclient;
#else  // like PICO W // MQTT PubsubClient by Nick O'Leary 2.8.0
#include <PubSubClient.h> // old most easy mqtt lib, works NO TLS, local RPI broker .. NO send feedback provided?
PubSubClient mqttclient(wifiClient);
#endif

#define BIGbuf 5000
char DYN_HTML[BIGbuf];
String DYN_HTMLs = " DYN_HTMLs "; // try for creation of HTML a string type, only before send convert back to char

const char *ssid = SECRET_SSID;
const char *password = SECRET_PASSWORD;
const char *rev = REV;
const int port = PORT;
IPAddress thisip(FIXIP); // ___________________________________ fix IP for webserver

String URLHASH=SECRET_URLHASH; // if useFakeAuth true use long URL what looks like LOGIN
String thisURL="/";

// the selection what broker is used / and if TLS / see secrets.h
const char *mqtt_broker = SECRET_MQTTBROKER; // ___________________________________ MQTT IP broker // SECRET_LMQTTBROKER not work
const char *mqtt_clientid = MQTT_CLIENTID;
const int  mqtt_port = MQTT_port;
const char *mqtt_user = SECRET_MQTTUSER;
const char *mqtt_pass = SECRET_MQTTPW;

const char *mqtt_stopic = sTOPIC; // mqtt init status
const char *mqtt_btopic = bTOPIC; // mqtt BootLog
const char *mqtt_dtopic = dTOPIC; // data reporting
const char *mqtt_rtopic = rTOPIC; // remote relay operation
const char *mqtt_ftopic = fTOPIC;
//fTOPIC ___________ 1 sec reads A0

unsigned long lastNTPms = millis();
unsigned long nowNTPms=lastNTPms;
unsigned long lastWIFIms = millis();
unsigned long nowWIFIms=lastWIFIms;

void setClock() {
  msg="www Setup ntp\n";
  Serial.println(msg);
  BootLog += msg + "\n";

#ifdef ESP32
#define NTP_SERVER     "pool.ntp.org"
#define UTC_OFFSET     0
#define UTC_OFFSET_DST 0

#ifdef useESP32s
  //configTzTime("EST5EDT,M3.2.0,M11.1.0","pool.ntp.org");
  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
#else // ESP32S3
  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
#endif
  
#else
  NTP.begin("pool.ntp.org", "time.nist.gov");
#endif

  msg+="www Waiting for NTP time sync / max 30 sec ";
  // https://arduino-pico.readthedocs.io/en/latest/wifintp.html
  //NTP.waitSet([]() { Serial.print("."); }); // NTP callback
  time_t now = time(nullptr);
  lastNTPms = millis(); // cpu uptime
  nowNTPms=lastNTPms;
  while ( ( now < 365 * 24 * 3600 ) && ( lastNTPms + 30000 > nowNTPms ) ) {  // add own timeout, give it 30 sec max ??
    delay(1000);
    Serial.print("t");
    nowNTPms = millis(); // cpu uptime
    now = time(nullptr);
  }

  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  msg="\nwww Current time: GMT ";
  msg+=asctime(&timeinfo); // 
  nows();
  msg+="\nwww Local time: ";
  msg+=Tnows; // LOCAL TIME set by nows();
  Serial.println(msg);
  BootLog += msg + "\n";
}

bool MQTT_OK = false; // change because i not want to get it stuck if no broker online BUT now, if start broker later, must reset board

void setup_MQTT(){
  msg="mqtt setup_MQTT";

#if MQTT_SECURE
  wifiClient.setInsecure();  // secure but not check server cert
#endif

#ifdef ESP32  // now using <MQTT.h> from espressiv
  mqttclient.begin(mqtt_broker, mqtt_port,wifiClient);
  mqttclient.setKeepAlive(100);
  mqttclient.onMessage(callback);
#else
  mqttclient.setServer(mqtt_broker, mqtt_port);
  mqttclient.setCallback(callback);
#endif

  if ( !MQTT_OK ) {
    msg+="\nmqtt try connect to MQTT broker ...";
    if (mqttclient.connect(mqtt_clientid, mqtt_user, mqtt_pass)) {
      msg+="\nmqtt broker: ";
      msg+=mqtt_broker;
      msg+=" connected";
      MQTT_OK = true;
      
    } else {
      msg+="failed ";
    }
  }
  Serial.println(msg);
  BootLog += msg + "\n";

  if ( MQTT_OK ) {
    // Publish and subscribe
    mqttclient.publish(mqtt_stopic, MQTT_STATUS); 
    msg ="mqtt publish to topic: \n" + String(mqtt_stopic);
    msg+="\n" + String(mqtt_btopic);
    msg+="\n" + String(mqtt_dtopic);
    msg+="\nmqtt subscribe to: \n" + String(mqtt_rtopic);
    mqttclient.subscribe(mqtt_rtopic);
    msg+="\n" + String(mqtt_ftopic);
    mqttclient.subscribe(mqtt_ftopic); // read A0 every second on NR Dashboard command : FAST A0
    Serial.println(msg);
    BootLog += msg + "\n";
  }
}

#ifdef ESP32
void callback(String &atopic, String &apayload) { // try new MQTT.h callback
  // what MQTT commands we might get:
  // TOGGLE R1ON R1OFF R2ON R2OFF R3ON R3OFF R4ON R4OFF
  String PAYLOAD = ""; //(apayload);
  PAYLOAD = apayload;
#else  // PICO W
void callback(char *atopic, byte *apayload, unsigned int length) {
  String PAYLOAD = ""; //(apayload);
  PAYLOAD = "";
  for (int i = 0; i < int(length); i++) {  // ? length  is unsigned int
    PAYLOAD += (char) apayload[i];
  }
#endif
  // Serial.println(PAYLOAD); // test ?82
  REMOTE_CMD="";
  REMOTE_IN = false;

  if (PAYLOAD == "TOGGLE") {
      REMOTE_CMD="TOGGLE";
      REMOTE_IN = true;
  }
#ifdef use4RELAY 
  else if (PAYLOAD == "R1OFF") {
      REMOTE_CMD="R1OFF";
      REMOTE_IN = true;
  }
  else if (PAYLOAD == "R2OFF") {
      REMOTE_CMD="R2OFF";
      REMOTE_IN = true;
  }
  else if (PAYLOAD == "R3OFF") {
      REMOTE_CMD="R3OFF";
      REMOTE_IN = true;
  }
  else if (PAYLOAD == "R4OFF") {
      REMOTE_CMD="R4OFF";
      REMOTE_IN = true;
  }
  else if (PAYLOAD == "R1ON") {
      REMOTE_CMD="R1ON";
      REMOTE_IN = true;
  }
  else if (PAYLOAD == "R2ON") {
      REMOTE_CMD="R2ON";
      REMOTE_IN = true;
  }
  else if (PAYLOAD == "R3ON") {
      REMOTE_CMD="R3ON";
      REMOTE_IN = true;
  }
  else if (PAYLOAD == "R4ON") {
      REMOTE_CMD="R4ON";
      REMOTE_IN = true;
  }
#endif
  else if (PAYLOAD == "FASTON") {
      MQTTFAST = true;
  }
  else if (PAYLOAD == "FASTOFF") {
      MQTTFAST = false;
  }
  else { // something not undestood?
      Serial.print("\nmqtt Message arrived in topic: ");
      Serial.print(atopic);
      Serial.print(" : ");
#ifdef ESP32
      Serial.println(apayload);
#else
      for (int i = 0; i < int(length); i++) {
        Serial.print((char) apayload[i]);
      }
      Serial.println();
#endif
  }

}

int MQTT_count = 0;

void make_MQTT() { // for analog data use last reading
  // by job already nows(); // make Tnows
  //snprintf(thisMQTTs,200, "{ \"id\": %d, \"dev\":\"%s\", \"datetimes\": \"%s\", \"PS_Temp\":  %.2f, 
  // <br/> \"PID_PV\": %.2f, \"PID_SP\": %.2f, \"PID_OUT\": %.2f, \"PID_MODE\": %d }"
  // ,MQTT_count,mqtt_dtopic,tnows,get_internalT(),get_PV(),get_SP(),get_OUT(),get_MODE() );
  MQTTs  = "{ \"id\": ";
  MQTTs += String(MQTT_count);
  MQTTs += ", \"dev\":\"";
  MQTTs += String(mqtt_dtopic);
  MQTTs += "\", \"datetimes\": \"";
  MQTTs += Tnows;
  MQTTs += "\", \"PS_Temp\": ";
  MQTTs += String(sensorT0val,1);
  MQTTs += ", \"A0\": ";
  MQTTs += String(sensorA0val,3);
  MQTTs += ", \"A1\": ";
  MQTTs += String(sensorA1val,3);
  MQTTs += ", \"A2\": ";
  MQTTs += String(sensorA2val,3);
#ifdef use4RELAY
  MQTTs += ", \"DOUTS\": ";
  MQTTs += DOUTS; // RELAY status string made by Ains() now every second
#endif
#ifdef use_INA_219
  MQTTs += MQTTina; // powermeter measurement every second as string
#endif
#ifdef use_INA_260
  MQTTs += MQTTina; // if run both must have different mqtt variable names VDC2 ADC2 POW2, NOW SAME
#endif
  MQTTs += " }";

  MQTT_count += 1; // for next loop

  //Serial.print("___ MQTTstr :\n");
  Serial.println("");
  Serial.println(MQTTs); 
}

void send_MQTT() {
  MQTTs.toCharArray(MQTTc, 250);
  if ( mqttclient.connected() ) {
    MQTT_OK = true;
    mqttclient.publish(mqtt_dtopic,MQTTc);  // ____________________________ here NO ERROR when broker is disconnected HOW TO KNOW
  } else {
    MQTT_OK = false;
    //reconnect
    Serial.println("www want publish MQTT but NOT connected");
    mqttclient.connect(mqtt_clientid, mqtt_user, mqtt_pass);
    Serial.println("www MQTT broker try reconnect");

  }
}

void send_MQTT_FAST() {
  MQTTA0 = " { \"A0V\": "+String(sensorA0val,3)+" } ";
  MQTTA0.toCharArray(MQTTc,200);
  mqttclient.publish(mqtt_ftopic,MQTTc);
}

void send_MQTT_LOG() {
  BootLog.toCharArray(BootLogc,210);
  mqttclient.publish(mqtt_btopic,BootLogc);
}

void send_MQTT_BootLog() {
  int BLl = BootLog.length();
  Serial.print("mqtt send BootLog len : ");
  Serial.println( BLl ); // _________________________________________ 750 // GRRR for ESP and INA and remote host  815 char
  String temp = ""; 
  int chunks = BLl / 200; // ________________________________________ auto send chunks of 200 ++ chars
  for ( int i = 0; i <= chunks; i++) {
    int subsA = i *200;
    temp = " *"+String(i+1)+"*\n" + BootLog.substring(subsA,subsA+200);
    temp.toCharArray(BootLogc,210);
    mqttclient.publish(mqtt_btopic,BootLogc);
  }
  temp = ""; // clean
}

void run_MQTT() { // ________________________________________________ only if subscribe to topic
  mqttclient.loop();
}

WebServer server(port);

void handleData() {
    server.sendHeader("Location", thisURL,true); // REDIRECT  makes new HTML string with new analog and mqtt data
  server.send(302, "text/plain", "");
}

void handleLED() {
  LED_toggle();
  server.sendHeader("Location", thisURL,true); // REDIRECT  makes new HTML string with new analog and mqtt data
  server.send(302, "text/plain", "");
}

#ifdef use4RELAY 
void handleR1on() {
  RELAY_on(1);
  server.sendHeader("Location", thisURL,true); // REDIRECT  makes new HTML string with new analog and mqtt data
  server.send(302, "text/plain", "");
}

void handleR1off() {
  RELAY_off(1);
  server.sendHeader("Location", thisURL,true); // REDIRECT  makes new HTML string with new analog and mqtt data
  server.send(302, "text/plain", "");
}

void handleR2on() {
  RELAY_on(2);
  server.sendHeader("Location", thisURL,true); // REDIRECT  makes new HTML string with new analog and mqtt data
  server.send(302, "text/plain", "");
}

void handleR2off() {
  RELAY_off(2);
  server.sendHeader("Location", thisURL,true); // REDIRECT  makes new HTML string with new analog and mqtt data
  server.send(302, "text/plain", "");
}

void handleR3on() {
  RELAY_on(3);
  server.sendHeader("Location", thisURL,true); // REDIRECT  makes new HTML string with new analog and mqtt data
  server.send(302, "text/plain", "");
}

void handleR3off() {
  RELAY_off(3);
  server.sendHeader("Location", thisURL,true); // REDIRECT  makes new HTML string with new analog and mqtt data
  server.send(302, "text/plain", "");
}

void handleR4on() {
  RELAY_on(4);
  server.sendHeader("Location", thisURL,true); // REDIRECT  makes new HTML string with new analog and mqtt data
  server.send(302, "text/plain", "");
}

void handleR4off() {
  RELAY_off(4);
  server.sendHeader("Location", thisURL,true); // REDIRECT  makes new HTML string with new analog and mqtt data
  server.send(302, "text/plain", "");
}
#endif

void handleRoot() {
  Serial.println();
  Serial.print("www index.html served");
  // uptime char
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  char uptimec[100];
  snprintf(uptimec,100,"%02d:%02d:%02d",hr, min % 60, sec % 60);

  nows(); // and use global Tnows STRING

// copy from Micrpython and use [backslash] at each line end and before each " to esape or use ' instead
  DYN_HTMLs  = "<!DOCTYPE html>\
<html>\
<head>\
<title>";
  DYN_HTMLs += HTML_TITLE;
  DYN_HTMLs += "</title>\
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
<style>\
* {\
  box-sizing: border-box;\
}\
input[type=submit] {\
        background-color: Blue; //Violet;\
        border: none;\
        -webkit-border-radius: 5px;\
        border-radius: 5px;\
        color: #fff;\
        padding: 15px 30px;\
        text-decoration: none;\
        font-size: 16px;\
        font-weight: bold;\
        margin: 4px 2px;\
        cursor: pointer;\
      }\
\
.menu {\
  float: left;\
  width: 20%;\
  text-align: center;\
}\
\
.menu a {\
  background-color: #e5e5e5;\
  padding: 8px;\
  margin-top: 7px;\
  display: block;\
  width: 100%;\
  color: black;\
}\
\
.main {\
  float: left;\
  width: 60%;\
  padding: 0 20px;\
}\
\
.right {\
  background-color: #e5e5e5;\
  float: left;\
  width: 20%;\
  padding: 15px;\
  margin-top: 7px;\
  text-align: center;\
}\
\
@media only screen and (max-width: 620px) {\
  /* For mobile phones: */\
  .menu, .main, .right {\
    width: 100%;\
  }\
}\
</style>\
</head>\
\
<body style=\"font-family:Verdana;color:OliveDrab;\">\
\
<div style=\"background-color:#e5e5e5;padding:15px;text-align:center;\">\
  <h1>";
  DYN_HTMLs += HTML_HEADER;
  DYN_HTMLs += "</h1>\
</div>\
\
<div style=\"overflow:auto\">\
  <div class=\"menu\">\
    <a href='";
  DYN_HTMLs  += BLOGs;
  DYN_HTMLs  += " ' target='_blank'>KLL Blog</a>\
    <a href='https://docs.arduino.cc/software/ide/#ide-v2' target='_blank'>Arduino IDE</a>\
    <a href='https://www.raspberrypi.com/documentation/microcontrollers/pico-series.html' target='_blank'>RPI PICO W</a>\
    <a href='https://github.com/vcc-gnd/YD-ESP32-S3' target='_blank'>ESP32 S3 N16R8</a>\
  </div>\
<div class=\"main\">\
\
<h2>MQTT</h2><p>";
if ( MQTT_OK ) {
  DYN_HTMLs += "connected";
} else {
  DYN_HTMLs += "NOT connected";
}

  DYN_HTMLs += "</p><hr>\
<h2>Led Control</h2>\
<form action=\"./light\">\
<input type=\"submit\" value=\"LED toggle\" />\
</form>\
\
<p>LED state: :";
  if (thisLED_status) {
    DYN_HTMLs  += "ON";
  } else {
    DYN_HTMLs  += "OFF";
  }
  DYN_HTMLs  += " ( might be overwritten by remote MQTT )</p><hr>";
#ifdef use4RELAY
  DYN_HTMLs  += RelayOP();
#endif
  DYN_HTMLs  += "<h2>Fetch New Readings</h2>\
<form action=\"./value\">\
<input type=\"submit\" value=\"Fetch value\" />\
</form>\
<p>Readings: ( No Autorefresh ) <h4>Analog read</h4><h3>";
  DYN_HTMLs  += String(A_Reads); 
  DYN_HTMLs  += "</h3><h4>MQTT pub</h4><h3>";
  DYN_HTMLs  += MQTTs;
  DYN_HTMLs  += "</h3></p>\
  </div>\
\
<div class=\"right\">\
<h2>About</h2>\
<p>using <b>";
DYN_HTMLs  += HTML_TITLE;
DYN_HTMLs  += "</b></br>via Arduino IDE</br>+ WEB SERVER</br>+ MQTT (TLS)</br>+ 4 relay board</br>+ INA 219/260</p>\
</div>\
\
</div>\
\
<div style=\"background-color:#e5e5e5;text-align:center;padding:10px;margin-top:7px;\">copyright CC-BY-SA 4.0 KLL engineering\
<p>";
DYN_HTMLs  += Tnows;
DYN_HTMLs  += " , uptime: ";
DYN_HTMLs  += String(uptimec);;
DYN_HTMLs  += "</p><p>";
DYN_HTMLs  += PROJECT;
DYN_HTMLs  += " : ";
DYN_HTMLs  += REV;
DYN_HTMLs  += "</p></div>\
</body>\
</html>";


  DYN_HTMLs.toCharArray(DYN_HTML,5000); // for HTML back to CHAR, still hope the creation of the string is more save as snprintf ?( about PANIC and Stack smashing protect failure!)
  //Serial.println(DYN_HTML);
  //Serial.printf(" len INDEX : %d \n",strlen(DYN_HTML));
  server.send(200, "text/html", DYN_HTML);
  DYN_HTML[0]= '\0'; // kind of clear

}

void handleNoLogin() {
  String message = "login or F*CK OFF";
  server.send(404, "text/plain", message);
}

void handleNotFound() {
  //LED_toggle();
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
  //LED_toggle();
}

byte mac[6];

const char *macToString(uint8_t mac[6]) {
  static char s[20];
  sprintf(s, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return s;
}


bool WIFI_OK = false; // change because i not want to get it stuck if not online

void WIFI_connect() {
  if ( !WIFI_OK ) {
    Serial.println();
    //WiFi.disconnect();
    //delay(1000);
    msg = "www WIFI_connect / begin returns: ";
    msg += String( WiFi.begin(ssid, password) )+"\n";

    lastWIFIms = millis(); // cpu uptime
    nowWIFIms=lastWIFIms;
    // Wait for connection : replace:
    //while ( ( WiFi.status() != WL_CONNECTED ) && ( lastWIFIms + 30000 > nowWIFIms ) ) { // add own timeout, give it 30 sec max ??
#ifdef ESP32
    while ( ( !WiFi.isConnected() ) && ( lastWIFIms + 30000 > nowWIFIms ) )
#else // PICO W
    while ( ( !WiFi.connected() ) && ( lastWIFIms + 30000 > nowWIFIms ) ) 
#endif
    {
      delay(1000);
      Serial.print("w"); // never see that 'w' with pico 2w
      nowWIFIms = millis(); // cpu uptime
    }

    if ( WiFi.status() == WL_CONNECTED ){
      WIFI_OK = true;
#ifdef use_boot_ms
      msg+="www setup_WIFI OK uptime [s]: " + String(millis()/1000.0 , 2);
#endif
    } else {
      WIFI_OK = false;
#ifdef use_boot_ms
      msg+="www setup_WIFI FAILED uptime [s]: " + String(millis()/1000.0 , 2);
#endif
    }
    Serial.println(msg);
    BootLog += msg + "\n";
  }
}

// WIFI_connect(); // put in 1MIN job for reconnect

void setup_WIFI() {
  // _______________________________________ connect to router
#ifdef use_boot_ms
  msg="www setup_WIFI uptime [s]: " + String(millis()/1000.0 , 2);
  msg+="\nwww try WIFI connect to " + String(ssid);
  Serial.println(msg);
  BootLog += msg + "\n";
#endif

  WiFi.mode(WIFI_STA);
#ifdef ESP32
  WiFi.setMinSecurity(WIFI_AUTH_WEP);  // should help on certain router problems
#endif // error on PICO 2W
  if ( useFIXIP ) {
    WiFi.config(thisip);
  }

  WIFI_connect();

  Serial.println();
#ifdef ESP32
  msg="www ESP32 board MAC Address: ";
  msg+=WiFi.macAddress();
#else
  WiFi.macAddress(mac);
  msg="www PICO board MAC Address: ";
  msg +=macToString(mac);
#endif
  Serial.println(msg);
  BootLog += msg + "\n";
  msg="www Connected to ";
  msg+=ssid;
  msg+="\nwww IP address: ";
  String myIPs = WiFi.localIP().toString();
  msg += myIPs;
  Serial.println(msg);
  BootLog += msg + "\n";

  setClock(); // ____________________________ setup NTP to RTC


#ifdef use_boot_ms
  msg="uptime [s]: " + String(millis()/1000.0 , 2);
  Serial.println(msg);
  BootLog += msg + "\n";
#endif

  if ( useFakeAuth ) {
    thisURL = URLHASH;
    server.on("/", handleNoLogin);
  } 
  server.on(thisURL, handleRoot);
  server.on(thisURL+"value", handleData);
  server.on(thisURL+"light", handleLED);
#ifdef use4RELAY
  server.on(thisURL+"R1on", handleR1on);
  server.on(thisURL+"R1off", handleR1off);
  server.on(thisURL+"R2on", handleR2on);
  server.on(thisURL+"R2off", handleR2off);
  server.on(thisURL+"R3on", handleR3on);
  server.on(thisURL+"R3off", handleR3off);
  server.on(thisURL+"R4on", handleR4on);
  server.on(thisURL+"R4off", handleR4off);
#endif
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  
  server.onNotFound(handleNotFound);
  server.begin();

  msg="www HTTP server started on http://";
  msg+=myIPs;              // thisip.toString();
  msg+=":";
  msg+=String(port);
  if ( useFakeAuth ) {
    msg+=URLHASH;
  }
  Serial.println(msg);
  BootLog += msg + "\n";

}

void run_WIFI() {
  server.handleClient();
  //MDNS.update();
}