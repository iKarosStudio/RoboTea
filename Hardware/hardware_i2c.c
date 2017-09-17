#include <robotea_parameters.h>
#include <rbt_hw.h>

#include <linux/i2c-dev.h>
#include <fcntl.h>

#define I2C_1_PATH "/dev/i2c-1"

int I2cFd;

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

