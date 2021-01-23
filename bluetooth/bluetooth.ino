#include "hardwareSerial.h"
#include <stdio.h>
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //initialization
}

void loop() {
  // put your main code here, to run repeatedly:
  static String SerialPortData = "";
  uint8_t c = "";  

    while ( Serial.available() > 0) //Forcibly wait for a frame of data to finish receiving
    {

      Serial.println( (char)Serial.read());
    }
  
}
