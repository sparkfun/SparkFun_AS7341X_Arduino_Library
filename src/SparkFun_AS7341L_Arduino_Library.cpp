/*
  This is a library written for the AMS AS7341L 10-Channel Spectral Sensor Frontend
  SparkFun sells these at its website:
  https://www.sparkfun.com/products/17719

  Do you like this library? Help support open source hardware. Buy a board!

  Written by Ricardo Ramos  @ SparkFun Electronics, March 15th, 2021
  This file defines all core functions implementations available in the AS7341L sensor library.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
#include "SparkFun_AS7341L_Constants.h"
#include "SparkFun_AS7341L_IO.h"
#include "SparkFun_AS7341L_Arduino_Library.h"

bool SparkFun_AS7341L::begin(byte AS7341L_address, TwoWire& wirePort)
{	
	// Reset error variable
	lastError = ERROR_NONE;
	
	// Initialize AS7341L I2C interface
	bool result = as7341_io.begin(AS7341L_address, wirePort);
	if (result == false)
	{
		lastError = ERROR_AS7341L_I2C_COMM_ERROR;
		return false;
	}
		
	// Are we talking to the correct chip ? 
	result = isConnected();
		
	// Wake AS7341L up
	enable_AS7341L();
	
	// Set default ADC integration steps registers (599)
	setASTEP();
	
	// Set default ADC integration time register (29)
	setATIME();
	
	// Set ADC gain to default (x256)
	setGain();

	// Have AS7341L control both WHITE and IR leds cathodes
	as7341_io.setRegisterBit(REGISTER_CONFIG, 3);
	
	// Initialize PCA9536 I2C interface
	result = pca9536_io.begin(wirePort);
	if (result == false)
	{
		lastError = ERROR_PCA9536_I2C_COMM_ERROR;
		return false;
	}
	
	// Configure GPIO0, GPIO1 and GPIO2 as outputs 
	pca9536_io.pinMode(POWER_LED_GPIO, OUTPUT);
	pca9536_io.pinMode(WHITE_LED_GPIO, OUTPUT);
	pca9536_io.pinMode(IR_LED_GPIO, OUTPUT);
	
	// Turn off the WHITE and IR leds and turn on POWER led .
	enablePowerLed();
	disableWhiteLed();
	disableIRLed();

	return true;
}

bool SparkFun_AS7341L::isConnected()
{
	bool asConnected = as7341_io.isConnected();
	if (!asConnected)
		return false;
	// The returned byte shifted right by two must be 0x09
	byte device = as7341_io.readSingleByte(REGISTER_ID);
	device = device >> 2;
	return ((device == 0x09) && (pca9536_io.isConnected()));
}

void SparkFun_AS7341L::enablePowerLed()
{
	pca9536_io.digitalWrite(POWER_LED_GPIO, LOW);
}

void SparkFun_AS7341L::disablePowerLed()
{
	pca9536_io.digitalWrite(POWER_LED_GPIO, HIGH);
}

void SparkFun_AS7341L::enableWhiteLed()
{
	whiteLedPowered = true;
	if (!IRLedPowered)
		as7341_io.setRegisterBit(REGISTER_LED, 7);
	pca9536_io.write(WHITE_LED_GPIO, LOW);
}

void SparkFun_AS7341L::disableWhiteLed()
{
	whiteLedPowered = false;
	if (!IRLedPowered)
		as7341_io.clearRegisterBit(REGISTER_LED, 7);
	pca9536_io.write(WHITE_LED_GPIO, HIGH);
}

void SparkFun_AS7341L::enableIRLed()
{
	IRLedPowered = true;
	if (!whiteLedPowered)
		as7341_io.setRegisterBit(REGISTER_LED, 7);
	pca9536_io.write(IR_LED_GPIO, LOW);
}

void SparkFun_AS7341L::disableIRLed()
{
	IRLedPowered = false;
	if (!whiteLedPowered)
		as7341_io.clearRegisterBit(REGISTER_LED, 7);
	pca9536_io.write(IR_LED_GPIO, HIGH);
}

void SparkFun_AS7341L::enable_AS7341L()
{
	as7341_io.setRegisterBit(REGISTER_ENABLE, 0);
}

void SparkFun_AS7341L::disable_AS7341L()
{
	as7341_io.clearRegisterBit(REGISTER_ENABLE, 0);
}

byte SparkFun_AS7341L::getLastError()
{
	return lastError;
}

void SparkFun_AS7341L::setLedDrive(unsigned int current)
{
	// Do not allow invalid values to be set
	if(current < 4)
		current = 4;
	if (current > 258)
		current = 258;
	
	// Calculate register value to program
	byte registerValue = byte((current - 4) >> 1);
	// Read current REGISTER_LED value
	byte currentValue = as7341_io.readSingleByte(REGISTER_LED);
	// Clear bits 6:0
	currentValue &= 0x80;
	// Update bits 6:0 accordingly
	currentValue |= registerValue;
	// Write register back
	as7341_io.writeSingleByte(REGISTER_LED, registerValue);
}

unsigned int SparkFun_AS7341L::getLedDrive()
{
	byte currentRegister = as7341_io.readSingleByte(REGISTER_LED);
	currentRegister &= 0x7f;
	return (currentRegister << 1) + 4;
}

void SparkFun_AS7341L::setATIME(byte aTime /* = 29 */)
{
	as7341_io.writeSingleByte(REGISTER_ATIME, aTime);
}

