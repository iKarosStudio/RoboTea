#include <robotea_parameters.h>
#include <comm.h>
#include <oa.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

int *SocketFd = NULL;
struct sockaddr_in *ClientSocket;
pthread_t *ClientListenerId = NULL;

void CommClientOrderListener (int ClientNo) {
	int Res = 0; 
	int Index = 0;
	uint8_t *TempOrderBuffer = NULL;
	struct sockaddr_in AddrIn;
	uint32_t AddrLen;
	
	printf ("Client listener [%02d]\n", ClientNo) ;

	TempOrderBuffer = malloc (MAX_ORDER_BUFFER_SIZE) ;
	
	while (1) {
		Res = recvfrom (
			*(SocketFd + ClientNo),
			TempOrderBuffer,
			MAX_ORDER_BUFFER_SIZE,
			0,
			&AddrIn,
			(unsigned int *) &AddrLen
		) ;

		if (Res < 0) {
			//printf ("Client[%02d] - time out\n", ClientNo) ;
		} else {
			printf ("Client[%02d] - recv %d bytes\n", ClientNo, Res) ;
			OaNewOrder (TempOrderBuffer) ;
		}
	}
	free (TempOrderBuffer) ;
	pthread_exit (NULL) ;
}

void CommRoutine (void *p) 
{
	System *Sys = (System *) p;
	struct timeval RecvTimeout;
	int ClientNode = 0;
	int Res = 0;
	
	if (Sys == NULL) {
		printf ("CommRoutine->Host ptr error\n") ;
		return ;
	} else {
		printf ("Comm Routine\n") ;
	}


	/*
		Create socket listener for client machines
	*/
	SocketFd         = malloc (sizeof (int ) * CLIENT_NO) ;
	ClientSocket     = malloc (sizeof (struct sockaddr_in) * CLIENT_NO) ;
	ClientListenerId = malloc (sizeof (pthread_t) * CLIENT_NO) ;
	for (ClientNode = 0; ClientNode < CLIENT_NO; ClientNode++) {
		printf ("comm->create client[%02d] socket...", ClientNode) ;
		*(SocketFd + ClientNode) = socket (AF_INET, SOCK_DGRAM, 0) ;
		if (*(SocketFd + ClientNode) > 0) {
			//printf ("success\n") ;
		} else {
			//printf ("fail\n") ;
			perror ("socket") ;
		}

		(ClientSocket + ClientNode)->sin_family      = AF_INET;
		(ClientSocket + ClientNode)->sin_port        = htons (CLIENT_SOCKET + ClientNode) ;
		(ClientSocket + ClientNode)->sin_addr.s_addr = INADDR_ANY; //source ip

		Res = bind (
			*(SocketFd + ClientNode),
			(struct sockaddr *) (ClientSocket + ClientNode),
			sizeof (struct sockaddr_in) ) ;
		if (Res < 0) {
			printf ("fail\n") ;
			perror ("    * bind socket") ;
			close (*(SocketFd + ClientNode) ) ;
		} else {
			printf ("success\n") ;
		}
		RecvTimeout.tv_sec = 2;
		RecvTimeout.tv_usec = 0;
		setsockopt(*(SocketFd + ClientNode),
			SOL_SOCKET,
			SO_RCVTIMEO,
			(char *) &RecvTimeout,
			sizeof (RecvTimeout) ) ;

		pthread_create (
			(ClientListenerId + ClientNode),
			NULL, 
			CommClientOrderListener, 
			ClientNode) ;
	} //end of for


	
	while (!Sys->SystemEnd) {
		sleep (1) ;
	}
	free (SocketFd) ;
	pthread_exit (NULL) ;
}
