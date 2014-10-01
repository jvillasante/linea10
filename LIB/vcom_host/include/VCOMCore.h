/******************************<%BEGIN LICENSE%>******************************/
// (c) Copyright 2011 Lumidigm, Inc. (Unpublished Copyright) ALL RIGHTS RESERVED.
//
// For a list of applicable patents and patents pending, visit www.lumidigm.com/patents/
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//
/******************************<%END LICENSE%>******************************/

#ifndef DOXYGEN_SHOULD_SKIP_THIS	// Skip this for auto-docs.

#pragma once

#ifdef __cplusplus
	#define _C_ "C"
#else
	#define _C_
#endif

#ifdef	VCOMCORE_EXPORTS
	#define VCOM_CORE_EXPORT extern _C_ __declspec(dllexport)
	#define _STDCALL
#else
	#define VCOM_CORE_EXPORT extern _C_   // for static lib
	#define _STDCALL
#endif
#endif

#include "V100_shared_types.h"

typedef _V100_GENERAL_ERROR_CODES V100_ERROR_CODE;

/*********************** V100_Get_Num_USB_Devices ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Get_Num_USB_Devices
///  Returns the number of devices attached to the system
///
///  @param [in, out]  nNumDevices int * Number of devices attached to system
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks
///
///  @see
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE _STDCALL V100_Get_Num_USB_Devices(int* nNumDevices);

/*********************** V100_Open ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Open
///  Opens a connection to the device using USB or RS232 interface
///
///  @param [in, out]  pDev V100_DEVICE_TRANSPORT_INFO *    pointer to Device handle
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks An application must call this function to begin communication with a device.
///			  For M31x and V31x devices it may return GEN_ERROR_DEVICE_NOT_READY if the call is made after a V100_Reset. 
///			  If this happens wait for up to a second an try again (these devices may take up to 1.5 seconds to fully restart)
///
///  @see
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE _STDCALL V100_Open( V100_DEVICE_TRANSPORT_INFO *pDev);

/*********************** V100_Close ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Close
///  Closes communication to device
///
///  @param [in, out]  pDev V100_DEVICE_TRANSPORT_INFO *   pointer to Device handle
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks Applications must call this function to close communication with a device before exiting
///
///  @see
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Close(V100_DEVICE_TRANSPORT_INFO *pDev);

/*********************** V100_Capture ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Capture
///  Acquires images, returns composite image(if set), minutia template(if set) and spoof score(if supported)
///
///  @param [in, out]  pDev V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in, out]  pCompositeImage uchar *				pointer to the Composite image to be returned
///  @param [in, out]  nWidth uint &						Composite image width (number of pixels per row)
///  @param [in, out]  nHeight uint &						Composite image height (number of pixels per column)
///  @param [in, out]  pTemplate uchar *					pointer to the minutia template(max 2048 bytes) to be returned
///  @param [in, out]  nTemplateSize uint &					Size of returned template
///  @param [in, out]  Spoof int &							Returned spoof score. -1 if not supported
///  @param [in]       getComposite int						1 get composite image otherwise 0
///  @param [in]       getTemplate int						1 get template otherwise 0
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks Arms device trigger, waits for user presence detection, acquires image and
///			  returns composite image and minutia template if set. Returns spoof score if supported.
///
///  @see V100_Arm_Trigger, V100_Get_Acq_Status, V100_Get_Composite_Image, V100_Get_Template
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Capture(V100_DEVICE_TRANSPORT_INFO *pDev,
											  uchar* pCompositeImage,
											  uint&	 nWidth,
											  uint&  nHeight,
											  uchar* pTemplate,
											  uint&	 nTemplateSize,
											  int&	 Spoof,
											  int	 getComposite,
											  int	 getTemplate);

/*********************** V100_Verify ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Verify
///  Captures image, extracts minutia template, verifies against input template, returns
///  match score and spoof score(if supported).
///
///  @param [in, out]  pDev V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in, out]  pTemplate uchar *					pointer to the minutia template to be verified
///  @param [in]       nTemplateSize uint					Size of the template to be verified
///  @param [in, out]  Spoof int &							Returned spoof score. -1 if not supported
///  @param [in, out]  MatchScore uint &					Returned similarity score ranging from 0 - 100000
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks Arms device, captures an image upon presence detection, extracts template,
///			  verifies against the input template, then returns a match score and spoof score, if supported.
///
///  @see V100_Arm_Trigger, V100_Get_Acq_Status, V100_Get_Template, V100_Match, V100_Match_Ex
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Verify(V100_DEVICE_TRANSPORT_INFO *pDev,
											  uchar* pTemplate,
											  uint	 nTemplateSize,
											  int&	 Spoof,
											  uint&	 MatchScore);

/*********************** V100_Match ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Match
///  Performs similarity match between two minutia templates and returns the score
///
///  @param [in, out]  pDev V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in, out]  pProbeTemplate uchar *				pointer to minutia template1 to be matched
///  @param [in]       nProbeTemplateSize uint				Size of template1 to be matched
///  @param [in, out]  pGalleryTemplate uchar *				pointer to minutia template2 to be matched
///  @param [in]       nGalleryTemplateSize uint			Size of template2 to be matched
///  @param [in, out]  MatchScore uint &					Returned similarity score ranging from 0 - 100000
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks The format of the input templates must correspond to the template mode set, during V100_Set_Option call using OPTION_SET_TEMPLATE_MODE.
///
///  @see V100_Verify, V100_Match_Ex
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Match(V100_DEVICE_TRANSPORT_INFO *pDev,
											  uchar* pProbeTemplate,
											  uint	 nProbeTemplateSize,
											  uchar* pGalleryTemplate,
											  uint	 nGalleryTemplateSize,
											  uint&	 MatchScore);

/*********************** V100_Vid_Stream ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Vid_Stream
///  Sets Video stream mode
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in]       mode _V100_VID_STREAM_MODE				  Turn on/off Video stream mode
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks Set the Video Stream Mode on or off. When video stream is on, one can retrieve the
///           current raw image using V100_Get_Image. It is highly recommended that the only VCOM
///			  calls made to the system between modes V100_Vid_Stream(on) and  V100_Vid_Stream(off) is
///			  V100_Get_Image. Recommended for high-bandwidth transport modes only. The dimensions and
///           image format of the image returned using IMAGE_VID_STREAM as image type with V100_Get_Image
///           can be found by issuing a call to V100_Get_Config, and apply as follows:
///
///			  Width: Native_Image_Size_X
///			  Height: Native_Image_Size_Y
///			  Format: Bayer-pattern BGGR
///
///  @see
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Vid_Stream(const V100_DEVICE_TRANSPORT_INFO *pDev, _V100_VID_STREAM_MODE mode);

/*********************** V100_Get_Image ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Get_Image
///  Returns current image from _V100_IMAGE_TYPE buffer
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in]       type _V100_IMAGE_TYPE					  Type of image to be returned
///  @param [in, out]  pImage uchar *							  pointer to the image to be returned
///  @param [in, out]  nImageSize uint &						  Size of returned image
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks See _V100_IMAGE_TYPE for information on types of images. Valid types of images to get for general
///			  use are IMAGE_COMPOSITE, IMAGE_VID_STREAM and IMAGE_WSQ(M30x, M31x and V31x only). The dimensions and image format of the image returned
///           can be found by issuing a call to V100_Get_Config, and apply as follows:
///
///			  IMAGE_COMPOSITE
///
///			  Width: Composite_Image_Size_X
///			  Height: Composite_Image_Size_Y
///			  Format: 8-BPP monochrome
///
///			  IMAGE_VID_STREAM
///
///			  Width: Native_Image_Size_X
///			  Height: Native_Image_Size_Y
///			  Format: Bayer-pattern BGGR
///
///			  IMAGE_WSQ
///
///			  The size of the image buffer client needs to allocated is Composite_Image_Size_X*Composite_Image_Size_Y bytes. The actual size of the 
///			  WSQ image returned will be nImageSize. Client can set the wsq compression ratio using V100_Set_Option call with 
///			  OPTION_SET_WSQ_COMPRESSION_RATIO. Default Compression ratio used is 11:1( bit rate of 0.7273 ).	
///
///  @see V100_Get_Composite_Image
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Get_Image(const V100_DEVICE_TRANSPORT_INFO *pDev, _V100_IMAGE_TYPE type, uchar* pImage, uint& nImageSize);

/*********************** V100_Get_Composite_Image ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Get_Composite_Image
///  Returns the latest Composite Image from Compostie image buffer and associated spoof value
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in, out]  pImage uchar *							  pointer to the Composite image to be returned
///  @param [in, out]  SpoofValue int *							  pointer to the returned spoof value. -1 if not supported
///  @param [in, out]  nImageSize uint *						  pointer to the size of returned Composite image
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks Returned Composite image is the last composite image processed. The dimensions and image format of the image returned
///           can be found by issuing a call to V100_Get_Config, and apply as follows:
///
///			  Width: Composite_Image_Size_X
///			  Height: Composite_Image_Size_Y
///			  Format: 8-BPP monochrome
///
///  @see V100_Get_Template, V100_Get_Image
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Get_Composite_Image(const V100_DEVICE_TRANSPORT_INFO *pDev, uchar* pImage, int* SpoofValue, uint* nImageSize);

/*********************** V100_Set_Composite_Image ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Set_Composite_Image
///  Sets the current composite image buffer
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in, out]  pImage uchar *							  pointer to the composite image to write to composite image buffer
///  @param [in]       nImageSize uint							  Size of composite image
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks The dimensions and image format of the image set must be the same as the size
///           returned by V100_Get_Config, and apply as follows:
///
///			  Width: Composite_Image_Size_X
///			  Height: Composite_Image_Size_Y
///			  Format: 8-BPP monochrome
///
///  @see V100_Set_Image
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Set_Composite_Image(const V100_DEVICE_TRANSPORT_INFO *pDev, uchar* pImage, uint nImageSize);

/*********************** V100_Get_FIR_Image ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Get_FIR_Image
///  Returns the latest Composite Image in FIR(Finger Image Record) format 
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *		pointer to device handle
///  @param [in]       FIRType _V100_FIR_RECORD_TYPE				Type of FIR record to be returned
///  @param [in]       FingerType _V100_FINGER_PALM_POSITION		Type of finger to be set
///  @param [in, out]  pFIRImage uchar *							pointer to the FIR image to be returned							
///  @param [in, out]  nFIRImageSize uint *							pointer to the size of FIRImage allocated/returned
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks Returned FIR image is the last composite image processed and packed in FIR format. 
///			  Client must allocate the pFIRImage buffer with size as follows.
///
///			  For LUMI_FIR_ANSI: nFIRImageSize = ANSI_381_HDR_SIZE + Composite Image size 
///			  For LUMI_FIR_ISO:	 nFIRImageSize = ISO_19794_4_HDR_SIZE + Composite Image size		
///
///			  The dimensions of the Composite image can be found by issuing a call to V100_Get_Config, and apply as follows:
///			  Width: Composite_Image_Size_X
///			  Height: Composite_Image_Size_Y
///			  Format: 8-BPP monochrome
///
///			 FIR types supported are 
///			 LUMI_FIR_ISO	// ISO/IEC 19794-4:2005
///			 LUMI_FIR_ANSI	// ANSI/INCITS 381-2004
///
///  @see V100_Get_Config
///
///  @author www.lumidigm.com @date 5/19/2010
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Get_FIR_Image(const V100_DEVICE_TRANSPORT_INFO *pDev, _V100_FIR_RECORD_TYPE FIRType, _V100_FINGER_PALM_POSITION FingerType, uchar* pFIRImage, uint* nFIRImageSize);

/*********************** V100_Get_Template ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Get_Template
///  Returns the minutia Template in the current Template buffer. This was the last template processed.
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in, out]  pTemplate uchar *						  pointer to the minutia template(max 2048 bytes) to be returned
///  @param [in, out]  nTemplateSize uint *						  pointer to the size of returned template
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks Gets the latest extracted template from the device from probe template buffer. The format of the template returned 
///			  corresponds to the template mode set, during V100_Set_Option call using OPTION_SET_TEMPLATE_MODE. 
///
///  @see
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Get_Template(const V100_DEVICE_TRANSPORT_INFO *pDev, uchar* pTemplate, uint* nTemplateSize);

/*********************** V100_Set_Template ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Set_Template
///  Downloads the minutia Template to current template buffer
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in, out]  pTemplate uchar *						  pointer to the minutia template to write to template buffer
///  @param [in]       nTemplateSize uint						  Size of the template to write to template buffer
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks Sets the gallery template buffer. The format of the input template must correspond to the template mode set, 
///			  during V100_Set_Option call using OPTION_SET_TEMPLATE_MODE.
///
///  @see
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Set_Template(const V100_DEVICE_TRANSPORT_INFO *pDev, uchar* pTemplate, uint nTemplateSize);

/*********************** V100_Arm_Trigger ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Arm_Trigger
///  Starts presence detection and image stack acquisition
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in]	   mode _V100_TRIGGER_MODE					  Type of trigger mode to set
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///   @remarks See _V100_TRIGGER_MODE for information on types of trigger modes
///				
///			TRIGGER_ON:				Arms the device trigger, and starts the processing chain as described in the _V100_INTERFACE_COMMAND_TYPE structure. 
///			TRIGGER_OFF:			If presence detection is running, this disarms the trigger and returns the device to an idle state. 
///									It returns GEN_OK or GEN_ERROR_APP_BUSY if the system is busy. In either case user must poll for completion using 
///									V100_Get_OP_Status for macro commands and V100_Get_Acq_Status for atomic commands.
///			TRIGGER_FINGER_DETECT:  Arms the device trigger and starts finger detection mode. This mode is supported for M30x, M31x and V31x sensors only. 
///									User can poll for status using CMD_GET_ACQ_STATUS for finger presence or not.
///
///  @see V100_Get_OP_Status, V100_Get_Acq_Status
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Arm_Trigger(const V100_DEVICE_TRANSPORT_INFO *pDev, _V100_TRIGGER_MODE mode);

/*********************** V100_Get_Acq_Status ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Get_Acq_Status
/// Returns status of current acquisition
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in, out]  pACQ_Status _V100_ACQ_STATUS_TYPE *		  pointer to acquisition status to be returned
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks See _V100_ACQ_STATUS_TYPE for information on types of acquisition status.
///
///  @see V100_Capture
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Get_Acq_Status(const V100_DEVICE_TRANSPORT_INFO *pDev, _V100_ACQ_STATUS_TYPE* pACQ_Status);

/*********************** V100_Get_Config ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Get_Config
///  Returns device Configuration structure which includes definitions of all supported services
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *     pointer to device handle
///  @param [in, out]  ICT _V100_INTERFACE_CONFIGURATION_TYPE *    pointer to Configuration structure to be returned
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks See _V100_INTERFACE_CONFIGURATION_TYPE structure for information on Configuration structure
///
///  @see
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Get_Config(const V100_DEVICE_TRANSPORT_INFO *pDev, _V100_INTERFACE_CONFIGURATION_TYPE* ICT);

/*********************** V100_Get_Status ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Get_Status
///  Returns device Status structure which contains all device error codes, conditions and health monitoring data
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in, out]  pIST _V100_INTERFACE_STATUS_TYPE *		  pointer to Status structure to be returned
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks See _V100_INTERFACE_STATUS_TYPE structure for information on Status structure
///
///  @see
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Get_Status(const V100_DEVICE_TRANSPORT_INFO *pDev, _V100_INTERFACE_STATUS_TYPE* pIST) ;

/*********************** V100_Get_Cmd ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Get_Cmd
///  Returns Command structure
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in, out]  pCmd _V100_INTERFACE_COMMAND_TYPE *		  pointer to Command structure to be returned
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks Returns current settings of USER controllable features. See _V100_INTERFACE_COMMAND_TYPE
///			  for information on Command Structure
///
///  @see
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Get_Cmd(const V100_DEVICE_TRANSPORT_INFO *pDev, _V100_INTERFACE_COMMAND_TYPE* pCmd);

/*********************** V100_Set_Cmd ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Set_Cmd
///  Sets Command Structure
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in, out]  pCmd _V100_INTERFACE_COMMAND_TYPE *		  pointer to Command structure
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks Sets current settings of USER controllable features. See _V100_INTERFACE_COMMAND_TYPE
///			  for information on Command Structure
///
///  @see
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Set_Cmd(const V100_DEVICE_TRANSPORT_INFO *pDev, _V100_INTERFACE_COMMAND_TYPE* pCmd);

/*********************** V100_Set_LED ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Set_LED
///  Sets user feedback LED's on/off
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in]       Control _V100_LED_CONTROL				  LED to control
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks Permits manual on/off control of the user feedback LED's. See _V100_LED_CONTROL
///			  for information on LED controls
///
///  @see
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Set_LED(const V100_DEVICE_TRANSPORT_INFO *pDev, _V100_LED_CONTROL Control);

/*********************** V100_Get_Serial ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Get_Serial
///  Returns device serial number
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in, out]  pSerialNumber uint *		  pointer to serial number to be returned
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks Overrides serial number returned in _V100_INTERFACE_CONFIGURATION_TYPE structure
///           fields 'Device_Serial_Number' and 'Device_Serial_Number_Ex'.  This method of
///	          obtaining the serial number is preferred, but they are equivalent.
///
///  @see
///
///  @author www.lumidigm.com @date 6/11/2012
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Get_Serial(const V100_DEVICE_TRANSPORT_INFO *pDev, uint* pSerialNumber) ;

/*********************** V100_Config_Comport ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Config_Comport
///  Used to change the baud rate, data bits and flow control settings of the serial communication channel
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in]       nBaudRate uint							  Baud rate to set.
///																  Selectable(9600, 19200, 38400, 57600, 115200, 230400)
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks Only supported for RS232 communication
///
///  @see
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Config_Comport(const V100_DEVICE_TRANSPORT_INFO *pDev, uint nBaudRate);

/*********************** V100_Reset ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Reset
///  Reboots the device
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks Issues a System Reset(Reboot) command to the device
///
///  @see
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Reset(const V100_DEVICE_TRANSPORT_INFO *pDev);

/*********************** V100_Set_Option ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Set_Option
///  Sets system options
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in]       OptType _V100_OPTION_TYPE				  Type of Option that will be set
///  @param [in, out]  pData uchar *							  pointer to Option data
///  @param [in]       nDataSize uint							  Size of Option data
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks See _V100_OPTION_TYPE for information on types of options. Supported options include
///			  OPTION_PD_LEVEL, OPTION_SET_TEMPLATE_MODE((M30x, M31x and V31x) and OPTION_SET_WSQ_COMPRESSION_LEVEL(M30x, M31x and V31x only)
///			  Template types supported using OPTION_SET_TEMPLATE_MODE are as follows
///
///				TEMPLATE_ANSI_378	ANSI/INCITS 378-2004
///				TEMPLATE_ISO_NORMAL	ISO/IEC 19794-2:2005
///				
///			  The format of the input/output template for the following commands corresponds to template mode set using V100_Set_Option call. 
///			  The default template mode will be TEMPLATE_ANSI_378.
///
///				CMD_GET_TEMPLATE
///				CMD_MATCH
///				CMD_MATCH_EX
///				CMD_SET_TEMPLATE
///				CMD_TRUNCATE_378
///				CMD_ID_GET_USER_RECORD
///				CMD_ID_IDENTIFY_378
///				CMD_ID_SET_USER_RECORD
///				CMD_ID_VERIFY_378
///				CMD_ID_VERIFY_MANY
///				
///			  Using OPTION_SET_WSQ_COMPRESSION_RATIO the compression level can be set from 2 to MAX_WSQ_COMPRESSION_RATIO. Compression level should be provided in
///			  unsigned integer value. The default compression ratio is 11:1.
///
///			  User can turn On/Off latent detection using OPTION_SET_LATENT_DETECTION_MODE as follows:
///				LATENT_DETECTION_OFF	Turns off latent detection
///				LATENT_DETECTION_ON		Turns on latent detection
///
///			  For M30x sensors user can On/Off the force finger lift for enrollment using V100_Set_Option call with OPTION_SET_FORCE_FINGER_LIFT_MODE as follows:
///				FORCE_FINGER_LIFT_MODE_ON	Turns on force finger lift mode. User required to lift finger after each capture during enrollment.
///				FORCE_FINGER_LIFT_MODE_OFF  Turns off force finger lift mode. This is the default behavior for M30x sensors.
///  @see
///
///  @author www.lumidigm.com @date 6/12/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Set_Option(const V100_DEVICE_TRANSPORT_INFO *pDev, _V100_OPTION_TYPE OptType, uchar* pData, uint nDataSize);

/*********************** V100_Match_Ex ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Match_Ex
///  Matches one or two templates, returns match score and spoof score
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in, out]  Tpl1 uchar *								  pointer to template1 to be matched
///  @param [in]       nSzT1 uint								  Size of template1 to be matched
///  @param [in, out]  Tpl2 uchar *								  pointer to template2 to be matched
///  @param [in]       nSzT2 uint								  Size of template2 to be matched
///  @param [in, out]  nMatchScore uint &						  Returned similarity score ranging from 0 - 100000
///  @param [in, out]  nSpoofScore uint &						  Spoof Not Supported, returns -1
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks The format of the input templates must correspond to the template mode set, during V100_Set_Option call using OPTION_SET_TEMPLATE_MODE.
///
///
///  @see V100_Match
///
///  @author www.lumidigm.com @date 1/24/2007
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Match_Ex(const V100_DEVICE_TRANSPORT_INFO *pDev, uchar* Tpl1, uint nSzT1, uchar* Tpl2, uint nSzT2, int& nMatchScore, int& nSpoofScore);

/*********************** V100_Set_Tag ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Set_Tag
///  Sets a tag which persists on the file system, to be retrieved using V100_Get_Tag
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in]  pTag char *    Opaque data array of tag to store.
///  @param [in]       nTagLength ushort    size of tag, in bytes
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful.
///
///  @remarks Maximum length of 255 bytes.
///
///  @see V100_Get_Tag
///
///  @author www.Lumidigm.com @date 7/25/2008
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Set_Tag(const V100_DEVICE_TRANSPORT_INFO *pDev, uchar* pTag, ushort  nTagLength);

/*********************** V100_Get_Tag ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Get_Tag
///  Retrieves tag data set by V100_Set_Tag
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in, out]  pTag char *			  data buffer which stores tag, upon success.
///  @param [out]	   nTagLength ushort &    size of data returned, in bytes
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful.
///
///  @remarks If tag size is unknown create output buffer with maximum length of 255 bytes.
///
///  @see V100_Set_Tag
///
///  @author www.Lumidigm.com @date 7/25/2008
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Get_Tag(const V100_DEVICE_TRANSPORT_INFO *pDev, uchar* pTag, ushort& nTagLength);

/*********************** V100_Truncate_378 ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Truncate_378
///  Truncates a Template obtained from call to V100_Get_Template
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *		pointer to device handle
///  @param [in]       nMaxTemplateSize uint						Size to be truncated to.
///  @param [in]       pInTemplate const uchar *					Template to be truncated. 
///  @param [in]       nTplSize uint								Size of template to be truncated
///  @param [in, out]  pOutTemplate uchar *							Output template buffer (must be allocated)
///  @param [in, out]  nActualSize uint &							Size of output template.
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful.
///
///  @remarks The format of the input template must correspond to the template mode set, during V100_Set_Option call using OPTION_SET_TEMPLATE_MODE.
///
///  @see V100_Get_Template
///
///  @author www.Lumidigm.com @date 7/25/2008
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Truncate_378(const V100_DEVICE_TRANSPORT_INFO *pDev, uint nMaxTemplateSize, const uchar* pInTemplate, uint nTplSize, uchar* pOutTemplate, uint& nActualSize);

/*********************** V100_Get_OP_Status ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Get_OP_Status
///  Retrives current status of Macro operation.
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [out]	   opStatus _V100_OP_STATUS &    OP status - returned upon success.
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful.
///
///  @remarks None.
///
///  @see V100_Enroll_User, V100_Verify_User, V100_Format_DB, V100_ID_Create_DB, V100_ID_Set_Working_DB, V100_ID_Identify, V100_ID_Identify_378,
///		  V100_ID_Verify_User_Record, V100_ID_Enroll_User_Record,
///		  V100_ID_Delete_DB
///
///  @author www.Lumidigm.com @date 6/30/2008
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Get_OP_Status(const V100_DEVICE_TRANSPORT_INFO *pDev, _V100_OP_STATUS& opStatus);

/*********************** V100_Save_Last_Capture ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_ID_Save_Last_Capture
///  Saves last transaction on the micro-sd card
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in]       Record _MX00_SAVE_CAPTURE				  Record to save
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks To check completion of this opertion, poll using V100_Get_OP_Status. Currently not supported
///
///  @see None.
///
///  @author www.Lumidigm.com @date 8/17/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Save_Last_Capture(const V100_DEVICE_TRANSPORT_INFO *pDev, _MX00_SAVE_CAPTURE Record);

/*********************** V100_Update_Firmware ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Update_Firmware
///  Allows user to update firmware on the unit
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *		pointer to device handle
///  @param [in, out]  pFirmwareStream uchar *						Firmware stream
///  @param [in]       nFWStreamSize uint							Size of Firmware stream in bytes
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks To check completion of this opertion, poll using V100_Get_OP_Status.
///
///  @see None.
///
///  @author www.Lumidigm.com @date 8/17/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Update_Firmware(const V100_DEVICE_TRANSPORT_INFO *pDev, uchar* pFirmwareStream, uint nFWStreamSize);

/*********************** V100_Set_GPIO ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Set_GPIO
///  Sets GPIO mask
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in]       mask uchar								  GPIO mask to set
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful.
///
///  @remarks None.
///
///  @see V100_Get_GPIO
///
///  @author www.Lumidigm.com @date 10/15/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Set_GPIO(const V100_DEVICE_TRANSPORT_INFO *pDev, uchar mask);

/*********************** V100_Get_GPIO ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Get_GPIO
///  Gets GPIO mask
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in, out]  mask uchar &								  If successful returns GPIO mask
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful.
///
///  @remarks None.
///
///  @see V100_Set_GPIO
///
///  @author www.Lumidigm.com @date 10/15/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Get_GPIO(const V100_DEVICE_TRANSPORT_INFO *pDev, uchar& mask);

/*********************** V100_Cancel_Operation ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Cancel_Operation
///  Cancels capture-related commands
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful.
///
///  @remarks You can cancel V100_Arm_Trigger, V100_ID_Identify, V100_ID_Verify_User_Record, V100_ID_Enroll_User_Record
///			  and V100_ID_Verify_Many using this command. It returns GEN_OK or GEN_ERROR_APP_BUSY if the system is busy. 
///			  In either case user must poll for completion using V100_Get_OP_Status for macro commands and V100_Get_Acq_Status
///			  for atomic commands.
///
///  @see  V100_Arm_Trigger, V100_ID_Identify, V100_ID_Verify_User_Record, V100_ID_Enroll_User_Record, V100_ID_Verify_Many, 
///		   V100_Get_OP_Status, V100_Get_Acq_Status
///
///  @author www.Lumidigm.com @date 10/15/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Cancel_Operation(const V100_DEVICE_TRANSPORT_INFO *pDev);

/*********************** V100_WaitForFingerClear ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_WaitForFingerClear
///  Arms device trigger to finger detection mode and waits for finger clear
///
///  @param [in, out]  pDev V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of 
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks This call can be used with V100_ID_Identify, V100_ID_Verify_User_Record, V100_Capture, V100_Verify to make sure there  
///			  is no valid finger placement on device platen. This call is supported for M30x, M31x and V31x sensors only.
///
///  @see V100_Arm_Trigger, V100_Get_Acq_Status, V100_ID_Identify, V100_ID_Verify_User_Record, V100_Capture, V100_Verify
///
///  @author www.lumidigm.com @date 2/23/2011
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_WaitForFingerClear(V100_DEVICE_TRANSPORT_INFO *pDev);

/************** 1-1 Verification (VenusOnly)****************/
/*********************** V100_Set_Verification_Rules ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Set_Verification_Rules
///  Sets rules for enrollment and verification.
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in]       verRules _V100_VERIFICATION_RULES    Verification Rules Structure
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful.
///
///  @remarks None.
///
///  @see None.
///
///  @author www.Lumidigm.com @date 6/28/2008
///////////////////////////////////////////////////////////////////////////////
VCOM_CORE_EXPORT V100_ERROR_CODE V100_Set_Verification_Rules(const V100_DEVICE_TRANSPORT_INFO *pDev, _V100_VERIFICATION_RULES  verRules);

/*********************** V100_Get_Verification_Rules ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Get_Verification_Rules
///  Gets rules for enrollment and verification.
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in, out]  verRules _V100_VERIFICATION_RULES &    Verification Rules Structure
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful.
///
///  @remarks None.
///
///  @see
///
///  @author www.Lumidigm.com @date 6/28/2008
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Get_Verification_Rules(const V100_DEVICE_TRANSPORT_INFO *pDev, _V100_VERIFICATION_RULES& verRules);

/*********************** V100_Enroll_User ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Enroll_User
///  Requests that the User described in UserRecord is enrolled.
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in]       UserRecord _V100_USER_RECORD				  User Record describing Enrollment ID and Metadata.
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful.
///
///  @remarks Client must poll for completion using V100_Get_OP_Status.
///
///  @see V100_Get_OP_Status, V100_Verify_User
///
///  @author www.Lumidigm.com @date 6/28/2008
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Enroll_User(const V100_DEVICE_TRANSPORT_INFO *pDev, _V100_USER_RECORD UserRecord);

/*********************** V100_Verify_User ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Verify_User
///  Verifies against user described in User Record.
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in]       UserRecord _V100_USER_RECORD				  User Record describing Enrollment ID and Metadata.
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful.
///
///  @remarks Client must poll for completion using V100_Get_OP_Status.
///
///  @see V100_Get_OP_Status, V100_Enroll
///
///  @author www.Lumidigm.com @date 6/28/2008
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Verify_User(const V100_DEVICE_TRANSPORT_INFO *pDev, _V100_USER_RECORD UserRecord);

/*********************** V100_Delete_User ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Delete_User
///  Deletes a user record from the database.
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in]       UserRecord _V100_USER_RECORD    User Record describing which Record to delete.
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful.
///
///  @remarks UID member must be populated.  All other members in UserRecord are ignored.
///
///  @see V100_Get_OP_Status, V100_Get_DB_Metrics
///
///  @author www.Lumidigm.com @date 6/28/2008
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Delete_User(const V100_DEVICE_TRANSPORT_INFO *pDev, _V100_USER_RECORD UserRecord);

/*********************** V100_Get_User ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Get_User
///  Gets a user from the database, based on UserRecord identifier.
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in, out]  pUserRecord _V100_USER_RECORD *   User Record structure, IN/OUT parameter.
///  @param [in, out]  pRecordData char *    Opaque record data structure.
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful.
///
///  @remarks None.
///
///  @see UID member must be populated.  All other members in UserRecord are ignored.
///
///  @author www.Lumidigm.com @date 6/28/2008
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Get_User(const V100_DEVICE_TRANSPORT_INFO *pDev, _V100_USER_RECORD* pUserRecord, uchar* pRecordData);

/*********************** V100_Get_User_By_Index ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Get_User_By_Index
///  Gets an existing user record, based on zero-based index.
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///	 @param [in]	   nIndex uint						Index of user to get.
///  @param [in, out]  pUserRecord _V100_USER_RECORD *   Contains User Record upon success
///  @param [in, out]  pRecordData char *    Contains User Record data upon success.
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful.
///
///  @remarks None.
///
///  @see V100_Get_User_By_Index
///
///  @author www.Lumidigm.com @date 6/28/2008
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Get_User_By_Index(const V100_DEVICE_TRANSPORT_INFO *pDev, uint nIndex, _V100_USER_RECORD* pUserRecord, uchar* pRecordData);

/*********************** V100_Add_User ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Add_User
///  Adds a user to a device, user having been previously retrieved using
///	 V100_Get_User_By_Index, or V100_Get_User calls.
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in]       UserRecord _V100_USER_RECORD    User Record to Add
///  @param [in, out]  pRecordData char *    Opaque User record data to Add.
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful.
///
///  @remarks User is commited to DB by the time the call returns.
///
///  @see
///
///  @author www.Lumidigm.com @date 6/28/2008
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Add_User(const V100_DEVICE_TRANSPORT_INFO *pDev, _V100_USER_RECORD UserRecord, uchar* pRecordData);

/*********************** V100_Get_DB_Metrics ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Get_DB_Metrics
///  Retrieves database metrics structure, which includes size of database, maximum
///  number of records in database, last spoof score, last match score.
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in, out]  dbMetrics _V100_DB_METRICS &    DB Metrics structure.
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful.
///
///  @remarks Useful for database management, and for determining index range for command
///			  V100_Get_User_By_Index.
///
///  @see V100_Get_User_By_Index
///
///  @author www.Lumidigm.com @date 6/28/2008
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Get_DB_Metrics(const V100_DEVICE_TRANSPORT_INFO *pDev, _V100_DB_METRICS* dbMetrics);

/*********************** V100_Format_DB ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Format_DB
///  Erases all User Records in database.   Does not erase Tags generated by calls
///	 to V100_Set_Tag.
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful.
///
///  @remarks Client must poll for completion using V100_Get_OP_Status.  nParamater in
///	  _V100_OP_STATUS determines percentage completion of formatting command.  May take up to
///	  30 seconds to complete on high-capacity units.
///
///  @see V100_Get_OP_Status
///
///  @author www.Lumidigm.com @date 6/28/2008
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Format_DB(const V100_DEVICE_TRANSPORT_INFO *pDev);


/************** End of 1-1 VERIFICATION (Venus Only)****************/