void SparkFun_AS7341L::setASTEP(unsigned int aStep /* = 599 */)
{
	byte temp = byte(aStep >> 8);
	as7341_io.writeSingleByte(REGISTER_ASTEP_H, temp);
	temp = byte(aStep &= 0xff);
	as7341_io.writeSingleByte(REGISTER_ASTEP_L, temp);
}

byte SparkFun_AS7341L::getATIME()
{
	return as7341_io.readSingleByte(REGISTER_ATIME);
}

unsigned int SparkFun_AS7341L::getASTEP()
{
	unsigned int result = as7341_io.readSingleByte(REGISTER_ASTEP_H);
	result = result << 8;
	result |= as7341_io.readSingleByte(REGISTER_ASTEP_L);
	return result;
}

void SparkFun_AS7341L::setGain(AS7341L_GAIN gain)
{
	byte value;
	
	switch (gain)
	{
	case AS7341L_GAIN::GAIN_HALF:
		value = 0;
		break;
		
	case AS7341L_GAIN::GAIN_X1:
		value = 1;
		break;
		
	case AS7341L_GAIN::GAIN_X2:
		value = 2;
		break;
		
	case AS7341L_GAIN::GAIN_X4:
		value = 3;
		break;
		
	case AS7341L_GAIN::GAIN_X8:
		value = 4;
		break;
		
	case AS7341L_GAIN::GAIN_X16:
		value = 5;
		break;
		
	case AS7341L_GAIN::GAIN_X32:
		value = 6;
		break;
		
	case AS7341L_GAIN::GAIN_X64:
		value = 7;
		break;
		
	case AS7341L_GAIN::GAIN_X128:
		value = 8;
		break;
		
	case AS7341L_GAIN::GAIN_X256:
		value = 9;
		break;
		
	case AS7341L_GAIN::GAIN_X512:
		value = 10;
		break;
		
	case AS7341L_GAIN::GAIN_INVALID:
	default:
		return;
		
	}
	
	as7341_io.writeSingleByte(REGISTER_CFG_1, value);
}

AS7341L_GAIN SparkFun_AS7341L::getGain()
{
	byte value = as7341_io.readSingleByte(REGISTER_CFG_1);
	value &= 0x1f;
	AS7341L_GAIN returnValue;
	
	switch (value)
	{
	case 0:
		returnValue = AS7341L_GAIN::GAIN_HALF;
		break;
		
	case 1:
		returnValue = AS7341L_GAIN::GAIN_X1;
		break;
		
	case 2:
		returnValue = AS7341L_GAIN::GAIN_X2;
		break;
		
	case 3:
		returnValue = AS7341L_GAIN::GAIN_X4;
		break;
		
	case 4:
		returnValue = AS7341L_GAIN::GAIN_X8;
		break;
		
	case 5:
		returnValue = AS7341L_GAIN::GAIN_X16;
		break;
		
	case 6:
		returnValue = AS7341L_GAIN::GAIN_X32;
		break;
		
	case 7:
		returnValue = AS7341L_GAIN::GAIN_X64;
		break;
		
	case 8:
		returnValue = AS7341L_GAIN::GAIN_X128;
		break;
		
	case 9:
		returnValue = AS7341L_GAIN::GAIN_X256;
		break;
		
	case 10:
		returnValue = AS7341L_GAIN::GAIN_X512;
		break;
		
	default:
		returnValue = AS7341L_GAIN::GAIN_INVALID;
		break;
	}
	
	return returnValue;
}

