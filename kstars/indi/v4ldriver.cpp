#if 0
    V4L INDI Driver
    INDI Interface for V4L devices
    Copyright (C) 2003 Jasem Mutlaq (mutlaqja@ikarustech.com)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <zlib.h>

#include "webcam/QCamV4L.h"
#include "indidevapi.h"
#include "indicom.h"
#include "fitsrw.h"
#include "eventloop.h"

void ISInit(void);
void ISPoll(void *);
void connectV4L(void);
void updateStream(void * p);
void getBasicData(void);
void uploadFile(const char * filename);
int  writeFITS(const char *filename, char errmsg[]);
int  grabImage(void);
int  checkPowerN(INumberVectorProperty *np);
int  checkPowerS(ISwitchVectorProperty *sp);
int  checkPowerT(ITextVectorProperty *tp);
FITS_HDU_LIST * create_fits_header (FITS_FILE *ofp, uint width, uint height, uint bpp);
void updateImageSettings();


extern char* me;
extern int errno;

#define mydev           "Video4Linux Generic Device"

#define COMM_GROUP	"Main Control"
#define IMAGE_GROUP	"Image Settings"
#define DATA_GROUP      "Data Channel"


#define MAX_PIXELS	4096		/* Max number of pixels in one dimension */
#define POLLMS		1000		/* Polling time (ms) */

#define FILENAMESIZ	2048
#define LIBVERSIZ 	1024
#define PREFIXSIZ	64
#define PIPEBUFSIZ	8192
#define FRAME_ILEN	64

typedef struct {
int  width;
int  height;
int  expose;
unsigned char  *Y;
unsigned char  *U;
unsigned char  *V;
unsigned char  *colorBuffer;
unsigned char  *compressedFrame;
} img_t;

img_t * V4LFrame;			/* V4L frame */
static int streamTimerID;		/* Stream ID */

/*INDI controls */

/* Connect/Disconnect */
static ISwitch PowerS[]          	= {{"CONNECT" , "Connect" , ISS_OFF, 0, 0},{"DISCONNECT", "Disconnect", ISS_ON, 0, 0}};
static ISwitchVectorProperty PowerSP	= { mydev, "CONNECTION" , "Connection", COMM_GROUP, IP_RW, ISR_1OFMANY, 60, IPS_IDLE, PowerS, NARRAY(PowerS), "", 0};

/* Ports */
static IText PortT[]			= {{"PORT", "Port", 0, 0, 0, 0}};
static ITextVectorProperty PortTP	= { mydev, "DEVICE_PORT", "Ports", COMM_GROUP, IP_RW, 0, IPS_IDLE, PortT, NARRAY(PortT), "", 0};

/* Camera Name */
static IText camNameT[]		        = {{"Model", "", 0, 0, 0, 0 }};
static ITextVectorProperty camNameTP    = { mydev, "Camera Model", "", COMM_GROUP, IP_RO, 0, IPS_IDLE, camNameT, NARRAY(camNameT), "", 0};

/* Video Stream */
static ISwitch StreamS[]		= {{"ON", "", ISS_OFF, 0, 0}, {"OFF", "", ISS_ON, 0, 0} };
static ISwitchVectorProperty StreamSP   = { mydev, "VIDEO_STREAM", "Video Stream", COMM_GROUP, IP_RW, ISR_1OFMANY, 0, IPS_IDLE, StreamS, NARRAY(StreamS), "", 0 };

/* Frame Rate */
static INumber FrameRateN[]		= {{"RATE", "Rate", "%0.f", 1., 50., 1., 10., 0, 0, 0}};
static INumberVectorProperty FrameRateNP= { mydev, "FRAME_RATE", "Frame Rate", COMM_GROUP, IP_RW, 60, IPS_IDLE, FrameRateN, NARRAY(FrameRateN), "", 0};

/* Image color */
static ISwitch ImageTypeS[]		= {{ "Grey", "", ISS_ON, 0, 0}, { "Color", "", ISS_OFF, 0, 0 }};
static ISwitchVectorProperty ImageTypeSP= { mydev, "Image Type", "", IMAGE_GROUP, IP_RW, ISR_1OFMANY, 0, IPS_IDLE, ImageTypeS, NARRAY(ImageTypeS), "", 0};

