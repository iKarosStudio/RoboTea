#include <robotea_parameters.h>
#include <rbt_hw.h>
#include <linux/i2c-dev.h>

#define I2C_SLAVE_0 0x74
#define I2C_SLAVE_1 0x75	

int Init_tca1116_0 (int I2cFd) 
{
	uint8_t Res = 0;
	uint8_t Buf[2]; 
	
	Res = ioctl (I2cFd, I2C_SLAVE, 0x74) ;
	Buf[0] = 0x06;
	Buf[1] = 0x9;
	write (I2cFd, Buf, 2) ;

	Buf[0] = 0x02;
	Buf[1] = 0xFF;
	write (I2cFd, Buf, 2) ;
}

int Init_tca1116_1 (int I2cFd)
{
	//
}


