/*
  Using the AS7341L 10 channel spectral sensor
  By: Ricardo Ramos
  SparkFun Electronics
  Date: March 17th, 2021
  SparkFun code, firmware, and software is released under the MIT License. Please see LICENSE.md for further details.
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/17719

  This example shows how to write AS7341L GPIO output pin to drive a LED.
  GPIO is an open drain output so it will not write a HIGH value but only LOW values. 
  In this example, GPIO sinks current through a LED, lighting it when the GPIO pin is grounded.
  The pin will float when the GPIO is written HIGH. If you need to use GPIO as an input for a logic circuit
  you may need to add a pull-up resistor. You can find further information on that in this tutorial:
  https://learn.sparkfun.com/tutorials/pull-up-resistors/all
  
  Notice that GPIO is 1.8V tolerant ONLY - DO NOT connect GPIO to voltages above 1.8 volts or the sensor may get damaged !
  
  Hardware Connections:
  - Plug the Qwiic device to your Arduino/Photon/ESP32 using a cable
  - Connect a LED cathode to GPIO pin, the LED anode to 1.8V through an appropriate voltage dropping resistor.
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
	
	// Configure GPIO as output
	as7341L.setGpioPinOutput();
	
	// Turn off GPIO
	as7341L.digitalWrite(LOW);
	
	// Bring AS7341L to the powered up state
	as7341L.enable_AS7341L();
	
	// Turn off the led connected to pin 13
	digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
	// Turn on the LED by connecting it's cathode to GND through the GPIO pin
	as7341L.digitalWrite(LOW);
	// Wait a little...
	delay(500);
	
	// Turn off the LED by disconnecting it's cathode pin
	as7341L.digitalWrite(HIGH);
	// Wait a little and start over
	delay(500);
}
