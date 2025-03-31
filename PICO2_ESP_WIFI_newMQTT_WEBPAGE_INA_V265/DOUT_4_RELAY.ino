// see secrets.h if enabled

#ifdef use4RELAY
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


int R1stat=0;
int R2stat=0;
int R3stat=0;
int R4stat=0;


void RELAY_on(int Rx=1) {
    if ( Rx == 1 ) {
        digitalWrite(R1, LOW); // LOW
        R1stat=1; // OptoCoupler Led ON, RELAY ON
        Serial.println("pio R1 ON");
    }
    else if ( Rx == 2 ) {
        digitalWrite(R2, LOW); // LOW
        R2stat=1; // OptoCoupler Led ON, RELAY ON
        Serial.println("pio R2 ON");
    }
    else if ( Rx == 3 ) {
        digitalWrite(R3, LOW); // LOW
        R3stat=1; // OptoCoupler Led ON, RELAY ON
        Serial.println("pio R3 ON");
    }
    else if ( Rx == 4 ) {
        digitalWrite(R4, LOW); // LOW
        R4stat=1; // OptoCoupler Led ON, RELAY ON
        Serial.println("pio R4 ON");
    }
  }
    
void RELAY_off( int Rx=1) {
    if ( Rx == 1 ){
        digitalWrite(R1, HIGH); // HIGH
        R1stat=0; // OptoCoupler Led OFF, RELAY OFF
        Serial.println("pio R1 OFF");
    }
    else if ( Rx == 2 ) {
        digitalWrite(R2, HIGH); // HIGH
        R2stat=0; // OptoCoupler Led OFF, RELAY OFF
        Serial.println("pio R2 OFF");
    }
    else if ( Rx == 3 ) {
        digitalWrite(R3, HIGH); // HIGH
        R3stat=0; // OptoCoupler Led OFF, RELAY OFF
        Serial.println("pio R3 OFF");
    }
    else if ( Rx == 4 ) {
        digitalWrite(R4, HIGH); // HIGH
        R4stat=0; // OptoCoupler Led OFF, RELAY OFF
        Serial.println("pio R4 OFF");
    }
}

void RelayOp_pending() {
 if ( REMOTE_IN ) { //____________________ tells us there was a subscribed msg
    REMOTE_IN = false; // reset
            if ( REMOTE_CMD == "TOGGLE" ) {
                LED_toggle();
            }
            if ( REMOTE_CMD == "R1OFF" ) {
                RELAY_off(1);
            }
            if ( REMOTE_CMD == "R1ON" ) {
                RELAY_on(1);
            }
            if ( REMOTE_CMD == "R2OFF" ) {
                RELAY_off(2);
            }
            if ( REMOTE_CMD == "R2ON" ) {
                RELAY_on(2);
            }
            if ( REMOTE_CMD == "R3OFF" ) {
                RELAY_off(3);
            }
            if ( REMOTE_CMD == "R3ON" ) {
                RELAY_on(3);
            }
            if ( REMOTE_CMD == "R4OFF" ) {
                RELAY_off(4);
            }
            if ( REMOTE_CMD == "R4ON" ) {
                RELAY_on(4);
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
  DOUTS += String(R1stat);
  DOUTS += ", ";
  DOUTS += String(R2stat);
  DOUTS += ", ";
  DOUTS += String(R3stat);
  DOUTS += ", ";
  DOUTS += String(R4stat);
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