#include <avr/sleep.h>

/**
 * Tiny Doorbell Remote
 * 
 * Attiny85 based RF transmitter that acts as a remote doorbell button.
 * 
 * https://github.com/anroots/tiny-doorbell-remote
 * Ando Roots 2015
 * MIT licence
**/

// Setting this HIGH will power on the radio
const byte RADIO_POWER_PIN = 3;

// Pipe binary data here to be transmitted over the air
const byte TX_PIN = 4;

// A standard light for blinking purposes (it's ON!)
const byte ACTIVITY_LED_PIN = 0;

// Input pin to trigger the signal
const byte TRIGGER_PIN = 2;

// Length of the 'triggerSignal' array
const byte SIGNAL_LENGTH = 27;

// How many times to repeat the signal when transmitting
const byte REPEAT_COUNT = 98;

// Microseconds between each signal repetition
const unsigned int REPEAT_DELAY = 5000;

// Microsecond durations for different signals
const unsigned int LONG_UP = 730;
const unsigned int SHORT_UP = 350;
const unsigned int SHORT_DOWN = 250;
const unsigned int LONG_DOWN = 550;

// The actual doorbell trigger signal that we send 
// This is 0111010011100 in binary and 0E9C in hex
// This particular signal is for one instance of REV 0046830 doorbell
unsigned int triggerSignal[SIGNAL_LENGTH] = {
  SHORT_UP, SHORT_DOWN, LONG_UP, SHORT_DOWN, LONG_UP, SHORT_DOWN, LONG_UP, LONG_DOWN, SHORT_UP, SHORT_DOWN, LONG_UP, LONG_DOWN, SHORT_UP, LONG_DOWN,
  SHORT_UP, SHORT_DOWN, LONG_UP, SHORT_DOWN, LONG_UP, SHORT_DOWN, LONG_UP, LONG_DOWN, SHORT_UP, LONG_DOWN, SHORT_UP, LONG_DOWN, SHORT_UP
};


ISR(WDT_vect) {
  // Placeholder interrupt routine
  // Do not add stuff here or delete this if you don't know what you're doing
}

void setup()
{

  // Set transmit pin to OUT and turn it LOW until needed
  pinMode(TX_PIN, OUTPUT);
  digitalWrite(TX_PIN, LOW);

  // The activity pin will start blinking the LED
  pinMode(ACTIVITY_LED_PIN, OUTPUT);
  digitalWrite(ACTIVITY_LED_PIN, LOW);

  // Set radio pin to OUTPUT and power the radio down
  pinMode(RADIO_POWER_PIN, OUTPUT);
  digitalWrite(RADIO_POWER_PIN, LOW);

  // The triggering sensor pin is an active low input pin
  // Set it to HIGH (with a pullup), the trigger activates once it gets LOW
  pinMode(TRIGGER_PIN, INPUT_PULLUP);

  // Power down various bits of hardware to lower power usage
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  // Disable ADC, saves ~230uA
  ADCSRA &= ~(1 << ADEN);

  // This sets the timing between sensor readings.
  setupWatchdog(7);
}


// Ring the doorbell
// Power on the radio, send the activation signal, power radio off.
void ringBell()
{
  digitalWrite(RADIO_POWER_PIN, HIGH);
  delay(100);

  for (int j = 0; j < REPEAT_COUNT; j++)
  {
    singleRing();
    delayMicroseconds(REPEAT_DELAY);
  }

  digitalWrite(RADIO_POWER_PIN, LOW);
}

// Send the doorbell activation signal ONCE
void singleRing()
{

  boolean previousLevel = false;

  for (int i = 0; i < SIGNAL_LENGTH - 1; i++)
  {

    if (previousLevel) {
      digitalWrite(TX_PIN, LOW);
    } else {
      digitalWrite(TX_PIN, HIGH);
    }

    previousLevel = !previousLevel;
    delayMicroseconds(triggerSignal[i]);
  }

  digitalWrite(TX_PIN, LOW);
}

// Return true if the triggering pin is LOW
boolean isTriggerActive() {
  if (!digitalRead(TRIGGER_PIN) == LOW) {
    return false;
  }

  // Software debouncing
  delay(10);

  return digitalRead(TRIGGER_PIN) == LOW;
}

void loop()
{
  // Sleep a bit...
  sleep_mode();

  // Blink the activity LED to indicate that yes, we are ON
  digitalWrite(ACTIVITY_LED_PIN, HIGH);
  delay(100);
  digitalWrite(ACTIVITY_LED_PIN, LOW);

  // Take a sensor reading and ring the bell if the trigger is active
  if (isTriggerActive()) {
    ringBell();
    
    // Sleep a bit longer to avoid accidental re-triggers
    delay(10000);
  }
}


// 0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms
// 6=1sec, 7=2sec, 8=4sec, 9=8sec
// From: http://interface.khm.de/index.php/lab/experiments/sleep_watchdog_battery/
void setupWatchdog(int timerPrescaler) {

  if (timerPrescaler > 9 ) timerPrescaler = 9; //Correct incoming amount if need be

  byte bb = timerPrescaler & 7;
  if (timerPrescaler > 7) bb |= (1 << 5); //Set the special 5th bit if necessary

  //This order of commands is important and cannot be combined
  MCUSR &= ~(1 << WDRF); //Clear the watch dog reset
  WDTCR |= (1 << WDCE) | (1 << WDE); //Set WD_change enable, set WD enable
  WDTCR = bb; //Set new watchdog timeout value
  WDTCR |= _BV(WDIE); //Set the interrupt enable, this will keep unit from resetting after each int
}
