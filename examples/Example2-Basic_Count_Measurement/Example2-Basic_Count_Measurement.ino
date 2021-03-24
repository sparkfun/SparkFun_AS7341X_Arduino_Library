/*
  Using the AS7341L 10 channel spectral sensor
  By: Ricardo Ramos
  SparkFun Electronics
  Date: March 15th, 2021
  SparkFun code, firmware, and software is released under the MIT License. Please see LICENSE.md for further details.
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/17719

  This example shows how to read all channels (F1 to F8, CLEAR and NIR) and get basic count values from the sensor.
  Further information can be found in AMS' AN000633 on page 7.
  
  Hardware Connections:
  - Plug the Qwiic device to your Arduino/Photon/ESP32 using a cable
  - Open a serial monitor at 115200bps
*/

#include <Wire.h>
#include "SparkFun_AS7341L_Arduino_Library.h"

// Main AS7341L object
SparkFun_AS7341L as7341L;

// Sample number variable
unsigned int sampleNumber = 0;

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
  // Array which contains all channels processed values
  float channelReadings[12];

  // Read all channels
  bool result = as7341L.readAllChannelsBasicCounts(channelReadings);

  // Check if the read operation was successful and print out results with 5 decimal digits
  if (result == true)
  {
    Serial.println("---------------------------------");
    Serial.print("Sample number: ");
    Serial.println(++sampleNumber);
    Serial.println();
    Serial.print("F1 (415 nm): ");
    Serial.println(channelReadings[0],5);
    Serial.print("F2 (445 nm): ");
    Serial.println(channelReadings[1],5);
    Serial.print("F3 (480 nm): ");
    Serial.println(channelReadings[2],5);
    Serial.print("F4 (515 nm): ");
    Serial.println(channelReadings[3],5);
    // channelReadings[4] and [5] hold Clear and NIR values, respectively (same as [10] and [11])
    Serial.print("F5 (555 nm): ");
    Serial.println(channelReadings[6],5);
    Serial.print("F6 (590 nm): ");
    Serial.println(channelReadings[7],5);
    Serial.print("F7 (630 nm): ");
    Serial.println(channelReadings[8],5);
    Serial.print("F8 (680 nm): ");
    Serial.println(channelReadings[9],5);
    Serial.print("Clear: ");
    Serial.println(channelReadings[10],5);
    Serial.print("NIR: ");
    Serial.println(channelReadings[11],5);
    Serial.println();
  }
  else
  {
    // Ooops ! We got an error !
    PrintErrorMessage();
  }

  // Wait 1 second and start over
  delay(1000);
}
