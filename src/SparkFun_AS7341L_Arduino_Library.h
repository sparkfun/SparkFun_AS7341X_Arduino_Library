/*
  This is a library written for the AMS AS7341L 10-Channel Spectral Sensor Frontend
  SparkFun sells these at its website:
  https://www.sparkfun.com/products/17719

  Do you like this library? Help support open source hardware. Buy a board!

  Written by Ricardo Ramos  @ SparkFun Electronics, March 15th, 2021
  This file declares the core functions available in the AS7341L sensor library.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __SPARKFUN_AS7341L_LIBRARY__
#define __SPARKFUN_AS7341L_LIBRARY__

#include "SparkFun_AS7341L_Constants.h"
#include "SparkFun_AS7341L_IO.h"
#include <SparkFun_PCA9536_Arduino_Library.h>		// Get library here: https://github.com/sparkfun/SparkFun_PCA9536_Arduino_Library

class SparkFun_AS7341L
{
private:

	// AS7341L I2C interface object
	SparkFun_AS7341L_IO as7341_io;
	
	// PCA9536 I2C interface object
	PCA9536 pca9536_io;

	byte lastError;	
	
	bool whiteLedPowered = false;
	
	bool IRLedPowered = false;
	
	// Sets F1 to F4 + Clear + NIR to ADCs inputs
	void setMuxLo();
	
	// Sets F5 to F8 + Clear + NIR to ADCs inputs
	void setMuxHi();	

	// Reads single channel value after mux setup
	uint16_t readSingleChannelValue();
	
	// Converts raw value to basic count value
	float readSingleBasicCountChannelValue(uint16_t raw);
	
public:
	// Default empty constructor
	SparkFun_AS7341L() {}
	
	// Initialize AS7341L and PCA9536
	bool begin(byte AS7341L_address = DEFAULT_AS7341L_ADDR, TwoWire& wirePort = Wire);	
	
	// Check if board's devices are connected and responding properly
	bool isConnected();
	
	// Read all channels raw values
	bool readAllChannels(unsigned int* channelData);
	
	// Read all channels basic counts. Further information can be found in AN000633, page 7
	bool readAllChannelsBasicCounts(float* channelDataBasicCounts);
	
	// Enable AS7341L
	void enable_AS7341L();
	
	// Power down AS7341L
	void disable_AS7341L();
	
	// Return last error 
	byte getLastError();
	
	// Set LED driver forward current value
	void setLedDrive(unsigned int current);
	
	// Return LED driver forward current value
	unsigned int getLedDrive();
	
	// Enable power on red LED
	void enablePowerLed();
	
	// Disable power on red LED
	void disablePowerLed();
	
	// Turn white LED on  
	void enableWhiteLed();
	
	// Turn white LED off
	void disableWhiteLed();
	
	// Turn infrared LED on 
	void enableIRLed();
	
	// Turn infrared LED off
	void disableIRLed();
	
	// Set ADC integration time (defaults to 29)
	void setATIME(byte aTime = 29);
	
	// Set ADC integration steps (defaults to 599)
	void setASTEP(unsigned int aStep = 599);
	
	// Return ADC integration time
	byte getATIME();
	
	// Get ADC integration steps
	unsigned int getASTEP();
	
	// Set ADC gain
	void setGain(AS7341L_GAIN gain = AS7341L_GAIN::GAIN_X256);
	
	// Return ADC gain
	AS7341L_GAIN getGain();

	// Enables interrupt pin functionality
	void enablePinInterupt();
	
	// Disables interrupt pin functionality
	void disablePinInterrupt();
	
	// Clears the interrupt flag
	void clearPinInterrupt();
	
	// Reads register
	byte readRegister(byte reg);
	
	// Writes register
	void writeRegister(byte reg, byte value);
	
	// Sets GPIO as input
	void setGpioPinInput();
	
	// Sets GPIO as output
	void setGpioPinOutput();
	
	// Reads GPIO pin value
	bool digitalRead();
	
	// Inverts GPIO pin output
	void invertGpioOutput(bool isInverted);
	
	// Writes GPIO pin
	void digitalWrite(byte value);
	
	// Read raw value of 415 nm channel
	unsigned int read415nm();
	
	// Read 445 nm channel raw value
	unsigned int read445nm();
	
	// Read raw value of 480 nm channel
	unsigned int read480nm();
	
	// Read raw value of 515 nm channel
	unsigned int read515nm();
	
	// Read raw value of 555 nm channel
	unsigned int read555nm();
	
	// Read raw value of 590 nm channel
	unsigned int read590nm();
	
	// Read raw value of 630 nm channel
	unsigned int read630nm();
	
	// Read raw value of 680 nm channel
	unsigned int read680nm();
	
	// Read raw value of clear channel
	unsigned int readClear();
	
	// Read raw value of NIR channel
	unsigned int readNIR();
	
	// Read basic count value of 445 nm channel
	float readBasicCount415nm();
	
	// Read basic count value of 445 nm channel
	float readBasicCount445nm();
	
	// Read basic count value of 480 nm channel
	float readBasicCount480nm();
	
	// Read basic count value of 515 nm channel
	float readBasicCount515nm();
	
	// Read basic count value of 555 nm channel
	float readBasicCount555nm();
	
	// Read basic count value of 590 nm channel
	float readBasicCount590nm();
	
	// Read basic count value of 630 nm channel
	float readBasicCount630nm();
	
	// Read basic count value of 680 nm channel
	float readBasicCount680nm();
	
	// Read basic count value of clear channel
	float readBasicCountClear();
	
	// Read basic count value of NIR channel
	float readBasicCountNIR();

	// Sets the low threshold value
	void setLowThreshold(unsigned int threshold);
	
	// Sets the high threshold value
	void setHighThreshold(unsigned int threshold);
	
	// Reads low threshold value
	unsigned int getLowThreshold();
	
	// Reads high threshold value;
	unsigned int getHighThreshold();
	
	// Sets APERS register which will dictate how many consecutive samples must be above/below thresholds to trigger an interrupt. Values allowed: 0 to 15.
	void setAPERS(byte value);
	
	// Gets APERS register value
	byte getAPERS();
	
	// Enable measurements by setting SP_EN bit
	void enableMeasurements();
	
	// Disables measurements by clearing SP_EN bit
	void disableMeasurements();
	
	// Returns true if AS7341L has SP_EN set
	bool isMeasurementEnabled();
		
	// Enable threshold interrupt generation
	void enableThresholdInterrupt();
	
	// Disable threshold interrupt generation
	void disableThresholdInterrupt();
	
	// Clear threshold interrupt for allowing new interrupts to be triggered
	void clearThresholdInterrupts();
	
	// Returns true if the channel value is lower than the low threshold value
	bool lowThresholdInterruptSet();
	
	// Returns true if the channel value is higher than the high threshold value
	bool highThresholdInterruptSet();
	
};

#endif // ! __SPARKFUN_AS7341L_LIBRARY__