/* Frame dimension */
static INumber ImageSizeN[]		= {{"WIDTH", "Width", "%0.f", 0., 0., 10., 0., 0, 0, 0},
					   {"HEIGHT", "Height", "%0.f", 0., 0., 10., 0., 0, 0, 0}};
static INumberVectorProperty ImageSizeNP = { mydev, "IMAGE_SIZE", "Image Size", IMAGE_GROUP, IP_RW, 60, IPS_IDLE, ImageSizeN, NARRAY(ImageSizeN), "", 0};

/* Exposure time */
static INumber ExposeTimeN[]    = {{ "EXPOSE_S", "Duration (s)", "%5.2f", 1., 1., .0, 1., 0, 0, 0}};
static INumberVectorProperty ExposeTimeNP = { mydev, "EXPOSE_DURATION", "Expose", COMM_GROUP, IP_RW, 60, IPS_IDLE, ExposeTimeN, NARRAY(ExposeTimeN), "", 0};
  
static INumber ImageAdjustN[] = {{"Contrast", "", "%0.f", 0., 256., 1., 0., 0, 0, 0 }, 
                                   {"Brightness", "", "%0.f", 0., 256., 1., 0., 0 ,0 ,0}, 
				   {"Hue", "", "%0.f", 0., 256., 1., 0., 0, 0, 0}, 
				   {"Color", "", "%0.f", 0., 256., 1., 0., 0 , 0 ,0}, 
				   {"Whiteness", "", "%0.f", 0., 256., 1., 0., 0 , 0 ,0}};
				   
static INumberVectorProperty ImageAdjustNP = {mydev, "Image Adjustments", "", IMAGE_GROUP, IP_RW, 0, IPS_IDLE, ImageAdjustN, NARRAY(ImageAdjustN), "", 0 };

/* BLOB for sending image */
static IBLOB imageB = {"CCD1", "Feed", "", 0, 0, 0, 0, 0, 0, 0};
static IBLOBVectorProperty imageBP = {mydev, "Video", "Video", COMM_GROUP,
  IP_RO, 0, IPS_IDLE, &imageB, 1, "", 0};

  
/* send client definitions of all properties */
void ISInit()
{
  static int isInit=0;

 if (isInit)
  return;
 
 V4LFrame = new img_t;
 
 if (V4LFrame == NULL)
 {
   IDMessage(mydev, "Error: unable to initialize driver. Low memory.");
   IDLog("Error: unable to initialize driver. Low memory.");
   return;
 }
 
 streamTimerID = -1;
 
 PortT[0].text     = strcpy(new char[32], "/dev/video0");
 camNameT[0].text  = new char[MAXINDILABEL];
 
 isInit = 1;

}

void ISGetProperties (const char *dev)
{ 

   ISInit();
  
  if (dev && strcmp (mydev, dev))
    return;

  /* COMM_GROUP */
  IDDefSwitch(&PowerSP, NULL);
  IDDefText(&PortTP, NULL);
  IDDefText(&camNameTP, NULL);
  IDDefSwitch(&StreamSP, NULL);
  IDDefNumber(&FrameRateNP, NULL);
  IDDefNumber(&ExposeTimeNP, NULL);
  IDDefBLOB(&imageBP, NULL);
  
  /* Image properties */
  IDDefSwitch(&ImageTypeSP, NULL);
  IDDefNumber(&ImageSizeNP, NULL);
  IDDefNumber(&ImageAdjustNP, NULL);
  
  
}
 
 void ISNewBLOB (const char */*dev*/, const char */*name*/, int */*sizes[]*/, char **/*blobs[]*/, char **/*formats[]*/, char **/*names[]*/, int /*n*/)
{}

