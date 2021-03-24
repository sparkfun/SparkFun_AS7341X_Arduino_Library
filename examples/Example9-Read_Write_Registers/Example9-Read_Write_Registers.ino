/*
  Using the AS7341L 10 channel spectral sensor
  By: Ricardo Ramos
  SparkFun Electronics
  Date: March 22nd, 2021
  SparkFun code, firmware, and software is released under the MIT License. Please see LICENSE.md for further details.
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/17719

  This example shows how to directly read and write AS7341L registers.
  
  Hardware Connections:
  - Plug the Qwiic device to your Arduino/Photon/ESP32 using a cable
  - Open a serial monitor at 115200bps
*/

#include <Wire.h>
#include "SparkFun_AS7341L_Arduino_Library.h"

// Main AS7341L object
SparkFun_AS7341L as7341L;

// Print a friendly error message
void PrintErrorMessage()
{
  switch (as7341L.getLastError())
  {
  case ERROR_AS7341L_I2C_COMM_ERROR:
    Serial.println("Error: AS7341L I2C communication error");
    break;

  case ERROR_PCA9536_I2C_COMM_ERROR:
    Serial.println("Error: PCA9536 I2C communication error");
    break;
    
  case ERROR_AS7341L_MEASUREMENT_TIMEOUT:
    Serial.println("Error: AS7341L measurement timeout");
    break;
    
  default:
    break;
  }
}

void setup()
{
  // Configure Arduino's built in LED as output
  pinMode(LED_BUILTIN, OUTPUT);

  // Initialize serial port at 115200 bps
  Serial.begin(115200);

  // Initialize the I2C port
  Wire.begin();
  
  // Initialize AS7341L
  boolean result = as7341L.begin();

  // If the board did not properly initialize print an error message and halt the system
  if(result == false)
  {
    PrintErrorMessage();
    Serial.println("Check your connections. System halted !");
    digitalWrite(LED_BUILTIN, LOW); 
    while (true) ;
  }

  // Bring AS7341L to the powered up state
  as7341L.enable_AS7341L();

  Serial.print("Register ENABLE value: ");
  Serial.println(as7341L.readRegister(0x80), HEX);

  // Register ENABLE = 0x80
  byte registerValue = as7341L.readRegister(0x80);

  // Set PON bit to 0
  registerValue &= 0xfe;

  // Write register back
  as7341L.writeRegister(0x80, registerValue);
  
  Serial.print("Register ENABLE value after writing is: ");
  Serial.println(as7341L.readRegister(0x80), HEX);

  // If the board was properly initialized, turn on LED_BUILTIN
  if(result == true)
    digitalWrite(LED_BUILTIN, HIGH);
  
}

void loop()
{
}
