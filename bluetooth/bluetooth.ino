#include "hardwareSerial.h"
#include <stdio.h>
#include <Servo.h>

enum CONNECTION_STATUS
{
  START,
  END,
} connectTionStatus = END;

const static String BLUETOOTH_INITIALISATION = "INIT";
const static String BLUETOOTH_CONNECTED = "START";

#define PIN_Servo 3
Servo servo;             //  Create a DC motor drive object

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //initialization
  ServoControl(90);
}

void loop() {
  // put your main code here, to run repeatedly:
  char c;
  static String messageConcat = "";

  // 63 bytes free in the serial
  while ( Serial.available() > 0) //Forcibly wait for a frame of data to finish receiving
  {
    // sizeof return the number of bytes
    c =   (char)Serial.read();
    messageConcat += c;

    int start = messageConcat.indexOf('{');
    int endM = messageConcat.indexOf('}');
    if ( start >= 0 && endM >= 0) {
      String message = messageConcat.substring(start + 1, endM);
      messageConcat = "";

      int indexPoints = message.indexOf(':');
      String key = message.substring(0, indexPoints);
      String value = message.substring(indexPoints, message.length());


      Serial.println(Serial.availableForWrite());
      Serial.flush();

      Serial.println("{LOG: key=" + key);
      Serial.flush();
      Serial.println(" Value=" + value + "}");
      Serial.flush();
      if (BLUETOOTH_INITIALISATION == key) {
        Serial.println("{" + BLUETOOTH_CONNECTED + ":}");
        Serial.flush();
       //  ServoControl(0);
      }else{
        // ServoControl(180);
      }
    }
    //Serial.println(messageConcat);

  }

}

/*
 Servo Control angle Setting
*/
void ServoControl(uint8_t angleSetting)
{
  if (angleSetting > 175)
  {
    angleSetting = 175;
  }
  else if (angleSetting < 5)
  {
    angleSetting = 5;
  }
  servo.attach(3);
  servo.write(angleSetting); //sets the servo position according to the  value
  delay(500);
  servo.detach();
}
