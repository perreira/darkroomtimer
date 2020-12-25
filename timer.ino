// Fototimer Prototype 1
/*
Nils Pickert 2014
Uses Arduino Mega 2560 plus SainSmart TFT TouchPad
Features:
- Base exposure is hard coded 10 seconds
- Buttons to raise/lower base exposure by 1 f-stop (double/half the exposure time)
- Poti to finetune the exposure (half basetime down or up, does not correspond to 0.5 f-stop!)
- display count down when exposing
- Cancel exposure by pressing button for ~1 second
To Do:
- Test Strip mode with beeper (1/3 f-stop increments)
- Test Strip mode with Servo?
- Re-Scale Poti to logarithmic?
Advanced features:
- Multigrade split grade exposure: set ratio and split exposure into two, with time inbetween to change filters
*/
#define LED 12
#define BUTTON 8
#define POTI 0
// Libraries
#include <UTFT.h> // TFT Screen
#include <UTouch.h> // Touch functionality
// Declare which fonts we will be using
extern uint8_t BigFont[];
extern uint8_t SmallFont[];
// Constants
const int numReadings = 10;
const String zerostring="0";
// Global Variables
int mydelay = 1000;
int myadc;
float mybase = 10; // base exosure in seconds
String mystring, string_one, string_two;
int x, y;
int timeremaining;
long readings[numReadings];
int index = 0;
int total = 0;
int average = 0;
// Define TFT
UTFT myGLCD(SSD1289,38,39,40,41,LANDSCAPE);
// Initialize Touch Screen
UTouch  myTouch( 6, 5, 4, 3, 2);
void drawMainScreen()
{
  // Button 1
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRoundRect (10, 130, 150, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (10, 130, 150, 180);
  myGLCD.print("+1 Stop", 20, 147);
  // Button 2
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRoundRect (160, 130, 300, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (160, 130, 300, 180);
  myGLCD.print("-1 Stop", 170, 147);
}
// Draw a red frame while a button is touched
void waitForIt(int x1, int y1, int x2, int y2)
{
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable())
    myTouch.read();
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
}
void printRemaining()
{
        myGLCD.setFont(BigFont);
        string_two = " s";
        if(timeremaining < 10) {
          mystring = zerostring + zerostring + timeremaining + string_two;
        } else  if (timeremaining < 100) {
         mystring = zerostring + timeremaining + string_two;
        } else {
         mystring = timeremaining + string_two;
        }
        myGLCD.print(mystring, CENTER, 30);
        myGLCD.setFont(SmallFont);
}
void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);
  // enable debuggin on serial
  Serial.begin(9600);
  // Setup the LCD
  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);
  myGLCD.clrScr();
  myGLCD.setColor(255, 0, 0);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.print("* Phototimer v0.1 *", CENTER, 1);
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(255, 0, 0);
  drawMainScreen();
  myGLCD.setFont(SmallFont);
  // initialize ADC reading
  for(int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;
}
void loop()
{
  // Read the Poti settings, average over numReading readings
  total = total - readings[index];
  readings[index] = analogRead(POTI);
  total += readings[index];
  index++;
  if(index>=numReadings) index = 0;
  myadc = total / numReadings;
// Debugging: display current ADC value at bottom of screen
  string_one = "Current ADC Reading: ";
  mystring = string_one + myadc;
  myGLCD.print(mystring , CENTER, 227);
// end debugging
  // check if button on touch pad was pressed
  if (myTouch.dataAvailable())
  {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();
      // button 1: 10, 130, 150, 180
      // button 2:160, 130, 300, 180
      if((y>=130) && (y<=180))
      {
       if((x>=10) && (x<=150))
       { // Button 1
        waitForIt(10, 130, 150, 180);
        mybase *= 2;
       }
       if((x>=160) && (x<=300))
       { // Button 1
        waitForIt(160, 130, 300, 180);
        mybase /=2;
       }
      }
  }
  // calculate and display set exposure time
  timeremaining = mybase + int(mybase*(myadc-512)/1024);
  printRemaining();
  // check if button was pressed, start timer
  if( digitalRead(BUTTON) == HIGH)
  {
    digitalWrite(LED,HIGH);
    do {
        delay(1000);
        timeremaining -= 1;
        printRemaining();
        // add cancel: press button for about 1 second to cancel current run
        if( digitalRead(BUTTON) == HIGH) {
          timeremaining = 0;
          delay(1000);
        }
      } while(timeremaining > 0);
    digitalWrite(LED,LOW);
  }
}