bool SparkFun_AS7341L::readAllChannels(unsigned int* channelData)
{
	lastError = ERROR_NONE;
	
	byte channelDataLength = sizeof(channelData) / sizeof(channelData[0]);
	setMuxLo();
	delay(5);
	as7341_io.setRegisterBit(REGISTER_ENABLE, 1);
	
	unsigned int start = millis();
	unsigned int delta = 0;
	
	do
	{
		delta = millis();
		if (delta - start > 5000)
		{
			lastError = ERROR_AS7341L_MEASUREMENT_TIMEOUT;
			return false;
		}
	} while (!as7341_io.isBitSet(REGISTER_STATUS_2, 6));
	
	byte buffer[12];
		
	as7341_io.readMultipleBytes(REGISTER_CH0_DATA_L, buffer, 12);
	
	for (int i = 0; i < 6; i++)
		channelData[i] = buffer[2*i + 1] << 8 | buffer[2*i];
	
	setMuxHi();
	delay(5);
	as7341_io.setRegisterBit(REGISTER_ENABLE, 1);
	start = millis();
	delta = 0;
	do
	{
		delta = millis();
		if (delta - start > 5000)
		{
			lastError = ERROR_AS7341L_MEASUREMENT_TIMEOUT;
			return false;
		}
	} while (!as7341_io.isBitSet(REGISTER_STATUS_2, 6));
	
	as7341_io.readMultipleBytes(REGISTER_CH0_DATA_L, buffer, 12);
	
	for (int i = 0; i < 6; i++)
		channelData[i + 6] = buffer[2*i + 1] << 8 | buffer[2*i];
	
	return true;
}

void SparkFun_AS7341L::setMuxLo()
{
	// According to AMS application note V1.1
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_9, 0x10);
	as7341_io.writeSingleByte(REGISTER_INTENAB, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_6, 0x10);
	as7341_io.writeSingleByte(0x0, 0x30);
	as7341_io.writeSingleByte(0x01, 0x01);
	as7341_io.writeSingleByte(0x02, 0x00);
	as7341_io.writeSingleByte(0x03, 0x00);
	as7341_io.writeSingleByte(0x04, 0x00);
	as7341_io.writeSingleByte(0x05, 0x42);
	as7341_io.writeSingleByte(0x06, 0x00);
	as7341_io.writeSingleByte(0x07, 0x00);
	as7341_io.writeSingleByte(0x08, 0x50);
	as7341_io.writeSingleByte(0x09, 0x00);
	as7341_io.writeSingleByte(0x0a, 0x00);
	as7341_io.writeSingleByte(0x0b, 0x00);
	as7341_io.writeSingleByte(0x0c, 0x20);
	as7341_io.writeSingleByte(0x0d, 0x04);
	as7341_io.writeSingleByte(0x0e, 0x00);
	as7341_io.writeSingleByte(0x0f, 0x30);
	as7341_io.writeSingleByte(0x10, 0x01);
	as7341_io.writeSingleByte(0x11, 0x50);
	as7341_io.writeSingleByte(0x12, 0x00);
	as7341_io.writeSingleByte(0x13, 0x06);
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x11);
}

void SparkFun_AS7341L::setMuxHi()
{
	// According to AMS application note V1.1
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_9, 0x10);
	as7341_io.writeSingleByte(REGISTER_INTENAB, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_6, 0x10);
	as7341_io.writeSingleByte(0x00, 0x00);
	as7341_io.writeSingleByte(0x01, 0x00);
	as7341_io.writeSingleByte(0x02, 0x00);
	as7341_io.writeSingleByte(0x03, 0x40);
	as7341_io.writeSingleByte(0x04, 0x02);
	as7341_io.writeSingleByte(0x05, 0x00);
	as7341_io.writeSingleByte(0x06, 0x10);
	as7341_io.writeSingleByte(0x07, 0x03);
	as7341_io.writeSingleByte(0x08, 0x50);
	as7341_io.writeSingleByte(0x09, 0x10);
	as7341_io.writeSingleByte(0x0a, 0x03);
	as7341_io.writeSingleByte(0x0b, 0x00);
	as7341_io.writeSingleByte(0x0c, 0x00);
	as7341_io.writeSingleByte(0x0d, 0x00);
	as7341_io.writeSingleByte(0x0e, 0x24);
	as7341_io.writeSingleByte(0x0f, 0x00);
	as7341_io.writeSingleByte(0x10, 0x00);
	as7341_io.writeSingleByte(0x11, 0x50);
	as7341_io.writeSingleByte(0x12, 0x00);
	as7341_io.writeSingleByte(0x13, 0x06);
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x11);
}

bool SparkFun_AS7341L::readAllChannelsBasicCounts(float* channelDataBasicCounts)
{
	lastError = ERROR_NONE;
	
	unsigned int rawChannelData[12];
	bool result = readAllChannels(rawChannelData);
	
	if (result == false)
		return false;
	
	unsigned int aTime = (unsigned int) getATIME();
	unsigned int aStep = getASTEP();
	unsigned int tint = (aTime + 1) * (aStep * 1) * 2.78 / 1000;
		
	byte value = as7341_io.readSingleByte(REGISTER_CFG_1);
	value &= 0x1f;

	float gain;
	if (value == 0)
	{
		gain = 0.5f;
	}
	else
	{
		gain = pow(2, (value - 1));
	}
	
	for (int i = 0; i < 12; i++)
	{
		channelDataBasicCounts[i] = float(rawChannelData[i]) / (gain * tint);
	}
	
	return true;
}

