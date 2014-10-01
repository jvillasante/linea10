#ifndef WORKERSENSORMULTI_H
#define WORKERSENSORMULTI_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>

#include "vcomwrapper.h"
#include "idkitwrapper.h"
#include "printerserial.h"
#include "dbaccess.h"

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
    void setSQLiteManager(EventsDB *manager);

  private:
    Method _method;
    bool _abort;
    bool _interrupt;
    QMutex mutex;
    QWaitCondition condition;

    VCOMWrapper *vcom;
    IDKITWrapper *idkit;
    PrinterSerial *printer;
    EventsDB *eventsDB;

    bool isButtonPressed(QString &typeStr, int &typeInt);
    void doIdentify();
    void doEnroll();
    void doWait();

  signals:
    void finished();
    
    void match(QString userIdentifier, QString userName, QString userRut);
    void buttonPressed(int button, QString userName);  // 0: TIMEOUT, 1: IN, 2: OUT
    
    void identifierWorkDone();
    void enrollWorkDone(uchar *compositeImage, uchar *templateImage, int width, int height);

    void error(QString msg);

  public slots:
      void mainLoop();
};

#endif // WORKERSENSORMULTI_H

