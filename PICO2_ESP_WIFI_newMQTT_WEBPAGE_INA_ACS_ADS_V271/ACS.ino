// ACS712 connected to A0
// ACS758 connected to A0
// read A0
// filter
// make filter diagnostic string // mqtt style
// optional print parameters ( and send to mqtt /log )
// overwrite A0
#include "secrets.h"

#ifdef useACSxxx

#ifdef useACS712
float Coffset = 1.650; // default 1.650; for 3 * 10K Vdiv
float Cfix = 8.190; // aka 5.0/3.3 / 0.185 Vdivider and hall factor
float Ctune = 1.0;  //
#endif
#ifdef useACS758
float Coffset = 2.489;
float Cfix = -50.0; // aka / 0.020 hall factor // current reverse !
float Ctune = 1.0;  //
#endif

float ACSxxxamp = 0.0;

// add filter
float Ma = 0.3; // 0.3 weak filter // 0.1 strong filter
float Mb = 1.0 - Ma;
float A0signal = 0.0;
boolean Finit = true;

void setup_ACS() {
#ifdef useACS712
  msg="ACS712 on A0 enabled";
#endif
#ifdef useACS758
  msg="ACS758 on A0 enabled";
#endif

  Serial.println(msg);
  BootLog += msg + "\n";
}

void calc_ACS() {
// here use the A0 sensorA0val (Volt) and calc about the ACS AMP  ( for 5Amp version 0.185 V/A)
// Coffset = 1.650; // Cfix = 8.190; Ctune = 1.0; ACSxxxamp = 0.0; ACSxxxs = ""; // add filter  float Ma = 0.15; float Mb = 1.0 - Ma; float A0signal = 0.0; boolean useFilter = true; boolean Finit = true;

  //sensorA0val = 1.65;  // _____________________________ simulate input test 1.65V as 0Amp //if no hardware available

  if ( Finit ) {
    Finit = false;
    A0signal = sensorA0val;  // ________________________ init to avoid the first 15 sec SWING IN
    Serial.print("ACS filter init ");
    Serial.println(sensorA0val);
  }
  if ( ACSxxxuseFilter ) {
    A0signal = Ma * sensorA0val + Mb * A0signal;
  } else {
    A0signal = sensorA0val;
  }
  ACSxxxamp = ( A0signal - Coffset ) * Cfix * Ctune;
  // we do the creation of diagnostic string now in every loop so we can show it in web-page
  ACSxxxs = "{ \"input\": "+String(sensorA0val,3)+", \"Ma\": "+String(Ma,3)+", \"Mb\": "+String(Mb,3)+", \"Filter\": "+String(ACSxxxuseFilter)+", \"A0signal\": "+String(A0signal,3)+", \"Coffset\": "+String(Coffset,3)+", \"Cfix\": "+String(Cfix,3)+", \"Ctune\": "+String(Ctune,3)+", \"ACSxxxamp\": " + String(ACSxxxamp,3)+" }";
 
  if ( ACSxxxdiag ) {  // diagnostic print // now make diagnostic print a mqtt /log dataset
    Serial.println(ACSxxxs);
    BootLog = ACSxxxs;
    send_MQTT_LOG();
  }

  sensorA0val = ACSxxxamp;     // ________________________  temporary overwrite the measured Volt with the ACS calculation and it goes all the way to broker and database without any code
}


#endif