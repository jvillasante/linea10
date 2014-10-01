#include "vcomwrapper.h"

#include "debug_macros.h"
#include "cstring"
#include "unistd.h"

const char* DeviceTypeStr[6] = {
  "V30x", "M10x", "M30x", "M31x", "V31x", "Unknown Device"
};

VCOMWrapper::VCOMWrapper(QSettings *settings)
{
  this->settings = settings;
  isOpen = false;
  deviceType = V300;
  fingerLift = false;
  serialNumber = 0;
  memset(&config, 0, sizeof(_V100_INTERFACE_CONFIGURATION_TYPE));
  memset(&dev, 0, sizeof(V100_DEVICE_TRANSPORT_INFO));
}

VCOMWrapper::~VCOMWrapper()
{
  if (isOpen) {
    closeConnection();
    isOpen = false;
    LOG_INFO("VCOM is now closed");
  }
}

bool VCOMWrapper::setupConnection()
{
  V100_ERROR_CODE rc = GEN_OK;
  int numDevices = 0;

  CHECK(!isOpen, "Device is already oppened.");

  // Query number of devices connected
  rc = V100_Get_Num_USB_Devices(&numDevices);
  CHECK_VCOM(rc, "V100_Get_Num_USB_Devices");
  DEBUG("%d devices found.", numDevices);

  // Open the device
  rc = V100_Open(&dev);
  CHECK_VCOM(rc, "V100_Open");
  DEBUG("Device %d is now open.", dev.DeviceNumber);

  // Get Configuration structure
  rc = V100_Get_Config(&dev, &config);
  CHECK_VCOM(rc, "V100_Get_Config");

  // Get Serial Number
  // V30x and M30x use a second field to support values over 65535
  if (V31X_PROD_ID == config.Product_Id || M31X_PROD_ID == config.Product_Id) {
    serialNumber = (config.Device_Serial_Number << 16) | (config.Device_Serial_Number & 0xFFFF);
  } else {
    serialNumber = config.Device_Serial_Number;
  }
  DEBUG("Device serial number: %d.", serialNumber);

  // Set the device type
  deviceType = UNKNOWN_DEV;
  if (config.Vendor_Id == 0x0525 && config.Product_Id == 0x3424) {
    deviceType = V300;  // V30x sensor
    fingerLift = false;
  } else if (config.Vendor_Id == 0x1FAE) {
    if (config.Product_Id == 0x212C) {
      deviceType = M300;  // M30x sensor
      fingerLift = false;
    } else if (config.Product_Id == 0x0041) {
      deviceType = M310;  // M31 sensor
      fingerLift = true;
    } else if (config.Product_Id == 0x0021) {
      deviceType = V310;  // V31x sensor
      fingerLift = true;
    } else {
      deviceType = UNKNOWN_DEV;
      fingerLift = false;
    }
  } else {
    deviceType = UNKNOWN_DEV;
    fingerLift = false;
  }
  DEBUG("Device type: %s.", DeviceTypeStr[deviceType]);
  DEBUG("Device Vendor_Id: %d", config.Vendor_Id);
  DEBUG("Device Product_Id: %d", config.Product_Id);
  DEBUG("DEvice Firmware_Rev: %d", config.Firmware_Rev);
  DEBUG("Finger Lift: %d.", fingerLift);
  // fingerLift = false;
  // DEBUG("Finger Lift Hack: %d.", fingerLift);

  // Get Command Structure
  _V100_INTERFACE_COMMAND_TYPE cmd;
  rc = V100_Get_Cmd(&dev, &cmd);
  CHECK_VCOM(rc, "V100_Get_Cmd");

  // Set preprocessing, extraction, matching
  // Turn on all on board processing
  cmd.Select_PreProc = 1;
  cmd.Select_Matcher = 0;
  cmd.Select_Extractor = 1;
  cmd.Override_Trigger = 0;
  cmd.Select_Spoof_Model = 0;
  cmd.TimeOut = 1;

  // Set the Command structure with all on board processing turned on
  rc = V100_Set_Cmd(&dev, &cmd);
  CHECK_VCOM(rc, "V100_Set_Cmd");

  if (deviceType == V310) {
    // For legacy reasons turning off latent detection and using finger lift mode
    _V100_OPTION_TYPE optType = OPTION_SET_LATENT_DETECTION_MODE;
    _V100_LATENT_DETECTION_MODE latentType = LATENT_DETECTION_OFF;
    rc = V100_Set_Option(&dev, optType, (uchar *)(&latentType), sizeof(_V100_LATENT_DETECTION_MODE));
    CHECK_VCOM(rc, "V100_Set_Option");
  }

  isOpen = true;
  LOG_INFO("VCOMWrapper::setupConnection OK");
  return true;

error:
  isOpen = false;
  LOG_ERROR("VCOMWrapper::setupConnection FAIL.");
  return false;
}

