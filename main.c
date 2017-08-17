#include <stdio.h>
#include <stdlib.h>

#include <robotea_parameters.h>
#include <rbt_hw.h>
#include <fsm.h>
#include <comm.h>
#include <oa.h>

System *Host = NULL;

int InitSystemPara (void )
{
	int Res = 0;
	
	Host = malloc (sizeof (System) ) ;
	if (Host == NULL) {
		printf ("CRITICAL ERROR\n") ;
		return -1;
	} else {
		printf ("Allocate system success @ 0x%08X\n", Host) ;
		memset (Host, 0, sizeof (System) ) ;
		return 0;
	}
}



int main (int argc, char *argv[])
{
	int Res = 0;
	int Index = 0;
	int Counter = 0;
	pthread_t id;
	
	printf ("-----------------\n") ;
	printf ("--Hello RoboTea--\n") ;
	printf ("-----------------\n") ;
	printf ("Version : %d.%02d\n", MAJOR_VER, SUB_VER) ;
	printf ("Args:") ;
	for (Index = 1; Index < argc; Index++) {
		printf ("%s ", argv[Index]) ;
	}
	printf ("\n\n") ;

	/*
		Initial system start
	*/
	InitSystemPara () ;

	sem_init (&(Host->SystemStatusLock), 0, 1) ; //lock system status
	sem_init (&(Host->OrderCountLock), 0, 1) ; 
	sem_init (&(Host->OrderLock), 0, 1) ; //lock order pointer
	sem_init (&(Host->ProcessingOrderLockX), 0, 1) ; 
	sem_init (&(Host->RailYLock), 0, 1) ; 
	sem_init (&(Host->CameraLock), 0, 1) ; 
	
	/*
		System threads
	*/
	Res = pthread_create (
		&(Host->SysThreads.hardware),
		NULL,
		HardwareRoutine,
		Host) ;

	Res = pthread_create (
		&(Host->SysThreads.comm),
		NULL, 
		CommRoutine,
		Host) ;

	Res = pthread_create (
		&(Host->SysThreads.fsm),
		NULL,
		FsmRoutine,
		Host) ;

	
	/*
		IDLE LOOP
	*/
	Host->SystemStatus = SYSTEM_STATUS_READY;
	while (!Host->SystemEnd) {
		//printf ("SystemStaus[0x%02X], Orders:%d\n", Host->SystemStatus, Host->PendingOrderCount) ;
		//OaDisplayAllOrders () ;
		sleep (1) ;
	}

	free (Host) ;
	return 0;
}
