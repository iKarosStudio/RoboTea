/*
	RoboTea Hardware
*/
char *HardwareGetRailYStatusString (uint8_t Status) ;
void HardwareRoutine (void *p) ;

int InitGpioReg (void ) ;
int InitI2c (void ) ;
int Init_tca1116_0 (void)  ;
int Init_tca1116_1 (void) ;
//int Init_PCA9685 (void ) ;
int InitSpi (void ) ;
int InitUart (void ) ;


int CameraRoutine (void *p) ;

void GpioToggleLed (void ) ;
void Gpio (uint8_t Pin, uint8_t Level) ;