void VCOMWrapper::reset()
{
  V100_ERROR_CODE rc = GEN_OK;
  CHECK(isOpen, "Device is not oppened.");

  rc = V100_Reset(&dev);
  CHECK_VCOM(rc, "V100_Reset");

  LOG_INFO("VCOMWrapper::closeReset OK.");
  return;

error:
  LOG_ERROR("VCOMWrapper::closeReset FAIL.");
}

void VCOMWrapper::closeConnection()
{
  V100_ERROR_CODE rc = GEN_OK;
  CHECK(isOpen, "Device is not oppened.");

  rc = V100_Close(&dev);
  CHECK_VCOM(rc, "V100_Close");

  LOG_INFO("VCOMWrapper::closeConnection OK.");
  isOpen = false;
  return;

error:
  LOG_ERROR("VCOMWrapper::closeConnection FAIL.");
}

int VCOMWrapper::getImageWidth()
{
  CHECK(isOpen, "Device is not oppened.");
  CHECK(&config != NULL, "Config structure is not initialized.");
  return config.Composite_Image_Size_X;

error:
  return -1;
}

int VCOMWrapper::getImageHeight()
{
  CHECK(isOpen, "Device is not oppened.");
  CHECK(&config != NULL, "Config structure is not initialized.");
  return config.Composite_Image_Size_Y;

error:
  return -1;
}

int VCOMWrapper::capture(uchar *pCompositeImage, uint &nWidth, uint &nHeight, uchar *pTemplate,
    uint &nTemplateSize, int &nSpoof, int getComposite, int getTemplate)
{
  V100_ERROR_CODE rc = GEN_OK;
  CHECK(isOpen, "Device is not oppened.");

  rc = V100_Capture(&dev, pCompositeImage, nWidth, nHeight, pTemplate, nTemplateSize, nSpoof, getComposite, getTemplate);
  if ((rc == GEN_ERROR_TIMEOUT) || (rc == GEN_ERROR_TIMEOUT_LATENT)) { return 1; }
  CHECK_VCOM(rc, "Error capturing image."); 

  if (fingerLift) {
    rc = V100_WaitForFingerClear(&dev);
    CHECK_VCOM(rc, "V100_WaitForFingerClear");
  }

  LOG_INFO("VCOMWrapper::capture OK.");
  return 0;

error:
  LOG_ERROR("VCOMWrapper::capture FAIL.");
  return -1;
}

bool VCOMWrapper::cancelOperation()
{
  V100_ERROR_CODE rc = GEN_OK;
	bool bTimeout = false;
	_V100_ACQ_STATUS_TYPE Acq_Status;
  
  CHECK(isOpen, "Device is not oppened.");
  
  rc = V100_Cancel_Operation(&dev);
  DEBUG("*****V100_Cancel_Operation: %s*****", VCOM_GetErrorMsg(rc));
	if (rc != GEN_OK && rc != GEN_ERROR_APP_BUSY) { 
    DEBUG("V100_Cancel_Operation GEN_ERROR_INTERNAL");
    return false;
  }
  
	while (bTimeout == false) {
		if (GEN_OK != V100_Get_Acq_Status(&dev, &Acq_Status)) {
      DEBUG("V100_GET_Acq_Status GEN_ERROR_INTERNAL");
      return false;
		}
    
		switch (Acq_Status) {
      case ACQ_SPOOF_DETECTED:     { sleep(1); continue; } break;
			case ACQ_BUSY:               { sleep(1); continue; } break;
			case ACQ_NOOP:               { sleep(1); continue; } break;
			case ACQ_PROCESSING:         { sleep(1); continue; } break;
			case ACQ_FINGER_PRESENT:     { sleep(1); continue; } break;
			case ACQ_MOVE_FINGER_UP:     { sleep(1); continue; } break;
			case ACQ_MOVE_FINGER_DOWN:   { sleep(1); continue; } break;
			case ACQ_MOVE_FINGER_LEFT:   { sleep(1); continue; } break;
			case ACQ_MOVE_FINGER_RIGHT:  { sleep(1); continue; } break;
			case ACQ_FINGER_POSITION_OK: { sleep(1); continue; } break;
			case ACQ_NO_FINGER_PRESENT:  { sleep(1); continue; } break;
			case ACQ_TIMEOUT:	           { sleep(1); continue; } break;
			case ACQ_LATENT_DETECTED:	   { sleep(1); continue; } break;

			// case ACQ_TIMEOUT:			{ bTimeout = true; break;}
			case ACQ_DONE:				      { bTimeout = true; break;}
			case ACQ_CANCELLED_BY_USER:	{ bTimeout = true; break;}
			// case ACQ_LATENT_DETECTED:	{ bTimeout = true; break;}		
		}
	}
  
	if(Acq_Status != ACQ_DONE || Acq_Status != ACQ_CANCELLED_BY_USER) {
    DEBUG("Acq_Status != ACQ_DONE");
		return false;
	}

  DEBUG("VCOMWrapper::cancelOperation OK");
	return true;

error:
  DEBUG("VCOMWrapper::cancelOperation FAIL");
  return false;
}

