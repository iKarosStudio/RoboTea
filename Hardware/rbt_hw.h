/*
	RoboTea Hardware
*/
char *HardwareGetRailYStatusString (uint8_t Status) ;
void HardwareRoutine (void *p) ;

int InitGpioReg (void ) ;
int InitI2c (void ) ;
int Init_tca1116_0 (int I2cFd) ;
int Init_tca1116_1 (int I2cFd) ;
//int Init_PCA9685 (void ) ;
int InitSpi (void ) ;
int InitUart (void ) ;


int CameraRoutine (void *p) ;

void RelayUpdate (uint8_t Value) ;

void GpioToggleLed (void ) ;
void Gpio (uint8_t Pin, uint8_t Level) ;

