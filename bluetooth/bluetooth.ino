//#include "hardwareSerial.h"
#include <stdio.h>
#include <Servo.h>
#include <pt.h>   // include protothread library
#include "DigitalIO.h"

// Thread
static struct pt ptServoWheel, ptMotor, ptRadar; // each protothread needs one of these

//Message
const static String BLUETOOTH_INITIALISATION = "I";
const static String BLUETOOTH_CONNECTED = "C";
const static String BLUETOOTH_SERVO_WHEEL = "W";
const static String BLUETOOTH_RADAR = "R";
const static String BLUETOOTH_MOTOR = "M";

//Servo
const static char PIN_Servo = 3;
static unsigned int  angleWheelSettingOld = 90;
static unsigned int  angleWheelSetting = 90;
static unsigned int  angleRadarSetting = 90;

Servo servoWheel;             //  Create a motor drive object for the wheel
Servo servoRadar;             //  Create a motor drive object for the Radar

//Moto
unsigned int carSpeed_rocker = 250;
const static char ENA = 5;
const static char ENB = 6;
const static char IN1 = 7;
const static char IN2 = 8;
const static char IN3 = 9;
const static char IN4 = 11;
static int motorSpeed = 0;
static int motorSpeedOld = 0;
static int rotation = 1; // forward / backward
static int rotationOld = 0;

const static char LED_Pin = 13;

/*Arduino pin is connected to the Ultrasonic sensor module*/
const static int  ECHO_PIN = A4;
const static int TRIG_PIN = A5;
const int ObstacleDetection = 3;
static boolean radarWork = false;

void setup() {
  //Bluetooth
  Serial.begin(9600);
  servoWheel.attach(3);
  servoRadar.attach(10);

  //Motor
  fastPinMode(IN1, OUTPUT); //Motor-driven port configuration
  fastPinMode(IN2, OUTPUT);
  fastPinMode(IN3, OUTPUT);
  fastPinMode(IN4, OUTPUT);
  fastPinMode(ENA, OUTPUT);
  fastPinMode(ENB, OUTPUT);
  fastDigitalWrite(ENA, HIGH);  //Enable left motor
  fastDigitalWrite(ENB, HIGH);  //Enable right motor

  fastPinMode(ECHO_PIN, INPUT); //Ultrasonic module initialization
  fastPinMode(TRIG_PIN, OUTPUT);

  //Thread
  PT_INIT(&ptServoWheel);
  PT_INIT(&ptMotor);
  PT_INIT(&ptRadar);

}

void loop() {
  threadServoWheelControl(&ptServoWheel, 10);
  threadMotorControl(&ptMotor, 10);
  threadRadarControl(&ptRadar, 10);

  while ( Serial.available() > 0) //Forcibly wait for a frame of data to finish receiving
  {
    cutMessage(Serial.readString());
  }
}
void cutMessage(String message) {
  int start = message.indexOf('{');
  int endd = message.indexOf('}');
  int idSeparator = message.indexOf(':');

  Serial.println("{" + message + "}");
  Serial.flush();
  if (start >= 0 && start >= -1) {
    manageMessage(message.substring(start + 1, endd), endd, idSeparator);
    String newMessage = message.substring(endd + 1, message.length());
    if (newMessage.indexOf('{') >= 0 && newMessage.indexOf('}') >= 0) {
      cutMessage(newMessage);
    }
  }
}
void manageMessage(String message, int end, int idSeparator)
{

  String key = message.substring(0, idSeparator - 1);
  String value = message.substring(idSeparator , end);
  Serial.println("|key:" + key + " value:" + value + "|");
  Serial.flush();

  if (BLUETOOTH_INITIALISATION == key) {
    Serial.println("{" + BLUETOOTH_CONNECTED + ":}");
    Serial.flush();
  } else if (BLUETOOTH_SERVO_WHEEL == key) {
    Serial.println("{" + BLUETOOTH_SERVO_WHEEL + " " + value + "}");
    Serial.flush();
    angleWheelSetting = value.toInt();
  } else if (BLUETOOTH_MOTOR == key) {
    motorSpeed = value.toInt();
    Serial.println("{BLUETOOTH_MOTOR " + key + " motorSpeed " + motorSpeed + "}");
  } else if (BLUETOOTH_RADAR == key) {
    Serial.println("{BLUETOOTH_RADAR " + value + "}");
    radarWork = value.toInt();
  }
}

