#include <robotea_parameters.h>
#include <rbt_hw.h>


CvFont SysFont;
uint8_t CurrentCupIndex = 0;

void GuiDispSysStatus (void *p, IplImage *Image)
{
	System *Sys = (System *) p;
	char StrBuffer[128] = {0} ;

	/*	
		System status
	*/
	sprintf (StrBuffer, "--SYSTEM STATUS [%s]--", SysGetStatusString (Sys->SystemStatus) ) ;
	cvPutText (Image, StrBuffer, cvPoint (0, 20), &SysFont, CV_RGB (0, 255, 0) ) ;

	/*	
		Pending order
	*/
	sprintf (StrBuffer, "Pending Order : %d", OaGetPendingOrder () ) ;
	cvPutText (Image, StrBuffer, cvPoint (0, 40), &SysFont, CV_RGB (0, 255, 0) ) ;

	/*	
		Rail X
	*/
	if (Sys->ProcessingOrderX != NULL) {
		sprintf (StrBuffer, "Rail X : %p", Sys->ProcessingOrderX) ;
		cvPutText (Image, StrBuffer, cvPoint (0, 60), &SysFont, CV_RGB (0, 255, 0) ) ;
		sprintf (StrBuffer, "    Process Serial No : %d", Sys->ProcessingOrderX->SerialNo) ;
		cvPutText (Image, StrBuffer, cvPoint (0, 80), &SysFont, CV_RGB (0, 255, 0) ) ;
		sprintf (StrBuffer, "    Process Client No : %d", Sys->ProcessingOrderX->ClientNo) ;
		cvPutText (Image, StrBuffer, cvPoint (0, 100), &SysFont, CV_RGB (0, 255, 0) ) ;
		sprintf (StrBuffer, "    Process Tea counts: %d", Sys->ProcessingOrderX->TeaCount) ;
		cvPutText (Image, StrBuffer, cvPoint (0, 120), &SysFont, CV_RGB (0, 255, 0) ) ;
		sprintf (StrBuffer, "    Process Now %d of %d", CurrentCupIndex + 1, Sys->ProcessingOrderX->TeaCount) ;
		cvPutText (Image, StrBuffer, cvPoint (0, 140), &SysFont, CV_RGB (0, 255, 0) ) ;
	} else {
		sprintf (StrBuffer, "Rail X : ---") ;
		cvPutText (Image, StrBuffer, cvPoint (0, 60), &SysFont, CV_RGB (0, 255, 0) ) ;
		sprintf (StrBuffer, "    Process Serial No : ---") ;
		cvPutText (Image, StrBuffer, cvPoint (0, 80), &SysFont, CV_RGB (0, 255, 0) ) ;
		sprintf (StrBuffer, "    Process Client No : ---") ;
		cvPutText (Image, StrBuffer, cvPoint (0, 100), &SysFont, CV_RGB (0, 255, 0) ) ;
		sprintf (StrBuffer, "    Process Tea counts: ---") ;
		cvPutText (Image, StrBuffer, cvPoint (0, 120), &SysFont, CV_RGB (0, 255, 0) ) ;
		sprintf (StrBuffer, "    Process Now Idle") ;
		cvPutText (Image, StrBuffer, cvPoint (0, 140), &SysFont, CV_RGB (0, 255, 0) ) ;
	}

	/*
		Rail Y
	*/
	sprintf (StrBuffer, "Rail Y : %s", HardwareGetRailYStatusString (Sys->RailY.Status) ) ;
	cvPutText (Image, StrBuffer, cvPoint (0, 160), &SysFont, CV_RGB (0, 255, 0) ) ;
	if (HardwareGetRailYStatus (p) != RAIL_STATUS_WAIT) {
		sprintf (StrBuffer, "    Target Client  : %d", Sys->RailY.ShipTargetClient) ;
		cvPutText (Image, StrBuffer, cvPoint (0, 180), &SysFont, CV_RGB (0, 255, 0) ) ;
		sprintf (StrBuffer, "    Target Tea Type: %s", OaGetTeaType (Sys->RailY.ProcessingTeaItem.TeaType) ) ;
		cvPutText (Image, StrBuffer, cvPoint (0, 200), &SysFont, CV_RGB (0, 255, 0) ) ;
	} else {
		sprintf (StrBuffer, "    Target Client  : ---") ;
		cvPutText (Image, StrBuffer, cvPoint (0, 180), &SysFont, CV_RGB (0, 255, 0) ) ;
		sprintf (StrBuffer, "    Target Tea Type: ---") ;
		cvPutText (Image, StrBuffer, cvPoint (0, 200), &SysFont, CV_RGB (0, 255, 0) ) ;
	}
	
}