void ISNewSwitch (const char *dev, const char *name, ISState *states, char *names[], int n)
{
	
	/* ignore if not ours */
	if (dev && strcmp (dev, mydev))
	    return;
	    
	ISInit();
	
     /* Connection */
     if (!strcmp (name, PowerSP.name))
     {
          IUResetSwitches(&PowerSP);
	  IUUpdateSwitches(&PowerSP, states, names, n);
   	  connectV4L();
	  return;
     }
    
     /* Image Type */
     if (!strcmp(name, ImageTypeSP.name))
     {
       IUResetSwitches(&ImageTypeSP);
       IUUpdateSwitches(&ImageTypeSP, states, names, n);
       ImageTypeSP.s = IPS_OK;
       
       IDSetSwitch(&ImageTypeSP, NULL);
       return;
     }
     
     /* Video Stream */
     if (!strcmp(name, StreamSP.name))
     {
     
      if (checkPowerS(&StreamSP))
         return;
       
       IUResetSwitches(&StreamSP);
       IUUpdateSwitches(&StreamSP, states, names, n);
       StreamSP.s = IPS_IDLE;
       
          
       if (StreamS[0].s == ISS_ON && streamTimerID == -1)
       {
         IDLog("Starting the video stream.\n");
         streamTimerID = addTimer(1000 / (int) FrameRateN[0].value, updateStream, NULL);
	 StreamSP.s  = IPS_BUSY; 
       }
       else
       {
         IDLog("The video stream has been disabled.\n");
	 rmTimer(streamTimerID);
	 streamTimerID = -1;
       }
       
       IDSetSwitch(&StreamSP, NULL);
       return;
     }
     
}

void ISNewText (const char *dev, const char *name, char *texts[], char *names[], int n)
{
	IText *tp;

	ISInit();
 
       /* ignore if not ours */ 
       if (dev && strcmp (mydev, dev))
         return;

	/* suppress warning */
	n=n;
	
	if (!strcmp(name, PortTP.name) )
	{
	  PortTP.s = IPS_OK;
	  tp = IUFindText( &PortTP, names[0] );
	  if (!tp)
	   return;

	  tp->text = new char[strlen(texts[0])+1];
	  strcpy(tp->text, texts[0]);
	  IDSetText (&PortTP, NULL);
	  return;
	}
	
}


void ISNewNumber (const char *dev, const char *name, double values[], char *names[], int n)
{

	/* ignore if not ours */
	if (dev && strcmp (dev, mydev))
	    return;
	    
	ISInit();
    
    /* Frame Size */
    if (!strcmp (ImageSizeNP.name, name))
    {
      if (checkPowerN(&ImageSizeNP))
        return;
	
      ImageSizeNP.s = IPS_OK;
      
      if (IUUpdateNumbers(&ImageSizeNP, values, names, n) < 0)
       return;
      
      if (setSize( (int) ImageSizeN[0].value, (int) ImageSizeN[1].value))
      {
         ImageSizeN[0].value = getWidth();
	 ImageSizeN[1].value = getHeight();
         IDSetNumber(&ImageSizeNP, NULL);
	 return;
      }
      else
      {
        ImageSizeNP.s = IPS_IDLE;
	IDSetNumber(&ImageSizeNP, "Failed to set a new image size.");
      }
      
      return;
   }
   
   /* Frame rate */
   if (!strcmp (FrameRateNP.name, name))
   {
     if (checkPowerN(&FrameRateNP))
      return;
      
     FrameRateNP.s = IPS_IDLE;
     
     if (IUUpdateNumbers(&FrameRateNP, values, names, n) < 0)
       return;
       
     setFPS( (int) FrameRateN[0].value );
     
     FrameRateNP.s = IPS_OK;
     IDSetNumber(&FrameRateNP, NULL);
     return;
   }
   
   if (!strcmp (ImageAdjustNP.name, name))
   {
     if (checkPowerN(&ImageAdjustNP))
       return;
       
     ImageAdjustNP.s = IPS_IDLE;
     
     if (IUUpdateNumbers(&ImageAdjustNP, values, names, n) < 0)
       return;
     
     setContrast( (int) (ImageAdjustN[0].value * 128));
     setBrightness( (int) (ImageAdjustN[1].value * 128));
     setHue( (int) (ImageAdjustN[2].value * 128));
     setColor( (int) (ImageAdjustN[3].value * 128));
     setWhiteness( (int) (ImageAdjustN[4].value * 128));
     
     ImageAdjustN[0].value = getContrast() / 128.;
     ImageAdjustN[1].value = getBrightness() / 128.;
     ImageAdjustN[2].value = getHue() / 128.;
     ImageAdjustN[3].value = getColor() / 128.;
     ImageAdjustN[4].value = getWhiteness() / 128.;
     
     ImageAdjustNP.s = IPS_OK;
     IDSetNumber(&ImageAdjustNP, NULL);
     return;
   }
   
    /* Exposure */
    if (!strcmp (ExposeTimeNP.name, name))
    {
       
       if (checkPowerN(&ExposeTimeNP))
         return;
    
	streamTimerID = -1;
	StreamS[0].s  = ISS_OFF;
	StreamS[1].s  = ISS_ON;
	StreamSP.s    = IPS_IDLE;
	IDSetSwitch(&StreamSP, NULL);
	
	V4LFrame->expose = 1000;
	V4LFrame->Y      = getY();
	V4LFrame->U      = getU();
	V4LFrame->V      = getV();
	
        if (grabImage() < 0)
	{
	 ExposeTimeNP.s   = IPS_IDLE;
	 IDSetNumber(&ExposeTimeNP, NULL);
	}
	  
	
     return;
    } 
      
  
  	
}

