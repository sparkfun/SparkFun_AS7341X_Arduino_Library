/*
  Using the AS7341L 10 channel spectral sensor
  By: Ricardo Ramos
  SparkFun Electronics
  Date: March 15th, 2021
  SparkFun code, firmware, and software is released under the MIT License. Please see LICENSE.md for further details.
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/17719

  This example shows how to set up the LED current in AS7341L and turn the on board white and IR led on and off

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
  if (result == false)
  {
    PrintErrorMessage();
    Serial.println("Check your connections. System halted !");
    digitalWrite(LED_BUILTIN, LOW); 
    while (true) ;
  }

  // Bring AS7341L to the powered up state
  as7341L.enable_AS7341L();

  // If the board was properly initialized, turn on LED_BUILTIN
  if (result == true)
    digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
  // Set white LED drive current to 60 mA
  as7341L.setLedDrive(60);
  // Turn on the white LED
  as7341L.enableWhiteLed();
  // Wait 1 second
  delay(1000);
  // Turn on the white LED
  as7341L.disableWhiteLed();
  
  // Set IR LED drive current to 20 mA
  as7341L.setLedDrive(20);
  // Turn on the IR LED
  as7341L.enableIRLed();
  // Wait 1 second
  delay(1000);
  // Turn on the IR LED and start over
  as7341L.disableIRLed();
}
