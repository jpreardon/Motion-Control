// 5 Servos Controlled by a total amature
// Created: 2011-11-25
// JP Reardon http://jpreardon.com/
// Modified 2012-02-12: It's sloppy, but it is working over the hardware serial port. The web server seems to be 
//                      getting some invalid responses, will need to check into why, or at least account for it
//                      somehow.
// Modified 2012-02-12: It seems like the "host" bit doesn't need to be sent to the webserver, not the test one
//                      we have set up anyway. Taking it out of the code.
// Modified 2012-02-12: Put in some basic error checking on the serial comms. Next step is to rip start sending
//                      the WiFly commands to the hardware serial port.
// Modified 2012-02-05: Rewired a bunch of stuff only to find that the SoftwareSerial library is incompatable 
//                      with the Servo library (serial comms cause the servos to jump). This is kind of sucky
//                      since I'll have to use the onboard serial, so no debugging. On the other hand, it will
//                      probably be more stable to use onboard serial anyway. Will get rid of the SoftwareSerial
//                      code in the next rev.
// Modified 2012-01-15: Adding serial comms for RN-134 WiFly board
// Modified 2012-01-02: Getting rid of the pushbutton and LED stuff, for now anyway
// Modified 2011-12-13: Refactoring a bit, adding center offsets, min/max angles, and serial control
// Modified 2011-12-11: Testing out slow servo functions

#include <Servo.h>
#include <SoftwareSerial.h>

// WARNING: Setting debug to "true" will send debug information to the software serial port, this will cause the 
//          servos to jump (probably). You may want to disconnect the servos if you want to debug...
const boolean debug = false;
Servo servo[5];           // Servo array
int servoCenterOffset[5];  // Declare the servoCenterOffset array
int servoMinAngle;         // Declare the minimum angle variable
int servoMaxAngle;         // Declare the maximum angle variable
int servoSpeed;            // Declare the speed variable
const String versionNumber = "0.1";
String commandString;
int servoPosition[5] = {50, 50, 50, 50, 50};
SoftwareSerial softSerial = SoftwareSerial(10, 11);

void setup()
{
 

  if (debug) {
    // Set up the serial connection for debug messages
    softSerial.begin(9600);
    softSerial.print("fiveServo ");
    softSerial.println(versionNumber);
  }
  
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
  servoCenterOffset[1] = -3; 
  servoCenterOffset[2] = 0; 
  servoCenterOffset[3] = -2; 
  servoCenterOffset[4] = 2;
  
  servoMinAngle = 15;     // Set the servo minimum angle
  servoMaxAngle = 165;    // Set the servo maximum angle
  servoSpeed = 9;         // Set the speed
  
  if (debug) softSerial.println("Begining Startup Exercise");
  startUpExercise();                                          // Run the startup exercise, this should be cooler than it is
  if (debug) softSerial.println("Exercise complete, ready for input...");
}


void loop()
{
  
  // Get the stock data  
  String rawStockData = getStockData();
  // Check to make sure we got reasonable data
  if (rawStockData.indexOf("*") == -1) {
    // Split it up so the servo function can read it
    String stockValues[5];
    int delimiterPosition = rawStockData.indexOf(",");
    int lastDelimiterPosition = 0;
    for (int i = 0; i < 5; i++) {
      stockValues[i] = rawStockData.substring(lastDelimiterPosition, delimiterPosition);
      lastDelimiterPosition = delimiterPosition + 1;
      delimiterPosition = rawStockData.indexOf(",", lastDelimiterPosition);
    }
    // Send it to the servos
    moveAllByPercent(stringToInt(stockValues[0]), stringToInt(stockValues[1]), stringToInt(stockValues[2]), stringToInt(stockValues[3]), stringToInt(stockValues[4]));
  }
  delay(5000);
  
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
  moveAllByPercent(50, 50, 50, 50, 50);
  moveAllByPercent(0, 100, 0, 100, 0);
  moveAllByPercent(100, 0, 100, 0, 100);
  moveAllByPercent(50, 50, 50, 50, 50);
}

// Moves all servos to the requested positions, positions should be given as an integer between 0 and 100
// TODO: This needs to check the input values to make sure they are valid. The map function doesn't seem to do that, invalid values can move servos past the min/max
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
// TODO: This needs to check the input values to make sure they are valid, or at least ensure that we won't try to move to invalid locations
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

// This converts String objects to integers, it does no error checking at all
// TODO: this should be cleaned up and possibly moved to a library
int stringToInt(String ourString){
  char this_char[ourString.length() + 1];
  ourString.toCharArray(this_char, sizeof(this_char));
  int iOurString = atoi(this_char); 
  return iOurString;
}

String getBuffer(int timeOutSecs=2) {
  String buffer;
  long timeOut = millis() + (timeOutSecs * 1000);
  while (timeOut > millis() || millis() < 5000) {
    if (Serial.available()) {
      while (Serial.available()) {
        buffer += char(Serial.read());
      }
    }
  }
  return buffer;
}

