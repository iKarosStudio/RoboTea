#include <robotea_parameters.h>
#include <rbt_hw.h>

#include <linux/i2c-dev.h>
#include <fcntl.h>

#define I2C_1_PATH "/dev/i2c-1"

extern System *Host;

int I2cFd;

void RelayUpdate (uint8_t Value) 
{
	uint8_t Res = ioctl (I2cFd, I2C_SLAVE, 0x74) ;
	uint8_t Buf[2];

	Buf[0] = 0x03;
	Host->RelayOut = Value;
	Buf[1] = Host->RelayOut;
	write (I2cFd, Buf, 2) ;
}

int InitI2c (void )
{
	int Res = 0;

	I2cFd = open (I2C_1_PATH, O_RDWR | O_SYNC) ;
	if (I2cFd < 0) {
		perror ("I2C") ;
		return 1;
	}
	
	Init_tca1116_0 (I2cFd) ;
	Init_tca1116_1 (I2cFd) ;
	//Init_PCA9685 () ;
	
	printf ("i2c ok!\n") ;
	return 0;
}



int CloseI2c (void )
{
	close (I2cFd) ;
}