void VCOMWrapper::printAcqStatus()
{
	_V100_ACQ_STATUS_TYPE Acq_Status;
  
  if (GEN_OK != V100_Get_Acq_Status(&dev, &Acq_Status)) {
    DEBUG("V100_GET_Acq_Status GEN_ERROR_INTERNAL");
    return;
  }

  switch (Acq_Status) {
    case ACQ_SPOOF_DETECTED:     { DEBUG("#####ACQ_SPOOF_DETECTED"); } break;
    case ACQ_BUSY:               { DEBUG("#####ACQ_BUSY"); } break;
    case ACQ_NOOP:               { DEBUG("#####ACQ_NOOP"); } break;
    case ACQ_PROCESSING:         { DEBUG("#####ACQ_PROCESSING"); } break;
    case ACQ_FINGER_PRESENT:     { DEBUG("#####ACQ_FINGER_PRESENT"); } break;
    case ACQ_MOVE_FINGER_UP:     { DEBUG("#####ACQ_MOVE_FINGER_UP"); } break;
    case ACQ_MOVE_FINGER_DOWN:   { DEBUG("#####ACQ_MOVE_FINGER_DOWN"); } break;
    case ACQ_MOVE_FINGER_LEFT:   { DEBUG("#####ACQ_MOVE_FINGER_LEFT"); } break;
    case ACQ_MOVE_FINGER_RIGHT:  { DEBUG("#####ACQ_MOVE_FINGER_RIGHT"); } break;
    case ACQ_FINGER_POSITION_OK: { DEBUG("#####ACQ_FINGER_POSITION_OK"); } break;
    case ACQ_NO_FINGER_PRESENT:  { DEBUG("#####ACQ_NO_FINGER_PRESENT"); } break;
    case ACQ_TIMEOUT:	           { DEBUG("#####ACQ_TIMEOUT"); } break;
    case ACQ_LATENT_DETECTED:	   { DEBUG("#####ACQ_LATENT_DETECTED"); } break;
    case ACQ_DONE:				       { DEBUG("#####ACQ_DONE"); } break;
    case ACQ_CANCELLED_BY_USER:	 { DEBUG("#####ACQ_CANCELLED_BY_USER"); } break;
  }
}

