/*  Author: Michael F. Varga
 *  Email: michael@engineerutopia.com
 *  Website: engineerutopia.com
 *  Date: February 5, 2011
 *  
 *  This is a simple program written to control a few fuctions of
 *  a still camera electronically. A character is sent to this 
 *  arduino via serial to signal that one of the buttons on the 
 *  camera needs to be toggled. A value of HIGH is assigned to a
 *  pin to trigger a relay which activates a relay to 
 *  electronically "push" the button. This microcontroller also
 *  controls a pan tilt unit ( two servos ) that moves the camera
 *  so we may look around in the environment.
 *  
 *  Definitions
 *  Button        Pin    Character
 *  Power         5      p
 *  Capture       4      c
 *  Zoom In       3      i
 *  Zoom Out      2      o
 *  Usb Enable    7      u
 *  Usb Disable   7      b
 *  Pan           9      q
 *  Tilt          10     w
 *  
 *  The microcontroller continuously writes a message to the serial
 *  port. Once the computer receives this message it should respond
 *  with the character 'r'. After this arduino receives the character
 *  it will stop emitting the token message. This handshaking is 
 *  performed to distinguish itself from any other device that may 
 *  be on a serial port without having to hassle with udev rules.
*/

#include <SoftwareSerial.h>
#include <Servo.h>

#define PWR_PIN  5
#define CAP_PIN  4
#define ZIN_PIN  3
#define ZOUT_PIN 2
#define USB_PIN  7
#define PAN_PIN  9
#define TILT_PIN 10

#define PWR_CHAR  'p'
#define CAP_CHAR  'c'
#define ZIN_CHAR  'i'
#define ZOUT_CHAR 'o'
#define USBE_CHAR 'u'
#define USBD_CHAR 'b'
#define PAN_CHAR  'q'
#define TILT_CHAR 'w'

int val;
bool emit;
Servo pan;
Servo tilt;

void setup() {
  Serial.begin(115200);
  
  //setup the pins to output a value
  pinMode(PWR_PIN, OUTPUT);
  pinMode(CAP_PIN, OUTPUT);
  pinMode(ZIN_PIN, OUTPUT);
  pinMode(ZOUT_PIN, OUTPUT);
  pinMode(USB_PIN, OUTPUT);
  
  //set the initial output value of the pins
  digitalWrite(PWR_PIN, LOW);
  digitalWrite(CAP_PIN, LOW);
  digitalWrite(ZIN_PIN, LOW);
  digitalWrite(ZOUT_PIN, LOW);
  digitalWrite(USB_PIN, LOW);
  
  //setup the servos so we can control the pan tilt unit
  pan.attach(PAN_PIN);
  tilt.attach(TILT_PIN);
  
  val = 0;
  emit = true;
}

void loop() {
  if(Serial.available()) {
    char ch = Serial.read();
    switch(ch) {
      case '0'...'9':
        val = val*10 + ch - '0';
        break;
      case PWR_CHAR:
        digitalWrite(PWR_PIN, HIGH);
        delay(100);
        digitalWrite(PWR_PIN, LOW);
        break;
      case CAP_CHAR:
        digitalWrite(CAP_PIN, HIGH);
        delay(100);
        digitalWrite(CAP_PIN, LOW);
        break;
      case ZIN_CHAR:
        digitalWrite(ZIN_PIN, HIGH);
        delay(val);
        digitalWrite(ZIN_PIN, LOW);
        val = 0;
        break;
      case ZOUT_CHAR:
        digitalWrite(ZOUT_PIN, HIGH);
        delay(val);
        digitalWrite(ZOUT_PIN, LOW);
        val = 0;
        break;
      case USBE_CHAR:
        digitalWrite(USB_PIN, HIGH);
        break;
      case USBD_CHAR:
        digitalWrite(USB_PIN, LOW);
        break;
      case 'r':
        emit = false;
        break;
      case PAN_CHAR:
        pan.write(val);
        val = 0;
        break;
      case TILT_CHAR:
        tilt.write(val);
        val = 0;
        break;
    }
  }
  
  if(emit) {
    Serial.write("$camera_controller\r\n");
    delay(100);
  }
}