void SparkFun_AS7341L::enablePinInterupt()
{
	as7341_io.setRegisterBit(REGISTER_INTENAB, 0);
}

void SparkFun_AS7341L::disablePinInterrupt()
{
	as7341_io.clearRegisterBit(REGISTER_INTENAB, 0);
}

void SparkFun_AS7341L::clearPinInterrupt()
{
	as7341_io.writeSingleByte(REGISTER_STATUS, 0xff);
}

byte SparkFun_AS7341L::readRegister(byte reg)
{
	return as7341_io.readSingleByte(reg);
}

void SparkFun_AS7341L::writeRegister(byte reg, byte value)
{
	as7341_io.writeSingleByte(reg, value);
}

void SparkFun_AS7341L::setGpioPinInput()
{
	// Disable GPIO as output driver
	as7341_io.setRegisterBit(REGISTER_GPIO_2, 1);
	// Enable GPIO as input
	as7341_io.setRegisterBit(REGISTER_GPIO_2, 2);
}

void SparkFun_AS7341L::setGpioPinOutput()
{
	// Disable GPIO input
	as7341_io.clearRegisterBit(REGISTER_GPIO_2, 2);
}

bool SparkFun_AS7341L::digitalRead()
{
	return as7341_io.isBitSet(REGISTER_GPIO_2, 0);
}

void SparkFun_AS7341L::invertGpioOutput(bool isInverted)
{
	if (isInverted)
		as7341_io.setRegisterBit(REGISTER_GPIO_2, 3);
	else
		as7341_io.clearRegisterBit(REGISTER_GPIO_2, 3);
}

void SparkFun_AS7341L::digitalWrite(byte value)
{
	// If value is 0, let go the driver
	if(value == 0)
		as7341_io.clearRegisterBit(REGISTER_GPIO_2, 1);
	else
		as7341_io.setRegisterBit(REGISTER_GPIO_2, 1);
}

uint16_t SparkFun_AS7341L::readSingleChannelValue()
{
	as7341_io.setRegisterBit(REGISTER_ENABLE, 1);
	
	lastError = ERROR_NONE;

	uint16_t start = millis();
	uint16_t delta = 0;
	
	do
	{
		delta = millis();
		if (delta - start > 5000)
		{
			lastError = ERROR_AS7341L_MEASUREMENT_TIMEOUT;
			return 0;
		}
	} while (!as7341_io.isBitSet(REGISTER_STATUS_2, 6));
	
	byte buffer[2] = { 0 };
	
	as7341_io.readMultipleBytes(REGISTER_CH0_DATA_L, buffer, 2);
	uint16_t result = buffer[1] << 8;
	result += buffer[0];
	
	return result;
}

unsigned int SparkFun_AS7341L::read415nm()
{	
	// F1 -> ADC0
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_9, 0x10);
	as7341_io.writeSingleByte(REGISTER_INTENAB, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_6, 0x10);
	as7341_io.writeSingleByte(0x0, 0x00);
	as7341_io.writeSingleByte(0x01, 0x01);
	as7341_io.writeSingleByte(0x02, 0x00);
	as7341_io.writeSingleByte(0x03, 0x00);
	as7341_io.writeSingleByte(0x04, 0x00);
	as7341_io.writeSingleByte(0x05, 0x00);
	as7341_io.writeSingleByte(0x06, 0x00);
	as7341_io.writeSingleByte(0x07, 0x00);
	as7341_io.writeSingleByte(0x08, 0x00);
	as7341_io.writeSingleByte(0x09, 0x00);
	as7341_io.writeSingleByte(0x0a, 0x00);
	as7341_io.writeSingleByte(0x0b, 0x00);
	as7341_io.writeSingleByte(0x0c, 0x00);
	as7341_io.writeSingleByte(0x0d, 0x00);
	as7341_io.writeSingleByte(0x0e, 0x00);
	as7341_io.writeSingleByte(0x0f, 0x00);
	as7341_io.writeSingleByte(0x10, 0x01);
	as7341_io.writeSingleByte(0x11, 0x00);
	as7341_io.writeSingleByte(0x12, 0x00);
	as7341_io.writeSingleByte(0x13, 0x00);
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x11);
	
	return (unsigned int)readSingleChannelValue();
}

