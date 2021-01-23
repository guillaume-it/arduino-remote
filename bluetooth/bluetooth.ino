#include "hardwareSerial.h"
#include <stdio.h>

enum CONNECTION_STATUS
{
  START,
  END,
} connectTionStatus = END;

const static String INITIALISATION_BLUETOOTH = "Initialisation Bluetooth";
const static String CONNECTED = "Connected";

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
    Serial.println(c);

    if (c == '{') {
      connectTionStatus = START;
      Serial.println("Début transmission");
    } else  if (c == '}') {
      connectTionStatus = END;
      Serial.println("Fin transmission");
      if (message == INITIALISATION_BLUETOOTH) {
        Serial.write("Connected");
      }
    } else if ( connectTionStatus = START) {
      message += c;
    }
  }

}
