// see secrets.h if enabled

#ifdef use4RELAY
// from file EEPROM
// // https://arduino-pico.readthedocs.io/en/latest/eeprom.html
// examples :
// PICO eeprom_write.ino
// PICO eeprom_read.ino

// we want use the first 4 byte of 512 of a FLASH EEPROM
// to remember the setting of our 4 relay board using 0 or 1 each
// where commands are given by web-site operation or MQTT SITE/DEVICE/set R1ON R1OFF ..
// that status want remember for a power fail reboot...

// looks like when read first time ( on this PICO 2W board ) reading shows 255
// so need a CLEAR

// now try do same on a ESP32S3
// https://github.com/espressif/arduino-esp32/blob/master/libraries/EEPROM/examples/eeprom_extra/eeprom_extra.ino


String DOUTS = "0,0,0,0";

#ifdef ESP32

#ifdef useESP32s
int R1 = 13; //GPIO
int R2 = 12; //GPIO
int R3 = 14; //GPIO
int R4 = 27; //GPIO
#else // ESP32S3
//+++ ESP32 : DO1 on GP05 pin 5
int R1 = 13; //GPIO
int R2 = 12; //GPIO
int R3 = 11; //GPIO
int R4 = 10; //GPIO
#endif

#else
//PICO W
int R1 = 13; //GPIO
int R2 = 12; //GPIO
int R3 = 11; //GPIO
int R4 = 10; //GPIO
#endif



#include <EEPROM.h>
byte value; // ____________________________________________ 8-bit unsigned number, from 0 to 255

byte myRelayStat[4] = {0,0,0,0}; // _______________________ initial 4 RELAY board memory
byte myRelaySet[4] = {0,0,0,0}; // ________________________ wanted setpoint like from EEPROM / web-site / MQTT

void setGPIO(int fromArray = 0,bool ONOFF = LOW ){
  if ( fromArray == 0 ) digitalWrite(R1, ONOFF);
  if ( fromArray == 1 ) digitalWrite(R2, ONOFF);
  if ( fromArray == 2 ) digitalWrite(R3, ONOFF);
  if ( fromArray == 3 ) digitalWrite(R4, ONOFF);
}

void myRelayControl(){
  String msg = ""; // _______________________________________ local loop info
  //msg += "myRelayControl\n";
  for (int i=0;i < 4 ;i++){
    if ( myRelayStat[i] != myRelaySet[i] ) {
      msg += "R";
      msg += String(i+1);
      msg += " status: ";
      msg += String(myRelayStat[i]);
      msg += " setpoint: ";
      msg += String(myRelaySet[i]);
      msg += " : ";
      if ( myRelaySet[i] == byte(1) ) {

        msg += "here need set RELAY\n";
        setGPIO(i,HIGH); // _________________________________ set hardware
        myRelayStat[i] = 1; // ______________________________ follow
#ifdef ESP32
        EEPROM.writeByte(i,1); // ___________________________ and remember for reboot
#else // PICO W
        EEPROM.write(i,1);
#endif
        EEPROM.commit();
      }

      if ( myRelaySet[i] == byte(0) ) { // __________________ could use else but want be sure no garbage numbers
        msg += "here need reset RELAY\n";
        setGPIO(i,LOW); // __________________________________ set hardware
        myRelayStat[i] = 0; // ______________________________ follow
#ifdef ESP32
        EEPROM.writeByte(i,0); // ___________________________ and remember for reboot
#else // PICO W
        EEPROM.write(i,0);
#endif
        EEPROM.commit();
      }

    }
  }
  Serial.print(msg);
}

void EEPROM_read(){
  msg += "EEPROM_read\n";
  for (int i=0;i < 4 ;i++){
    value = EEPROM.read(i);
    myRelaySet[i] = value;
    msg += "R";
    msg += String(i+1);
    msg += "\t";
    msg += String(value, DEC);
    msg += "\n";
    if ( myRelaySet[i] == byte(1) ) {
      msg += "here need preset RELAY\n";
      setGPIO(i,HIGH); // _________________________________ set hardware
      myRelayStat[i] = 1; // ______________________________ follow
    }
  }
}

