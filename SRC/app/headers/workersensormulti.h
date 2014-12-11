#ifndef WORKERSENSORMULTI_H
#define WORKERSENSORMULTI_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>

#include "vcomwrapper.h"
#include "idkitwrapper.h"
#include "printerserial.h"
#include "dbaccess.h"
#ifdef SNACK
#include "dao_service.h"
#endif

class WorkerSensorMulti : public QObject
{
  Q_OBJECT

  public:
    explicit WorkerSensorMulti(QObject *parent = 0);

    enum Method {
      Identify = 1,
      Enroll   = 2,
      Wait     = 3
    };

    void requestMethod(Method method);
    void abort();

    void setVCOMWrapper(VCOMWrapper *vcom);
    void setIDKITWrapper(IDKITWrapper *idkit);
    void setPrinterSerial(PrinterSerial *printer);
    void setSQLiteManager(GeneraDB *manager);

  private:
    Method _method;
    bool _abort;
    bool _interrupt;
    QMutex mutex;
    QWaitCondition condition;

    VCOMWrapper *vcom;
    IDKITWrapper *idkit;
    PrinterSerial *printer;
    GeneraDB *generaDB;

#ifdef SNACK
    char keypadDevice[20];
#endif

    void doIdentify();
    void doWait();

#ifdef TEMPO
    void doIdentifyTempo();
    void doEnroll();
    bool isButtonPressed(QString &typeStr, int &typeInt);
#elif SNACK
    void doIdentifySnack();
    void giveService(ServiceDAO *service, int userId, char *userIdentifier, char *userName, char *userRut, 
        char *userEmp, char *userCentroCosto);
    std::string execute(const char* cmd);
    int determineInputDeviceIndex();
    int waitForKeyboard();
    ServiceDAO *getLastServedFromGroup(QMap<int, ServiceDAO*> services, ServiceDAO *service);
#endif

  signals:
#ifdef TEMPO
    void match(QString userIdentifier, QString userName, QString userRut);
    void buttonPressed(int button, QString userName);  // 0: TIMEOUT, 1: IN, 2: OUT
    void enrollWorkDone(uchar *compositeImage, uchar *templateImage, int width, int height);
#elif SNACK
    void match(QString userIdentifier, QString userName, QString userRut, QString service, int servicesCount);
#endif
    
    void finished();
    void identifierWorkDone();
    void message(QString msg);

  public slots:
      void mainLoop();
};

#endif // WORKERSENSORMULTI_H

