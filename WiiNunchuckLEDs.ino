/*
 * Arduino LEDs controlled by a Wii Nunchuck - Astrid Farmer (Dec, 2015)
 * http://www.astridfarmer.com
 * **************************************************
 * 
 * Inspiration and code excepts used from:
 * 2007-11 Tod E. Kurt, http://todbot.com/blog/
 *
 * Windmeadow Labs, http://www.windmeadow.com/node/42
 * (Link no longer available)
 * 
 */
 
#include <Wire.h>
#define powerAnalogPin3 PC3
#define groundAnalogPin2 PC2
static uint8_t data_byte[6];

int leftLED = 11;
int forwardLED = 10;
int backLED = 9;
int rightLED = 8;
int cButtonLED = 13;
int zButtonLED = 12;

void setup() {
  pinMode(leftLED, OUTPUT);
  pinMode(forwardLED, OUTPUT);
  pinMode(backLED, OUTPUT);
  pinMode(rightLED, OUTPUT);
  pinMode(cButtonLED, OUTPUT);
  pinMode(zButtonLED, OUTPUT);
  
  Serial.begin(9600);
  /* The next three lines may look daunting (and that's because they are,
   * or at least they were until I googled them extensively). They are written
   * in AVR-C, and make it much easier to plug in I2C things such as our nunchucks,
   * without having to play with wiring. Changing them will break everything. */
   //The next line sets both the analog pins 2 & 3 into write mode.
  DDRC |= _BV(powerAnalogPin3) | _BV(groundAnalogPin2);
  //Then we set the ground pin (-) to write low.
  PORTC &= ~ _BV(groundAnalogPin2);
  //Finally, we set the power pin (+) to write high.
  PORTC |= _BV(powerAnalogPin3);
  delay(100);
  initialise_nunchuck_via_I2C();
}

void loop() {
  retrieveNunchuckData();
  printDataToSerial();
  delay(100);
}

/* This function retrieves the next byte of data from the nunchuck, 
 * and reformats it for later use. */
int retrieveNunchuckData(){
  int i = 0;
  Wire.requestFrom (0x52, 6); 
  while (Wire.available ()) {
    data_byte[i] = ((Wire.read() ^ 0x17) + 0x17);
    i++;
  }
  Wire.beginTransmission(0x52);
  Wire.write(0x00);
  Wire.endTransmission();
}

/* This function will extract the data from the data bytes
 * and print them in an easy-to-read format. 
 * Buttons: 0 = Not pressed, 1 = Pressed */
void printDataToSerial(){
  int joystickX = data_byte[0];
  int joystickY = data_byte[1];
  int accelerometerX = data_byte[2];
  int accelerometerY = data_byte[3];
  int accelerometerZ = data_byte[4];
  int buttonZ = 1;
  int buttonC = 1;
  if ((data_byte[5] >> 0) & 1)
    buttonZ = 0;
  if ((data_byte[5] >> 1) & 1)
    buttonC = 0;
    
digitalWrite(leftLED, LOW); 
digitalWrite(rightLED, LOW); 
digitalWrite(forwardLED, LOW); 
digitalWrite(backLED, LOW); 
digitalWrite(zButtonLED, LOW); 
digitalWrite(cButtonLED, LOW); 

if (accelerometerX < 100){
  digitalWrite(rightLED, HIGH); 
}
if (accelerometerX > 150){
  digitalWrite(leftLED, HIGH); 
}
if (accelerometerY > 150){
  digitalWrite(forwardLED, HIGH); 
}
if (accelerometerY < 110){
  digitalWrite(backLED, HIGH); 
}
if (buttonZ == 1){
  digitalWrite(zButtonLED, HIGH); 
}
if (buttonC == 1){
  digitalWrite(cButtonLED, HIGH); 
}
if(joystickX < 70){
  digitalWrite(rightLED, HIGH);
}
if(joystickX > 160){
  digitalWrite(leftLED, HIGH);
}
if(joystickY < 100){
  digitalWrite(backLED, HIGH);
}
if(joystickY > 160){
  digitalWrite(forwardLED, HIGH);
}
}

/* Below is the code needed to create a connection with 
 * the Nintendo Wii Nunchuck, as well as the cheap knockoff 
 * nunchucks not produced by Nintendo, the reason so much 
 * code is required is because the original and knockoff
 * nunchucks have different initialisation sequences. 
*/
void initialise_nunchuck_via_I2C(){
  uint8_t ctrlr_type[6];
  byte i;
  Wire.begin();
  delay(1);
  Wire.beginTransmission(0x52);
  Wire.write(0xF0);
  Wire.write(0x55);
  Wire.endTransmission();
  delay(1);
  Wire.beginTransmission(0x52);
  Wire.write(0xFB);
  Wire.write(0x00);
  Wire.endTransmission();
  delay(1);
  Wire.beginTransmission(0x52);
  Wire.write(0xFA);
  Wire.endTransmission();
  Wire.beginTransmission(0x52);
  Wire.requestFrom(0x52, 6);
  for (i = 0; i < 6; i++) {
    if (Wire.available()) {
      ctrlr_type[i] = Wire.read();
    }
  }
  Wire.endTransmission();
  delay(1);
  Wire.beginTransmission(0x52);
  Wire.write(0xF0);
  Wire.write(0xAA);
  Wire.endTransmission();
  delay(1);
  Wire.beginTransmission(0x52);
  Wire.write(0x40);
  for (i = 0; i < 6; i++) {
    Wire.write(0x00);
  }
  Wire.endTransmission();
  Wire.beginTransmission(0x52);
  Wire.write(0x40);
  for (i = 6; i < 12; i++) {
    Wire.write(0x00);
  }
  Wire.endTransmission();
  Wire.beginTransmission(0x52);
  Wire.write(0x40);
  for (i = 12; i < 16; i++) {
    Wire.write(0x00);
  }
  Wire.endTransmission();
  delay(1);
}
