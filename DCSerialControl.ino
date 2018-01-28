#include <CmdMessenger.h>  // CmdMessenger
// Attach a new CmdMessenger object to the default Serial port
CmdMessenger cmdMessenger = CmdMessenger(Serial);

// Blinking led variables 
bool ledState                   = 0;   // Current state of Led
const int kBlinkLed             = 13;  // Pin of internal Led

// Motor Shield stuff
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *TopMotor = AFMS.getMotor(1);


// Commands
enum
{
  kSetLed, // Command to request led to be set in specific state
  kSetMotorSpeed,
};

// Callbacks define on which received commands we take action 
void attachCommandCallbacks()
{
  cmdMessenger.attach(kSetLed, OnSetLed);
  cmdMessenger.attach(kSetMotorSpeed, OnSetMotorSpeed);
}

// Callback function that sets led on or off
void OnSetLed()
{
  // Read led state argument, interpret string as boolean
  ledState = cmdMessenger.readBoolArg();
  // Set led
  digitalWrite(kBlinkLed, ledState?HIGH:LOW);
}

void OnSetMotorSpeed() {
  int16_t input = cmdMessenger.readInt16Arg();
  if (input == 0) {
    TopMotor->run(RELEASE);
  } else if (input < 0) {
    TopMotor->run(BACKWARD);
    input *= -1;
  } else {
    TopMotor->run(FORWARD);
  }
  TopMotor -> setSpeed(constrain(input, 0, 255));
}

// Setup function
void setup() 
{
  Serial.begin(115200);

  // Adds newline to every command
  cmdMessenger.printLfCr();   
  attachCommandCallbacks();

  // set pin for blink LED
  pinMode(kBlinkLed, OUTPUT);

  // motor stuff
  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  // Set the speed to start, from 0 (off) to 255 (max speed)
  TopMotor->setSpeed(150);
  TopMotor->run(FORWARD);
  // turn on motor
  TopMotor->run(RELEASE);
}

// Loop function
void loop() 
{
  // Process incoming serial data, and perform callbacks
  cmdMessenger.feedinSerialData();
}


