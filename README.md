# MMD-684U
Experiments with Mystery MMD-684U front panel  
  
![image_1](https://raw.githubusercontent.com/lxxxxl/mmd-684u/master/images/mmd-684u.jpg?raw=true)  


## Wires on unit:
1. FLASH	- LED on back panel
2. GND
3. LAMP		- keyboard lighs
4. RIN		- AUX right
5. LIN		- AUX left
6. IR		- IR receiver
7. +5V 		- LC75823 Vdd
8. KEY2		- keys
9. KEY1		- keys
10. GND
11. CE		- LC75823 CE
12. EN-		- volume encoder B
13. EN+		- volume encoder A
14. GND
15. CLK		- LC75823 CL
16. DATA	- LC75823 DI
17. GND
18. \+		- USB D+
19. \-		- USB D-
20. USB5V	- USB power


## Panel pinout
Applies on rear panel view  
20. USB5V | 19. -   | 18. +   | 16. DATA | 15. CLK | 14. GND | 13. EN+ | 12. EN- | 11. CE   
------------------------------------------------------------------------------------------  
14. GND   | 9. KEY1 | 8. KEY2 | 7. +5V   | 6. IR   | 5. LIN  | 4. RIN  | 3. LAMP | 2. GND   


## LCD segments bit encoding
![image_1](https://raw.githubusercontent.com/lxxxxl/mmd-684u/master/images/led-segments.png?raw=true)  

## LCD connection
LCD panel controlled via SPI interface.  
`DATA` pin should be connected to Arduino `PIN D11 (MOSI)`  
`CLK` pin should be connected to Arduino `PIN D13 (SCK)`  
`CE` pin should be connected to Arduino `PIN D10` 

## Buttons
`KEY1` pin emits signals from 11 buttons.  
`KEY2` pin emits signals from 7 buttons.  
Pins should be pulled up to 5V.  
To remove noise from DAC output we can use bit shift:

```C++

pinMode(KEY_PIN1, INPUT_PULLUP); // 10 buttons
pinMode(KEY_PIN2, INPUT_PULLUP); // 7 buttons

// ...

int button = analogRead(KEY_PIN1);
if (button < 1000){    // signals>1000 are garbage because because PIN pulled to 5V
  Serial.println(button >> 4);
}
button = analogRead(KEY_PIN2);
if (button < 1000){    // signals>1000 are garbage because because PIN pulled to 5V
  Serial.println(button >> 4);
}
```
Here are button ids received after bit shift.  

### KEY1
* Play/Pause - 0
* INT - 1
* RPT - 2
* RDM - 3
* STOP - 4
* OSD - 5
* Band - 7
* Seek up - 10
* Seek down - 12
* Encoder - 16
* AMS - 30

### KEY2
* Mute - 1
* LOC - 2
* DISP - 4
* Power - 5-6
* MOD - 7
* EQ - 10
* ST - 14

## Digital encoder
[Digital encoder](https://en.wikipedia.org/wiki/Encoder_(digital)) pins `EN+` and `EN-` should be pulled up to 5V.  
Encoder can be polled via interrupts this way:
```C++
void setup() {
  // attach interrupts for encoder poll
  attachPCINT(ENCODER_A);
  attachPCINT(ENCODER_B);
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
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
  pollEncoder();
}
ISR(PCINT1_vect) {
}
ISR(PCINT2_vect) {
  pollEncoder();
}
```
Full source code can be found in [LED-test/LED-test.ino](LED-test/LED-test.ino)

## IR receiver
`IR` pin should be connected to any `PWM` Arduino pin.  
To decode IR data [IRremote Arduino Library](https://github.com/Arduino-IRremote/Arduino-IRremote/) can be used.

```C++
#include <IRremote.h>

#define IR_SENSOR 6

void setup() {
  pinMode(IR_SENSOR, INPUT);  
  IrReceiver.begin(IR_SENSOR);
  Serial.begin(9600);
}

void loop() {
  if (IrReceiver.decode()) {
    IrReceiver.printIRResultMinimal(&Serial);
    IrReceiver.resume();
  }
  delay(100);
}
```

# LCD Test
Arduino sketch that turns on single segment on LCD at a time.  
To go to next/previous segment one should rotate encoder on panel.  

Sketch uses standard Arduino SPI library. Wires should be connected as follows:
* Data line to Arduino pin 11 (or MOSI pin 4 on the ICSP socket)
* CLK to Arduino pin 13 (or SCK pin 3 on the ICSP socket)
* FCE (ChipEnabled, SlaveSelect) input needs to be connected to Arduino pin 10
* Encoder pin A (EN+) to Arduino pin 6
* Encoder pin B (EN-) to Arduino pin 7

Used SPI data address can be found in datasheet. In my case it is `130`.  
You can adjust encoder and chip select pins by editing this section of sketch.  
```C++
#define ENCODER_A 6       // Encoder PIN A
#define ENCODER_B 7       // Encoder PIN B
#define CS_PIN 10         // chip select pin for LC75823

#define LED_ADDR 130     // LED SPI data address
```

# LC75823 Font Creator
This app can be used to create bitmasks for desired symbols according to `LCD segments bit encoding`.  

# Controller
Arduino sketch that implements all described interfaces.

# Useful links
https://pdf1.alldatasheet.com/datasheet-pdf/download/40944/SANYO/LC75823E.html
http://payaem-vmeste.blogspot.com/2015/10/begushhaja-stroka-na-displee-ot-magnitoly-part-1.html  
https://www.drive2.ru/l/471761682767544577/  
https://github.com/firatsoygul/LC75823  
