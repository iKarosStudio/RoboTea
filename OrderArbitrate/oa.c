#include <robotea_parameters.h>
#include <oa.h>

extern System *Host;

int OaTerminateOrderFromX (void ) 
{
	sem_wait (&(Host->ProcessingOrderLockX) ) ;
	if (Host->ProcessingOrderX == NULL) {
		sem_post (&(Host->ProcessingOrderLockX) ) ;
		return 1;
	} else {
		free (Host->ProcessingOrderX->TeaItem) ;
		Host->ProcessingOrderX->TeaItem = NULL;
		free (Host->ProcessingOrderX) ;
		Host->ProcessingOrderX = NULL;
	}
	sem_post (&(Host->ProcessingOrderLockX) ) ;
	return 0;
}
int OaAssignOrderToRailX (void ) 
{
	int Res = 0;
	OrderList *Tmp = NULL;

	sem_wait (&(Host->OrderLock) ) ;
	if (Host->OrderQuere == NULL) {
		printf ("Error : No order waitting but try to assign X\n") ;
		sem_post (&(Host->OrderLock)  );
		return 1;
	} else {
		sem_wait(&(Host->ProcessingOrderLockX) ) ;
		Host->ProcessingOrderX = malloc (sizeof (OrderList) ) ;
		memcpy (Host->ProcessingOrderX, Host->OrderQuere, sizeof (OrderList) ) ;

		Tmp = Host->OrderQuere;
		if (Host->OrderQuere->NextOrder == NULL) {
			Host->OrderQuere = NULL;
		} else {
			Host->OrderQuere = Host->OrderQuere->NextOrder;
		}
		
		free (Tmp) ;
		Tmp = NULL;
		OaSubPendingOrder () ;
		sem_post (&(Host->ProcessingOrderLockX) ) ;
	}
	sem_post (&(Host->OrderLock) ) ;

	return Res;
}

int OaAssignItemToRailY (uint8_t ItemIndex)
{
	int Res = 0;
	TeaList *TmpItem = Host->ProcessingOrderX->TeaItem;// + ItemIndex;

	sem_wait (&(Host->RailYLock) ) ;

	Host->RailY.ShipTargetClient = Host->ProcessingOrderX->ClientNo;
	Host->RailY.SerialNo = Host->ProcessingOrderX->SerialNo;
	memcpy (&(Host->RailY.ProcessingTeaItem), (TmpItem + ItemIndex), sizeof (TeaList) ) ;
	
	Host->RailY.Status = RAIL_STATUS_NEW;
	sem_post (&(Host->RailYLock) ) ;
	return 0;
}

int OaTerminateItemFromRailY (void )
{
	int Res = 0;

	sem_wait (&(Host->RailYLock) ) ;

	memset (&(Host->RailY), 0, sizeof (RailYControl) ) ;
	
	sem_post (&(Host->RailYLock) ) ;
	
	return Res;
}

char *OaGetTeaType (uint8_t Type) 
{
	const char *Tea[] = {
		"GREEN TEA",
		"BLACK TEA",
		"OOLONG TEA"
		"DOG SHIT",
		"PUSSY JUICE"
	} ;

	return Tea[Type];
}

void OaDisplayItemContent (TeaList Item) 
{
	printf ("Tea Type     : %s\n", OaGetTeaType (Item.TeaType) ) ;
	printf ("Tea percent  : %d %\n", Item.TeaPercent) ;
	printf ("Sugar percent: %d %\n", Item.SugarPercent) ;
	printf ("Ice percent  : %d %\n", Item.IcePercent) ;
	printf ("Misc flag    : 0x%02X\n", Item.MiscFlags) ;
}

void OaDisplayOrderContent (OrderList Order) 
{
	int ItemIndex = 0;
	TeaList TeaListBuffer;
	
	printf ("  \n") ;
	printf ("Order Type : %d\n", Order.OrderType) ;
	printf ("Order Version : %d\n", Order.OrderVersion) ;
	printf ("Order SerialNo: %d\n", Order.SerialNo) ;
	printf ("Order ClientNo: %d\n", Order.ClientNo) ;
	printf ("Order TimeStamp: %d\n", Order.TimeStamp) ;
	printf ("Order Tea Count: %d\n", Order.TeaCount) ;
	printf ("Order Tea Item : 0x%08X\n", Order.TeaItem) ;
	printf ("Next Order     : 0x%08X\n", Order.NextOrder) ;

	for (ItemIndex = 0; ItemIndex < Order.TeaCount; ItemIndex++) {
		memcpy (&TeaListBuffer, (Order.TeaItem), sizeof (TeaList) ) ;
		OaDisplayItemContent (TeaListBuffer) ;
		//debug^^^^
	}
	
}

