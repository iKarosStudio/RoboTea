/*
	RoboTea Order Arbitrate
*/

int OaTerminateOrderFromX (void ) ;
int OaTerminateItemFromRailY (void ) ;
int OaAssignOrderToRailX (void ) ;
int OaAssignItemToRailY (uint8_t ItemIndex) ;
uint8_t HardwareGetRailYStatus (void *p) ; 
uint8_t HardwareSetRailYStatus (void *p, uint8_t Status) ;


void OaDisplayAllOrders (void ) ;
int OaAddPendingOrder (void ) ;
int OaSubPendingOrder (void ) ;
uint8_t OaGetPendingOrder (void ) ;
int OaNewOrder (uint8_t *TempOrder) ;

