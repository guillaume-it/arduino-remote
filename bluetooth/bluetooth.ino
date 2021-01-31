#include "hardwareSerial.h"
#include <stdio.h>
#include <Servo.h>
#include <pt.h>   // include protothread library
static struct pt ptServo, ptMotor; // each protothread needs one of these

//Message
const static String BLUETOOTH_INITIALISATION = "I";
const static String BLUETOOTH_CONNECTED = "C";
const static String BLUETOOTH_SERVO = "S";
const static String BLUETOOTH_MOTOR = "M";
const static byte idSeparator = 2;

//Servo
const static char PIN_Servo = 3;
static uint8_t angleSettingOld;
static uint8_t angleSetting = 90;
Servo servo;             //  Create a DC motor drive object

//Moto
unsigned int carSpeed_rocker = 250;
const static char ENA = 5;
const static char ENB = 6;
const static char IN1 = 7;
const static char IN2 = 8;
const static char IN3 = 9;
const static char IN4 = 11;

const static char LED_Pin = 13;


void setup() {
  //Bluetooth
  Serial.begin(9600);
  servo.attach(3);

  //Motor
  pinMode(IN1, OUTPUT); //Motor-driven port configuration
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  digitalWrite(ENA, HIGH);  //Enable left motor
  digitalWrite(ENB, HIGH);  //Enable right motor

  //Thread
  PT_INIT(&ptServo);  // initialise the two
  PT_INIT(&ptMotor);  // protothread variables
}

void loop() {
  servoControl(&ptServo, 100);
  while ( Serial.available() > 0) //Forcibly wait for a frame of data to finish receiving
  {
    String    c =   Serial.readString();

    int end = c.indexOf('}');
    if (c[0] == '{' && c[end] == '}') {
      Serial.println("{" + c + "}");
      Serial.flush();
      manageMessage(c, end);
    }
  }
}

void manageMessage(String message, int end)
{

  String key = message.substring(1, idSeparator);
  String value = message.substring(idSeparator + 1, end);


  if (BLUETOOTH_INITIALISATION == key) {
    Serial.println("{" + BLUETOOTH_CONNECTED + ":}");
    Serial.flush();
  } else if (BLUETOOTH_SERVO == key) {
    angleSetting = value.toInt();
  } else if (BLUETOOTH_MOTOR == key) {
    int motor = value.toInt();
    if (motor >= 0) {
      forward(false, motor);
    } else {
      back(false, motor * -1);
    }

  }
}

void servoControl(uint8_t angle){
      if (angleSettingOld != angle) {
      angleSettingOld = angle;
      if (angle > 175)
      {
        angle = 175;
      }
      else if (angle < 5)
      {
        angle = 5;
      }
      servo.write(angle); //sets the servo position according to the  value
    }
}


/*
  Servo Control angle Setting
*/
static int servoControl(struct pt *pt, int interval)
{
  static unsigned long timestamp = 0;
  PT_BEGIN(pt);
  while (1) { // never stop
    /* each time the function is called the second boolean
       argument "millis() - timestamp > interval" is re-evaluated
       and if false the function exits after that. */
    PT_WAIT_UNTIL(pt, millis() - timestamp > interval);
    timestamp = millis(); // take a new timestamp
    servoControl(angleSetting);
  }
  PT_END(pt);
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
