#ifndef PRINTERSERIAL_H
#define PRINTERSERIAL_H

#include "qextserialport.h"
#include "qextserialenumerator.h"

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QSettings>

class PrinterSerial : public QObject
{
  Q_OBJECT
  public:
    explicit PrinterSerial(QSettings *settings);
    ~PrinterSerial();

    bool init();
    void write_hello(QString mac, QString ip, QString gateway, QString masc, QString broadcast);
    void write_user(QString type, QString userIdentifier, QString userName, QString userRut, QString userEmp);
    bool getRealTimePaperStatus(QString &msg);
    bool getRealTimeStatus(QString &msg);
    bool getRealTimeOfflineStatus(QString &msg);
    void setStatus(bool status);
    bool getStatus();

  private:
    QSettings *settings;
    QString lang;
    QextSerialPort *serialPort;
    bool connectedStatus;

    bool selectPrinter();
    bool writeBytes(QByteArray bytes);
    bool send_cut();

    bool boldOn();
    bool boldOff();
    bool alignLeft();
    bool alignCenter();
    bool alignRight();
    bool fontStandard();
    bool fontCompressed();

    QString getDate(QDateTime now);

signals:

    public slots:
      void onReadyRead();

};

#endif // PRINTERSERIAL_H
