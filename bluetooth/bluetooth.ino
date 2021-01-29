#include "hardwareSerial.h"
#include <stdio.h>
#include <Servo.h>

const static String BLUETOOTH_INITIALISATION = "INIT";
const static String BLUETOOTH_CONNECTED = "START";
const static String BLUETOOTH_SERVO = "SERVO";
const static String BLUETOOTH_MOTOR = "MOTOR";

#define PIN_Servo 3
Servo servo;             //  Create a DC motor drive object
unsigned int carSpeed_rocker = 250;
#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11

#define LED_Pin 13
void setup() {
  //Bluetooth
  Serial.begin(9600);
  ServoControl(90);

  //Motor
  //  pinMode(IN1, OUTPUT); //Motor-driven port configuration
  //  pinMode(IN2, OUTPUT);
  //  pinMode(IN3, OUTPUT);
  //  pinMode(IN4, OUTPUT);
  //  pinMode(ENA, OUTPUT);
  //  pinMode(ENB, OUTPUT);
  //  digitalWrite(ENA, HIGH);  //Enable left motor
  //  digitalWrite(ENB, HIGH);  //Enable right motor
  pinMode(IN3, OUTPUT); //set IO pin mode OUTPUT
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  digitalWrite(ENB, HIGH);  //Enable right motor
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
      String value = message.substring(indexPoints + 1, message.length());
      Serial.println("{LOG: key=" + key+" value="+value+" indexPoints:"+indexPoints);
      Serial.flush();

      if (BLUETOOTH_INITIALISATION == key) {
        Serial.println("{" + BLUETOOTH_CONNECTED + ":}");
        Serial.flush();
        //  ServoControl(0);
      } else if (BLUETOOTH_SERVO == key) {
        ServoControl(value.toInt());
      } else if (BLUETOOTH_MOTOR == key) {
        //        digitalWrite(IN3, LOW);
        //        digitalWrite(IN4, HIGH);//Right wheel turning forwards
        //        delay(1500);             //delay 500ms
        //        digitalWrite(IN3, LOW);
        //        digitalWrite(IN4, LOW); //Right wheel stoped
        //        delay(1500);
        //        digitalWrite(IN3, HIGH);
        //        digitalWrite(IN4, LOW); //Right wheel turning backwards
        //        delay(1500);
        //        digitalWrite(IN3, HIGH);
        //        digitalWrite(IN4, HIGH); //Right wheel stoped
        //        delay(1500);
        int motor = value.toInt();
        if(motor>=0){
        forward(false, motor);
        }else{
          back(false,motor*-1);
        }
        
      }
    }
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

/*
  Control motor：Car movement forward
*/
void forward(bool debug, int16_t in_carSpeed)
{
  analogWrite(ENA, in_carSpeed);
  analogWrite(ENB, in_carSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  if (debug)
    Serial.println("Go forward!");
}
/*
  Control motor：Car moving backwards
*/
void back(bool debug, int16_t in_carSpeed)
{
  analogWrite(ENA, in_carSpeed);
  analogWrite(ENB, in_carSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  if (debug)
    Serial.println("Go back!");
}
