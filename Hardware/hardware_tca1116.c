#include <robotea_parameters.h>
#include <rbt_hw.h>

int Init_tca1116_0 (int I2cFd) 
{
	Res = ioctl (I2cFd, I2C_SLAVE, 0x74) ;
	buffer[0] = 0x06;
	buffer[1] = 0x09;
	write (I2cFd, &buffer, 2) ;

	buffer[0] = 0x02;
	buffer[1] = 0x06;
	write (I2cFd, &buffer, 2) ;
}

int Init_tca1116_1 (int I2cFd)
{
	//
}


