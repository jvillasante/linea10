#include "firmwareupdater.h"

#include "debug_macros.h"
#include "utils.h"

#include <QCoreApplication>
#include <QXmlStreamReader>
#include <QDateTime>

FirmwareUpdater::FirmwareUpdater()
{
  QString appPath = QCoreApplication::applicationDirPath();
  settings = new QSettings("/mnt/jffs2/app.ini", QSettings::IniFormat);

  soapHandler = new SoapHandler(settings);
  connect(soapHandler, SIGNAL(finished(QString)), this, SLOT(finished(QString)));
  connect(soapHandler, SIGNAL(error(QString)), this, SLOT(error(QString)));

  timer = new QTimer(this);
  timer->setInterval(settings->value("timerActualizacion", 3600000).toInt());
  connect(timer, SIGNAL(timeout()), this, SLOT(doWork()));
  timer->start(); 
}

FirmwareUpdater::~FirmwareUpdater()
{
  delete timer;
  delete settings;
  delete soapHandler;
}

void FirmwareUpdater::doWork()
{
  if (Utils::fileExists("/usr/local/firmware.tar.gz") && Utils::fileExists("/usr/local/firmware.tar.gz.md5")) {
    DEBUG("firmware.tar.gz already exists in /usr/local");
    timer->start();
    return;
  }

  timer->stop();

  const QString method = "getActualizacionM";
  const QString wsNamespace = settings->value("wsNamespace").toString();
  QMap<QString, QString> map;
  map["equSerial"] = settings->value("serialEquipo").toString();
  map["vApp"] = settings->value("fwVersion").toString();
  const QString strSOAP = soapHandler->getXmlPacket(method, wsNamespace, map);

  const QUrl url(settings->value("wsFirmwareUpdateURL").toString());
  soapHandler->fetch(method, wsNamespace, url, strSOAP);
}

void FirmwareUpdater::finished(QString response)
{
  bool rc;
  QString fw = settings->value("fw").toString();
  QString fwMd5 = settings->value("fwMD5").toString();
  QString pathFw, pathFwMd5;
  int sizeFw = 0;
  int sizeFwMd5 = 0;

  CHECK(!response.contains("<title>Error en tiempo"), "Response error");

  rc = parseXml(response, pathFw, pathFwMd5, sizeFw, sizeFwMd5);
  CHECK(rc == true, "Error parsing XML");

  if ((pathFw == "E") || (pathFwMd5 == "E")) {
    DEBUG("WS Error. pathFw = %s, pathFwMd5 = %s", pathFw.toStdString().c_str(), pathFwMd5.toStdString().c_str());
    goto error;
  } else if ((pathFw == "N") || (pathFwMd5 == "N")) {
    DEBUG("No updates available. pathFw = %s, pathFwMd5 = %s", pathFw.toStdString().c_str(), pathFwMd5.toStdString().c_str());
  } else {
    rc = Utils::wgetCall(fw, pathFw);
    CHECK(rc == true, "Error sending wget command");

    rc = Utils::wgetCall(fwMd5, pathFwMd5);
    CHECK(rc == true, "Error sending wget command");

    rc = Utils::verifyFileSize(fw, sizeFw);
    CHECK(rc == true, "Error verifying firmware.tar.gz file size");

    rc = Utils::verifyFileSize(fwMd5, sizeFwMd5);
    CHECK(rc == true, "Error verifying firmware.tar.gz.md5 file size");

    rc = Utils::moveFile(fw, "/usr/local/");
    CHECK(rc == true, "Error sending mv command");

    rc = Utils::moveFile(fwMd5, "/usr/local/");
    CHECK(rc == true, "Error sending mv command");

    if (!Utils::verifyMd5("/usr/local/firmware.tar.gz.md5")) {
      DEBUG("FIRMWARE MD5 FAIL... REMOVING UPDATE FILES...");
      Utils::removeFile("/usr/local/firmware.tar.gz");
      Utils::removeFile("/usr/local/firmware.tar.gz.md5");
      return;
    }

    DEBUG("Updates copied to /usr/local/");
  }

  timer->start();
  return;

error:
  DEBUG("Error updating firmware. Restarting daemon...");
  timer->start();
}

void FirmwareUpdater::error(QString error)
{
  UNUSED(error);
  DEBUG("soaphandler error: %s", error.toStdString().c_str());
  timer->start();
}

bool FirmwareUpdater::parseXml(QString &response, QString &pathFw, QString &pathFwMd5, int &sizeFw, int &sizeFwMd5)
{
  QXmlStreamReader xml(response);

  while (!xml.atEnd() && !xml.hasError()) {
    QXmlStreamReader::TokenType token = xml.readNext();

    if (token == QXmlStreamReader::StartDocument) { continue; }

    if (token == QXmlStreamReader::StartElement) {
      if (xml.name() == "pathFw") {
        token = xml.readNext();
        if(xml.tokenType() == QXmlStreamReader::Characters) {
          DEBUG("pathFw %s", xml.text().toString().toStdString().c_str());
          pathFw = xml.text().toString();
          continue;
        }
      }

      if (xml.name() == "pathFwMd5") {
        token = xml.readNext();
        if(xml.tokenType() == QXmlStreamReader::Characters) {
          DEBUG("pathFwMd5 %s", xml.text().toString().toStdString().c_str());
          pathFwMd5 = xml.text().toString();
          continue;
        }
      }

      if (xml.name() == "sizeFw") {
        token = xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::Characters) {
          DEBUG("sizeFw %s", xml.text().toString().toStdString().c_str());
          sizeFw = xml.text().toString().toInt();
          continue;
        }
      }

      if (xml.name() == "sizeFwMd5") {
        token = xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::Characters) {
          DEBUG("sizeFwMd5 %s", xml.text().toString().toStdString().c_str());
          sizeFwMd5 = xml.text().toString().toInt();
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

  if (pathFw.isEmpty() || pathFwMd5.isEmpty()) {
    DEBUG("WS Error. pathFw = %s, pathFwMd5 = %s", pathFw.toStdString().c_str(), pathFwMd5.toStdString().c_str());
    return false;
  }

  DEBUG("WS OK. pathFw = %s, pathFwMd5 = %s", pathFw.toStdString().c_str(), pathFwMd5.toStdString().c_str());
  return true;
}
