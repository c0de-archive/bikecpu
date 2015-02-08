/* Bike Computer V1.1

  Using the serial functions signifigantly increases compiled size, use this if you have a problem in execution or are testing.
  Use the version without diagnostics in production (once you work out the bugs of course)

  Provides controls to basic functions on a bike, such as turning on and off lights,
  Measuring distance traveled, average speed, and logs to a microSD card.

  Future versions will also be able to communicate to other
  microcontrollers through the serial interface.

  Version 1 (This version) will be responsible for detecting ambiant light and ramping
  up/down the front and rear LED brightness once light levels reach a certain level.
  Other responsibilities will be blinking pairs of LEDs (front and back) on either side
  of the bike to indicate turning.

*/

#include <SoftwareSerial.h>

SoftwareSerial lcd(2, 8);  // This is required, to start an instance of an LCD
const int dispSw = A3;

// D4 and D5 are reserved for I2C

// Headlight
const int ambientIn = A0; // Analog input pin that the potentiometer is attached to
const int headlight = 3; // Digital output pin that the LED is attached to
const int incrBy = 5; // Amount to increase or decrease LED brightness by
int sensorValue = 0; // value read from the pot
int outputValue = 0; // value output to the PWM (analog out)

// Blinkers
const int lBnkSw = A1; // Left Blinker switch
const int rBnkSw = A2; // Right Blinker switch
const int lBnk = 6; // Left Blinker
const int rBnk = 7; // Right Blinker
const int bnkSpeed = 500; // in ms, 500 = twice a second
int lBnkSt = LOW;
int rBnkSt = LOW;
int lBnkSwSt = 0;
int rBnkSwSt = 0;
unsigned long previousMillis = 0;

void setup() {
  lcd.begin(9600);  // Start the LCD at 9600 baud
  clearDisplay();  // Clear the display
  setBacklight(20);

  // Set out outputs
  pinMode(headlight, OUTPUT);
  pinMode(lBnk, OUTPUT);
  pinMode(rBnk, OUTPUT);

  // Set our inputs
  pinMode(ambientIn, INPUT);
  pinMode(lBnkSw, INPUT);
  pinMode(rBnkSw, INPUT);
  pinMode(dispSw, INPUT);

  setLCDCursor(0);
  lcd.print("c0defox.es BCPU");
  setLCDCursor(18);
  lcd.print("Bike Computer");
}

void headLight(){ // Main function for determining whether or not to ramp up or ramp down the headlight and taillight. Uses ambient light sensor for that
  sensorValue = analogRead(ambientIn);

  if(sensorValue <= 45 && outputValue <= 250){
    outputValue = outputValue + incrBy;
  }

  if(sensorValue >= 50 && outputValue > 0){
    outputValue = outputValue - incrBy;
  }
  analogWrite(headlight, outputValue);
  if(outputValue >= 20){ // Ramp up/down lcd brightness with headlight - anything lower than 20 is too hard to read
    setBacklight(outputValue);

    setLCDCursor(16);
    lcd.print("*"); // Headlight indicator on lcd
    lcd.print(outputValue); // Brightness level of headlight
  }
}

void blinker(){ // Controls the blinkers
  unsigned long currentMillis = millis();

  // Get info from switches
  lBnkSwSt = digitalRead(lBnkSw);
  rBnkSwSt = digitalRead(rBnkSw);

  // Automatically turn off LEDs if switch is off
  if(lBnkSwSt == LOW){ digitalWrite(lBnk, LOW); }
  if(rBnkSwSt == LOW){ digitalWrite(rBnk, LOW); }

  if(lBnkSwSt == HIGH){ // Test if left blinker switch is on
     if(currentMillis - previousMillis >= bnkSpeed){
      previousMillis = currentMillis;

      if(lBnkSt == LOW){
        lBnkSt = HIGH;

        setLCDCursor(0);
        lcd.print("<");
      }else{
        lBnkSt = LOW;

        setLCDCursor(0);
        lcd.print(" ");
      }

      digitalWrite(lBnk, lBnkSt); // Begin blinking
    }
  }

  if(rBnkSwSt == HIGH){ // Test if right blinker is on
    echo("rBlinkSw", "ON");
    if(currentMillis - previousMillis >= bnkSpeed){
      previousMillis = currentMillis;

      if(rBnkSt == LOW){
        rBnkSt = HIGH;

        setLCDCursor(15);
        lcd.print(">");
      }else{
        rBnkSt = LOW;

        setLCDCursor(15);
        lcd.print(" ");
      }

      digitalWrite(rBnk, rBnkSt); // Begin blinking
    }
  }
}

void setBacklight(byte brightness){
  lcd.write(0x80);  // send the backlight command
  lcd.write(brightness);  // send the brightness value
}

void clearDisplay(){
  lcd.write(0xFE);  // send the special command
  lcd.write(0x01);  // send the clear screen command
}

void setLCDCursor(byte cursor_position){
  lcd.write(0xFE);  // send the special command
  lcd.write(0x80);  // send the set cursor command
  lcd.write(cursor_position);  // send the cursor position
}

void loop(){

  delay(500);
  clearDisplay();

  setLCDCursor(4);
  lcd.print("MPH: 7.5");

  headLight();
  blinker();
}
