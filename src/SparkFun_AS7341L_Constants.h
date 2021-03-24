/*
  This is a library written for the AMS AS7341L 10-Channel Spectral Sensor Frontend
  SparkFun sells these at its website:
  https://www.sparkfun.com/products/17719

  Do you like this library? Help support open source hardware. Buy a board!

  Written by Ricardo Ramos  @ SparkFun Electronics, March 15th, 2021
  This file declares all constants used in the AS7341L sensor library.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef  __SPARKFUN_AS7341L_CONSTANTS__
#define __SPARKFUN_AS7341L_CONSTANTS__

#include <Arduino.h>

// Constants definitions
const byte DEFAULT_AS7341L_ADDR = 0x39;

// PCA9536 GPIO pins
const byte POWER_LED_GPIO = 0x0;
const byte WHITE_LED_GPIO = 0x01;
const byte IR_LED_GPIO = 0x02;

// Error constants
const byte ERROR_NONE = 0x0;
const byte ERROR_AS7341L_I2C_COMM_ERROR = 0x01;
const byte ERROR_PCA9536_I2C_COMM_ERROR = 0x02;
const byte ERROR_AS7341L_WRONG_CHIP_ID = 0x03;
const byte ERROR_AS7341L_MEASUREMENT_TIMEOUT = 0x04;

// Gain enumeration
enum class AS7341L_GAIN
{
	GAIN_HALF,
	GAIN_X1,
	GAIN_X2,
	GAIN_X4,
	GAIN_X8,
	GAIN_X16,
	GAIN_X32,
	GAIN_X64,
	GAIN_X128,
	GAIN_X256,
	GAIN_X512,
	GAIN_INVALID
};

// Registers definitions
const byte REGISTER_CH0_DATA_L		= 0x95;
const byte REGISTER_CH0_DATA_H		= 0x96;
const byte REGISTER_ITIME_L			= 0x63;
const byte REGISTER_ITIME_M			= 0x64;
const byte REGISTER_ITIME_H			= 0x65;
const byte REGISTER_CH1_DATA_L		= 0x97;
const byte REGISTER_CH1_DATA_H		= 0x98;
const byte REGISTER_CH2_DATA_L		= 0x99;
const byte REGISTER_CH2_DATA_H		= 0x9a;
const byte REGISTER_CH3_DATA_L		= 0x9b;
const byte REGISTER_CH3_DATA_H		= 0x9c;
const byte REGISTER_CH4_DATA_L		= 0x9d;
const byte REGISTER_CH4_DATA_H		= 0x9e;
const byte REGISTER_CH5_DATA_L		= 0x9f;
const byte REGISTER_CH5_DATA_H		= 0xa0;
const byte REGISTER_CONFIG			= 0x70;
const byte REGISTER_STAT			= 0x71;
const byte REGISTER_EDGE			= 0x72;
const byte REGISTER_GPIO			= 0x73;
const byte REGISTER_LED				= 0x74;
const byte REGISTER_ENABLE			= 0x80;
const byte REGISTER_ATIME			= 0x81;
const byte REGISTER_WTIME			= 0x82;
const byte REGISTER_SP_TH_L_LSB		= 0x84;
const byte REGISTER_SP_TH_L_MSB		= 0x85;
const byte REGISTER_SP_TH_H_LSB		= 0x86;
const byte REGISTER_SP_TH_H_MSB		= 0x87;
const byte REGISTER_AUXID			= 0x90;
const byte REGISTER_REVID			= 0x91;
const byte REGISTER_ID				= 0x92;
const byte REGISTER_STATUS			= 0x93;
const byte REGISTER_ASTATUS			= 0x94;
const byte REGISTER_STATUS_2		= 0xa3;
const byte REGISTER_STATUS_3		= 0xa4;
const byte REGISTER_STATUS_5		= 0xa6;
const byte REGISTER_STATUS_6		= 0xa7;
const byte REGISTER_CFG_0			= 0xa9;
const byte REGISTER_CFG_1			= 0xaa;
const byte REGISTER_CFG_3			= 0xac;
const byte REGISTER_CFG_6			= 0xaf;
const byte REGISTER_CFG_8			= 0xb1;
const byte REGISTER_CFG_9			= 0xb2;
const byte REGISTER_CFG_10			= 0xb3;
const byte REGISTER_CFG_12			= 0xb5;
const byte REGISTER_PERS			= 0xbd;
const byte REGISTER_GPIO_2			= 0xbe;
const byte REGISTER_ASTEP_L			= 0xca;
const byte REGISTER_ASTEP_H			= 0xcb;
const byte REGISTER_AGC_GAIN_MAX	= 0xcf;
const byte REGISTER_AZ_CONFIG		= 0xd6;
const byte REGISTER_INTENAB			= 0xf9;
const byte REGISTER_CONTROL			= 0xfa;
const byte REGISTER_FIFO_MAP		= 0xfc;
const byte REGISTER_FIFO_LVL		= 0xfd;
const byte REGISTER_FDATA_L			= 0xfe;
const byte REGISTER_FDATA_H			= 0xff;

#endif // ! __SPARKFUN_AS7341L_CONSTANTS__