unsigned int SparkFun_AS7341L::read445nm()
{
	// F2 -> ADC0
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_9, 0x10);
	as7341_io.writeSingleByte(REGISTER_INTENAB, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_6, 0x10);
	as7341_io.writeSingleByte(0x0, 0x0);
	as7341_io.writeSingleByte(0x01, 0x0);
	as7341_io.writeSingleByte(0x02, 0x0);
	as7341_io.writeSingleByte(0x03, 0x0);
	as7341_io.writeSingleByte(0x04, 0x0);
	as7341_io.writeSingleByte(0x05, 0x01);
	as7341_io.writeSingleByte(0x06, 0x0);
	as7341_io.writeSingleByte(0x07, 0x0);
	as7341_io.writeSingleByte(0x08, 0x0);
	as7341_io.writeSingleByte(0x09, 0x0);
	as7341_io.writeSingleByte(0x0a, 0x0);
	as7341_io.writeSingleByte(0x0b, 0x0);
	as7341_io.writeSingleByte(0x0c, 0x10);
	as7341_io.writeSingleByte(0x0d, 0x0);
	as7341_io.writeSingleByte(0x0e, 0x0);
	as7341_io.writeSingleByte(0x0f, 0x0);
	as7341_io.writeSingleByte(0x10, 0x0);
	as7341_io.writeSingleByte(0x11, 0x0);
	as7341_io.writeSingleByte(0x12, 0x0);
	as7341_io.writeSingleByte(0x13, 0x0);
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x11);
		
	return (unsigned int)readSingleChannelValue();
}

unsigned int SparkFun_AS7341L::read480nm()
{
	// F3 -> ADC0
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_9, 0x10);
	as7341_io.writeSingleByte(REGISTER_INTENAB, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_6, 0x10);
	as7341_io.writeSingleByte(0x0, 0x10);
	as7341_io.writeSingleByte(0x01, 0x0);
	as7341_io.writeSingleByte(0x02, 0x0);
	as7341_io.writeSingleByte(0x03, 0x0);
	as7341_io.writeSingleByte(0x04, 0x0);
	as7341_io.writeSingleByte(0x05, 0x0);
	as7341_io.writeSingleByte(0x06, 0x0);
	as7341_io.writeSingleByte(0x07, 0x0);
	as7341_io.writeSingleByte(0x08, 0x0);
	as7341_io.writeSingleByte(0x09, 0x0);
	as7341_io.writeSingleByte(0x0a, 0x0);
	as7341_io.writeSingleByte(0x0b, 0x0);
	as7341_io.writeSingleByte(0x0c, 0x0);
	as7341_io.writeSingleByte(0x0d, 0x0);
	as7341_io.writeSingleByte(0x0e, 0x0);
	as7341_io.writeSingleByte(0x0f, 0x10);
	as7341_io.writeSingleByte(0x10, 0x0);
	as7341_io.writeSingleByte(0x11, 0x0);
	as7341_io.writeSingleByte(0x12, 0x0);
	as7341_io.writeSingleByte(0x13, 0x0);
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x11);
		
	return (unsigned int)readSingleChannelValue();
}

unsigned int SparkFun_AS7341L::read515nm()
{
	// F4 -> ADC0
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_9, 0x10);
	as7341_io.writeSingleByte(REGISTER_INTENAB, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_6, 0x10);
	as7341_io.writeSingleByte(0x0, 0x0);
	as7341_io.writeSingleByte(0x01, 0x0);
	as7341_io.writeSingleByte(0x02, 0x0);
	as7341_io.writeSingleByte(0x03, 0x0);
	as7341_io.writeSingleByte(0x04, 0x0);
	as7341_io.writeSingleByte(0x05, 0x10);
	as7341_io.writeSingleByte(0x06, 0x0);
	as7341_io.writeSingleByte(0x07, 0x0);
	as7341_io.writeSingleByte(0x08, 0x0);
	as7341_io.writeSingleByte(0x09, 0x0);
	as7341_io.writeSingleByte(0x0a, 0x0);
	as7341_io.writeSingleByte(0x0b, 0x0);
	as7341_io.writeSingleByte(0x0c, 0x0);
	as7341_io.writeSingleByte(0x0d, 0x01);
	as7341_io.writeSingleByte(0x0e, 0x0);
	as7341_io.writeSingleByte(0x0f, 0x0);
	as7341_io.writeSingleByte(0x10, 0x0);
	as7341_io.writeSingleByte(0x11, 0x0);
	as7341_io.writeSingleByte(0x12, 0x0);
	as7341_io.writeSingleByte(0x13, 0x0);
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x11);
		
	return (unsigned int)readSingleChannelValue();
}

