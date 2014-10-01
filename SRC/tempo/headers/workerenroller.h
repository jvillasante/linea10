#ifndef WORKERENROLLER_H
#define WORKERENROLLER_H

#include <QObject>
#include <QMutex>
#include <QSettings>
#include <QTimer>

#include "idkitwrapper.h"
#include "soaphandler.h"

class WorkerEnroller : public QObject
{
  Q_OBJECT
  public:
    explicit WorkerEnroller(QSettings *settings, IDKITWrapper *idkit, QObject *parent = 0);
    ~WorkerEnroller();

    void requestWork();
    void abort();

  private:
    bool _abort;
    bool _working;
    QMutex mutex;

    QSettings *settings;
    IDKITWrapper *idkit;
    QTimer *timer;
    SoapHandler *soapHandler;

signals:
    void workRequested();
    void finished();
    void enrollProgressSignal(int);
    void enrollFinished();
    void enrollError();

    public slots:
      void doWork();

  private slots:
    void run();
    void enrollProgress(int);
    void enrollFinished(QString);
    void enrollError(QString);

  private:
    bool parseXml(QString &response, QString &pathGenMaster, int &sizeGenMaster, int &restart);
    void import(QString db, int restart);
    bool isAborted();
};

#endif // WORKERENROLLER_H