void GuiDispCameraStatus (void *p, IplImage *Image)
{
	System *Sys = (System *) p;
	char StrBuffer[128] = {0} ;

	sprintf (StrBuffer, "--Camera[%dx%d]--", Sys->Camera.Height, Sys->Camera.Width) ;
	cvPutText (Image, StrBuffer, cvPoint (0, 240), &SysFont, CV_RGB (0, 255, 0) ) ;
	sprintf (StrBuffer, "    Device Name : %s", "??") ;
	cvPutText (Image, StrBuffer, cvPoint (0, 260), &SysFont, CV_RGB (0, 255, 0) ) ;
	sprintf (StrBuffer, "    Level Check : %d", Sys->Camera.LevelCheck) ;
	cvPutText (Image, StrBuffer, cvPoint (0, 280), &SysFont, CV_RGB (0, 255, 0) ) ;
}

double ThresholdValue = 100.0;
double SmoothSize1 = 5;
double SmoothSize2 = 5;
double SmoothSigma = 10;
CvPoint Target[3];

void GuiDispLevelTarget (void *p, IplImage *Image)
{
	System *Sys = (System *) p;
	int Index = 0;
	const int TargetCursorSize = 10;
	CvScalar RectColor;
	
	if (Sys->Camera.LevelCheck) {
		RectColor = CV_RGB (0, 255, 0) ;
	} else {
		RectColor = CV_RGB (255, 0, 0) ;
	}

	for (Index = 0; Index < 3; Index++) {
		CvPoint P1 = cvPoint (Target[Index].x - (TargetCursorSize / 2), Target[Index].y - (TargetCursorSize / 2) ) ;
		CvPoint P3 = cvPoint (Target[Index].x + (TargetCursorSize / 2), Target[Index].y + (TargetCursorSize / 2) ) ;
		cvRectangle (Image, P1, P3, RectColor, 1, 8, 0) ;
	}
	
}

int CameraLevelCheck (void *p, IplImage *SrcImage) 
{
	System *Sys = (System *) p;
	
	int Index = 0;
	int Res = 0;
	int CheckFlag = 0;

	IplImage *Result = NULL;
	CvSize SrcSize;
	
	if (SrcImage == NULL) {
		return -1;
	}

	SrcSize = cvGetSize (SrcImage) ;

	Result = cvCreateImage (SrcSize, IPL_DEPTH_8U, 3) ;
	cvCvtColor (SrcImage, Result, CV_GRAY2RGB) ;

	CheckFlag = 0;
	for (Index = 0; Index < 3; Index++) {
		CvScalar TargetColor = cvGet2D (Result, Target[Index].y, Target[Index].x) ;
		
		if ( (TargetColor.val[0] == 0) && (TargetColor.val[1] == 0) && (TargetColor.val[2] == 0) ) {
			//
		} else {
			CheckFlag = 1;
		}
	}

	Sys->Camera.LevelCheck = CheckFlag;
	return Result;
}

int CameraRoutine (void *p)
{
	System *Sys = (System *) p;
	int Index = 0;
	IplImage *LogoImage = cvLoadImage("./pic.jpg", CV_LOAD_IMAGE_COLOR) ;
	CvCapture *VideoIn = cvCaptureFromCAM (0) ;
	IplImage *FrameSrc;
	IplImage *Tmp;
	CvSize SrcSize;
	IplImage *SmoothedImage, *ThresholdImage, *GrayImage;

	
	cvNamedWindow (WINDOW_NAME, CV_WINDOW_AUTOSIZE) ;
	
	cvInitFont (&SysFont, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 1, 0, 8) ;
	
	FrameSrc = cvQueryFrame (VideoIn) ;
	SrcSize = cvGetSize (FrameSrc) ;
	Sys->Camera.Height = SrcSize.height;
	Sys->Camera.Width  = SrcSize.width;

	for (Index = 0; Index < 3; Index++) {
		Target[Index] = cvPoint (230 + (Index * 100),Sys->Camera.Height / 3);
	}
	
	SmoothedImage = cvCreateImage (SrcSize, IPL_DEPTH_8U, 3) ;
	ThresholdImage = cvCreateImage (SrcSize, IPL_DEPTH_8U, 1) ;
	GrayImage = cvCreateImage (SrcSize, IPL_DEPTH_8U, 1) ;
	
	while (Sys->SystemEnd == 0) {
		FrameSrc = cvQueryFrame (VideoIn) ;
		if (FrameSrc == NULL) {
		}
		
		//level check here
		cvSmooth (FrameSrc, SmoothedImage, CV_GAUSSIAN, SmoothSize1, SmoothSize2, SmoothSigma ,0) ;
		cvCvtColor (SmoothedImage, GrayImage, CV_RGB2GRAY) ;
		cvThreshold (GrayImage, ThresholdImage, ThresholdValue, 255, CV_THRESH_BINARY) ;
		Tmp = CameraLevelCheck (p, ThresholdImage) ;
		
		GuiDispSysStatus (p, FrameSrc) ;
		GuiDispCameraStatus (p, FrameSrc) ;
		GuiDispLevelTarget (p, FrameSrc) ;
		cvShowImage (WINDOW_NAME, FrameSrc) ;

		cvReleaseImage (&Tmp) ;
		char c = cvWaitKey (33) ; //ESC
		if (c == 27) {
			Sys->SystemEnd = 1;
			break;
		}
	}
	cvDestroyWindow (WINDOW_NAME) ;
	return 0;
}

