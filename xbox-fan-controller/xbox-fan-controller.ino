#include <DS18B20.h>

#define FAN_PIN 9
#define TEMP_PIN 5

#define LED_RED A2
#define LED_GREEN A1
#define LED_BLUE A3

#define MIN_FAN_SPEED 170

DS18B20 ds(TEMP_PIN);

float temperature = 0;
float targetTempColor = 0;

void setup()
{
  Serial.begin(9600);

  // PC fan required 25KHz pulse width modulation signal
  // https://arduino.stackexchange.com/questions/25609/set-pwm-frequency-to-25-khz
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

  // Setup LED pins
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
}

void loop()
{
  analogWrite25k(FAN_PIN, MIN_FAN_SPEED);
  RGB_color(20, 20, 20);

  while(1){
    delay(1000);
    ds.selectNext();
    temperature = ds.getTempC();
    
    Serial.println(temperature);

    // 40C => cpu 52C, 45C => cpu 61C, 59C => cpu 80C
    analogWrite25k(FAN_PIN, map(temperature, 45, 60, MIN_FAN_SPEED, 320));    

    // ramp up the color to desired setting to avoid sudden change in colour 
    if (targetTempColor == 0 || targetTempColor == temperature){
      targetTempColor = temperature;
    }
    else {          
      targetTempColor = (targetTempColor < temperature) ? targetTempColor + 0.1 : targetTempColor - 0.1;
    }
    RGB_color(map(targetTempColor, 45, 60, 0, 200), 20, 20);
  }
}

void RGB_color(int red, int green, int blue)
 {
  analogWrite(LED_RED, 255 - red);
  analogWrite(LED_GREEN, 255 - green);
  analogWrite(LED_BLUE, 255 - blue);
}

// PWM output @ 25 kHz, only on pins 9 and 10.
// Output value should be between 0 and 320, inclusive.
// https://arduino.stackexchange.com/questions/25609/set-pwm-frequency-to-25-khz
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
