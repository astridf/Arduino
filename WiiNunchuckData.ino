/*
 * Wii Nunchuck to Arduino - Astrid Farmer (Dec, 2015)
 * http://www.astridfarmer.com
 * **************************************************
 * A single file solution to reading data from wii nunchuck
 * controllers and sending the data via serial communication.
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
//Create a 6 byte array to store the data
static uint8_t data_byte[6];

void setup() {
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
    
  Serial.print("Accelerometer Data:");
  Serial.print(accelerometerX, DEC);Serial.print(",");
  Serial.print(accelerometerY, DEC);Serial.print(",");
  Serial.print(accelerometerZ, DEC);Serial.print("\t");
  
  Serial.print("Joystick Data:");
  Serial.print(joystickX, DEC);Serial.print(",");
  Serial.print(joystickY, DEC);Serial.print("\t");

  Serial.print("Button Data:");
  Serial.print(buttonZ, DEC);Serial.print(",");
  Serial.print(buttonC, DEC);Serial.print("\r\n");
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

