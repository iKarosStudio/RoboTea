#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 1000
#define IP "192.168.0.35"

#define CUPS 5

int sock_fd = 0;
struct sockaddr_in addr;

typedef struct _header {
	uint8_t order_type;
	uint8_t order_version;
	uint8_t serial_no;
	uint8_t client_no;
	time_t time;
	uint8_t tea_count;
} header;

typedef struct _tea_item {
	uint8_t TeaType;
	uint8_t TeaPercent;
	uint8_t SugarPercent;
	uint8_t IcePercent;
	uint8_t MiscFlags;
} tea_item;

tea_item tea[CUPS] = {
	[0] = {.TeaType=0, .TeaPercent=70, .SugarPercent=50, .IcePercent=10, .MiscFlags=0} ,
	[1] = {.TeaType=1, .TeaPercent=70, .SugarPercent=00, .IcePercent=30, .MiscFlags=0} ,
	[2] = {.TeaType=0, .TeaPercent=70, .SugarPercent=30, .IcePercent=50, .MiscFlags=0} ,
	[3] = {.TeaType=1, .TeaPercent=70, .SugarPercent=00, .IcePercent=30, .MiscFlags=0} ,
	[4] = {.TeaType=0, .TeaPercent=70, .SugarPercent=20, .IcePercent=30, .MiscFlags=0} 
} ;

int main (void )
{
	int res = 0;
	int index = 0;
	int pack_size = 0;
	
	uint8_t *p = NULL;
	header order_header;
	
	order_header.order_type = 0;
	order_header.order_version = 1;
	order_header.serial_no = 123;
	order_header.client_no = 0;
	order_header.tea_count = CUPS;
	
	sock_fd = socket (AF_INET, SOCK_DGRAM, 0) ;
	if (sock_fd < 0) {
		perror ("socket") ;
		return -1;
	}
	addr.sin_family = AF_INET;
	addr.sin_port   = htons (PORT) ;
	addr.sin_addr.s_addr = inet_addr (IP) ;
	
	/*
	res = bind (sock_fd, (struct sockaddr *) &addr, sizeof (struct sockaddr_in) ) ;
	if (res < 0) {
		perror ("bind socket") ;
		return -1;
	}
	*/
	pack_size = 13 + (5 * CUPS) ;
	printf ("size : %d\n", pack_size) ;
	p = malloc (pack_size) ;
	memset (p, 0x00, pack_size) ;
	memcpy (p, &order_header, sizeof (header) ) ;
	memcpy (p + 13, tea,5 * CUPS) ;
	for (index = 0; index < pack_size; index++) {
		//printf ("0x%02x -> 0x%02x\n", index, *(p + index) ) ;
	}
	res = sendto (
		sock_fd,
		p,
		pack_size,
		NULL,
		(struct sockaddr *) &addr,
		sizeof (addr) 
		) ;
	
	if (res) {
		printf ("sent %d byte.\n", res) ;
	}
	
	close (sock_fd) ;
	return 0;
}