void EEPROM_clear(){
  msg += "EEPROM_clear\n";
  value = 0;
  for (int i=0;i < 4 ;i++) {
#ifdef ESP32
    EEPROM.writeByte(i, value);
#else // PICO W
    EEPROM.write(i, value);
#endif
  }
  if (EEPROM.commit()) {
    msg += "EEPROM_clear successfully committed\n";
  } else {
    msg += "ERROR! EEPROM commit failed\n";
  }
}

void setup_EEPROM() {
  EEPROM.begin(512); // ___________________________________ 256…4096

  EEPROM_read();

  //EEPROM_clear(); // ____________________________________ we do this only first time on a new board to overwrite 255
  //EEPROM.end(); // ______________________________________ must use EEPROM.begin(512); again
}

void RelayOp_pending() {
 if ( REMOTE_IN ) { //____________________ tells us there was a subscribed msg
    REMOTE_IN = false; // reset
            if ( REMOTE_CMD == "TOGGLE" ) {
                LED_toggle();
            }
            if ( REMOTE_CMD == "R1OFF" ) {
                myRelaySet[0] = 0;
            }
            if ( REMOTE_CMD == "R1ON" ) {
                myRelaySet[0] = 1;
            }
            if ( REMOTE_CMD == "R2OFF" ) {
                myRelaySet[1] = 0;
            }
            if ( REMOTE_CMD == "R2ON" ) {
                myRelaySet[1] = 1;
            }
            if ( REMOTE_CMD == "R3OFF" ) {
                myRelaySet[2] = 0;
            }
            if ( REMOTE_CMD == "R3ON" ) {
                myRelaySet[2] = 1;
            }
            if ( REMOTE_CMD == "R4OFF" ) {
                myRelaySet[3] = 0;
            }
            if ( REMOTE_CMD == "R4ON" ) {
                myRelaySet[3] = 1;
            }
        }
}

void Relayboard_board_setup(){
  pinMode(R1, OUTPUT);
  digitalWrite(R1, HIGH);
  pinMode(R2, OUTPUT);
  digitalWrite(R2, HIGH);
  pinMode(R3, OUTPUT);
  digitalWrite(R3, HIGH);
  pinMode(R4, OUTPUT);
  digitalWrite(R4, HIGH);
}

void board_DOUT_stat() { // relay status as string
  DOUTS  = "\"";
  DOUTS += String(myRelayStat[0]);
  DOUTS += ", ";
  DOUTS += String(myRelayStat[1]);
  DOUTS += ", ";
  DOUTS += String(myRelayStat[2]);
  DOUTS += ", ";
  DOUTS += String(myRelayStat[3]);
  DOUTS += ",\"";
}


String RelayOP() {
  String DYN_HTMLs = "<h2>RELAY Control</h2>\
\
<table>\
  <tr>\
    <td><form action=\"./R1on\">\
<input type=\"submit\" value=\"R1 on\" />\
</form>\
</td>\
    <td><form action=\"./R2on\">\
<input type=\"submit\" value=\"R2 on\" />\
</form></td>\
    <td><form action=\"./R3on\">\
<input type=\"submit\" value=\"R3 on\" />\
</form>\
</td>\
    <td><form action=\"./R4on\">\
<input type=\"submit\" value=\"R4 on\" />\
</form>\
</td>\
  </tr>\
  <tr>\
    <td><form action=\"./R1off\">\
<input type=\"submit\" value=\"R1 off\" />\
</form>\
</td>\
    <td><form action=\"./R2off\">\
<input type=\"submit\" value=\"R2 off\" />\
</form>\
</td>\
    <td><form action=\"./R3off\">\
<input type=\"submit\" value=\"R3 off\" />\
</form>\
</td>\
    <td><form action=\"./R4off\">\
<input type=\"submit\" value=\"R4 off\" />\
</form>\
</td>\
  </tr>\
</table>\
<p>RELAY state: ";
  DYN_HTMLs  += DOUTS;
  DYN_HTMLs  += "( might be overwritten by remote MQTT )</p>\
<hr>";
return   DYN_HTMLs;
}

#endif