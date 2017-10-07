#include <robotea_parameters.h>
#include <rbt_hw.h>

uint8_t HardwareGetRailYStatus (void *p) 
{
	uint8_t Status;
	System *Sys = (System *) p;
	
	sem_wait (&(Sys->RailYLock) ) ;
	Status = Sys->RailY.Status;
	sem_post (&(Sys->RailYLock) ) ;
	
	return Status;
}

char *HardwareGetRailYStatusString (uint8_t Status) 
{
	const char *RailYStatus[] = {
		"RAIL_WAIT",
		"RAIL_NEW",
		"RAIL_BUZY",
		"RAIL_RETURNING"
	} ;
	const char *e = "RAIL_ERROR";

	if (Status != 0x80) {
		return RailYStatus[Status];
	} else {
		return e;
	}	
}

uint8_t HardwareSetRailYStatus (void *p, uint8_t Status) 
{
	System *Sys = (System *) p;
	
	sem_wait (&(Sys->RailYLock) ) ;
	Sys->RailY.Status = Status;
	sem_post (&(Sys->RailYLock) ) ;
	
	return Status;
}

extern uint8_t CurrentCupIndex;
void HardwareXRail (void *p)
{
	
	System *Sys = (System *) p;
	uint8_t Index = 0;
	OrderList *OrderOnX = NULL;
	TeaList *TeaOnX = NULL;
	while (!Sys->SystemEnd) {
		if (Sys->ProcessingOrderX != NULL) { //Got order
			printf ("process X->%p\n", Sys->ProcessingOrderX) ;
			OrderOnX = Sys->ProcessingOrderX;
			TeaOnX = OrderOnX->TeaItem;

			for (Index = 0; Index < OrderOnX->TeaCount; Index++) {
				CurrentCupIndex = Index;
				printf ("\nProcessing x order[%3d] tea %d of %d...\n", OrderOnX->SerialNo, (Index + 1), OrderOnX->TeaCount) ;	
				//sleep (1) ;
				printf ("process X load ice    %2d%\n", (TeaOnX + Index)->IcePercent) ;
				//sleep (1) ;
				printf ("process X load sugar  %2d%\n", (TeaOnX + Index)->SugarPercent) ;
				//sleep (1) ;
				printf ("process X load tea    %s\n", OaGetTeaType ( (TeaOnX + Index)->TeaType) ) ;
				//sleep (1) ;
				printf ("process X seal cup\n") ;
				sleep (1) ;
				//printf ("assign to y rail.\n") ;
				while (HardwareGetRailYStatus (p) != RAIL_STATUS_WAIT) {
					usleep (500000) ;
				}
				OaAssignItemToRailY (Index) ;
			}
			OaTerminateOrderFromX () ;
		} else { //Wait for order
			sleep (1) ;
		}
	}
}

void HardwareYRail (void *p) 
{
	int Index = 0;
	System *Sys = (System *) p;
	OrderList *OrderOnY = NULL;
	TeaList *TeaOnY = NULL;
	
	while (!Sys->SystemEnd) {
		if (HardwareGetRailYStatus (p) == RAIL_STATUS_NEW) {
			HardwareSetRailYStatus (p, RAIL_STATUS_BUZY) ;
		} else if (HardwareGetRailYStatus (p) == RAIL_STATUS_WAIT) {
			sleep (1) ;
		} else if (HardwareGetRailYStatus (p) == RAIL_STATUS_BUZY) {
			printf ("process Y-> %s\n", OaGetTeaType (Sys->RailY.ProcessingTeaItem.TeaType) ) ;
			printf ("process Y-> ship to client-%02d\n", Sys->RailY.ShipTargetClient) ;
			sleep (1) ;
			OaTerminateItemFromRailY () ;
		} else if (HardwareGetRailYStatus (p) == RAIL_STATUS_RET) {
			//
		} else if (HardwareGetRailYStatus (p) == RAIL_STATUS_ERR) {
			//
		} else {
			sleep (1) ;
		}
	}
}

void HardwareRoutine (void *p) 
{
	System *Sys = (System *) p;
	pthread_t WebcamThreadId;
	int Res = 0;

	uint8_t tmp = 1;
	
	printf ("Hardware Routine\n") ;

	InitGpioReg () ;		
	InitI2c () ;
	InitSpi () ;
	InitUart () ;
	//InitWebcam () ;
	
	Res = pthread_create (
		&(WebcamThreadId),
		NULL,
		CameraRoutine,
		p) ;
	

	Res = pthread_create (
		&(Sys->SysThreads.x_rail),
		NULL,
		HardwareXRail,
		p) ;

	Res = pthread_create (
		&(Sys->SysThreads.y_rail),
		NULL,
		HardwareYRail,
		p) ;

	while (!Sys->SystemEnd) {
		sleep (1) ;

		//RelayUpdate (tmp) ;
		tmp <<= 1;
		if (tmp == 0) tmp = 1;
		
		//usleep (16000) ;
		//GpioToggleLed () ;
	}

	//release gpio map
	//release i2c 
	//release uart
	//release spi
	
}

