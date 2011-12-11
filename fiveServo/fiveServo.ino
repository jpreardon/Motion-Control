// 5 Servos Controlled by a total amature
// Created: 2011-11-25
// JP Reardon http://jpreardon.com/
// Modified 2011-12-11: Testing out slow servo functions


#include <Servo.h>
//include <VarSpeedServo.h>
//Servo array
//VarSpeedServo servos[5];
Servo servos[5];
// The number of the pushbutton pin
const int buttonPin = 8;     
// Declare and set the run variable
boolean run = false;

void setup()
{
  servos[0].attach(2);
  servos[1].attach(3);
  servos[2].attach(4);
  servos[3].attach(5);
  servos[4].attach(6);
  
  // initialize the button pin as a input:
  pinMode(buttonPin, INPUT);
  // initialize the onboard LED as output
  pinMode(13, OUTPUT);
}


void loop()
{
  // Check the button state, this is a hack, need an interrupt here 
  if(digitalRead(buttonPin) == 0){
   // Toogle the run state
   if(run == false){
     run = true;
     digitalWrite(13, HIGH);
     delay(1000);
   }
   else
   {
     run = false;
     digitalWrite(13, LOW);
     // Without this delay, the loop started again and read the button as being pressed, hence, the thing never stopped :P
     delay(1000);
   }
  }
  
  // Check the run state, do stuff it it is true!!!
  if(run == true){
    int speed = 15;
    slowMoveAll(speed, 80, 80, 80, 80, 80);
    delay(2000);
    // run through 5 random poses
    for(int i = 0; i < 5; i++){
      slowMoveAll(speed, random(0,179), random(0,179), random(0,179), random(0,179), random(0,179));
      delay(2000);
    }
  }
}


// Moves all servos to the desired position at the desired speed, i really wish i could pass an array here, but i can't, a pointer might me good here
void slowMoveAll(int delayTime, int pos1, int pos2, int pos3, int pos4, int pos5){
  int finalPosition[5] = {pos1, pos2, pos3, pos4, pos5};
  
  int servoPos[] = {0, 0, 0, 0, 0};
  
  //get the servo positions
  for(int i = 0; i < 5; i++){
    servoPos[i] = servos[i].read();
  }
  
  //If we have any servos that need to move, move, then wait a bit
  for(int j = 0; j < 179; j++){
  
    //Move the servos towards desired position, one at a time
    //This is a hack, it should be a while loop and we should be checking if we actually need to move anything
    //but I just want to get something working here, cleanup later (famous last words)
    for(int i = 0; i < 5; i++){
      if (servoPos[i] != finalPosition[i]) {
        if (servoPos[i] > finalPosition[i]) {
          servoPos[i] = servoPos[i] - 1;
          servos[i].write(servoPos[i]);
        }
        else
        {
          servoPos[i] = servoPos[i] + 1;
          servos[i].write(servoPos[i]);
        } 
      }
    }
    delay(delayTime);
  }
}


