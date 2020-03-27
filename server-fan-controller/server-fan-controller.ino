#include <DS18B20.h>

#define FAN_PIN 9
#define TEMP_PIN 5

#define LED_RED 1
#define LED_GREEN 0
#define LED_BLUE 2

DS18B20 ds(TEMP_PIN);

// PWM output @ 25 kHz, only on pins 9 and 10.
// Output value should be between 0 and 320, inclusive.
void analogWrite25k(int pin, int value)
{
  switch (pin) {
      case 9:
          OCR1A = value;
          break;
      case 10:
          OCR1B = value;
          break;
      default:
          // no other pin will work
          break;
  }
}

void setup()
{
  Serial.begin(9600);
  
  // Configure Timer 1 for PWM @ 25 kHz.
  TCCR1A = 0;           // undo the configuration done by...
  TCCR1B = 0;           // ...the Arduino core library
  TCNT1  = 0;           // reset timer
  TCCR1A = _BV(COM1A1)  // non-inverted PWM on ch. A
             | _BV(COM1B1)  // same on ch; B
             | _BV(WGM11);  // mode 10: ph. correct PWM, TOP = ICR1
  TCCR1B = _BV(WGM13)   // ditto
             | _BV(CS10);   // prescaler = 1
  ICR1 = 320;           // TOP = 320
  
  // Set the PWM pins as output.
  pinMode(FAN_PIN, OUTPUT);
  //pinMode(10, OUTPUT);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
}

void loop()
{
  analogWrite25k(FAN_PIN, 180);
  RGB_color(255, 255, 255);
  
  while(1){
    delay(1000);
    ds.selectNext();
    Serial.println(ds.getTempC());
    analogWrite25k(FAN_PIN, map(ds.getTempC(), 40, 60, 200, 320));    
  }
}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(LED_RED, 255 - red_light_value);
  analogWrite(LED_GREEN, 255 - green_light_value);
  analogWrite(LED_BLUE, 255 - blue_light_value);
}
