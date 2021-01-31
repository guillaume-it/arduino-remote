#include "hardwareSerial.h"
#include <stdio.h>
#include <Servo.h>


const static String BLUETOOTH_INITIALISATION = "I";
const static String BLUETOOTH_CONNECTED = "C";
const static String BLUETOOTH_SERVO = "S";
const static String BLUETOOTH_MOTOR = "M";

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

const static byte idSeparator = 2;

void setup() {
  //Bluetooth
  Serial.begin(9600);
    servo.attach(3);

  servoControl(90);

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
  while ( Serial.available() > 0) //Forcibly wait for a frame of data to finish receiving
  {
    String    c =   Serial.readString();
    int end = c.indexOf('}');
    if (c[0] == '{' && c[end] == '}') {
     // Serial.println("{" + c + "}");
    //  Serial.flush();
      manageMessage(c, end);
    }
  }
}

void manageMessage(String message, int end)
{

  String key = message.substring(1, idSeparator);
  String value = message.substring(idSeparator + 1, end);

 //Serial.println("{L:k=" + key+" v="+value + "}");
   //   Serial.flush();
  if (BLUETOOTH_INITIALISATION == key) {
    Serial.println("{" + BLUETOOTH_CONNECTED + ":}");
    Serial.flush();
  } else if (BLUETOOTH_SERVO == key) {
    servoControl(value.toInt());
  } else if (BLUETOOTH_MOTOR == key) {
    int motor = value.toInt();
    if (motor >= 0) {
      forward(false, motor);
    } else {
      back(false, motor * -1);
    }

  }
}

/*
  Servo Control angle Setting
*/
void servoControl(uint8_t angleSetting)
{
  if (angleSetting > 175)
  {
    angleSetting = 175;
  }
  else if (angleSetting < 5)
  {
    angleSetting = 5;
  }
  Serial.println("{L:angleSetting "+(String)angleSetting+"}");
      Serial.flush();
  servo.write(angleSetting); //sets the servo position according to the  value
  delay(15);

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
