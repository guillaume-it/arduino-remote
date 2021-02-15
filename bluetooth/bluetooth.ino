
#include <stdio.h>
#include <Servo.h>
#include "hardwareSerial.h"

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
// how much serial data we expect before a newline
const unsigned int MAX_INPUT = 50;

void setup() {
  //Bluetooth
  Serial.begin(9600);
  servoWheel.attach(3);
  servoRadar.attach(10);

  //Motor
  digitalWrite(IN1, OUTPUT); //Motor-driven port configuration
  digitalWrite(IN2, OUTPUT);
  digitalWrite(IN3, OUTPUT);
  digitalWrite(IN4, OUTPUT);
  digitalWrite(ENA, OUTPUT);
  digitalWrite(ENB, OUTPUT);
  pinMode(ENA, HIGH);  //Enable left motor
  pinMode(ENB, HIGH);  //Enable right motor

  digitalWrite(ECHO_PIN, INPUT); //Ultrasonic module initialization
  digitalWrite(TRIG_PIN, OUTPUT);
}

void loop() {

  if (motorSpeed != motorSpeedOld) {
    motorSpeedOld = motorSpeed;

    if (motorSpeed >= 0) {
      rotationMotor(1, motorSpeed);
    } else {
      rotationMotor(-1, motorSpeed * -1);
    }
  }
  servoWheelControl(angleWheelSetting);
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

// https://www.arduino.cc/en/Tutorial/BuiltInExamples/SerialEvent
void serialEvent() {
  while ( Serial.available() > 0) //Forcibly wait for a frame of data to finish receiving
  {
    // https://arduino.stackexchange.com/questions/22762/why-does-the-arduino-respond-so-slow-to-serial-input
    //  cutMessage(Serial.readString());
    static char input_line [MAX_INPUT];
    static unsigned int input_pos = 0;
    const byte inByte = Serial.read();
    switch (inByte)
    {

      case '}':   // end of text
        input_line [input_pos++] = inByte;
        input_line [input_pos] = 0;  // terminating null byte

        // terminator reached! process input_line here ...
        // process_data (input_line);

        cutMessage(input_line);
        // reset buffer for next time
        input_pos = 0;
        break;

      default:
        // keep adding if not full ... allow for terminating null byte
        if (input_pos < (MAX_INPUT - 1))
          input_line [input_pos++] = inByte;
        break;

    }  // end of switch


  }
}


void cutMessage(String message) {
  int start = message.indexOf('{');
  int endd = message.indexOf('}');
  int idSeparator = message.indexOf(':');

  Serial.println("{L: message=" + message + " start=" + start + " endd=" + endd + " idSeparator=" + idSeparator + "}");
  Serial.flush();
  if (start >= 0 && start >= -1) {
    manageMessage(message.substring(start + 1, endd));
    String newMessage = message.substring(endd + 1, message.length());
    if (newMessage.indexOf('{') >= 0 && newMessage.indexOf('}') >= 0) {
      cutMessage(newMessage);
    }
  }
}
void manageMessage(String message)
{
  int idSeparator = message.indexOf(':');
  String key = message.substring(0, idSeparator );
  String value = message.substring(idSeparator+1 , message.length());
  Serial.println("{key=" + key + " value=" + value + "}");
  Serial.flush();

  if (BLUETOOTH_INITIALISATION == key) {
    // Serial.println("{" + BLUETOOTH_CONNECTED + ":}");
    // Serial.flush();
  } else if (BLUETOOTH_SERVO_WHEEL == key) {
    // Serial.println("{" + BLUETOOTH_SERVO_WHEEL + " " + value + "}");
    // Serial.flush();
    angleWheelSetting = value.toInt();
  } else if (BLUETOOTH_MOTOR == key) {
    motorSpeed = value.toInt();
    // Serial.println("{BLUETOOTH_MOTOR " + key + " motorSpeed " + motorSpeed + "}");
  } else if (BLUETOOTH_RADAR == key) {
    // Serial.println("{BLUETOOTH_RADAR " + value + "}");
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
    // Serial.println("{servoWheelControl angle:" + (String) angle + "}");
    servoWheel.write(angle); //sets the servo position according to the  value
  }
}

/*
  Control motor：Car movement forward
*/
void rotationMotor(int rotation, int in_carSpeed)
{
  if (in_carSpeed < 50) {
    in_carSpeed = 0;
  }

  analogWrite(ENA, in_carSpeed);
  analogWrite(ENB, in_carSpeed);

  if (rotation == 1 && rotationOld != rotation) {
    rotationOld = 1;
    digitalWrite(IN1, HIGH); // 7 -> PD7
    digitalWrite(IN2, LOW); // 8 -> PB0
    digitalWrite(IN3, LOW); // 9 -> PB1
    digitalWrite(IN4, HIGH); // 11 -> PB3
  } else if (rotation == -1 && rotationOld != rotation) {
    rotationOld = -1;
    digitalWrite(IN1, LOW); // 7 ->  PD7
    digitalWrite(IN2, HIGH); // 8 -> PB0
    digitalWrite(IN3, HIGH); // 9 -> PB1
    digitalWrite(IN4, LOW); // 11 -> PB3
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
