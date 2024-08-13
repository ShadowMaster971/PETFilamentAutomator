/*
  Stepper Moto Control
  Blink the blue LED on the ESP-01 module
  This example code is in the public domain

  This example is for a 128x32 pixel display using I2C to communicate
  3 pins are required to interface (two I2C and one reset).

  I2C Adress : 0x3c
*/
#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::FULL2WIRE,PIN_STEP,PIN_DIR);

int stepsPerRevolution = 200;  

bool stepperEnable = false;

void initStepper(){
  stepper.setPinsInverted(true,false,true); //set enable pin inverted
  stepper.setEnablePin(PIN_EN);
  stepper.disableOutputs();
  stepper.setMaxSpeed(40*stepsPerRevolution+1);
}

void stepperRunTask(){
  if (status == "stopped" && stepperEnable) {
    stepper.disableOutputs();
    stepperEnable = false;
  }
  if (status == "working" && !stepperEnable) {
    stepper.enableOutputs();
    stepperEnable = true;
  }
  if (status == "working") {
    stepper.setSpeed(Vo*stepsPerRevolution);
    stepper.runSpeed();
    
  }
}