/* Downloads the image from the CCD row by row and store them
   in a raw file.
 N.B. No processing is done on the image */
int grabImage()
{
   int err, fd;
   char errmsg[ERRMSG_SIZE];
   char filename[] = "/tmp/fitsXXXXXX";
  
   
   if ((fd = mkstemp(filename)) < 0)
   { 
    IDMessage(mydev, "Error making temporary filename.");
    IDLog("Error making temporary filename.\n");
    return -1;
   }
   close(fd);
  
   err = writeFITS(filename, errmsg);
   if (err)
   {
       IDMessage(mydev, errmsg, NULL);
       return -1;
   }
   
  return 0;
}

int writeFITS(const char * filename, char errmsg[])
{
  FITS_FILE* ofp;
  int i, bpp, bpsl, width, height;
  long nbytes;
  FITS_HDU_LIST *hdu;
  
  IDLog("in writeFITS with filename %s\n", filename);
  
  ofp = fits_open (filename, "w");
  if (!ofp)
  {
    snprintf(errmsg, ERRMSG_SIZE, "Error: cannot open file for writing.");
    return (-1);
  }
  
  width  = getWidth();
  height = getHeight();
  bpp    = 1;                      /* Bytes per Pixel */
  bpsl   = bpp * width;    	   /* Bytes per Line */
  nbytes = 0;
  
  hdu = create_fits_header (ofp, width, height, bpp);
  if (hdu == NULL)
  {
     snprintf(errmsg, ERRMSG_SIZE, "Error: creating FITS header failed.");
     return (-1);
  }
  if (fits_write_header (ofp, hdu) < 0)
  {
    snprintf(errmsg, ERRMSG_SIZE, "Error: writing to FITS header failed.");
    return (-1);
  }
  
  for (i= height - 1; i >=0 ; i--) 
  {
    fwrite(V4LFrame->Y + (i * width), 1, width, ofp->fp);
    nbytes += bpsl;
  }
  
  nbytes = nbytes % FITS_RECORD_SIZE;
  if (nbytes)
  {
    while (nbytes++ < FITS_RECORD_SIZE)
      putc (0, ofp->fp);
  }
  
  if (ferror (ofp->fp))
  {
    snprintf(errmsg, ERRMSG_SIZE, "Error: write error occured");
    return (-1);
  }
 
 fits_close (ofp);      
 
  /* Success */
 ExposeTimeNP.s = IPS_OK;
 IDSetNumber(&ExposeTimeNP, NULL);
 IDLog("Loading FITS image...\n");
 
 uploadFile(filename);
  
 return 0;

}


/* Retrieves basic data from the device upon connection.*/
void getBasicData()
{

  int xmax, ymax, xmin, ymin;
  
  getMaxMinSize(xmax, ymax, xmin, ymin);
  
  /* Width */
  ImageSizeN[0].value = getWidth();
  ImageSizeN[0].min = xmin;
  ImageSizeN[0].max = xmax;
  
  /* Height */
  ImageSizeN[1].value = getHeight();
  ImageSizeN[1].min = ymin;
  ImageSizeN[1].max = ymax;
  
  IUUpdateMinMax(&ImageSizeNP);
  IDSetNumber(&ImageSizeNP, NULL);
  
  strncpy(camNameT[0].text, getDeviceName(), MAXINDILABEL);
  IDSetText(&camNameTP, NULL);
  
  updateImageSettings();
  
}