/*
  Servo Control angle Setting
*/
void servoWheelControl(unsigned int  angle) {

  if (angleWheelSettingOld != angle) {

    angleWheelSettingOld = angle;
    if (angle > 155)
    {
      angle = 155;
    }
    else if (angle < 25)
    {
      angle = 25;
    }
    Serial.println("{servoWheelControl angle:" + (String) angle + "}");
    servoWheel.write(angle); //sets the servo position according to the  value
  }
}

/*
  Thread to drive the servo
*/
static int threadServoWheelControl(struct pt *pt, int interval)
{
  static unsigned long timestamp = 0;
  PT_BEGIN(pt);
  while (1) { // never stop
    /* each time the function is called the second boolean
       argument "millis() - timestamp > interval" is re-evaluated
       and if false the function exits after that. */
    PT_WAIT_UNTIL(pt, millis() - timestamp > interval);
    timestamp = millis(); // take a new timestamp
    servoWheelControl(angleWheelSetting);
  }
  PT_END(pt);
}

static int threadMotorControl(struct pt *pt, int interval)
{
  static unsigned long timestamp = 0;
  PT_BEGIN(pt);
  while (1) { // never stop
    /* each time the function is called the second boolean
       argument "millis() - timestamp > interval" is re-evaluated
       and if false the function exits after that. */
    PT_WAIT_UNTIL(pt, millis() - timestamp > interval);
    timestamp = millis(); // take a new timestamp
    if (motorSpeed != motorSpeedOld) {
      motorSpeedOld = motorSpeed;

      if (motorSpeed >= 0) {
        rotationMotor(1, motorSpeed);
      } else {
        rotationMotor(-1, motorSpeed * -1);
      }
    }
  }
  PT_END(pt);
}

static int threadRadarControl(struct pt *pt, int interval)
{
  static unsigned long timestamp = 0;
  PT_BEGIN(pt);
  while (1) { // never stop
    /* each time the function is called the second boolean
       argument "millis() - timestamp > interval" is re-evaluated
       and if false the function exits after that. */
    PT_WAIT_UNTIL(pt, millis() - timestamp > interval);
    timestamp = millis(); // take a new timestamp
    if (radarWork) {
      angleRadarSetting += 10;
      if (angleRadarSetting > 180) {
        angleRadarSetting = 0;
      }

      servoRadar.write(angleRadarSetting);
      double distance = getDistance();
      //  Serial.println("{ get_Distance: " + (String)distance + "}");
      // Serial.flush();
    }
  }
  PT_END(pt);
}

/*
  Control motor：Car movement forward
*/
void rotationMotor(int rotation, int in_carSpeed)
{
  analogWrite(ENA, in_carSpeed);
  analogWrite(ENB, in_carSpeed);

  if (rotation == 1 && rotationOld != rotation) {
    rotationOld = 1;
    fastDigitalWrite(IN1, HIGH); // 7 -> PD7
    fastDigitalWrite(IN2, LOW); // 8 -> PB0
    fastDigitalWrite(IN3, LOW); // 9 -> PB1
    fastDigitalWrite(IN4, HIGH); // 11 -> PB3
  } else if (rotation == -1 && rotationOld != rotation) {
    rotationOld = -1;
    fastDigitalWrite(IN1, LOW); // 7 ->  PD7
    fastDigitalWrite(IN2, HIGH); // 8 -> PB0
    fastDigitalWrite(IN3, HIGH); // 9 -> PB1
    fastDigitalWrite(IN4, LOW); // 11 -> PB3
  }
}


/*ULTRASONIC*/
double getDistance(void)
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  double distance = duration * 0.034 / 2;

  return distance;
}
