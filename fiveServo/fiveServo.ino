// 5 Servos Controlled by a total amature
// Created: 2011-11-25
// JP Reardon http://jpreardon.com/
// Modified 2011-12-11: Testing out slow servo functions
// Modified 2011-12-13: Refactoring a bit, adding center offsets and min/max angles

#include <Servo.h>


Servo servo[5];           // Servo array
const int buttonPin = 8;   // The number of the pushbutton pin
const int ledPin = 13;
boolean run = false;       // Declare and set the run variable
int servoCenterOffset[5];  // Declare the servoCenterOffset array
int servoMinAngle;         // Declare the minimum angle variable
int servoMaxAngle;         // Declare the maximum angle variable
int servoSpeed;            // Declare the speed variable

void setup()
{
  pinMode(buttonPin, INPUT);  // Initialize the button pin as an input
  pinMode(ledPin, OUTPUT);    // Initialize the LED pin as output
  
  // Attach servos to their respective outputs
  servo[0].attach(2);
  servo[1].attach(3);
  servo[2].attach(4);
  servo[3].attach(5);
  servo[4].attach(6);
  
  // Set servo center offsets
  // This is done to adjust for mechanical differences in the servos or in
  // the attached mechanisim. The center offset is applied to all movements.
  servoCenterOffset[0] = 0;
  servoCenterOffset[1] = -6; 
  servoCenterOffset[2] = -5; 
  servoCenterOffset[3] = -6; 
  servoCenterOffset[4] = 3;
  
  servoMinAngle = 15;     // Set the servo minimum angle
  servoMaxAngle = 165;    // Set the servo maximum angle
  servoSpeed = 15;        // Set the speed
  
  startUpExercise();      // Run the startup exercise, this should be cooler than it is
}


void loop()
{
  // Check the button state, this is a hack, need an interrupt here 
  if(digitalRead(buttonPin) == 0){
   // Toogle the run state
   if(run == false){
     run = true;
     digitalWrite(ledPin, HIGH);
     delay(1000);
   }
   else
   {
     run = false;
     digitalWrite(ledPin, LOW);
     delay(1000);                 // Without this delay, the loop starts again and reads the button as being pressed, hence, the thing never stopped :P
   }
  }
  
  // Start conditional run block if run == true
  if(run == true){
    
    

  } // End conditional run block
}

// This moves all of the servos their entire range a couple times
void startUpExercise(){
  moveAllByPercent(50, 50, 50, 50, 50);
  moveAllByPercent(0, 0, 0, 0, 0);
  moveAllByPercent(100, 0, 0, 0, 0);
  moveAllByPercent(0, 100, 0, 0, 0);
  moveAllByPercent(0, 0, 100, 0, 0);
  moveAllByPercent(0, 0, 0, 100, 0);
  moveAllByPercent(0, 0, 0, 0, 100);
  moveAllByPercent(0, 0, 0, 0, 0);
  moveAllByPercent(50, 50, 50, 50, 50);
  moveAllByPercent(0, 100, 0, 100, 0);
  moveAllByPercent(100, 0, 100, 0, 100);
  moveAllByPercent(50, 50, 50, 50, 50);
}

// Moves all servos to the requested positions, positions should be given as an integer between 0 and 100
void moveAllByPercent(int value1, int value2, int value3, int value4, int value5){
  
  // Map the percentage values to angles while respecting the minimum and maximum angles
  int angle1 = map(value1, 0, 100, servoMinAngle, servoMaxAngle);
  int angle2 = map(value2, 0, 100, servoMinAngle, servoMaxAngle);
  int angle3 = map(value3, 0, 100, servoMinAngle, servoMaxAngle);
  int angle4 = map(value4, 0, 100, servoMinAngle, servoMaxAngle);
  int angle5 = map(value5, 0, 100, servoMinAngle, servoMaxAngle);
    
  // Call the slowMoveFunction
  slowMoveAll(servoSpeed, angle1, angle2, angle3, angle4, angle5);
}

// Moves all servos to the desired position at the desired speed, positions should be expressed as degrees between 0 and 180
// The center offsets are applied in this function. No direct servo movement should happen outside of this function without
// refactoring something somewhere.
void slowMoveAll(int delayTime, int pos1, int pos2, int pos3, int pos4, int pos5){
  
  int finalPosition[5] = {pos1, pos2, pos3, pos4, pos5};  // Put desired final positions into an array
  
  // Apply the servo center offsets to the final positions
  for(int i = 0; i < 5; i ++){
    finalPosition[i] = finalPosition[i] + servoCenterOffset[i];
  }
  
  int servoPos[] = {0, 0, 0, 0, 0};
  
  //get the servo positions
  for(int i = 0; i < 5; i++){
    servoPos[i] = servo[i].read();
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
          servo[i].write(servoPos[i]);
        }
        else
        {
          servoPos[i] = servoPos[i] + 1;
          servo[i].write(servoPos[i]);
        } 
      }
    }
    delay(delayTime);
  }
}