/************** 1-N IDENTIFICATION (Mercury Only)****************/


/*********************** V100_ID_Create_DB ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_ID_Create_DB
///  Creates a new database.
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *		pointer to device handle
///  @param [in]       dbInitStructIn _MX00_DB_INIT_STRUCT			DB Initialization structure
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks See _MX00_DB_INIT_STRUCT for information on DB initialization structure. Identification capable
///			  databases require enrolling 3 instances per finger for maximum identification performance.
///			  One must use V100_Get_OP_Status to poll for operation completion and/or error codes
///			  that occured during the database creation process.
///
///  @see V100_ID_Delete_DB, V100_Get_OP_Status
///
///  @author www.Lumidigm.com @date 8/3/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_ID_Create_DB(const V100_DEVICE_TRANSPORT_INFO *pDev, _MX00_DB_INIT_STRUCT dbInitStructIn);

/*********************** V100_ID_Delete_DB ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_ID_Delete_DB
///  Deletes specified database/group
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *		pointer to device handle
///  @param [in]       nDbNo uint									Database/Group no. to delete.
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks To check completion of this opertion, poll using V100_Get_OP_Status. If nDbNo specified is currently set using V100_ID_Set_Working_DB,
///			  the database is unloaded from memory.
///
///  @see V100_Get_OP_Status, V100_ID_Create_DB
///
///  @author www.Lumidigm.com @date 8/3/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_ID_Delete_DB(const V100_DEVICE_TRANSPORT_INFO *pDev, uint nDbNo);

/*********************** V100_ID_Set_Working_DB ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_ID_Set_Working_DB
///  Sets the working database. Database must have been created using V100_ID_Create_DB for this command to succeed.
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *		pointer to device handle
///  @param [in]       nDB uint										Working Database/Group number to set
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks If database is empty this command will allow to enroll users into database using  V100_ID_Enroll_User_Record or
///			  will allow to add existing records using V100_ID_Set_User_Record. If this database is populated, it will load
///			  in to active memory. Once database is in active memory, V100_ID_Identify, or V100_ID_Identify_378, may be used
///			  to identify. To check progress/completion of this opertion, poll using V100_Get_OP_Status.
///
///  @see V100_ID_Create_DB, V100_ID_Enroll_User_Record, V100_ID_Set_User_Record, V100_ID_Identify, V100_ID_Identify_378, V100_Get_OP_Status
///
///  @author www.Lumidigm.com @date 8/3/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_ID_Set_Working_DB(const V100_DEVICE_TRANSPORT_INFO *pDev, uint nDB);

/*********************** V100_ID_Get_User_Record ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_ID_Get_User_Record
///  Retrieves the User record from a database
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *		pointer to device handle
///  @param [in]       nIndex short									Index of User Record to retrieve.
///																	If nIndex = -1, the structure the rec field contains is used to indicate
///																	which user-finger to retrieve. If nIndex = n,  this will retrieve the
///																	user-finger stored at 0-based index n, where n is between 0 to
///																	nCurEnrolledUserFingers field retrieved during a call to V100_ID_Get_DB_Metrics
///  @param [in, out]  rec _MX00_ID_USER_RECORD &					Input: nInstances member is ignored. If nIndex is positive then szUserID, nFinger is ignored.
///																	Output: Contains User Record header structure upon success. nInstances member
///																	of this structure defines how many _MX00_TEMPLATE_INSTANCE structures follow
///																	this structure
///  @param [in, out]  instanceRecords _MX00_TEMPLATE_INSTANCE[]	Contains nInstances of _MX00_TEMPLATE_INSTANCE structures 
///																	which hold the templates upon success. The format of the templates returned corresponds 
///																	to the template mode set, during V100_Set_Option call using OPTION_SET_TEMPLATE_MODE.
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks This command is useful for retrieving user-finger entries in the database sequentially, when
///			  user-finger information is unknown.
///
///  @see V100_ID_Get_DB_Metrics
///
///  @author www.Lumidigm.com @date 8/3/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_ID_Get_User_Record(const V100_DEVICE_TRANSPORT_INFO *pDev, short nIndex, _MX00_ID_USER_RECORD& rec, _MX00_TEMPLATE_INSTANCE instanceRecords[]);

/*********************** V100_ID_Get_User_Record_Header ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_ID_Get_User_Record_Header
///  Retrieves the User record header from working database 
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *		pointer to device handle
///  @param [in]       nIndex short									Index of User Record to retrieve. 
///																	This will retrieve the user-finger header stored at 0-based index n, where n is between 0 to 
///																	nCurEnrolledUserFingers field retrieved during a call to V100_ID_Get_DB_Metrics
///  @param [in, out]  rec _MX00_ID_USER_RECORD &					Contains User Record header structure upon success.  
///																	
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of 
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks This command is useful for retrieving user-finger header entries in the database sequentially, when 
///			  user-finger information is unknown. 
///
///  @see V100_ID_Get_DB_Metrics
///
///  @author www.Lumidigm.com @date 8/3/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_ID_Get_User_Record_Header(const V100_DEVICE_TRANSPORT_INFO *pDev, short nIndex, _MX00_ID_USER_RECORD& rec);

/*********************** V100_ID_Set_User_Record ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_ID_Set_User_Record
///  Adds an existing User Record to database.
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *				pointer to device handle
///  @param [in]       rec const _MX00_ID_USER_RECORD						User Record header struture to add. The nInstances
///																			member of this structure defines how many _MX00_TEMPLATE_INSTANCE
///																			structures follow this structure.
///  @param [in]       instanceRecords const _MX00_TEMPLATE_INSTANCE[]		nInstances of _MX00_TEMPLATE_INSTANCE structures which hold the templates.
///																			The format of the input templates must correspond to the template mode set, 
///																			during V100_Set_Option call using OPTION_SET_TEMPLATE_MODE.
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks This command is useful for adding existing user records enrolled from another sensor. If the database specified in nGroupID member of
///			  _MX00_ID_USER_RECORD has multiple instances per user-finger and the nFlags set during the V100_ID_Set_Parameters call contains the
///			  FLAG_ENROLLMENT_QUALIFICATION flag, each template instance is checked against each other template in order to ensure enrollment quality.
///			  The FLAG_ENROLLMENT_QUALIFICATION is a requirement while adding user record into any database which is capable of identification. This is a
///			  requirement to ensure good identification  performance. After adding all the user records using this command call V100_ID_Set_Working_DB 
///			  in order to commit all the records to the device
///
///  @see V100_ID_Enroll_User_Record, V100_ID_Set_Parameters, V100_ID_Set_Working_DB
///
///  @author www.Lumidigm.com @date 8/3/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_ID_Set_User_Record(const V100_DEVICE_TRANSPORT_INFO *pDev, const _MX00_ID_USER_RECORD rec, const _MX00_TEMPLATE_INSTANCE instanceRecords[]);

/*********************** V100_ID_Delete_User_Record ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_ID_Delete_User_Record
///  Deletes a User or User Record from a database. Because one can enroll multiple fingers per user,
///  V100_ID_Delete_User_Record allows to choose whether to remove a user completley or whether to remove user-finger record.
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *		pointer to device handle
///  @param [in]       rec const _MX00_ID_USER_RECORD				User record to delete
///  @param [in]       nDeleteAllFingers bool						If true deletes all user records corresponding to the user
///																	field in _MX00_ID_USER_RECORD. If false deletes user-finger
///																	specified in _MX00_ID_USER_RECORD.
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks If the database specified in _MX00_ID_USER_RECORD is currently loaded then it is unloaded from memory during this call.
///			  If desired client has to call V100_ID_Set_Working_DB to load the database into memory after this call.
///
///  @see V100_ID_Set_Working_DB
///
///  @author www.Lumidigm.com @date 8/3/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_ID_Delete_User_Record(const V100_DEVICE_TRANSPORT_INFO *pDev, const _MX00_ID_USER_RECORD rec, bool nDeleteAllFingers);

/*********************** V100_ID_Enroll_User_Record ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_ID_Enroll_User_Record
///  Begins enrollment process. Users are enrolled in the system using the "rules" set using the V100_ID_Set_Parameters call.
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in]       UserRecord _MX00_ID_USER_RECORD			  User record to enroll
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks You must call V100_ID_ENROLL_USER_RECORD for each User-Finger you wish to enroll.  However, this command does
///			  automatically enroll multiple instances of said User-Finger. To check progress/completion of this opertion, poll using V100_Get_OP_Status.
///
///			  If the database specified in the nGroupID member of the _MX00_ID_USER_RECORD has multiple instances per user finger specified,
///			  V100_ID_Enroll_User_Record will attempt to enroll that many instances of the user’s finger. If the nFlags set during V100_ID_Set_Parameters
///			  call contains the FLAG_ENROLLMENT_QUALIFICATION flag, each instance of the captured print is checked against each other captured print in
///			  order to ensure enrollment quality. The FLAG_ENROLLMENT_QUALIFICATION is a requirement when enrolling into any database which is capable
///			  of identification. This is a requirement to ensure good identification performance. 
///		
///			  If the FLAG_FAIL_ENROLL_ON_DUPLICATE flag is set, for Identification capable databases check for duplicate is performed. If the captured prints
///			  match with a user-finger in the database, V100_Get_OP_Status returns "ERROR_ID_DUPLICATE" _V100_OP_ERROR code in nParameter member of _V100_OP_STATUS 
///			  structure. You may call V100_ID_Get_Result to get the user information.
///
///			  If the FLAG_FAIL_ENROLL_ON_SPOOF flag is set, for each instance of the captured print check for spoof is performed
///			  
///			  For M31x and V31x sensors, a check for finger clear is done after each capture to make sure there is no valid finger placement on device 
///			  platen before capturing next print. User is required to lift the finger after each capture during enrollment. For M30x sensors user can 
///			  set the force finger lift for enrollment using V100_Set_Option call with OPTION_SET_FORCE_FINGER_LIFT_MODE as the _V100_OPTION_TYPE and 
///			  FORCE_FINGER_LIFT_MODE_ON as the option data.
///
///  @see V100_Get_OP_Status, V100_ID_Set_Parameters, V100_ID_Set_User_Record
///
///  @author www.Lumidigm.com @date 10/13/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_ID_Enroll_User_Record(const V100_DEVICE_TRANSPORT_INFO *pDev,  _MX00_ID_USER_RECORD UserRecord);

/*********************** V100_ID_Identify_378 ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_ID_Identify_378
///  Identifies a user from a template. A database must be loaded using V100_ID_Set_Working_DB
///  in order for this command to succeed.
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *		pointer to device handle
///  @param [in]       pTemplate const uchar *						Template to be identified. The format of the template must correspond to the 
///																    template mode set, during V100_Set_Option call using OPTION_SET_TEMPLATE_MODE.
///  @param [in]       nSizeTemplate uint							Size of the template
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks V100_ID_Identify_378 is a macro operation, thus one must poll for completion using V100_Get_OP_Status.
///			  After V100_Get_OP_Status returns completion, V100_ID_Get_Result must be used to get the
///			  result of the identification. If the operation completes successfully  V100_Get_OP_Status returns
///			  following _V100_OP_ERROR codes in nParameter member of _V100_OP_STATUS structure
///				STATUS_ID_USER_FOUND		- User found
///				STATUS_ID_USER_NOT_FOUND	- User not found
///
///  @see V100_Get_OP_Status, V100_ID_Get_Result, V100_ID_Identify
///
///  @author www.Lumidigm.com @date 8/3/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_ID_Identify_378(const V100_DEVICE_TRANSPORT_INFO *pDev, const uchar* pTemplate, uint nSizeTemplate);

/*********************** V100_ID_Identify ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_ID_Identify
///  Identifies a user from capture. Captures a fingerprint and identifies the fingerprint. A database must be loaded using
///  V100_ID_Set_Working_DB in order for this command to succeed.
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *		pointer to device handle
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks V100_ID_Identify is a macro operation, thus one must poll for completion using V100_Get_OP_Status.
///			  After V100_Get_OP_Status returns completion, V100_ID_Get_Result must be used to get the
///			  result of the identification.
///
///			  V100_ID_Identify works in two stages. Stage 1 is capturing the fingerprint.   Polling V100_GET_OP_STATUS during this stage will allow
///			  you to get status on the capture, and will also return error conditions in case of time-outs, latent prints detected, etc..
///			  If the FLAG_FAIL_IDENTIFY_ON_SPOOF flag is set, check for spoof is performed.
///
///			  After a successful capture, V100_ID_Identify will begin the identification phase. After Identification is complete, you may call
///			  V100_ID_Get_Result to get the status information related to the last identification performed. If the operation completes successfully
///			  V100_Get_OP_Status returns following _V100_OP_ERROR codes in nParameter member of _V100_OP_STATUS structure
///				STATUS_ID_USER_FOUND		- User found
///				STATUS_ID_USER_NOT_FOUND	- User not found
///
///  @see V100_Get_OP_Status, V100_ID_Get_Result, V100_ID_Identify_378
///
///  @author www.Lumidigm.com @date 10/13/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_ID_Identify(const V100_DEVICE_TRANSPORT_INFO *pDev);

/*********************** V100_ID_Get_DB_Metrics ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_ID_Get_DB_Metrics
///  Gets metrics and parameters of a database
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *		pointer to device handle
///  @param [in, out]  dbMetrics _MX00_DB_METRICS *					Input: If bGetCurrent is false then nGroupID member is used to retrieve the metrics.
///																	Output: Contains Database metrics upon success
///  @param [in]       bGetCurrent bool								If true retrieves the metrics of the currently loaded database.
///																	If false the nGroupID member of the _MX00_DB_METRICS structure
///																	that is sent in is used in order to determine which database metrics to retrieve.
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks If bGetCurrent is true and no group is currently loaded, an error will be returned.
///
///  @see V100_ID_Set_Working_DB
///
///  @author www.Lumidigm.com @date 8/3/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_ID_Get_DB_Metrics(const V100_DEVICE_TRANSPORT_INFO *pDev, _MX00_DB_METRICS* dbMetrics, bool bGetCurrent);

/*********************** V100_ID_Get_System_Metrics ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_ID_Get_System_Metrics
///  Provides information on database groups currently found on the system
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *		pointer to device handle
///  @param [in, out]  dbMetrics _MX00_DB_METRICS **				Database metrics records found
///  @param [in, out]  nNumDBsFound uint &							Contains number of groups, thus number of _MX00_DB_METRICS to follow
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks The nCurEnrolledUserFingers and nCurEnrolledUsers members of the _MX00_DB_METRICS structure returned by this call
///			  should be ignored since these are not valid values. To get this information call V100_ID_Get_DB_Metrics.
///
///  @see V100_ID_Get_DB_Metrics
///
///  @author www.Lumidigm.com @date 8/3/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_ID_Get_System_Metrics(const V100_DEVICE_TRANSPORT_INFO *pDev, _MX00_DB_METRICS** dbMetrics, uint& nNumDBsFound);

/*********************** V100_ID_Release_System_Metrics ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_ID_Release_System_Metrics
///  Release the memory created with call to V100_ID_Get_System_Metrics
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *		pointer to device handle
///  @param [in, out]  pDBMetrics _MX00_DB_METRICS *				Database metrics records returned from V100_ID_Get_System_Metrics
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks None.
///
///  @see V100_ID_Get_System_Metrics
///
///  @author www.Lumidigm.com @date 8/3/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_ID_Release_System_Metrics(const V100_DEVICE_TRANSPORT_INFO *pDev, _MX00_DB_METRICS* pDBMetrics);

/*********************** V100_ID_Get_Result ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_ID_Get_Result
///  Retrieves the result of last successful identify executed. This includes completion of the commands
///	 V100_ID_Identify_378, V100_ID_Identify, V100_ID_Verify_User_378 and V100_ID_Verify_User_Record as
//	 polled by V100_Get_OP_Status
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *		pointer to device handle
///  @param [in, out]  res _MX00_ID_RESULT &						The results structure
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks None.
///
///  @see V100_Get_OP_Status, V100_ID_Identify_378, V100_ID_Identify, V100_ID_Verify_User_378, V100_ID_Verify_User_Record
///
///  @author www.Lumidigm.com @date 8/3/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_ID_Get_Result(const V100_DEVICE_TRANSPORT_INFO *pDev, _MX00_ID_RESULT& res);

/*********************** V100_ID_Set_Parameters ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_ID_Set_Parameters
///  Sets the parameters which the identification engine uses to operate
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *		pointer to device handle
///  @param [in]       param _MX00_ID_PARAMETERS					ID parameters to set
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks See _MX00_ID_PARAMETERS for information on ID parameters.
///
///  @see None.
///
///  @author www.Lumidigm.com @date 8/3/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_ID_Set_Parameters(const V100_DEVICE_TRANSPORT_INFO *pDev, _MX00_ID_PARAMETERS  param);

/*********************** V100_ID_Get_Parameters ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_ID_Get_Parameters
///  Retrieves the current ID parameters
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *		pointer to device handle
///  @param [in, out]  param _MX00_ID_PARAMETERS &					Contains Identification parameters upon success
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks See _MX00_ID_PARAMETERS for information on ID parameters.
///
///  @see V100_ID_Set_Parameters
///
///  @author www.Lumidigm.com @date 8/3/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_ID_Get_Parameters(const V100_DEVICE_TRANSPORT_INFO *pDev, _MX00_ID_PARAMETERS& param);


/*********************** V100_ID_Verify_User_Record ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_ID_Verify_User_Record
///  Verifies a user from capture.  Captures a fingerprint and verifies the fingerprint. This command should be used
///	 to verify against a user record present in the database.
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in]       UserRecord _MX00_ID_USER_RECORD			  User record to verify. All members of the _MX00_ID_USER_RECORD structure should
///																  be populated except for nInstances member, which is ignored
///	 @param [in]       nConsiderFinger short					  If 0, verifies the user specified in _MX00_ID_USER_RECORD ignoring the nFinger member.
///																  All fingers enrolled for the user are considered.
///																  If 1, verifies the user-finger specified in _MX00_ID_USER_RECORD.
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks V100_ID_Verify_User_Record is a macro operation, thus one must poll for completion using V100_Get_OP_Status.
///			  After V100_Get_OP_Status returns completion, V100_ID_Get_Result must be used to get the
///			  result of the verification. We recommend you to set working DB to verify a user. The command latency is much lower if working DB is set.
///
///			  V100_ID_Verify_User_Record works in two stages. Stage 1 is capturing the fingerprint.   Polling V100_GET_OP_STATUS during this stage
///			  will allow you to get status on the capture, and will also return error conditions in case of time-outs, latent prints detected, etc..
///			  If the FLAG_FAIL_VERIFY_ON_SPOOF flag is set, check for spoof is performed.
///
///			  After a successful capture, V100_ID_Verify_User_Record will begin the verification phase. After verification is complete, you may call
///			  V100_ID_Get_Result to get the status information related to the last verification performed. If the operation completes successfully
///			  V100_Get_OP_Status returns following _V100_OP_ERROR codes in nParameter member of _V100_OP_STATUS structure
///				STATUS_ID_MATCH		- Match
///				STATUS_ID_NO_MATCH	- No match
///
///
///  @see V100_Get_OP_Status, V100_ID_Get_Result, V100_ID_Verify_378
///
///  @author www.Lumidigm.com @date 10/13/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_ID_Verify_User_Record(const V100_DEVICE_TRANSPORT_INFO *pDev, _MX00_ID_USER_RECORD UserRecord, short nConsiderFinger);

/*********************** V100_ID_Verify_378 ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_ID_Verify_378
///  Verifies a user from a template. This command should be used to verify an existing
///	 template against a user record present in the database.
///
///  @param [in]       pDev const V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in]       UserRecord _MX00_ID_USER_RECORD			  User record to verify. All members of the _MX00_ID_USER_RECORD structure should
///																  be populated except for nInstances member, which is ignored
///  @param [in, out]  pTemplate unsigned char *				  Template to be verified. The format of the template must correspond to the 
///																  template mode set, during V100_Set_Option call using OPTION_SET_TEMPLATE_MODE.
///  @param [in]       nSizeTemplate uint						  Size of the template
///	 @param [in]       nConsiderFinger short					  If 0, verifies the user specified in _MX00_ID_USER_RECORD ignoring the nFinger member.
///																  All fingers enrolled for the user are considered.
///																  If 1, verifies the user-finger specified in _MX00_ID_USER_RECORD.
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks V100_ID_Verify_378 is a macro operation, thus one must poll for completion using V100_Get_OP_Status.
///			  After V100_Get_OP_Status returns completion, V100_ID_Get_Result must be used to get the result
///			  of the verification. If the operation completes successfully  V100_Get_OP_Status returns following
///			 _V100_OP_ERROR codes in nParameter member of _V100_OP_STATUS structure
///				STATUS_ID_MATCH		- Match
///				STATUS_ID_NO_MATCH	- No match
///			  We recommend you to set working DB to verify a user. The command latency is much lower if working DB is set.
///
///  @see V100_Get_OP_Status, V100_ID_Get_Result, V100_ID_Verify_User_Record
///
///  @author www.Lumidigm.com @date 10/13/2009
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_ID_Verify_378(const V100_DEVICE_TRANSPORT_INFO *pDev, _MX00_ID_USER_RECORD UserRecord, unsigned char* pTemplate, uint nSizeTemplate, short nConsiderFinger);

/*********************** V100_ID_Verify_Many ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_ID_Verify_Many
///  Captures image, extracts minutia template, verifies against input templates
///
///  @param [in, out]  pDev V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in, out]  pTemplates uchar **					pointer to array of input templates. The format of the templates must  
///															correspond to the template mode set, during V100_Set_Option call using
///															OPTION_SET_TEMPLATE_MODE.
///  @param [in, out]  pSizeTemplates uint *				Array of input templates size
///  @param [in]       nNumTemplates uint					Number of input templates to verify. Max 30 templates are allowed
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of 
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks V100_ID_Verify_Many is a macro operation, thus one must poll for completion using V100_Get_OP_Status. 
///			  After V100_Get_OP_Status returns completion, V100_ID_Get_Result must be used to get the
///			  result of the verification. 
///	
///			  V100_ID_Verify_Many works in two stages. Stage 1 is capturing the fingerprint.   Polling V100_GET_OP_STATUS during this stage  
///			  will allow you to get status on the capture, and will also return error conditions in case of time-outs, latent prints detected, etc..
///			  If the FLAG_FAIL_VERIFY_ON_SPOOF flag is set, check for spoof is performed.
///
///			  After a successful capture, V100_ID_Verify_Many will begin the verification phase. If the operation completes successfully  
///			  V100_Get_OP_Status returns following _V100_OP_ERROR codes in nParameter member of _V100_OP_STATUS structure
///				STATUS_ID_MATCH		- Match
///				STATUS_ID_NO_MATCH	- No match
///			 
///			 After the operation completes successfully V100_ID_Get_Result returns (0-based)index of the input template which has higest 
///			 match score in szUserID member of _MX00_ID_RESULT structure
///
///  @see V100_Get_OP_Status, V100_ID_Get_Result, V100_ID_Verify_User_Record, V100_ID_Verify_378, V100_Verify
///
///  @author www.lumidigm.com @date 9/29/2010
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_ID_Verify_Many(V100_DEVICE_TRANSPORT_INFO *pDev,	uchar** pTemplates, uint* pSizeTemplates, uint nNumTemplates);

/*********************** V100_Verify_378 ***********************/
///////////////////////////////////////////////////////////////////////////////
///  global public  V100_Verify_378
///  Captures image, extracts minutia, verifies against input template.
///
///  @param [in, out]  pDev V100_DEVICE_TRANSPORT_INFO *    pointer to device handle
///  @param [in, out]  pTemplate uchar *					pointer to the minutia template to be verified
///  @param [in]       nTemplateSize uint					Size of the template to be verified
///
///  @return V100_ERROR_CODE Refer to Error code documentation for detailed description of 
///							 possible return values. GEN_OK indicates operation was successful
///
///  @remarks V100_Verify_378 is a macro operation, thus one must poll for completion using V100_Get_OP_Status. 
///			  After V100_Get_OP_Status returns completion, V100_ID_Get_Result must be used to get the
///			  result of the verification. 
///
///  @see V100_Get_OP_Status, V100_Get_Template, V100_ID_Get_Result
///
///  @author www.lumidigm.com @date 2/5/2013
///////////////////////////////////////////////////////////////////////////////

VCOM_CORE_EXPORT V100_ERROR_CODE V100_Verify_378( const V100_DEVICE_TRANSPORT_INFO *pDev,
												  uchar* pTemplate,
												  uint	 nTemplateSize);

/************** End of 1-N IDENTIFICATION (Mercury Only)****************/

