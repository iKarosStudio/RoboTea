/*
	RoboTea system parameters
*/

#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#include <cv.h>
#include <highgui.h>

//#include <gtk/gtk.h>

#define WINDOW_NAME "RoboTea Host"

//sys config
#define CLIENT_NO 2

/*

*/
#define TEA_TYPE_GREEN   0x00
#define TEA_TYPE_BLACK   0x01
#define TEA_TYPE_OOLONG  0x02

#define TEA_ITEM_SIZE 5
typedef struct _tea_list {
	uint8_t TeaType;
	uint8_t TeaPercent;
	uint8_t SugarPercent;
	uint8_t IcePercent;
	uint8_t MiscFlags;
} TeaList;

#define ORDER_HEADER_SIZE 13
typedef struct _order_list {
	uint8_t OrderType;
	uint8_t OrderVersion;
	uint8_t SerialNo;
	uint8_t ClientNo;
	time_t TimeStamp;

	uint8_t TeaCount; //Cups in this order
	struct TeaList *TeaItem; //Point to the head of list of teas
	struct _order_list *NextOrder;
} OrderList;

/*

*/
typedef struct _sys_threads {
	pthread_t fsm;
	pthread_t comm;
	pthread_t oa;
	pthread_t hardware;
	pthread_t x_rail; //茶湯含封口
	pthread_t y_rail; //前台配送
} SystemThreads;

/*

*/
#define RAIL_STATUS_WAIT 0x00
#define RAIL_STATUS_NEW  0x01 //Got new process
#define RAIL_STATUS_BUZY 0x02 //Shipping to client window
#define RAIL_STATUS_RET  0x03 //After shipping, return to default pos.
#define RAIL_STATUS_ERR  0x80 //unknown
typedef struct _rail_y_control {
	uint8_t Status;
	uint8_t SerialNo;
	uint8_t ShipTargetClient;
	TeaList ProcessingTeaItem;
} RailYControl;

/*
	
*/
typedef struct _camera_control {
	char *DeviceName;
	int Height;
	int Width;
	uint8_t LevelCheck;
} CameraControl;


#define SYSTEM_STATUS_BOOTING 0x00
#define SYSTEM_STATUS_READY   0x01
#define SYSTEM_STATUS_BUZY    0x02
#define SYSTEM_STATUS_ERROR   0x80
typedef struct _system {
	uint8_t SystemEnd;
	sem_t SystemStatusLock;
	uint8_t SystemStatus;
	SystemThreads SysThreads;

	sem_t OrderCountLock;
	uint8_t PendingOrderCount;
	sem_t OrderLock;
	OrderList *OrderQuere;

	sem_t ProcessingOrderLockX;
	OrderList *ProcessingOrderX;
	
	sem_t RailYLock;
	RailYControl RailY;

	sem_t CameraLock;
	CameraControl Camera;
	
} System;

