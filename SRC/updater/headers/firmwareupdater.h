#ifndef FIRMWAREUPDATER_H
#define FIRMWAREUPDATER_H

#include <QObject>
#include <QSettings>
#include <QTimer>

#include "debug_macros.h"
#include "soaphandler.h"

class FirmwareUpdater : public QObject
{
  Q_OBJECT
  public:
    explicit FirmwareUpdater();
    ~FirmwareUpdater();

signals:
    public slots:
      void finished(QString);
    void error(QString);

    private slots:
      void doWork();

  private:
    QSettings *settings;
    QTimer *timer;
    SoapHandler *soapHandler;

    bool parseXml(QString &response, QString &pathFw, QString &pathFwMd5, int &sizeFw, int &sizeFwMd5);
};

#endif // FIRMWAREUPDATER_H
