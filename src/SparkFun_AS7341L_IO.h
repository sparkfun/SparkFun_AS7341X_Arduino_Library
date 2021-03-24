/*
  This is a library written for the AMS AS7341L 10-Channel Spectral Sensor Frontend
  SparkFun sells these at its website:
  https://www.sparkfun.com/products/17719

  Do you like this library? Help support open source hardware. Buy a board!

  Written by Ricardo Ramos  @ SparkFun Electronics, March 15th, 2021
  This file declares all I2C communication functions used in the AS7341L sensor library.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __SPARKFUN_AS7341L_IO__
#define __SPARKFUN_AS7341L_IO__

#include <Arduino.h>
#include <Wire.h>

class SparkFun_AS7341L_IO
{
private:
	TwoWire* _i2cPort;
	byte _address;
	
	void setBankConfiguration(byte regAddress);
	
public:
	// Default constructor
	SparkFun_AS7341L_IO() {}
	// Starts two wire interface.
	bool begin(byte AS7341L_address = DEFAULT_AS7341L_ADDR, TwoWire& wirePort = Wire);

	// Returns true if we get a reply from the I2C device.
	bool isConnected();

	// Read a single byte from a register.
	byte readSingleByte(byte registerAddress);

	// Writes a single byte into a register.
	void writeSingleByte(byte registerAddress, byte value);

	// Reads multiple bytes from a register into buffer byte array.
	void readMultipleBytes(byte registerAddress, byte* buffer, byte packetLength);

	// Writes multiple bytes to register from buffer byte array.
	void writeMultipleBytes(byte registerAddress, const byte* buffer, byte packetLength);

	// Sets a single bit in a specific register. Bit position ranges from 0 (lsb) to 7 (msb).
	void setRegisterBit(byte registerAddress, byte bitPosition);

	// Clears a single bit in a specific register. Bit position ranges from 0 (lsb) to 7 (msb).
	void clearRegisterBit(byte registerAddress, byte bitPosition);

	// Returns true if a specific bit is set in a register. Bit position ranges from 0 (lsb) to 7 (msb).
	bool isBitSet(byte registerAddress, byte bitPosition);
};

#endif  // ! __SPARKFUN_AS7341L_IO__