void OaDisplayAllOrders (void ) 
{
	OrderList *Order = NULL; 
	int Counter = 0;
	
	sem_wait (&(Host->OrderLock) ) ;
	
	Order = Host->OrderQuere;
	if (Order == NULL) {
		//printf ("NO ORDERS\n") ;
		sem_post (&(Host->OrderLock) ) ;
		return ;
	}
	
	while (Order != NULL) {
		printf ("\n%03d Order @ 0x%08X", Counter, Order) ;
		OaDisplayOrderContent (*Order) ;
		
		if (Order->NextOrder != NULL) {
			Order = Order->NextOrder;
			Counter++;
		} else {
			//Reach the latest
			Order = NULL;
		}
	}
	sem_post (&(Host->OrderLock) ) ;
}

int OaAddPendingOrder (void )
{
	sem_wait (&(Host->OrderCountLock) ) ;
	Host->PendingOrderCount ++;
	sem_post (&(Host->OrderCountLock) ) ;
}

int OaSubPendingOrder (void )
{
	sem_wait (&(Host->OrderCountLock) ) ;
	if (Host->PendingOrderCount > 0) {
		Host->PendingOrderCount --;
	}
	sem_post (&(Host->OrderCountLock) ) ;
}

uint8_t OaGetPendingOrder (void )
{
	uint8_t i = 0;
	
	sem_wait (&(Host->OrderCountLock) ) ;
	i = Host->PendingOrderCount;
	sem_post (&(Host->OrderCountLock) ) ;
	
	return i;
}

OrderList *OaFindLatestOrder (void )
{
	OrderList *Ptr = NULL;

	Ptr = Host->OrderQuere;
	
	sem_wait (&(Host->OrderCountLock) ) ;
	while (Ptr->NextOrder != NULL) {
		Ptr = Ptr->NextOrder;
	}
	sem_post (&(Host->OrderCountLock) ) ;

	return Ptr;
}

int OaRemoveFirstOrder (void )
{
	return 0;
}

int OaRemoveOrderBySerial (uint8_t Serial) 
{
	return 0;
}

int OaNewOrder (uint8_t *TempOrder) 
{
	static uint8_t SystemSerial = 0;
	int Res;
	int Index = 0;
	OrderList *OrderPtr = NULL;
	TeaList   *ItemPtr = NULL;
	OrderList NewOrder;
	TeaList *NewItem;

	memset (&NewOrder, 0, sizeof (OrderList) ) ;
	memcpy (&NewOrder, TempOrder, ORDER_HEADER_SIZE) ;
	NewOrder.SerialNo = SystemSerial; SystemSerial++;
	/*
	printf ("type     :%d\n", NewOrder.OrderType) ;
	printf ("version  :%d\n", NewOrder.OrderVersion) ;
	printf ("serial no:%d\n", NewOrder.SerialNo) ;
	printf ("client no:%d\n", NewOrder.ClientNo) ;
	printf ("time     :%d\n", NewOrder.TimeStamp) ;
	printf ("tea count:%d\n", NewOrder.TeaCount) ;
	*/
	NewItem = malloc (NewOrder.TeaCount * TEA_ITEM_SIZE) ;
	memcpy (NewItem, TempOrder + ORDER_HEADER_SIZE, TEA_ITEM_SIZE * NewOrder.TeaCount) ;
	for (Index = 0; Index < NewOrder.TeaCount; Index++) {
		/*
		printf ("---ITEM [%d]---\n", (Index + 1) ) ;
		printf ("tea type  :%s\n", OaGetTeaType ((NewItem + Index)->TeaType) ) ;
		printf ("tea perc  :%d %\n", (NewItem + Index)->TeaPercent) ;
		printf ("sugar perc:%d %\n", (NewItem + Index)->SugarPercent) ;
		printf ("ice perc  :%d %\n", (NewItem + Index)->IcePercent) ;
		printf ("misc      :%d\n", (NewItem + Index)->MiscFlags) ;
		*/
	}
	
	/*
		Put into system order linked list
	*/
	sem_wait (&(Host->OrderLock) ) ;
	if (Host->OrderQuere == NULL) { //add new list
		Host->OrderQuere = malloc (sizeof (OrderList) ) ;
		memset (Host->OrderQuere, 0, sizeof (OrderList) ) ;
		OrderPtr = Host->OrderQuere;
	} else { //add to tail of linklist
		OrderPtr = OaFindLatestOrder () ;
		OrderPtr->NextOrder = malloc (sizeof (OrderList) ) ;
		memset (OrderPtr->NextOrder, 0, sizeof (OrderList) ) ;
		OrderPtr = OrderPtr->NextOrder;
	}
	
	memcpy (OrderPtr, &NewOrder, sizeof (OrderList) ) ;
	OrderPtr->TeaItem = malloc (sizeof (TeaList) * OrderPtr->TeaCount) ;
	ItemPtr = OrderPtr->TeaItem;
	memset (ItemPtr, 0, sizeof (TeaList) * OrderPtr->TeaCount) ;
	memcpy (ItemPtr, NewItem, TEA_ITEM_SIZE * OrderPtr->TeaCount) ;
	OaAddPendingOrder () ;
	
	free (NewItem) ;
	sem_post (&(Host->OrderLock) ) ;
	return 0;
}


