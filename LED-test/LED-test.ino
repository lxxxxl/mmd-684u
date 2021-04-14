#include <SPI.h>

#define ENCODER_A 6       // Encoder PIN A
#define ENCODER_B 7       // Encoder PIN B
#define CS_PIN 10         // chip select pin for LC75823

#define LED_ADDR 130     // LED SPI data address

volatile boolean encResetFlag;
volatile byte encCurState, encPrevState;

// total 20 bytes of data should be sent to chip:
// D1..D156 - display data (1 - on, 0 - off)
// DR - 1/2 bias drive or 1/3 bias drive switching control data
// SC - segments on/off control data
// BU - normal/powersafe mode control data
// * - unused
byte ledData[20];

void setup() {
  Serial.begin(9600);
  
  // attach interrupts for encoder poll
  attachPCINT(ENCODER_A);
  attachPCINT(ENCODER_B);
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);

  // setup SPI for LC75823 communication
  pinMode(CS_PIN, OUTPUT);
  SPISettings(30000, MSBFIRST, SPI_MODE0);
  SPI.begin();

  clearScreen();
}

void loop() {
  delay(1000);
}

void pollEncoder() {
  encCurState = readPin(ENCODER_A) | readPin(ENCODER_B) << 1;
  if (encResetFlag && encCurState == 0b11) {
    if (encPrevState == 0b10) ledDataIncrement();
    if (encPrevState == 0b01) ledDataDecrement();
    encResetFlag = 0;
    ledDataPrint();
  }
  if (encCurState == 0b00) encResetFlag = 1;
  encPrevState = encCurState;
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

// clears LED screen
void clearScreen(){
  memset(ledData, 0, sizeof(ledData));
  ledDataPrint();
  // setup fitst bit in array to shift it
  ledData[0] = 0b10000000;
  
  return;
}

// print data on LED screen
void ledDataPrint(){
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(LED_ADDR);
  digitalWrite(CS_PIN, HIGH);

  for (int i=0; i<=19; i++){
    SPI.transfer(ledData[i]);
    delay(10);
  }
       
  SPI.transfer(0);
  digitalWrite(CS_PIN, LOW); 

  printLedData();
}

// called on encoder clockwise rotation 
void ledDataIncrement(){
  rightShift(ledData, sizeof(ledData));
}

// called on encoder counterclockwise rotation 
void ledDataDecrement(){
  leftShift(ledData, sizeof(ledData));
}


// right shift array
void rightShift(unsigned char * array, int length){
   unsigned char preBuffer = 0;
   unsigned char postBuffer = 0;
   int i;
   for(i = 0; i < length; ++i){
    postBuffer = (array[i] & 0x01) << 7;
    array[i] = array[i] >> 1;
    array[i] |= preBuffer;
    preBuffer = postBuffer;
   }
  array[0] |= preBuffer;
}

// left shift array
void leftShift(unsigned char * array, int length){
   unsigned char preBuffer = 0;
   unsigned char postBuffer = 0;
   int i;
   for(i = length - 1; i >= 0; --i){
    postBuffer = (array[i] & 0x80) >> 7;
    array[i] = array[i] << 1;
    array[i] |= preBuffer;
    preBuffer = postBuffer;
   }
   array[length - 1] |= preBuffer;
}

//print current led data HEX
void printLedData(){
  for (int i = 0; i < sizeof(ledData); i++){
    Serial.print(ledData[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");
}