unsigned int SparkFun_AS7341L::read555nm()
{
	// F5 -> ADC0
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_9, 0x10);
	as7341_io.writeSingleByte(REGISTER_INTENAB, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_6, 0x10);
	as7341_io.writeSingleByte(0x0, 0x0);
	as7341_io.writeSingleByte(0x01, 0x0);
	as7341_io.writeSingleByte(0x02, 0x0);
	as7341_io.writeSingleByte(0x03, 0x0);
	as7341_io.writeSingleByte(0x04, 0x0);
	as7341_io.writeSingleByte(0x05, 0x0);
	as7341_io.writeSingleByte(0x06, 0x10);
	as7341_io.writeSingleByte(0x07, 0x0);
	as7341_io.writeSingleByte(0x08, 0x0);
	as7341_io.writeSingleByte(0x09, 0x10);
	as7341_io.writeSingleByte(0x0a, 0x0);
	as7341_io.writeSingleByte(0x0b, 0x0);
	as7341_io.writeSingleByte(0x0c, 0x0);
	as7341_io.writeSingleByte(0x0d, 0x0);
	as7341_io.writeSingleByte(0x0e, 0x0);
	as7341_io.writeSingleByte(0x0f, 0x0);
	as7341_io.writeSingleByte(0x10, 0x0);
	as7341_io.writeSingleByte(0x11, 0x0);
	as7341_io.writeSingleByte(0x12, 0x0);
	as7341_io.writeSingleByte(0x13, 0x0);
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x11);
		
	return (unsigned int)readSingleChannelValue();
}

unsigned int SparkFun_AS7341L::read590nm()
{
	// F6 -> ADC0
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_9, 0x10);
	as7341_io.writeSingleByte(REGISTER_INTENAB, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_6, 0x10);
	as7341_io.writeSingleByte(0x0, 0x0);
	as7341_io.writeSingleByte(0x01, 0x0);
	as7341_io.writeSingleByte(0x02, 0x0);
	as7341_io.writeSingleByte(0x03, 0x0);
	as7341_io.writeSingleByte(0x04, 0x01);
	as7341_io.writeSingleByte(0x05, 0x0);
	as7341_io.writeSingleByte(0x06, 0x0);
	as7341_io.writeSingleByte(0x07, 0x0);
	as7341_io.writeSingleByte(0x08, 0x0);
	as7341_io.writeSingleByte(0x09, 0x0);
	as7341_io.writeSingleByte(0x0a, 0x0);
	as7341_io.writeSingleByte(0x0b, 0x0);
	as7341_io.writeSingleByte(0x0c, 0x0);
	as7341_io.writeSingleByte(0x0d, 0x0);
	as7341_io.writeSingleByte(0x0e, 0x10);
	as7341_io.writeSingleByte(0x0f, 0x0);
	as7341_io.writeSingleByte(0x10, 0x0);
	as7341_io.writeSingleByte(0x11, 0x0);
	as7341_io.writeSingleByte(0x12, 0x0);
	as7341_io.writeSingleByte(0x13, 0x0);
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x11);
		
	return (unsigned int)readSingleChannelValue();
}

unsigned int SparkFun_AS7341L::read630nm()
{
	// F7 -> ADC0
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_9, 0x10);
	as7341_io.writeSingleByte(REGISTER_INTENAB, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_6, 0x10);
	as7341_io.writeSingleByte(0x0, 0x0);
	as7341_io.writeSingleByte(0x01, 0x0);
	as7341_io.writeSingleByte(0x02, 0x0);
	as7341_io.writeSingleByte(0x03, 0x0);
	as7341_io.writeSingleByte(0x04, 0x0);
	as7341_io.writeSingleByte(0x05, 0x0);
	as7341_io.writeSingleByte(0x06, 0x0);
	as7341_io.writeSingleByte(0x07, 0x01);
	as7341_io.writeSingleByte(0x08, 0x0);
	as7341_io.writeSingleByte(0x09, 0x0);
	as7341_io.writeSingleByte(0x0a, 0x01);
	as7341_io.writeSingleByte(0x0b, 0x0);
	as7341_io.writeSingleByte(0x0c, 0x0);
	as7341_io.writeSingleByte(0x0d, 0x0);
	as7341_io.writeSingleByte(0x0e, 0x0);
	as7341_io.writeSingleByte(0x0f, 0x0);
	as7341_io.writeSingleByte(0x10, 0x0);
	as7341_io.writeSingleByte(0x11, 0x0);
	as7341_io.writeSingleByte(0x12, 0x0);
	as7341_io.writeSingleByte(0x13, 0x0);
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x11);
		
	return (unsigned int)readSingleChannelValue();
}

