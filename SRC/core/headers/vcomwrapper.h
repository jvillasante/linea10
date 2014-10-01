#ifndef VCOMWRAPPER_H
#define VCOMWRAPPER_H

#include "VCOMCore.h"
#include <QSettings>

#define M30X_PROD_ID 8492
#define V30X_PROD_ID 13348
#define M31X_PROD_ID 65
#define V31X_PROD_ID 33

// A simple enumeration to keep track of the different Lumidigm devices quickly
typedef enum SUPPORTED_DEVICES {
  V300 = 0,   // V300 (All configurations)
  M100,       // M100
  M300,       // M300
  M310,       // M310
  V310,       // V310
  UNKNOWN_DEV,
} DeviceType;

const char* VCOM_GetErrorMsg(V100_ERROR_CODE rc);

class VCOMWrapper
{
  public:
    explicit VCOMWrapper(QSettings *settings);
    ~VCOMWrapper();

    bool setupConnection();
    void closeConnection();
    int  getImageWidth();
    int  getImageHeight();
    int capture(uchar *pCompositeImage, uint &nWidth, uint &nHeight, uchar *pTemplate,
        uint &nTemplateSize, int &nSpoof, int getComposite, int getTemplate);
    bool cancelOperation();
    void printAcqStatus();
    void reset();

  private:
    QSettings *settings;
    bool isOpen;
    DeviceType deviceType;
    bool fingerLift;
    uint serialNumber;
    _V100_INTERFACE_CONFIGURATION_TYPE config;
    V100_DEVICE_TRANSPORT_INFO dev;
};

#endif // VCOMWRAPPER_H
