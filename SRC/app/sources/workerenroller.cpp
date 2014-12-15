#include "workerenroller.h"
#include "debug_macros.h"
#include "dbaccess.h"
#include "utils.h"

#include <QXmlStreamReader>
#include <QProcess>
#include <QFile>
#include <QThread>
#include <QMap>

WorkerEnroller::WorkerEnroller(QSettings *settings, IDKITWrapper *idkit, QObject *parent) :
  QObject(parent)
{
  _working = false;
  _abort = false;

  this->settings = settings;
  this->idkit = idkit;
  this->generaDB = NULL;
}

WorkerEnroller::~WorkerEnroller()
{
  if (timer) { delete timer; }
  if (soapHandler) { delete soapHandler; }
}

void WorkerEnroller::setSQLiteManager(GeneraDB *manager) { this->generaDB = manager; }

void WorkerEnroller::requestWork()
{
  mutex.lock();
  _working = true;
  _abort = false;
  DEBUG("Request worker enroller start in Thread %p.", thread()->currentThreadId());
  mutex.unlock();

  emit workRequested();
}

void WorkerEnroller::abort()
{
  mutex.lock();
  if (_working) {
    _abort = true;
    DEBUG("Request worker enroller aborting in Thread %p.", thread()->currentThreadId());
  }
  mutex.unlock();
}

void WorkerEnroller::doWork()
{
  DEBUG("Starting worker enroller process in Thread %p.", thread()->currentThreadId());

  soapHandler = new SoapHandler(settings);
  connect(soapHandler, SIGNAL(finished(QString)), this, SLOT(enrollFinished(QString)));
  connect(soapHandler, SIGNAL(error(QString)), this, SLOT(enrollError(QString)));

  timer = new QTimer(this);
  timer->setInterval(settings->value("timerEnrolamiento", 60000).toInt());
  connect(timer, SIGNAL(timeout()), this, SLOT(run()));
  timer->start();
}

void WorkerEnroller::run()
{
  if (isAborted()) { return; }

  timer->stop();

  const QString method = "getCargaMasivaM";
  const QString wsNamespace = settings->value("wsNamespace").toString();
  QMap<QString, QString> map;
  map["equSerial"] = settings->value("serialEquipo").toString();
  map["opcion"] = "0";
#ifdef TEMPO
  map["nativo"] = "0";
#endif
  const QString strSOAP = soapHandler->getXmlPacket(method, wsNamespace, map);

  const QUrl url(settings->value("wsCargaMasivaURL").toString());
  soapHandler->fetch(method, wsNamespace, url, strSOAP);
}

void WorkerEnroller::enrollFinished(QString response)
{
  bool rc;
  QString db = settings->value("importDB").toString();
  QString pathGenMaster;
  int sizeGenMaster = 0;
  int restart = 0;

  CHECK(!response.contains("<title>Error en tiempo"), "Response error");

  rc = parseXml(response, pathGenMaster, sizeGenMaster, restart);
  CHECK(rc == true, "Error parsing XML");

  if (pathGenMaster == "E") {
    DEBUG("WS Error. pathGenMaster = %s", pathGenMaster.toStdString().c_str());
    goto error;
  } else if (pathGenMaster == "N") {
    DEBUG("No users available. pathGenMaster = %s", pathGenMaster.toStdString().c_str());
  } else {
    rc = Utils::wgetCall(db, pathGenMaster);
    CHECK(rc == true, "Error sending wget command");

    if (Utils::verifyFileSize(db, sizeGenMaster)) {
      this->import(db, restart);
      Utils::removeFile(db);
      emit enrollFinished();
    } else {
      emit enrollError();
    }
  }

  if (isAborted()) { return; }
  timer->start();
  return;

error:
  emit enrollError();

  if (isAborted()) { return; }
  timer->start();
}

void WorkerEnroller::enrollError(QString error)
{
  UNUSED(error);
  DEBUG("soaphandler error: %s", error.toStdString().c_str());
  emit enrollError();
  if (isAborted()) { return; }
  timer->start();
}

void WorkerEnroller::enrollProgress(int importCount)
{
  DEBUG("import count: %d", importCount);
  emit enrollProgressSignal(importCount);
}

bool WorkerEnroller::parseXml(QString &response, QString &pathGenMaster, int &sizeGenMaster, int &restart)
{
  QXmlStreamReader xml(response);

  while (!xml.atEnd() && !xml.hasError()) {
    QXmlStreamReader::TokenType token = xml.readNext();

    if (token == QXmlStreamReader::StartDocument) { continue; }

    if (token == QXmlStreamReader::StartElement) {
      if (xml.name() == "pathGenMaster") {
        token = xml.readNext();
        if(xml.tokenType() == QXmlStreamReader::Characters) {
          DEBUG("pathGenMaster %s", xml.text().toString().toStdString().c_str());
          pathGenMaster = xml.text().toString();
          continue;
        }
      }

      if (xml.name() == "sizeGenMaster") {
        token = xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::Characters) {
          DEBUG("sizeGenMaster %s", xml.text().toString().toStdString().c_str());
          sizeGenMaster = xml.text().toString().toInt();
          continue;
        }
      }

      if (xml.name() == "restart") {
        token = xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::Characters) {
          DEBUG("restart %s", xml.text().toString().toStdString().c_str());
          restart = xml.text().toString().toInt();
          continue;
        }
      }

      if (xml.hasError()) {
        DEBUG("XML Error: %s", xml.errorString().toStdString().c_str());
        xml.clear();
      }
    }
    continue;
  }

  if (pathGenMaster.isEmpty()) {
    DEBUG("WS Error. pathGenMaster = %s", pathGenMaster.toStdString().c_str());
    return false;
  }

  DEBUG("WS OK. pathGenMaster = %s", pathGenMaster.toStdString().c_str());
  return true;
}

void WorkerEnroller::import(QString importDbName, int restart)
{
  if (restart == 1) {
    emit enrollProgressSignal(-1);
    DEBUG("Cleaning database...");
    if (idkit->clearDatabase()) {
      ImportDB dbImport;
      dbImport.init(importDbName.toStdString().c_str());
      connect(&dbImport, SIGNAL(importProgress(int)), this, SLOT(enrollProgress(int)));
      dbImport.importDatabase(idkit, this->generaDB);
      disconnect(&dbImport, SIGNAL(importProgress(int)), this, SLOT(enrollProgress(int)));
      DEBUG("Mass Import OK.");
    }
  }
}

bool WorkerEnroller::isAborted()
{
  mutex.lock(); bool abort = _abort; mutex.unlock();
  if (abort) {
    DEBUG("Aborting worker enroller process in Thread %p.", thread()->currentThreadId());
    _working = false;
    DEBUG("Worker sensor process finished in Thread %p.", thread()->currentThreadId());
    emit finished();
    return true;
  }

  return false;
}