int checkPowerS(ISwitchVectorProperty *sp)
{
  if (PowerSP.s != IPS_OK)
  {
    if (!strcmp(sp->label, ""))
        IDMessage (mydev, "Cannot change property %s while the camera is offline.", sp->name);
    else
        IDMessage (mydev, "Cannot change property %s while the camera is offline.", sp->label);
    
    sp->s = IPS_IDLE;
    IDSetSwitch(sp, NULL);
    return -1;
  }

  return 0;
}

int checkPowerN(INumberVectorProperty *np)
{
   
  if (PowerSP.s != IPS_OK)
  {
    if (!strcmp(np->label, ""))
        IDMessage (mydev, "Cannot change property %s while the camera is offline.", np->name);
    else
        IDMessage (mydev, "Cannot change property %s while the camera is offline.", np->label);
	
    np->s = IPS_IDLE;
    IDSetNumber(np, NULL);
    return -1;
  }

  return 0;
}

int checkPowerT(ITextVectorProperty *tp)
{

  if (PowerSP.s != IPS_OK)
  {
    if (!strcmp(tp->label, ""))
        IDMessage (mydev, "Cannot change property %s while the camera is offline.", tp->name);
    else
        IDMessage (mydev, "Cannot change property %s while the camera is offline.", tp->label);
	
    tp->s = IPS_IDLE;
    IDSetText(tp, NULL);
    return -1;
  }

  return 0;

}

void connectV4L()
{
  char errmsg[1024];
  
    
  switch (PowerS[0].s)
  {
     case ISS_ON:
      if (connectCam(PortT[0].text, errmsg))
      {
	  PowerSP.s = IPS_IDLE;
	  PowerS[0].s = ISS_OFF;
	  PowerS[1].s = ISS_ON;
	  IDSetSwitch(&PowerSP, "Error: %s", errmsg);
	  IDLog("Error: %s\n", errmsg);
	  return;
      }
      
      /* Sucess! */
      PowerS[0].s = ISS_ON;
      PowerS[1].s = ISS_OFF;
      PowerSP.s = IPS_OK;
      IDSetSwitch(&PowerSP, "Video4Linux Generic Device is online. Retrieving basic data.");
      
      V4LFrame->compressedFrame = (unsigned char *) malloc (sizeof(unsigned char) * 1);
      
      IDLog("V4L Device is online. Retrieving basic data.\n");
      getBasicData();
      
      break;
      
    case ISS_OFF:
      PowerS[0].s = ISS_OFF;
      PowerS[1].s = ISS_ON;
      PowerSP.s = IPS_IDLE;
      
      // Disable stream if running
      if (streamTimerID != -1)
      {
	rmTimer(streamTimerID);
	streamTimerID = -1;
      }
      
      free(V4LFrame->compressedFrame);
      V4LFrame->compressedFrame = NULL;
      disconnectCam();
      
      IDSetSwitch(&PowerSP, "Video4Linux Generic Device is offline.");
      
      break;
     }
}

FITS_HDU_LIST * create_fits_header (FITS_FILE *ofp, uint width, uint height, uint bpp)
{
 FITS_HDU_LIST *hdulist;
 char expose_s[80];
 char obsDate[80];
 char instrumentName[80];
 char ts[32];
	
 struct tm *tp;
 time_t t;
 time (&t);
 tp = gmtime (&t);
 strftime (ts, sizeof(ts), "%Y-%m-%dT%H:%M:%S", tp);
 
 snprintf(instrumentName, 80, "INSTRUME= '%s'", getDeviceName());
 snprintf(obsDate, 80, "DATE-OBS= '%s' /Observation Date UTC", ts);

 hdulist = fits_add_hdu (ofp);
 if (hdulist == NULL) return (NULL);

 hdulist->used.simple = 1;
 hdulist->bitpix = 8;
 hdulist->naxis = 2;
 hdulist->naxisn[0] = width;
 hdulist->naxisn[1] = height;
 hdulist->naxisn[2] = bpp;
 hdulist->used.datamin = 0;
 hdulist->used.datamax = 0;
 hdulist->used.bzero = 1;
 hdulist->bzero = 0.0;
 hdulist->used.bscale = 1;
 hdulist->bscale = 1.0;
 
 snprintf(expose_s, sizeof(expose_s), "EXPOSURE= %d / milliseconds", V4LFrame->expose);
 
 fits_add_card (hdulist, expose_s);
 fits_add_card (hdulist, instrumentName);
 fits_add_card (hdulist, obsDate);
 
 return (hdulist);
}


