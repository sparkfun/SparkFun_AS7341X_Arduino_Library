/*
  Using the AS7341L 10 channel spectral sensor
  By: Ricardo Ramos
  SparkFun Electronics
  Date: March 21st, 2021
  SparkFun code, firmware, and software is released under the MIT License. Please see LICENSE.md for further details.
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/17719

  This example shows how to read channels individually and get raw and basic counts values.
  
  Hardware Connections:
  - Plug the Qwiic device to your Arduino/Photon/ESP32 using a cable
  - Open a serial monitor at 115200bps
*/

#include <Wire.h>
#include "SparkFun_AS7341L_Arduino_Library.h"

// Main AS7341L object
SparkFun_AS7341L as7341L;

// Sample number count
unsigned int sampleNumber;

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

	// Set sample number variable
	sampleNumber = 0;
	
	// If the board was properly initialized, turn on LED_BUILTIN
	if(result == true)
	  digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
	Serial.print("Sample number: ");
	Serial.println(++sampleNumber);
	Serial.println();
	
	// Read all channels individually in sequence
	
	unsigned int singleChannelRaw = as7341L.read415nm();
	float singleChannelBasicCount = as7341L.readBasicCount415nm();
	if (as7341L.getLastError() == ERROR_NONE)
	{
		Serial.print("415 nm raw value is: ");
		Serial.println(singleChannelRaw);
		Serial.print("415 nm basic count value is: ");
		Serial.println(singleChannelBasicCount, 5);
	}
	else
	{
		PrintErrorMessage();
	}
	
	singleChannelRaw = as7341L.read445nm();
	singleChannelBasicCount = as7341L.readBasicCount445nm();
	if (as7341L.getLastError() == ERROR_NONE)
	{
		Serial.print("445 nm raw value is: ");
		Serial.println(singleChannelRaw);
		Serial.print("445 nm basic count value is: ");
		Serial.println(singleChannelBasicCount, 5);
	}
	else
	{
		PrintErrorMessage();
	}
		
	singleChannelRaw = as7341L.read480nm();
	singleChannelBasicCount = as7341L.readBasicCount480nm();
	if (as7341L.getLastError() == ERROR_NONE)
	{
		Serial.print("480 nm raw value is: ");
		Serial.println(singleChannelRaw);
		Serial.print("480 nm basic count value is: ");
		Serial.println(singleChannelBasicCount, 5);
	}
	else
	{
		PrintErrorMessage();
	}
		
	singleChannelRaw = as7341L.read515nm();
	singleChannelBasicCount = as7341L.readBasicCount515nm();
	if (as7341L.getLastError() == ERROR_NONE)
	{
		Serial.print("515 nm raw value is: ");
		Serial.println(singleChannelRaw);
		Serial.print("515 nm basic count value is: ");
		Serial.println(singleChannelBasicCount, 5);
	}
	else
	{
		PrintErrorMessage();
	}
		
	singleChannelRaw = as7341L.read555nm();
	singleChannelBasicCount = as7341L.readBasicCount555nm();
	if (as7341L.getLastError() == ERROR_NONE)
	{
		Serial.print("555 nm raw value is: ");
		Serial.println(singleChannelRaw);
		Serial.print("555 nm basic count value is: ");
		Serial.println(singleChannelBasicCount, 5);
	}
	else
	{
		PrintErrorMessage();
	}
		
	singleChannelRaw = as7341L.read590nm();
	singleChannelBasicCount = as7341L.readBasicCount590nm();
	if (as7341L.getLastError() == ERROR_NONE)
	{
		Serial.print("590 nm raw value is: ");
		Serial.println(singleChannelRaw);
		Serial.print("590 nm basic count value is: ");
		Serial.println(singleChannelBasicCount, 5);
	}
	else
	{
		PrintErrorMessage();
	}
	
	singleChannelRaw = as7341L.read630nm();
	singleChannelBasicCount = as7341L.readBasicCount630nm();
	if (as7341L.getLastError() == ERROR_NONE)
	{
		Serial.print("630 nm raw value is: ");
		Serial.println(singleChannelRaw);
		Serial.print("630 nm basic count value is: ");
		Serial.println(singleChannelBasicCount, 5);
	}
	else
	{
		PrintErrorMessage();
	}
	
	singleChannelRaw = as7341L.read680nm();
	singleChannelBasicCount = as7341L.readBasicCount680nm();
	if (as7341L.getLastError() == ERROR_NONE)
	{
		Serial.print("680 nm raw value is: ");
		Serial.println(singleChannelRaw);
		Serial.print("680 nm basic count value is: ");
		Serial.println(singleChannelBasicCount, 5);
	}
	else
	{
		PrintErrorMessage();
	}
	
	singleChannelRaw = as7341L.readClear();
	singleChannelBasicCount = as7341L.readBasicCountClear();
	if (as7341L.getLastError() == ERROR_NONE)
	{
		Serial.print("Clear raw value is: ");
		Serial.println(singleChannelRaw);
		Serial.print("Clear basic count value is: ");
		Serial.println(singleChannelBasicCount, 5);
	}
	else
	{
		PrintErrorMessage();
	}
	
	singleChannelRaw = as7341L.readNIR();
	singleChannelBasicCount = as7341L.readBasicCountNIR();
	if (as7341L.getLastError() == ERROR_NONE)
	{
		Serial.print("NIR raw value is: ");
		Serial.println(singleChannelRaw);
		Serial.print("NIR basic count value is: ");
		Serial.println(singleChannelBasicCount, 5);
	}
	else
	{
		PrintErrorMessage();
	}
	
	Serial.println();
	Serial.println("------------------------------------");
	
	delay(2000);
}
