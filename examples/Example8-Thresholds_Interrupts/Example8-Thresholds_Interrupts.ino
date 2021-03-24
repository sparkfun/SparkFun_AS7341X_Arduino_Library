/*
  Using the AS7341L 10 channel spectral sensor
  By: Ricardo Ramos
  SparkFun Electronics
  Date: March 21st, 2021
  SparkFun code, firmware, and software is released under the MIT License. Please see LICENSE.md for further details.
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/17719

  This example shows how to define and use low and high threshold limits for triggering an interrupt.
  
  Hardware Connections:
  - Plug the Qwiic device to your Arduino/Photon/ESP32 using a cable
  - Open a serial monitor at 115200bps
*/

#include <Wire.h>
#include "SparkFun_AS7341L_Arduino_Library.h"

// Main AS7341L object
SparkFun_AS7341L as7341L;

// Selected threshold channel
const byte thresholdChannel = 0;

// Low threshold value
const unsigned int lowThreshold = 400;

// High threshold value
const unsigned int highThreshold = 10000;

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
	
	// Disable measurements
	as7341L.disableMeasurements();
	
	// Set low threshold value
	as7341L.setLowThreshold(lowThreshold);
	
	// Set high threshold value
	as7341L.setHighThreshold(highThreshold);

	// Set persistance so every spectral cycle generates an interrupt
	as7341L.setAPERS(0);
	
	// Enable measurements
	as7341L.enableMeasurements();
	
	// Enable threshold interrupts
	as7341L.enableThresholdInterrupt();

	// If the board was properly initialized, turn on LED_BUILTIN
	if(result == true)
	  digitalWrite(LED_BUILTIN, HIGH);
	
}

void loop()
{
	// Set threshold to NIR by reading NIR and prints it's value
	unsigned int value = as7341L.readNIR();
	Serial.print("NIR nm: ");
	Serial.println(value);
	
	// Prints out if the low threshold interrupt was triggered
	if (as7341L.lowThresholdInterruptSet())
		Serial.println("Low threshold interrupt generated!");
	
	// Prints out if the high threshold interrupt was triggered
	if (as7341L.highThresholdInterruptSet())
		Serial.println("High threshold interrupt generated!");

	// Mark threshold interrupts as processed to allow a new processing cycle
	as7341L.clearThresholdInterrupts();
	
	// Wait and start over
	delay(1000);
}
