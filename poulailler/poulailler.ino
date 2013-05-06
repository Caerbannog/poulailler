
/*
 * Arduino Poulailler
 * (4) Fantastiques
 * 
 * Open the chicken coop door at sunrise, closes it at sunset.
 *
 * Pinout :
 *    EDVACarduino shield has an L298P, RF serial (APC220), and motor control. Servomotor MKS DS95 is used.
 *    Desolder "VSENSE" to go to arduino's SENSE pin via a 2 Ohm resistor (high power rating).
 *    
 * Libraries needed :
 *    Sunrise (the version before timelord?)
 *    RTClib (ladyada's?)
 */

#include <Wire.h>
#include <RTClib.h>
#include <Sunrise.h>
#include <Servo.h>


// Pins
#define SERVO        3 // Unused
#define SWITCH       7 // User input
#define DIRECTION    8 // or 11 (shield)
#define ENABLE       9 // or 10 (shield)
#define LED          13
#define SENSE        A3 // Beware, A4 and A5 are for I2C, even on Uno boards

#define LATTITUDE    43.6053    // Toulouse
#define LONGITUDE    1.4552052
#define TIMEZONE     +2


RTC_DS1307 RTC;
Sunrise sunrise(LATTITUDE, LONGITUDE, TIMEZONE);
Sunrise sunset(LATTITUDE, LONGITUDE, TIMEZONE);
Servo servo;


void setup() {
   Wire.begin();
   Serial.begin(9600);
   RTC.begin();
   servo.attach(SERVO, 850, 2150);
   
   // Update the RTC with your computer clock:
// RTC.adjust(DateTime(__DATE__, __TIME__));

   sunrise.Civil();
   sunset.Civil();
   
   digitalWrite(SWITCH, HIGH); // Pull-up on input.
   pinMode(DIRECTION, OUTPUT);
   pinMode(ENABLE, OUTPUT);
   pinMode(LED, OUTPUT);
   
   Serial.println("Reboot.");
   delay(1000);
}


int measureCurrent() {
   int voltage = 0;
   
   for(int i = 0; i < 5; i++) {
      delay(10);
      voltage += analogRead(SENSE);
   }
   
   return voltage / 5;
}


void moveDoor(int state) {
   unsigned long duration = 0;
   unsigned long start = millis();
   
   digitalWrite(DIRECTION, state);
   digitalWrite(ENABLE, HIGH);

   delay(1000); // First inductive spike.
   float current;
   do {
      current = measureCurrent();
      
      if(current > 400)
         break;
      
      //if(digitalRead(CONTACT_TOP) == 0)
      //   break;
      
      delay(100);
      duration = millis() - start;
   } while(duration < 8000);

   Serial.print("The current was ");
   Serial.print(current, DEC);
   
   digitalWrite(ENABLE, LOW);

   Serial.print("The door was moved to ");
   Serial.print(state, DEC);
   Serial.print(" in ");
   Serial.print(duration / 1000., DEC);
   Serial.println("s");
}


int WorkingHour() {
   int working = 1;
   
   DateTime now = RTC.now();
   sunrise.Rise(now.month(), now.day());
   sunset.Set(now.month(), now.day());
   
   if(sunrise.Hour() > now.hour() || (sunrise.Hour() == now.hour() && sunrise.Minute() > now.minute())) {
      Serial.println("(before sunrise)");
      working = 0;
   }
   
   if(sunset.Hour() < now.hour() || (sunset.Hour() == now.hour() && sunset.Minute() < now.minute())) {
      Serial.println("(after sunset)");
      working = 0;
   }
   digitalWrite(LED, working);
   return working;
}


void loop() {
   /*
   moveDoor(0);
   delay(5000);
   moveDoor(1);
   delay(5000);
   //*/
   
   //*
   int state = -1; // Initial state unknown.
   
   while(true) {
      printTime();
      
      int should_invert = digitalRead(SWITCH);
      int new_state = WorkingHour() ^ should_invert;
      
      if(new_state != state) {
         Serial.println("Moving door...");
         moveDoor(new_state);
         state = new_state;
      }
      
      delay(5000);
   }
   //*/
   
   /*
   DateTime now = RTC.now();
   sunrise.Rise(now.month(), now.day());
   Serial.print("Lever du soleil a ");
   Serial.print(sunrise.Hour(), DEC);
   Serial.print('h');
   Serial.print(sunrise.Minute(), DEC);
   Serial.println();
   
   sunset.Set(now.month(), now.day());
   Serial.print("Coucher du soleil a ");
   Serial.print(sunset.Hour(), DEC);
   Serial.print('h');
   Serial.print(sunset.Minute(), DEC);
   Serial.println();
   //*/
}


void printTime()
{
   DateTime now = RTC.now();
   Serial.print(now.day(), DEC);
   Serial.print('/');
   Serial.print(now.month(), DEC);
   Serial.print('/');
   Serial.print(now.year(), DEC);
   Serial.print(' ');
   Serial.print(now.hour(), DEC);
   Serial.print(':');
   Serial.print(now.minute(), DEC);
   Serial.print(':');
   Serial.print(now.second(), DEC);
   Serial.println();
}

