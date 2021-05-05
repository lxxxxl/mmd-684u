#include <LC75823.h>
#include "controller.h"

#define ENCODER_A 7       // Encoder PIN A
#define ENCODER_B 6       // Encoder PIN B
#define CS_PIN 10         // chip select pin for LC75823

#define KEY_PIN1 A0
#define KEY_PIN2 A1


#define SYMBOLS_TOP 1     // symbols at top row
#define SYMBOLS_BOTTOM 2  // symbols at bottom row

// min and max volume levels
#define VOLUME_MIN 0
#define VOLUME_MAX 8

LC75823 lcd;

// poll encoder vars
volatile boolean encResetFlag;
volatile byte encCurState, encPrevState;

byte volumeLevel; // volume level

char currentTopSymbol;    // current active symbol on top row
char currentBottomSymbol; // current active symbol on bottom row

void setup() {
  // attach interrupts for encoder poll
  attachPCINT(ENCODER_A);
  attachPCINT(ENCODER_B);
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);

  // keyboard pins
  pinMode(KEY_PIN1, INPUT_PULLUP); // 10 buttons
  pinMode(KEY_PIN2, INPUT_PULLUP); // 7 buttons
  

  // setup SPI for LC75823 communication
  lcd.begin(10, 130); //SlaveSelect pin, chip address.
  lcd.speed(250); // Animation speed.
  lcd.clear(); // clear screen

  // serial debug
  Serial.begin(9600);

  // init vars
  volumeLevel = VOLUME_MIN;
  currentTopSymbol = Symbol::AF;
  currentBottomSymbol = Symbol::RPT;
}

void loop() {
  uint16_t key = pollKeys();
  
  if (key == KEY1_OSD)
    nextSymbol(SYMBOLS_TOP);
  else if (key == KEY1_STOP)
    prevSymbol(SYMBOLS_TOP);
  else if (key == KEY1_RDM)
    nextSymbol(SYMBOLS_BOTTOM);
  else if (key == KEY1_RPT)
    prevSymbol(SYMBOLS_BOTTOM);
    
  delay(100);
}

// attach PCINT for ATmega328 (UNO, Nano, Pro Mini)
// https://alexgyver.ru/encoder/
uint8_t attachPCINT(uint8_t pin) {
  if (pin < 8) { // D0-D7 // PCINT2
    PCICR |= (1 << PCIE2);
    PCMSK2 |= (1 << pin); return 2; } else if (pin > 13) { //A0-A5  // PCINT1
    PCICR |= (1 << PCIE1);
    PCMSK1 |= (1 << pin - 14);
    return 1;
  }
  else { // D8-D13  // PCINT0
    PCICR |= (1 << PCIE0);
    PCMSK0 |= (1 << pin - 8);
    return 0;
  }
}

// PCINT vectors
// pins 0-7: PCINT2
// pins 8-13: PCINT0
// pins A0-A5: PCINT1
ISR(PCINT0_vect) {
}
ISR(PCINT1_vect) {
}
ISR(PCINT2_vect) {
  pollEncoder();
}

// fast implementation of digitalRead
boolean readPin(uint8_t pin) {
  if (pin < 8) return bitRead(PIND, pin);
  else if (pin < 14) return bitRead(PINB, pin - 8);
  else if (pin < 20) return bitRead(PINC, pin - 14);
  else return false;
}

// polls encoder state
void pollEncoder() {
  encCurState = readPin(ENCODER_A) | readPin(ENCODER_B) << 1;
  if (encResetFlag && encCurState == 0b11) {
    if (encPrevState == 0b10) 
      volumeUp();
    if (encPrevState == 0b01)
      volumeDown();
  }
  if (encCurState == 0b00) encResetFlag = 1;
  encPrevState = encCurState;
}

// poll keyboard for keypress
uint16_t pollKeys(){
  uint16_t button = analogRead(KEY_PIN1);
  if (button < 900){    // signals>900 are garbage because because PIN pulled to 5V
    return (KEY1_MASK << 8) + (button >> 4);
  }
  button = analogRead(KEY_PIN2);
  if (button < 900){    // signals>900 are garbage because because PIN pulled to 5V
    return (KEY2_MASK << 8) + (button >> 4);
  }
  return -1;
}

// set volume up on lcd
void volumeUp() {
  volumeLevel++;
  if (volumeLevel > VOLUME_MAX)
    volumeLevel = 0;
  lcd.volumeLevel(volumeLevel);
  Serial.println(volumeLevel,DEC);
}

// set volume down on lcd
void volumeDown() {
  volumeLevel--;
  if (volumeLevel <= VOLUME_MIN)
    volumeLevel = VOLUME_MAX;
  lcd.volumeLevel(volumeLevel);
  Serial.println(volumeLevel,DEC);
}


// light up next top or bottom symbol
// @param row SYMBOLS_TOP for top symbols, SYMBOLS_BOTTOM for bottom
void nextSymbol(char row){
  
  if (row == SYMBOLS_TOP){
    lcd.symbol((Symbol)currentTopSymbol, 1);
    if (currentTopSymbol < Symbol::RINGS)
      currentTopSymbol++;
  }
  else if (row == SYMBOLS_BOTTOM){
    lcd.symbol((Symbol)currentBottomSymbol, 1);
    if (currentBottomSymbol < Symbol::STEREO)
      currentBottomSymbol++;
  }
}

// light down current top or bottom symbol
// @param row SYMBOLS_TOP for top symbols, SYMBOLS_BOTTOM for bottom
void prevSymbol(char row){
  if (row == SYMBOLS_TOP){
    lcd.symbol((Symbol)currentTopSymbol, 0);
    if (currentTopSymbol > Symbol::AF)
      currentTopSymbol--;
  }
  else if (row == SYMBOLS_BOTTOM){
    lcd.symbol((Symbol)currentBottomSymbol, 0);
    if (currentBottomSymbol > Symbol::RPT)
      currentBottomSymbol--;
  }
}
