
#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include <Servo.h>
#include "RTClib.h"

int feedbackpin = A2;
int servopin = 8;
const int ledPin = 13;      // the number of the LED pin
const int ledPin2 = 12;      // the number of the LED pin

RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


Servo myservo;  
int feedback = analogRead(feedbackpin);
int ledState = HIGH;         // the current state of the output pin
int ledState2 = LOW;         // the current state of the output pin

int minDegrees;
int maxDegrees;
int minFeedback;
int maxFeedback;
int tolerance = 2;
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

SoftwareSerial mySerial(2, 3);  //2 green 3 white 

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

#define WHITE 0x7

void setup()  
{
  myservo.attach(servopin);  // attaches the servo on pin 9 to the servo object
  calibrate(myservo,feedbackpin,0,179);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin, ledState);
  digitalWrite(ledPin2, ledState2);
  Serial.begin(57600);
  lcd.begin(16, 2);
  lcd.setBacklight(WHITE);
  lcd.clear();
  lcd.setCursor(0,0);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    lcd.print("Found fingerprint sensor!");
  } else {
    lcd.print("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Waiting for valid finger...");
}

void calibrate(Servo servo, int analogPin, int minPos, int maxPos)
{
   // Move to the maximum position and record the feedback value
  servo.write(maxPos);
  maxDegrees = maxPos;
  delay(2000); // make sure it has time to get there and settle
  maxFeedback = analogRead(analogPin);
  
  // Move to the minimum position and record the feedback value
  servo.write(minPos);
  minDegrees = minPos;
  delay(2000); // make sure it has time to get there and settle
  minFeedback = analogRead(analogPin);
  
}

int getPos(int analogPin)
{
  return map(analogRead(analogPin), minFeedback, maxFeedback, minDegrees, maxDegrees);
}




uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  lcd.clear();
  lcd.setCursor(0,0);
  switch (p) {
    case FINGERPRINT_OK:
      lcd.print("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      lcd.print("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.print("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      lcd.print("Imaging error");
      return p;
    default:
      lcd.print("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  lcd.clear();
  lcd.setCursor(0,0);
  switch (p) {
    case FINGERPRINT_OK:
      lcd.print("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      lcd.print("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.print("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      lcd.print("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      lcd.print("Could not find fingerprint features");
      return p;
    default:
      lcd.print("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  lcd.clear();
  lcd.setCursor(0,0);
  if (p == FINGERPRINT_OK) {
    lcd.print("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.print("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    lcd.print("Did not find a match");
    return p;
  } else {
    lcd.print("Unknown error");
    return p;
  }   
  
  // found a match!
  lcd.clear();
  lcd.setCursor(0,0);
  int num=finger.fingerID;
  int con=finger.confidence;
  lcd.print("Found ID #");
  lcd.print(num);
  lcd.setCursor(0,1);
  lcd.print("confidence "); 
  lcd.print(con); 
  int pos=getPos(feedbackpin);
  ledState = !ledState;
  ledState2 = !ledState2;
  digitalWrite(ledPin, ledState);
  digitalWrite(ledPin2, ledState2);
  myservo.write(pos+43);
  delay(1000);
 // myservo.write(0);
 // delay(1000);

  return finger.fingerID;
}



void loop()                     // run over and over again
{
 uint8_t y=0;
 uint8_t x=getFingerprintID();
 DateTime now = rtc.now();
 Serial.print(now.year(), DEC);
 Serial.print('/');
 Serial.print(now.month(), DEC);
 Serial.print('/');
 Serial.print(now.day(), DEC);
 Serial.print(" (");
 Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
 Serial.print(") ");
 Serial.print(now.hour(), DEC);
 Serial.print(':');
 Serial.print(now.minute(), DEC);
 Serial.print(':');
 Serial.print(now.second(), DEC);
 Serial.println();
  
 delay(100);            //don't ned to run this at full speed.
}
