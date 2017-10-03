#include "SD.h"
#include <Wire.h>
#include "RTClib.h"
#include <SoftwareSerial.h>

// A simple data logger for the Arduino analog pins
#define ECHO_TO_SERIAL   1 // echo data to serial port
#define WAIT_TO_START    0 // Wait for serial input in setup()

// the digital pins that connect to the LEDs
#define redLEDpin 3
#define greenLEDpin 4

SoftwareSerial mySerial(8,9); // RX, TX

RTC_DS1307 RTC; // define the Real Time Clock object

// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;
String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete
float soil1;
float soil2;

// the logging file
File logfile;
void serialEvent() {
   while (mySerial.available()&& stringComplete != true) {
     // get the new byte:
     char inChar = (char)mySerial.read();
     // add it to the inputString:
     inputString += inChar;
     // if the incoming character is a newline, set a flag so the main loop can
     // do something about it:
     if (inChar == '\n') {
       stringComplete = true;
     }
   }
}
void error(char *str)
{
  Serial.print("error: ");
  Serial.println(str);
  
  // red LED indicates error
  digitalWrite(redLEDpin, HIGH);
  
  while(1);
}

void setup() {

  pinMode(6, OUTPUT); //power pin for soil sensor 1
  pinMode(7, OUTPUT); //power pin for soil sensor 2
  
  RTC.begin();
  RTC.adjust(DateTime(__DATE__, __TIME__));

  mySerial.begin(9600);

  // initialize serial:
  Serial.begin(9600);
  Serial.println();
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

#if WAIT_TO_START
  Serial.println("Type any character to start");
  Serial.println();
  while (!Serial.available());
#endif //WAIT_TO_START

  // initialize the SD card
  Serial.print("Initializing SD card...");
  Serial.println();
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  Serial.println();
  
  // create a new file
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE); 
      break;  // leave the loop!
    }
  }
  
  if (! logfile) {
    error("couldnt create file");
  }
  
  Serial.print("Logging to: ");
  Serial.println(filename);
  Serial.println();
  
    Wire.begin();  
  if (!RTC.begin()) {
    logfile.println("RTC failed");
#if ECHO_TO_SERIAL
    Serial.println("RTC failed");
#endif  //ECHO_TO_SERIAL
  }
  
  logfile.println("DateTime,Soil1,Soil2,Weight");    
#if ECHO_TO_SERIAL
  Serial.println("DateTime,Soil1,Soil2,Weight");
#endif

 pinMode(redLEDpin, OUTPUT);
 pinMode(greenLEDpin, OUTPUT);
}

void loop() {
{
  DateTime now;
  
  digitalWrite(greenLEDpin, HIGH);

  // fetch the time
  now = RTC.now();
  // log time
  logfile.print(now.month(), DEC);
  logfile.print("/");
  logfile.print(now.day(), DEC);
  logfile.print("/");
  logfile.print(now.year(), DEC);
  logfile.print(" ");
  int hr = now.hour();
  int truehr;
  if (hr > 12) {
    truehr = (now.hour()-12);
    logfile.print(truehr);
  }
  else
    logfile.print(now.hour(), DEC);
  logfile.print(":");
  int mins = now.minute();
  if (mins < 10) {
    logfile.print('0');
  }
  logfile.print(now.minute(), DEC);
  logfile.print(":");
  int secs = now.second();
  if (secs < 10) {
    logfile.print('0');
  }
  logfile.print(now.second(), DEC);
  if (hr >= 12) {
    logfile.print(" PM, ");
  }
  else
    logfile.print(" AM, ");
#if ECHO_TO_SERIAL
 // Serial.print(now.secondstime()); // seconds since 2000
  Serial.print(now.month(), DEC);
  Serial.print("/");
  Serial.print(now.day(), DEC);
  Serial.print("/");
  Serial.print(now.year(), DEC);
  Serial.print(" ");
  if (hr > 12) {
    Serial.print(truehr);
  }
  else
    Serial.print(now.hour(), DEC);
  Serial.print(":");
  if (mins < 10) {
    Serial.print('0');
  }
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  if (secs < 10) {
    Serial.print('0');
  }
  Serial.print(now.second(), DEC);
  if (hr >= 12) {
    Serial.print(" PM, ");
  }
  else
    Serial.print(" AM, ");
#endif //ECHO_TO_SERIAL

  //read soil sensor 1
  digitalWrite(6, HIGH);
  delay(400);
  soil1 = analogRead(A0);
  digitalWrite(6, LOW);

  //read soil sensor 2
  digitalWrite(7, HIGH);
  delay(400);
  soil2 = analogRead(A1);
  digitalWrite(7, LOW);

  mySerial.print("*P");
  delay(200);
  
  serialEvent();

  logfile.print(soil1);
  logfile.print(", ");
  logfile.print(soil2);
  logfile.print(", ");
  logfile.print(inputString);
  logfile.println(' ');
  
  #if ECHO_TO_SERIAL
  Serial.print(soil1);
  Serial.print(", "); 
  Serial.print(soil2);
  Serial.print(", "); 
  Serial.print(inputString);
  Serial.println(' ');
  #endif //ECHO_TO_SERIAL

  if (stringComplete == true) {
  // clear the string:
  inputString = "";
  stringComplete = false;
  }
  
  
  digitalWrite(greenLEDpin, LOW);
  
  // delay for the amount of time we want between readings
    for (int k=1; k <= 1; k++) {
    delay(59800); //5-minute delay
    }
  
  logfile.flush();
  }
}




