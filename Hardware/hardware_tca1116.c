#include <robotea_parameters.h>
#include <rbt_hw.h>
#include <linux/i2c-dev.h>

#define I2C_SLAVE_0 0x74
#define I2C_SLAVE_1 0x75	
extern System *Host;

/*
	Stepper modules, Relays
*/
int Init_tca1116_0 (int I2cFd) 
{
	uint8_t Res = 0;
	uint8_t Buf[2]; 
	
	Res = ioctl (I2cFd, I2C_SLAVE, 0x74) ;

	Buf[0] = 0x06; //config p0
	Buf[1] = 0x05;
	write (I2cFd, Buf, 2) ;
	
	Buf[0] = 0x07; //config p1
	Buf[1] = 0x00;
	write (I2cFd, Buf, 2) ;

	Buf[0] = 0x02; //p0 out
	Buf[1] = 0xFF;
	write (I2cFd, Buf, 2) ;

	Host->RelayOut = 0;
	Buf[0] = 0x03; //p1 out
	Buf[1] = Host->RelayOut;
	write (I2cFd, Buf, 2) ;
}

/*
	Read Switches, LEDs
*/
int Init_tca1116_1 (int I2cFd)
{
	uint8_t Res = 0;
	uint8_t Buf[2];

	Res = ioctl (I2cFd, I2C_SLAVE, 0x75) ;
	
	Buf[0] = 0x06; //config p0
	Buf[1] = 0xFF;
	write (I2cFd, Buf, 2) ;
	
	Buf[0] = 0x07; //config p1
	Buf[1] = 0x00;
	write (I2cFd, Buf, 2) ;

	Host->Led = 0;
	Buf[0] = 0x03; //p1 out
	Buf[1] = Host->Led = 0;
	write (I2cFd, Buf, 2) ;
}