unsigned int SparkFun_AS7341L::read680nm()
{
	// F8 -> ADC0
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_9, 0x10);
	as7341_io.writeSingleByte(REGISTER_INTENAB, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_6, 0x10);
	as7341_io.writeSingleByte(0x0, 0x0);
	as7341_io.writeSingleByte(0x01, 0x0);
	as7341_io.writeSingleByte(0x02, 0x0);
	as7341_io.writeSingleByte(0x03, 0x10);
	as7341_io.writeSingleByte(0x04, 0x0);
	as7341_io.writeSingleByte(0x05, 0x0);
	as7341_io.writeSingleByte(0x06, 0x0);
	as7341_io.writeSingleByte(0x07, 0x0);
	as7341_io.writeSingleByte(0x08, 0x0);
	as7341_io.writeSingleByte(0x09, 0x0);
	as7341_io.writeSingleByte(0x0a, 0x0);
	as7341_io.writeSingleByte(0x0b, 0x0);
	as7341_io.writeSingleByte(0x0c, 0x0);
	as7341_io.writeSingleByte(0x0d, 0x0);
	as7341_io.writeSingleByte(0x0e, 0x01);
	as7341_io.writeSingleByte(0x0f, 0x0);
	as7341_io.writeSingleByte(0x10, 0x0);
	as7341_io.writeSingleByte(0x11, 0x0);
	as7341_io.writeSingleByte(0x12, 0x0);
	as7341_io.writeSingleByte(0x13, 0x0);
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x11);
		
	return (unsigned int)readSingleChannelValue();
}

unsigned int SparkFun_AS7341L::readClear()
{
	//	Clear -> ADC0
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_9, 0x10);
	as7341_io.writeSingleByte(REGISTER_INTENAB, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_6, 0x10);
	as7341_io.writeSingleByte(0x0, 0x0);
	as7341_io.writeSingleByte(0x01, 0x0);
	as7341_io.writeSingleByte(0x02, 0x0);
	as7341_io.writeSingleByte(0x03, 0x0);
	as7341_io.writeSingleByte(0x04, 0x0);
	as7341_io.writeSingleByte(0x05, 0x0);
	as7341_io.writeSingleByte(0x06, 0x0);
	as7341_io.writeSingleByte(0x07, 0x0);
	as7341_io.writeSingleByte(0x08, 0x10);
	as7341_io.writeSingleByte(0x09, 0x0);
	as7341_io.writeSingleByte(0x0a, 0x0);
	as7341_io.writeSingleByte(0x0b, 0x0);
	as7341_io.writeSingleByte(0x0c, 0x0);
	as7341_io.writeSingleByte(0x0d, 0x0);
	as7341_io.writeSingleByte(0x0e, 0x0);
	as7341_io.writeSingleByte(0x0f, 0x0);
	as7341_io.writeSingleByte(0x10, 0x0);
	as7341_io.writeSingleByte(0x11, 0x10);
	as7341_io.writeSingleByte(0x12, 0x0);
	as7341_io.writeSingleByte(0x13, 0x0);
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x11);
		
	return (unsigned int)readSingleChannelValue();
}

unsigned int SparkFun_AS7341L::readNIR()
{
	//	NIR -> ADC0
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_9, 0x10);
	as7341_io.writeSingleByte(REGISTER_INTENAB, 0x01);
	as7341_io.writeSingleByte(REGISTER_CFG_6, 0x10);
	as7341_io.writeSingleByte(0x0, 0x0);
	as7341_io.writeSingleByte(0x01, 0x0);
	as7341_io.writeSingleByte(0x02, 0x0);
	as7341_io.writeSingleByte(0x03, 0x0);
	as7341_io.writeSingleByte(0x04, 0x0);
	as7341_io.writeSingleByte(0x05, 0x0);
	as7341_io.writeSingleByte(0x06, 0x0);
	as7341_io.writeSingleByte(0x07, 0x0);
	as7341_io.writeSingleByte(0x08, 0x0);
	as7341_io.writeSingleByte(0x09, 0x0);
	as7341_io.writeSingleByte(0x0a, 0x0);
	as7341_io.writeSingleByte(0x0b, 0x0);
	as7341_io.writeSingleByte(0x0c, 0x0);
	as7341_io.writeSingleByte(0x0d, 0x0);
	as7341_io.writeSingleByte(0x0e, 0x0);
	as7341_io.writeSingleByte(0x0f, 0x0);
	as7341_io.writeSingleByte(0x10, 0x0);
	as7341_io.writeSingleByte(0x11, 0x0);
	as7341_io.writeSingleByte(0x12, 0x0);
	as7341_io.writeSingleByte(0x13, 0x01);
	as7341_io.writeSingleByte(REGISTER_ENABLE, 0x11);
		
	return (unsigned int)readSingleChannelValue();
}

float SparkFun_AS7341L::readSingleBasicCountChannelValue(uint16_t raw)
{
	uint16_t aTime = (uint16_t) getATIME();
	uint16_t aStep = getASTEP();
	uint16_t tint = (aTime + 1) * (aStep * 1) * 2.78 / 1000;
		
	byte value = as7341_io.readSingleByte(REGISTER_CFG_1);
	value &= 0x1f;

	float gain;
	if (value == 0)
	{
		gain = 0.5f;
	}
	else
	{
		gain = pow(2, (value - 1));
	}
	
	return (float(raw) / (gain * tint));
}

