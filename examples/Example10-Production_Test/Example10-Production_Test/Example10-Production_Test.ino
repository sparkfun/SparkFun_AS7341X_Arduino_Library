
#include <Wire.h>
#include "SparkFun_AS7341X_Arduino_Library.h"

// Comment the line below to bypass IR led test and NIR measurement
#define TEST_INFRARED

// Main AS7341L object
SparkFun_AS7341X as7341L;

// Print a friendly error message
void PrintErrorMessage()
{
  switch (as7341L.getLastError())
  {
  case ERROR_AS7341X_I2C_COMM_ERROR:
    Serial.println("Error: AS7341X I2C communication error");
    break;
  
  case ERROR_PCA9536_I2C_COMM_ERROR:
    Serial.println("Error: PCA9536 I2C communication error");
    break;
    
  case ERROR_AS7341X_MEASUREMENT_TIMEOUT:
    Serial.println("Error: AS7341X measurement timeout");
    break;
    
  case ERROR_AS7341X_INVALID_DEVICE:
    Serial.println("Error: AS7341L cannot measure flicker detection");
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
  as7341L.enable_AS7341X();

  // If the board was properly initialized, turn on LED_BUILTIN
  if (result == true)
  {
    Serial.println();
    Serial.println("AS7341 connected.");
  }
}

void loop()
{
  Serial.println();
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  Serial.println("Red power LED off");
  as7341L.disablePowerLed();
  
  // White LED is limited to 2 mA during testing so operator won't become obfuscated
  Serial.println("White LED drive @ 2 mA");
  // Set white LED drive current to 2 mA
  as7341L.setLedDrive(2);
  // Turn on the white LED
  as7341L.enableWhiteLed();
  // Wait 0.5 second
  delay(500);
  
  // Turn off the white LED
  as7341L.disableWhiteLed();
  Serial.println("White LED off");
  delay(500);
  
  Serial.println("Red power LED on");
  as7341L.enablePowerLed();

  #ifdef TEST_INFRARED
  Serial.println("IR LED drive @ 20 mA");
  // Set IR LED drive current to 20 mA
  as7341L.setLedDrive(20);
  // Turn on the IR LED
  as7341L.enableIRLed();

  // Measure IR channel
  unsigned int singleChannelRaw = as7341L.readNIR();
  if (as7341L.getLastError() == ERROR_NONE)
  {
    Serial.print("Near infrared raw value is: ");
    Serial.println(singleChannelRaw);
  }
  else
  {
    PrintErrorMessage();
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("ERROR - System halted!");
    while(true);
  }
  
  // Wait 0.5 second
  delay(500);

  Serial.println("IR LED off");
  as7341L.disableIRLed();
  #endif
  
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Device OK !");
  Serial.println("Please connect another board and reset the microcontroller.");

  // Wait 1 second and start over
  delay(1000);

}
