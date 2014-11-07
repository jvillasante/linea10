#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H

#include <QObject>
#include <QSettings>
#include <QTimer>

#include "debug_macros.h"
#include "soaphandler.h"
#include "dbaccess.h"

class Synchronizer : public QObject
{
  Q_OBJECT
  public:
    explicit Synchronizer();
    ~Synchronizer();

  public slots:
    void finished(QString);
    void error(QString);

  private slots:
    void doWork();

  private:
    QSettings *settings;
    QTimer *timer;
    SoapHandler *soapHandler;
    GeneraDB *generaDB;
    int eventsCount;

    bool parseXml(QString &response, int &result, int &id);
};

#endif // FIRMWAREUPDATER_H
