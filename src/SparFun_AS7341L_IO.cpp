/*
  This is a library written for the AMS AS7341L 10-Channel Spectral Sensor Frontend
  SparkFun sells these at its website:
  https://www.sparkfun.com/products/17719

  Do you like this library? Help support open source hardware. Buy a board!

  Written by Ricardo Ramos  @ SparkFun Electronics, March 15th, 2021
  This file defines all I2C communication functions implementations used in the AS7341L sensor library.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "SparkFun_AS7341L_Constants.h"
#include "SparkFun_AS7341L_IO.h"

bool SparkFun_AS7341L_IO::begin(byte AS7341L_address, TwoWire& wirePort)
{
	_i2cPort = &wirePort;
	_address = AS7341L_address;
	return isConnected();
}

bool SparkFun_AS7341L_IO::isConnected()
{
	_i2cPort->beginTransmission(_address);
	if (_i2cPort->endTransmission() != 0)
		return (false);
	return (true); 
}

void SparkFun_AS7341L_IO::writeMultipleBytes(byte registerAddress, const byte* buffer, byte const packetLength)
{
	setBankConfiguration(registerAddress);
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(registerAddress);
	for (byte i = 0; i < packetLength; i++) 
		_i2cPort->write(buffer[i]);
	
	_i2cPort->endTransmission();
}

void SparkFun_AS7341L_IO::readMultipleBytes(byte registerAddress, byte* buffer, byte const packetLength)
{	
	setBankConfiguration(registerAddress);
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(registerAddress);
	_i2cPort->endTransmission();

	_i2cPort->requestFrom(_address, packetLength);
	for (byte i = 0; (i < packetLength) && _i2cPort->available(); i++)
		buffer[i] = _i2cPort->read();

}

byte SparkFun_AS7341L_IO::readSingleByte(byte registerAddress)
{	
	setBankConfiguration(registerAddress);
	byte result;
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(registerAddress);
	_i2cPort->endTransmission();
	_i2cPort->requestFrom(_address, 1U);
	result = _i2cPort->read();
	return result;
}

void SparkFun_AS7341L_IO::writeSingleByte(byte registerAddress, byte const value)
{	
	setBankConfiguration(registerAddress);
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(registerAddress);
	_i2cPort->write(value);
	_i2cPort->endTransmission();
}

void SparkFun_AS7341L_IO::setRegisterBit(byte registerAddress, byte const bitPosition)
{
	byte value = readSingleByte(registerAddress);
	value |= (1 << bitPosition);
	writeSingleByte(registerAddress, value);
}

void SparkFun_AS7341L_IO::clearRegisterBit(byte registerAddress, byte const bitPosition)
{
	byte value = readSingleByte(registerAddress);
	value &= ~(1 << bitPosition);
	writeSingleByte(registerAddress, value);
}

bool SparkFun_AS7341L_IO::isBitSet(byte registerAddress, byte const bitPosition)
{
	byte value = readSingleByte(registerAddress);
	byte mask = 1 << bitPosition;
	if ((value & mask) != 0)
		return true;
	else
		return false;
}

void SparkFun_AS7341L_IO::setBankConfiguration(byte regAddress)
{
	byte result;
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(REGISTER_CFG_0);
	_i2cPort->endTransmission();
	_i2cPort->requestFrom(_address, 1U);
	result = _i2cPort->read();
	
	byte value;
	
	if (regAddress >= 0x60 && regAddress <= 0x74)
		value |= (1 << 4);
	else
		value &= ~(1 << 4);

	_i2cPort->beginTransmission(_address);
	_i2cPort->write(REGISTER_CFG_0);
	_i2cPort->write(value);
	_i2cPort->endTransmission();
}