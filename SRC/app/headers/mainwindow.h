#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QString>
#include <QLabel>
#include <QPushButton>
#include <QLCDNumber>
#include <QTimer>
#include <QThread>
#include <QSettings>
#include <QDateTime>
#include <QTcpSocket>

#include "vcomwrapper.h"
#include "idkitwrapper.h"
#include "printerserial.h"
#include "workersensormulti.h"
#include "workerenroller.h"
#include "dbaccess.h"
#ifdef TEMPO
#include "enrolldialog.h"
#include "soaphandler.h"
#endif
#include "networkmonitor.h"

class MainWindow : public QWidget
{
  Q_OBJECT
  public:
    explicit MainWindow(QSettings *settings, QWidget *parent = 0);
    ~MainWindow();

  public slots:
#ifdef TEMPO
    void match(QString userIdentifier, QString userName, QString userRut);
    void buttonPressed(int button, QString userName);
    
    void enrollButtonPressed();
    void enrollDialogClosed(QString);

    void alarmasFinished(QString);
    void alarmasError(QString);
#elif SNACK
    void match(QString userIdentifier, QString userName, QString userRut, QString service, int servicesCount);
#endif
    
    void identifierWorkDone();
    void message(QString msg);
    
    void enrollProgress(int);
    void enrollFinished();
    void enrollError();
    
    void updateEverySecond();
    void updateEveryHour();
    void updateRebootCountDown();

  private:
    QGridLayout *grid;
    QHBoxLayout *hbox;
    QWidget     *box;
    QLabel      *lblGeneraLogo;
    QLabel      *lblEmpresaHolding;
    QLabel      *lblDate;
    QLCDNumber  *lblTime;
    QLabel      *lblOutput;
    QLabel      *lblMsg;
#ifdef TEMPO
    QPushButton *enrollButton;
#endif
    QLabel      *lbl1, *lbl2, *lbl3;

    QSettings *settings;
    QString   lang;
    
    GeneraDB *generaDB;

    int lblOutputCounter;
    int rebootCountdownCounter;
    QTimer *everySecondTimer;
    QTimer *everyHourTimer;

    QThread            *threadSensor;
    WorkerSensorMulti  *workerSensor;
    
    QThread        *threadEnroller;
    WorkerEnroller *workerEnroller;

    VCOMWrapper  *vcom;
    IDKITWrapper *idkit;
    PrinterSerial *printer;
    NetworkMonitor *networkMonitor;
#ifdef TEMPO
    SoapHandler *soapHandler;
#endif

    bool ntp_is_running;

  private:
    void initializeUI();
    void initializeGeneraDatabase();
    void initializeEverySecondTimer();
    void initializeEveryHourTimer();
    void initializeCore();
    void initializeWorkerSensor();
    void initializeWorkerSensorEnroll();
    void initializeWorkerEnroller();
    void printInitTicket();

    inline void updatePrinterStatus();
    inline void updateInternetStatus();
    inline void updateDate(QDateTime now);
};

#endif // MAINWINDOW_H
