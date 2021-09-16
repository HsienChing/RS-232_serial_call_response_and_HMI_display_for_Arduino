/*
  Sketch name: RS-232 serial call response and HMI display
  Suggested board: Arduino MEGA 2560 Rev3
  Purpose: 
    1. Send command to the device through RS-232.
    2. Get responses from the device through RS-232.
    3. Send the device responses to the human-machine interface (HMI) through Serial1.
    4. Arduino can monitor the device responses from the serial monitor.
  Suggested system setup: 
    1. A "RS-232 to TTL module" is used to convert the RS-232 signal because Arduino MEGA 2560 Rev3 does not support RS-232 directly.
    2. A HMI with UART interface is used to show the device responses through the Serial1 channel of the Arduino.
    
  Date: 4 June 2021
  Author: Dr. Hsien-Ching Chung
  ORCID: https://orcid.org/0000-0001-9364-8858
  
  Project Link: https://github.com/HsienChing/RS-232_serial_call_response_and_HMI_display_for_Arduino
  License: MIT License
  Copyright (c) 2021 Hsien-Ching Chung
*/

#include <SoftwareSerial.h>

// The official Arduino MEGA 2560 Rev3 supports these pins for SoftwareSerial.
// For example:
//    const byte rxPin = 10; 
//    const byte txPin = 11;
// If the signal is not available normaly, try other pins.
// For example:
const byte rxPin = 50; 
const byte txPin =  2;

SoftwareSerial mySerial (rxPin, txPin, 1); // SoftwareSerial(rxPin, txPin, inverse_logic)
// Sometimes the "inverse_logic" setting is required to adjust the signal from the "RS-232 to TTL module."
// In this case, it is "inverse_logic=1" since the "RS-232 to TTL module" sends the Arduino an "inverted TTL" signal.

byte Command_RVA[4] = {0x52,0x56,0x41,0x0D};  // Command: RVA\r
byte Command_Q1[3]  = {0x51,0x31,0x0D};       // Command: Q1\r

String str;     // Temp string
String str_sub; // Temp substring

// Device responses to command RVA
int P_total; 
int P_current;
int P_curr_pointer;

// Device responses to command Q1
int   VAC_out;
float VAC_out_f;
int   VAC_Hz;
float VAC_Hz_f;
int   V_Batt;
float V_Batt_f;

byte EndHex[3] = {0xFF,0xFF,0xFF}; // End command for HMI

void setup() {
  Serial.begin(9600);     // Serial for Arduino serial monitor
  Serial1.begin(115200);  // Serial for HMI with UART interface
  mySerial.begin(2400);   // Software serial for RS-232 to TTL
}

void loop() {
  establishContact1();  // Send a byte to establish contact until receiver responds
  if (mySerial.available() > 0 && mySerial.find('#') ) {
    
    str = mySerial.readStringUntil('\r'); // Get incoming string
    Serial.print("Incoming string for RVA:");
    Serial.println(str);

    str_sub = str.substring(0,4); // Resolve the string to get P_total
    P_total = str_sub.toInt();
    Serial.print("P_total(W):"); 
    Serial.println(P_total); 

    str_sub = str.substring(5,9); // Resolve the string to get P_current
    P_current = str_sub.toInt();
    Serial.print("P_current(W):"); 
    Serial.println(P_current); 

    Serial1.print("n0.val="); // Send device response to HMI
    Serial1.print(P_current);
    Serial1.write(EndHex,3);

    P_curr_pointer = map(P_current, 0, 2000, 0, 240); // map(value, fromLow, fromHigh, toLow, toHigh)

    Serial1.print("z0.val="); // Send device response to HMI
    Serial1.print(P_curr_pointer);
    Serial1.write(EndHex,3);

    mySerial.flush(); // flush buffer
  }

  establishContact2();  // Send a byte to establish contact until receiver responds
  if (mySerial.available() > 0 && mySerial.find('(') ) {
    
    str = mySerial.readStringUntil('\r'); // Get incoming string
    Serial.print("Incoming string for Q1:");
    Serial.println(str);

    str_sub = str.substring(12,17);
    VAC_out_f = str_sub.toFloat();
    VAC_out = VAC_out_f * 10;
    Serial.print("VAC_out(V):"); 
    Serial.println(VAC_out_f); 

    Serial1.print("x1.val=");
    Serial1.print(VAC_out);
    Serial1.write(EndHex,3);

    str_sub = str.substring(22,26);
    VAC_Hz_f = str_sub.toFloat();
    VAC_Hz = VAC_Hz_f * 10;
    Serial.print("VAC_Hz:"); 
    Serial.println(VAC_Hz_f); 

    Serial1.print("x2.val=");
    Serial1.print(VAC_Hz);
    Serial1.write(EndHex,3);

    str_sub = str.substring(27,31);
    V_Batt_f = str_sub.toFloat();
    V_Batt = V_Batt_f * 10;
    Serial.print("V_Batt(V):"); 
    Serial.println(V_Batt_f); 

    Serial1.print("x0.val=");
    Serial1.print(V_Batt);
    Serial1.write(EndHex,3);

    mySerial.flush(); // flush buffer
  }
}

void establishContact1() {
  while (mySerial.available() <= 0) {
    //mySerial.print("RVA");
    //mySerial.print('\r');
    mySerial.write(Command_RVA,4); // Send a command
    //mySerial.flush();
    delay(100);
  }
}

void establishContact2() {
  while (mySerial.available() <= 0) {
    //mySerial.print("Q1");
    //mySerial.print('\r');
    mySerial.write(Command_Q1,3); // Send a command
    //mySerial.flush();
    delay(100);
  }
}
