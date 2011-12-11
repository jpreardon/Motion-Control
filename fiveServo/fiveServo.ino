// 5 Servos Controlled by a total amature
// 2011-11-25
// JP Reardon http://jpreardon.com/

// #include <Servo.h>
#include <VarSpeedServo.h>
// Servo array
VarSpeedServo servos[5];
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
    zeroAll();
    delay(1000);
    // oneAtATime(); 
    moveAll(89);
    delay(1000);
    moveAll(179);
    delay(1000);
    moveAll(89);
    delay(1000);
  }
}

// Moves all servos to zero degrees
void zeroAll(){
  for(int i = 0; i < 5; i++){
      servos[i].write(0);
    }
}

void onAtATime(){
    for(int i = 0; i < 5; i++){
      servos[i].write(0);
      delay(500);
    }
    for(int i = 0; i < 5; i++){
      servos[i].write(89);
      delay(500);
    }
    for(int i = 0; i < 5; i++){
      servos[i].write(179);
      delay(500);
    }
    for(int i = 0; i < 5; i++){
      servos[i].write(89);
      delay(500);
    }
}

void moveAll(int pos){
  for(int i = 0; i < 5; i++){
    servos[i].write(pos);
  }
}

void moveOne(int servoNo, int pos){
  servos[servoNo].write(pos);
}