void updateStream(void *p)
{
 
   int width  = getWidth();
   int height = getHeight();
   uLongf compressedBytes = 0;
   uLong  totalBytes;
   unsigned char *targetFrame;
   int r;
   p=p;
   
   V4LFrame->Y      		= getY();
   V4LFrame->U      		= getU();
   V4LFrame->V      		= getV();
   V4LFrame->colorBuffer 	= getColorBuffer();
  
   totalBytes  = ImageTypeS[0].s == ISS_ON ? width * height : width * height * 4;
   targetFrame = ImageTypeS[0].s == ISS_ON ? V4LFrame->Y : V4LFrame->colorBuffer;
   
   /* Compress frame */
   V4LFrame->compressedFrame = (unsigned char *) realloc (V4LFrame->compressedFrame, sizeof(unsigned char) * totalBytes + totalBytes / 64 + 16 + 3);
   
   compressedBytes = sizeof(unsigned char) * totalBytes + totalBytes / 64 + 16 + 3;
   
   r = compress2(V4LFrame->compressedFrame, &compressedBytes, targetFrame, totalBytes, VIDEO_COMPRESSION_LEVEL);
   if (r != Z_OK)
   {
 	/* this should NEVER happen */
 	IDLog("internal error - compression failed: %d\n", r);
	return;
   }
   
   /* #3 Send it */
   imageB.blob = V4LFrame->compressedFrame;
   imageB.bloblen = compressedBytes;
   imageB.size = totalBytes;
   strcpy(imageB.format, ".stream.z");
   imageBP.s = IPS_OK;
   IDSetBLOB (&imageBP, NULL);
  
  if (streamTimerID != -1)
    	streamTimerID = addTimer(1000/ (int) FrameRateN[0].value, updateStream, NULL);
 
}

void uploadFile(const char * filename)
{
   
   FILE * fitsFile;
   unsigned char *fitsData;
   int r=0;
   unsigned int nr = 0;
   uLong  totalBytes;
   uLongf compressedBytes = 0;
   struct stat stat_p; 
 
   if ( -1 ==  stat (filename, &stat_p))
   { 
     IDLog(" Error occoured attempting to stat %s\n", filename); 
     return; 
   }
   
   totalBytes = stat_p.st_size;
   fitsData = new unsigned char[totalBytes];

   fitsFile = fopen(filename, "r");
   
   if (fitsFile == NULL)
    return;
   
   /* #1 Read file from disk */ 
   for (unsigned int i=0; i < totalBytes; i+= nr)
   {
      nr = fread(fitsData + i, 1, totalBytes - i, fitsFile);
     
     if (nr <= 0)
     {
        IDLog("Error reading temporary FITS file.\n");
        return;
     }
   }
   
   /* #2 Compress it */
   V4LFrame->compressedFrame = (unsigned char *) realloc (V4LFrame->compressedFrame, sizeof(unsigned char) * totalBytes + totalBytes / 64 + 16 + 3);
   
    compressedBytes = sizeof(unsigned char) * totalBytes + totalBytes / 64 + 16 + 3;
     
     
   r = compress2(V4LFrame->compressedFrame, &compressedBytes, fitsData, totalBytes, 9);
   if (r != Z_OK)
   {
 	/* this should NEVER happen */
 	IDLog("internal error - compression failed: %d\n", r);
	return;
   }
   
   /* #3 Send it */
   imageB.blob = V4LFrame->compressedFrame;
   imageB.bloblen = compressedBytes;
   imageB.size = totalBytes;
   strcpy(imageB.format, ".fits.z");
   imageBP.s = IPS_OK;
   IDSetBLOB (&imageBP, NULL);
   
   delete (fitsData);
}

void updateImageSettings()
{

  ImageAdjustN[0].value = getContrast() / 128.;
  ImageAdjustN[1].value = getBrightness() / 128.;
  ImageAdjustN[2].value = getHue() / 128.;
  ImageAdjustN[3].value = getColor() / 128.;
  ImageAdjustN[4].value = getWhiteness() / 128.;

  ImageAdjustNP.s = IPS_OK;
  IDSetNumber(&ImageAdjustNP, NULL);

}