float SparkFun_AS7341L::readBasicCount415nm()
{
	return readSingleBasicCountChannelValue(read415nm());
}

float SparkFun_AS7341L::readBasicCount445nm()
{
	return readSingleBasicCountChannelValue(read445nm());
}

float SparkFun_AS7341L::readBasicCount480nm()
{
	return readSingleBasicCountChannelValue(read480nm());
}

float SparkFun_AS7341L::readBasicCount515nm()
{
	return readSingleBasicCountChannelValue(read515nm());
}

float SparkFun_AS7341L::readBasicCount555nm()
{
	return readSingleBasicCountChannelValue(read555nm());
}

float SparkFun_AS7341L::readBasicCount590nm()
{
	return readSingleBasicCountChannelValue(read590nm());
}

float SparkFun_AS7341L::readBasicCount630nm()
{
	return readSingleBasicCountChannelValue(read630nm());
}

float SparkFun_AS7341L::readBasicCount680nm()
{
	return readSingleBasicCountChannelValue(read680nm());
}

float SparkFun_AS7341L::readBasicCountClear()
{
	return readSingleBasicCountChannelValue(readClear());
}

float SparkFun_AS7341L::readBasicCountNIR()
{
	return readSingleBasicCountChannelValue(readNIR());
}

void SparkFun_AS7341L::setLowThreshold(unsigned int threshold)
{
	byte low = threshold & 0xff;
	byte high = threshold >> 8;
	
	as7341_io.writeSingleByte(REGISTER_SP_TH_L_LSB, low);
	as7341_io.writeSingleByte(REGISTER_SP_TH_L_MSB, high);
}

void SparkFun_AS7341L::setHighThreshold(unsigned int threshold)
{
	byte low = threshold & 0xff;
	byte high = threshold >> 8;
	
	as7341_io.writeSingleByte(REGISTER_SP_TH_H_LSB, low);
	as7341_io.writeSingleByte(REGISTER_SP_TH_H_MSB, high);
}

unsigned int SparkFun_AS7341L::getLowThreshold()
{
	byte low = as7341_io.readSingleByte(REGISTER_SP_TH_L_LSB);
	byte high = as7341_io.readSingleByte(REGISTER_SP_TH_L_MSB);
	return ((high << 8) | low);
}

unsigned int SparkFun_AS7341L::getHighThreshold()
{
	byte low = as7341_io.readSingleByte(REGISTER_SP_TH_H_LSB);
	byte high = as7341_io.readSingleByte(REGISTER_SP_TH_H_MSB);
	return ((high << 8) | low);
}

void SparkFun_AS7341L::setAPERS(byte value)
{
	// Register value must be less than 15
	value &= 0x0f;
	
	// Get current APERS value and change bits 3..0 only
	byte currentAPERS = getAPERS();
	currentAPERS &= 0x0f;
	currentAPERS |= value;
	// Write value back
	as7341_io.writeSingleByte(REGISTER_PERS, currentAPERS);
}

byte SparkFun_AS7341L::getAPERS()
{
	return as7341_io.readSingleByte(REGISTER_PERS);
}

void SparkFun_AS7341L::enableMeasurements()
{
	as7341_io.setRegisterBit(REGISTER_STATUS, 1);
}

void SparkFun_AS7341L::disableMeasurements()
{
	as7341_io.clearRegisterBit(REGISTER_STATUS, 1);
}

bool SparkFun_AS7341L::isMeasurementEnabled()
{
	return as7341_io.isBitSet(REGISTER_STATUS, 1);
}

void SparkFun_AS7341L::clearThresholdInterrupts()
{
	as7341_io.writeSingleByte(REGISTER_STATUS, 0xff);
}

void SparkFun_AS7341L::enableThresholdInterrupt()
{
	as7341_io.writeSingleByte(REGISTER_CFG_12, 0);
	delay(10);
	as7341_io.setRegisterBit(REGISTER_INTENAB, 3);
}

void SparkFun_AS7341L::disableThresholdInterrupt()
{
	as7341_io.clearRegisterBit(REGISTER_INTENAB, 3);
}

bool SparkFun_AS7341L::lowThresholdInterruptSet()
{
	bool lowSet = as7341_io.isBitSet(REGISTER_STATUS_3, 4);
	bool highSet = as7341_io.isBitSet(REGISTER_STATUS_3, 5);
	if (lowSet)
	{
		return (!highSet);
	}
	else
	{
		return false;
	}
}

bool SparkFun_AS7341L::highThresholdInterruptSet()
{
	bool lowSet = as7341_io.isBitSet(REGISTER_STATUS_3, 4);
	bool highSet = as7341_io.isBitSet(REGISTER_STATUS_3, 5);
	if (highSet)
	{
		return (!lowSet);
	}
	else
	{
		return false;
	}
}