const char* VCOM_GetErrorMsg(V100_ERROR_CODE rc)
{
  switch(rc) {
    case	GEN_OK:
      return "GEN_OK";
      break;
    case	GEN_ENCRYPTION_FAIL:
      return "GEN_ENCRYPTION_FAIL";
      break;
    case	GEN_DECRYPTION_FAIL:
      return "GEN_DECRYPTION_FAIL";
      break;
    case	GET_PD_INIT_FAIL:
      return "GET_PD_INIT_FAIL";
      break;
    case	PD_HISTOGRAM_FAIL:
      return "PD_HISTOGRAM_FAIL";
      break;
    case	INVALID_ACQ_MODE:
      return "INVALID_ACQ_MODE";
      break;
    case	BURNIN_THREAD_FAIL:
      return "BURNIN_THREAD_FAIL";
      break;
    case	UPDATER_THREAD_FAIL:
      return "UPDATER_THREAD_FAIL";
      break;
    case	GEN_ERROR_START:
      return "GEN_ERROR_START";
      break;
    case	GEN_ERROR_PROCESSING:
      return "GEN_ERROR_PROCESSING,";
      break;
    case	GEN_ERROR_VERIFY:
      return "GEN_ERROR_VERIFY,";
      break;
    case	GEN_ERROR_MATCH:
      return "GEN_ERROR_MATCH";
      break;
    case	GEN_ERROR_INTERNAL:
      return "GEN_ERROR_INTERNAL";
      break;
    case	GEN_ERROR_INVALID_CMD:
      return "GEN_ERROR_INVALID_CMD";
      break;
    case	GEN_ERROR_PARAMETER:
      return "GEN_ERROR_PARAMETER";
      break;
    case	GEN_NOT_SUPPORTED:
      return "GEN_NOT_SUPPORTED";
      break;
    case	GEN_INVALID_ARGUEMENT:
      return "GEN_INVALID_ARGUEMENT";
      break;
    case	GEN_ERROR_TIMEOUT:
      return "GEN_ERROR_TIMEOUT";
      break;
    case	GEN_ERROR_LICENSE:
      return "GEN_ERROR_LICENSE";
      break;
    case	GEN_ERROR_COMM_TIMEOUT:
      return "GEN_ERROR_COMM_TIMEOUT";
      break;
    case	GEN_FS_ERR_CD:
      return "GEN_FS_ERR_CD";
      break;
    case	GEN_FS_ERR_DELETE:
      return "GEN_FS_ERR_DELETE";
      break;
    case	GEN_FS_ERR_FIND:
      return "GEN_FS_ERR_FIND";
      break;
    case	GEN_FS_ERR_WRITE:
      return "GEN_FS_ERR_WRITE";
      break;
    case	GEN_FS_ERR_READ:
      return "GEN_FS_ERR_READ";
      break;
    case	GEN_FS_ERR_FORMAT:
      return "GEN_FS_ERR_FORMAT";
      break;
    case	GEN_ERROR_MEMORY:
      return "GEN_ERROR_MEMORY";
      break;
    case	GEN_ERROR_RECORD_NOT_FOUND:
      return "GEN_ERROR_RECORD_NOT_FOUND";
      break;
    case	GEN_VER_INVALID_RECORD_FORMAT:
      return "GEN_VER_INVALID_RECORD_FORMAT";
      break;
    case	GEN_ERROR_DB_FULL:
      return "GEN_ERROR_DB_FULL";
      break;
    case	GEN_ERROR_INVALID_SIZE:
      return "GEN_ERROR_INVALID_SIZE";
      break;
    case	GEN_ERROR_TAG_NOT_FOUND:
      return "GEN_ERROR_TAG_NOT_FOUND";
      break;
    case	GEN_ERROR_APP_BUSY:
      return "GEN_ERROR_APP_BUSY";
      break;
    case	GEN_ERROR_DEVICE_UNCONFIGURED:
      return "GEN_ERROR_DEVICE_UNCONFIGURED";
      break;
    case	GEN_ERROR_TIMEOUT_LATENT:
      return "GEN_ERROR_TIMEOUT_LATENT";
      break;
    case	GEN_ERROR_DB_NOT_LOADED:
      return "GEN_ERROR_DB_NOT_LOADED";
      break;
    case	GEN_ERROR_DB_DOESNOT_EXIST:
      return"GEN_ERROR_DB_DOESNOT_EXIST";
      break;
    case	GEN_ERROR_ENROLLMENTS_DO_NOT_MATCH:
      return "GEN_ERROR_ENROLLMENTS_DO_NOT_MATCH";
      break;
    case	GEN_ERROR_USER_NOT_FOUND:
      return "GEN_ERROR_USER_NOT_FOUND";
      break;
    case	GEN_ERROR_DB_USER_FINGERS_FULL:
      return "GEN_ERROR_DB_USER_FINGERS_FULL";
      break;
    case    GEN_ERROR_DB_USERS_FULL:
      return "GEN_ERROR_DB_USERS_FULL";
      break;
    case    GEN_ERROR_USER_EXISTS:
      return "GEN_ERROR_USER_EXISTS";
      break;
    case    GEN_ERROR_DEVICE_NOT_FOUND:
      return "GEN_ERROR_DEVICE_NOT_FOUND";
      break;
    case    GEN_ERROR_DEVICE_NOT_READY:
      return "GEN_ERROR_DEVICE_NOT_READY";
      break;
    case    GEN_ERROR_PIPE_READ:
      return "GEN_ERROR_PIPE_READ";
      break;
    case    GEN_ERROR_PIPE_WRITE:
      return "GEN_ERROR_PIPE_WRITE";
      break;
    case    GEN_ERROR_SENGINE_SHUTTING_DOWN:
      return "GEN_ERROR_SENGINE_SHUTTING_DOWN";
      break;
    case	GEN_LAST:
      return "GEN_LAST";
      break;
    default:
      return "UNKOWN ERROR";
      break;
  }
}
