#include <robotea_parameters.h>
#include <fsm.h>


int SysSetStatus (void *p, int Status) 
{
	System *Sys = (System *) p;

	sem_wait (&(Sys->SystemStatusLock) ) ;
	Sys->SystemStatus = Status;
	sem_post (&(Sys->SystemStatusLock) ) ;
	
	return 0;
}

int SysGetStatus (void *p, int Status) 
{
	System *Sys = (System *) p;
	uint8_t s = 0;

	sem_wait (&(Sys->SystemStatusLock) ) ;
	s = Sys->SystemStatus;
	sem_post (&(Sys->SystemStatusLock) ) ;
	
	return s;
}

char *SysGetStatusString (int Status) 
{
	const *StatusString[] = {
		"SYS_BOOTING",
		"SYS_READY",
		"SYS_BUZY",
		"SYS_ERROR"
	} ;

	return StatusString[Status] ;
}

void FsmCheckOrder (void *p) 
{
	System *Sys = (System *) p;

	if (Sys->OrderQuere != NULL) {
		SysSetStatus (p, SYSTEM_STATUS_BUZY) ;
		if (Sys->ProcessingOrderX == NULL) {
			OaAssignOrderToRailX () ;
		}
		
	} else {
		SysSetStatus (p, SYSTEM_STATUS_READY) ;
	}
}

void FsmRoutine (void *p)
{
	System *Sys = (System *) p;

	printf ("FSM Routine\n") ;

	
	while (!Sys->SystemEnd) {
		//OaDisplayAllOrders () ;
		sleep (1) ; // check system status routine
		switch (Sys->SystemStatus) {
		case SYSTEM_STATUS_BOOTING : //0
			break;

		case SYSTEM_STATUS_READY : //1
			FsmCheckOrder (p) ;
			break;

		case SYSTEM_STATUS_BUZY : //2
			FsmCheckOrder (p) ;
			break;
			
		case SYSTEM_STATUS_ERROR : //0x80
			break;

		default:
			break;
		}
	}
}