void resetWiFly() {
    // Send an exit/close/exit commands for good measure
    // This is hacky, but I've been in situations where it gets stuck in command mode and 
    // exit or close by itself didn't work :(
    Serial.println("close");
    delay(2000);
    Serial.println("exit");
    delay(2000);
    Serial.flush();
    delay(2000);
}

String getStockValues(String inputData) {
  // The data we want is between brackets, find that stuff!
  // TODO: This puppy needs some error checking
  int openBracket = inputData.indexOf("[");
  int closeBracket = inputData.indexOf("]");
  String outputData = inputData.substring(openBracket + 1, closeBracket);
  return outputData;
}

// Get the stock data from the web service, note that this expects data from OUR webservice and doesn't try
// to get data from Yahoo, Google etc. Our webservice is very simplified and sends back exactly what we need
// in order to move the sculpture. Most of the logic is happening on the web service side. 
// TODO: The host, port and URL data is hardcoded here. At minimum, the host should come from what is set on the WiFly
String getStockData() {
  /*
  We'll be talking to the RN-134 board via serial, here's what needs to happen:
  
  - Open the serial connection
  - Put the WiFly board in command mode (Send "$$$", then wait a few miliseconds)
  - Inspect the return value on the serial interface, if it is what we expect ("CMD"), turn on a "connected" LED, if it isn't on already
  - Actually connect to a server and get some data
     - send the open command ("open <server> <port>\r\n")
     - check to make sure it opened ("*OPEN*" is in the serial buffer)
     - send the GET command (GET <url> HTTP/1.0\r\nhost: <server>\r\n\r\n
  - Parse what is returned
     - If it is a 200 AND it has the data we want, light a "success" LED and move the sculpture to the desired positions, also, if "*CLOS* is in the return, we may not need to EXIT later
     - Else, light the "error" LED
  - Maybe issue "exit" command to serial interface, then close the interface, don't know if we really need to do that or not
  */
  
  // Open the serial connection
  if (debug) {
    Serial.println("---------------------------------------------------------------");
    Serial.println("Opening software serial connection");
  }
  Serial.begin(9600);
  String buffer;
  String stockValues;
  
  
  // Put the WiFly board in command mode (Send "$$$", then wait a few seconds)
  if (debug) softSerial.println("Entering CMD mode");
  Serial.print("$$$");
  delay(2000);
  // TODO: We should get "CMD" Back here, if we don't, stop this nonsense
  buffer = getBuffer();
  if (buffer.indexOf("CMD") == -1) {
    if (debug) {
      softSerial.println("ERROR: Not in command mode, exiting loop");
      softSerial.print("Buffer: ");
      softSerial.print(buffer);
    }
    resetWiFly();
    return "*";
  } 
  else
  {
    if (debug) softSerial.println("OK: In command mode");
  }
  
  if (debug) softSerial.println("Sending open command");
  Serial.print("open\r\n");
  // TODO: Some random crap ends up in the buffer here, but at the end, we should see "*OPEN*", if not, we gots problems
  buffer = getBuffer();
  if (buffer.indexOf("*OPEN*") == -1) {
    if (debug) {
      softSerial.println("ERROR: Not open, exiting loop");
      softSerial.print("Buffer: ");
      softSerial.print(buffer);
    }
    resetWiFly();
    return "*";
  } 
  else
  {
    if (debug) softSerial.println("OK: Open!");
  }
  
  
  // send the GET command (GET <url> HTTP/1.0\r\nhost: <server>\r\n\r\n
  // TODO: The host parameter should not be hardcoded here
  if (debug) softSerial.println("Sending GET command...");
  Serial.print("GET /data HTTP/1.0\r\n\r\n");
  // TODO: Here's were we should get the data. We should look for "*CLOS*", if we see that, we should have something to test.
  //       Inside the buffer, we should see out data (some numbers inside of brackets "[]", if we see those, parse them out,
  //       if not, bail with an error.
  buffer = getBuffer();
  if (buffer.indexOf("*CLOS*") == -1) {
    if (debug) {
      softSerial.println("ERROR: Malformed response from WiFly, exiting loop");
      softSerial.print("Buffer: ");
      softSerial.print(buffer);
    }
    resetWiFly();
    return "*";
  } 
  else
  {
    stockValues = getStockValues(buffer);
    if (debug) {
      softSerial.print("OK: Values = ");
      softSerial.println(stockValues);
    }
  }
  
  
  if (debug) softSerial.println("Closing CMD mode");
  Serial.println("exit");
  // Nothing seems to come back from WiFly after the exit command...
  Serial.end();
  if (debug) softSerial.println("---------------------------------------------------------------");
  
  return stockValues;
  
}
