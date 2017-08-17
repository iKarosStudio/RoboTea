#include <robotea_parameters.h>
#include <rbt_hw.h>

#include <linux/i2c-dev.h>
#include <fcntl.h>

#define I2C_1_PATH "/dev/i2c-1"

int I2cFd;

int InitI2c (void )
{
	int Res = 0;
	uint8_t buffer[2];
	
	I2cFd = open (I2C_1_PATH, O_RDWR | O_SYNC) ;
	if (I2cFd < 0) {
		perror ("I2C") ;
		return 1;
	}
	printf ("i2c ok!\n") ;


	/*
	Res = ioctl (I2cFd, I2C_SLAVE, 0x74) ;
	buffer[0] = 0x06;
	buffer[1] = 0x09;
	write (I2cFd, &buffer, 2) ;

	buffer[0] = 0x02;
	buffer[1] = 0x06;
	write (I2cFd, &buffer, 2) ;
	*/

	Init_tca1116_0 () ;
	Init_tca1116_1 () ;
	//Init_PCA9685 () ;

	return 0;
}



int CloseI2c (void )
{
	close (I2cFd) ;
}

