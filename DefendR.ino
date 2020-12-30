//import SPI and Pixy2 Library
#include <SPI.h>
#include <Pixy2.h>
Pixy2 pixy;

//create and initialize pin array
int myPins[6] = {5, 6, 7, 8, 9, 10};
float deadZone = 0.15;

//initialize errorHistory variable, which stores last error
float errorHistory;
int cont = 0;
int signature, x, y, width, height;
float cx, cy, area;
//Kp porpotional constant
int k = 200;
//base power
int baseSpeed = 150;
float leftPower = baseSpeed;
float rightPower = baseSpeed;

void setup() {
  Serial.begin(115200);
  Serial.print("Starting...\n");
  pixy.init();
  //set pin mode as voltage output
  for (int i = 0; i < 6; i++) {
    pinMode(myPins[i], OUTPUT);
  }
}

void loop() {

  //turn on Pixy2 front light 
  pixy.setLamp(1, 1);

  float error = findError();
  leftPower = (baseSpeed + k * error);
  rightPower = (baseSpeed - k * error);
  moveRobot(leftPower, rightPower);

}

//findError() method determines ball location from center  
float findError() {
  
  char buf[32];
  // If blocks are detected, find cx
  if (pixy.ccc.getBlocks())
  {
    cx = pixy.ccc.blocks[0].m_x;
    //scale ball pixel value from (0,320) to (-1,1) range
    cx = mapfloat(cx, 0, 320, -1, 1);
  }
  //if block are not detected...check error history for reference
  else {
    if (errorHistory < 0) {
      cx = -1;
    }
    else {
      cx = 1;
    }
  }
  errorHistory = cx;
  return cx;
}

//check for distance to ball 
float heightCheck() {
  if (pixy.ccc.blocks[0].m_height > 145) {
    return 0;
  }
  return 1;
}

//mapping function 
float mapfloat(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

//create moveRobot method to control Arduino ports 
void moveRobot(int leftSpeed, int rightSpeed)
{
  if (leftSpeed >= 0) {
    digitalWrite(myPins[1], 1);
    digitalWrite(myPins[2], 0);
  }
  else {
    digitalWrite(myPins[1], 0);
    digitalWrite(myPins[2], 1);
  }

  if (rightSpeed >= 0) {
    digitalWrite(myPins[3], 0);
    digitalWrite(myPins[4], 1);
  }
  else {
    digitalWrite(myPins[3], 1);
    digitalWrite(myPins[4], 0);
  }

  analogWrite(myPins[0], abs(leftSpeed));
  analogWrite(myPins[5], abs(rightSpeed));
}
