#include "synchronizer.h"

#include "debug_macros.h"
#include "utils.h"

#include <QCoreApplication>
#include <QXmlStreamReader>
#include <QDateTime>
#include <QSqlQuery>

Synchronizer::Synchronizer()
{
  eventsCount = 0;

  // QString appPath = QCoreApplication::applicationDirPath();
  settings = new QSettings("/mnt/jffs2/app.ini", QSettings::IniFormat);

  QString db = settings->value("generaDB").toString();
  generaDB = new GeneraDB();
  generaDB->init(db.toStdString().c_str());

  soapHandler = new SoapHandler(settings);
  connect(soapHandler, SIGNAL(finished(QString)), this, SLOT(finished(QString)));
  connect(soapHandler, SIGNAL(error(QString)), this, SLOT(error(QString)));

  timer = new QTimer(this);
  timer->setInterval(settings->value("timerSincronizacion", 60000).toInt());
  connect(timer, SIGNAL(timeout()), this, SLOT(doWork()));
  timer->start();
}

Synchronizer::~Synchronizer()
{
  delete timer;
  delete settings;
  delete generaDB;
  delete soapHandler;
}

void Synchronizer::doWork()
{
  int id, sense, date;
  bool gotResults = false;
  QString ident;
  QSqlQuery *qry;
#ifdef SNACK
  int serviceId;
#endif

  timer->stop();
  qry = generaDB->getEventsToSynchronize();
  if (qry == NULL) { timer->start(); return; }

  while(qry->next()) {
    gotResults = true;
    eventsCount += 1;

    id    = qry->value(0).toInt();
    sense = qry->value(1).toInt();
    ident = qry->value(2).toString();
    date  = qry->value(3).toInt();
#ifdef SNACK
    serviceId = qry->value(4).toInt();
#endif

    QDateTime dt;
    Utils::getFromUnixTimestamp(dt, date);
    QString serial = settings->value("serialEquipo").toString();
    QString fechaHora = dt.toString("yyyy-MM-dd hh:mm");
    QString min = dt.toString("ss");

    const QString method = "SincronizacionMarcasM";
    const QString wsNamespace = settings->value("wsNamespace").toString();
    QMap<QString, QString> map;
#ifdef TEMPO
    map["evhcodigo"] = QString::number(id);
#elif SNACK
    map["evcCodigo"] = QString::number(id);
#endif
    map["identificacion"] = ident;
    map["evhFechaHora"] = fechaHora;
#ifdef TEMPO
    map["evhCorrelMin"] = min;
#elif SNACK
    map["evcCorrelMin"] = min;
#endif
    map["tieCodigo"] = QString::number(sense);
    map["equSerial"] = serial;
#ifdef SNACK
    map["evcServicio"] = QString::number(serviceId);
#endif
    const QString strSOAP = soapHandler->getXmlPacket(method, wsNamespace, map);

    const QUrl url(settings->value("wsSincronizacionURL").toString());
    soapHandler->fetch(method, wsNamespace, url, strSOAP);
  }

  if (!gotResults) {
    DEBUG("No events to synchronize at this time...");
    timer->start();
  }

  qry->finish();
  delete qry;
}

void Synchronizer::finished(QString response)
{
  bool rc;
  int result, id;

  CHECK(!response.contains("<title>Error en tiempo"), "Response error");

  rc = parseXml(response, result, id);
  CHECK(rc == true, "Error parsing XML");

  if (result == -1) {
    DEBUG("WS Error. result = %d", result);
    goto error;
  } else if (result == 0) {
    DEBUG("Event exists on server. result = %d", result);
  } else if (result == 1) {
    DEBUG("Event received on server. result = %d", result);
  }

  generaDB->setEventSynchronized(id);
  if (--eventsCount <= 0) { timer->start(); }
  return;

error:
  DEBUG("Error. Restarting daemon...");
  if (--eventsCount <= 0) { timer->start(); }
}

void Synchronizer::error(QString error)
{
  UNUSED(error);
  DEBUG("soaphandler error: %s", error.toStdString().c_str());
  if (--eventsCount <= 0) { timer->start(); }
}

bool Synchronizer::parseXml(QString &response, int &result, int &id)
{
  UNUSED(id);
  QXmlStreamReader xml(response);

  while (!xml.atEnd() && !xml.hasError()) {
    QXmlStreamReader::TokenType token = xml.readNext();

    if (token == QXmlStreamReader::StartDocument) { continue; }

    if (token == QXmlStreamReader::StartElement) {
#ifdef TEMPO
      if (xml.name() == "evhcodigo") {
        token = xml.readNext();
        if(xml.tokenType() == QXmlStreamReader::Characters) {
          DEBUG("evhcodigo %s", xml.text().toString().toStdString().c_str());
          id = xml.text().toString().toInt();
          continue;
        }
      }
#elif SNACK
      if (xml.name() == "evccodigo") {
        token = xml.readNext();
        if(xml.tokenType() == QXmlStreamReader::Characters) {
          DEBUG("evccodigo %s", xml.text().toString().toStdString().c_str());
          id = xml.text().toString().toInt();
          continue;
        }
      }
#endif

      if (xml.name() == "estado") {
        token = xml.readNext();
        if(xml.tokenType() == QXmlStreamReader::Characters) {
          DEBUG("estado %s", xml.text().toString().toStdString().c_str());
          result = xml.text().toString().toInt();
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

#ifdef TEMPO
  DEBUG("WS OK. evhcodigo = %d, estado = %d", id, result);
#elif SNACK
  DEBUG("WS OK. evccodigo = %d, estado = %d", id, result);
#endif
  return true;
}

