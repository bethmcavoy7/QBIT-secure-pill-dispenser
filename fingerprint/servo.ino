
#include <Servo.h>

Servo myservo;

int buttonPin = 8;    // the number of the pushbutton pin
int servopin =2;

// Variables will change:
int servostate=LOW;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 20;    // the debounce time; increase if the output flickers

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(servopin, OUTPUT);
  myservo.attach(servopin);
  Serial.begin(9600);
  // set initial LED state
}

void loop() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);
  Serial.println(reading);
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) 
  {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    // if the button state has changed:
    if (reading != buttonState) 
    {
      buttonState = reading;
      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) 
        servostate = !servostate;
    }
  }

  // set the LED:

  if (servostate)
  {
    myservo.write(300);
  }
  else
  {
    myservo.write(0);
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}
