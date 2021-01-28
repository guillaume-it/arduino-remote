#include "hardwareSerial.h"
#include <stdio.h>

enum CONNECTION_STATUS
{
  START,
  END,
} connectTionStatus = END;

const static String BLUETOOTH_INITIALISATION = "INIT";
const static String BLUETOOTH_CONNECTED = "START";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //initialization
}

void loop() {
  // put your main code here, to run repeatedly:
  char c;
  static String message = "";

  while ( Serial.available() > 0) //Forcibly wait for a frame of data to finish receiving
  {

    c =   (char)Serial.read();

    if (c == '{') {
      connectTionStatus = START;
      Serial.println("LOG: Début transmission");
    } else  if (c == '}') {
      connectTionStatus = END;
      Serial.println("LOG: Fin transmission");
      if (message == BLUETOOTH_INITIALISATION) {
        Serial.println(BLUETOOTH_CONNECTED);
        message = "";
      }
    } else if ( connectTionStatus == START) {
      message += c;
    }
  }

}
