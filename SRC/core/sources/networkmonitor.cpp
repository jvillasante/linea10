#include "networkmonitor.h"

#include "debug_macros.h"

#include <QProcess>

NetworkMonitor::NetworkMonitor(QSettings *settings)
{
  this->settings = settings;
  this->wsIp = settings->value("wsIP").toString();
  this->wsPort = settings->value("wsPort").toInt();
  this->socket = new QTcpSocket();
}

NetworkMonitor::~NetworkMonitor()
{
  delete this->socket;
}

bool NetworkMonitor::pingServer()
{
  DEBUG("Connecting to %s on port %d", this->wsIp.toStdString().c_str(), this->wsPort);
  this->socket->connectToHost(this->wsIp, this->wsPort);

  if (this->socket->waitForConnected(1000)) {
    DEBUG("Connected to server!");
    this->socket->close();
    return true;
  } else {
    DEBUG("Error connecting to server...");
    this->socket->close();
    return false;
  }
}

void NetworkMonitor::getNetworkInfo(QString &mac, QString &ip, QString &gateway, QString &masc, QString &broadcast)
{
  QString localhostname = QHostInfo::localHostName();
  QString localDomainname = QHostInfo::localDomainName();
  QHostInfo hostInfo = QHostInfo::fromName(localhostname);
  QNetworkInterface *network = new QNetworkInterface();

  foreach (QNetworkInterface interface, network->allInterfaces()) {
    if (!(interface.flags() & QNetworkInterface::IsLoopBack)) {
      foreach (QNetworkAddressEntry entry, interface.addressEntries()) {
        if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
          ip = entry.ip().toString();
          masc = entry.netmask().toString();
          broadcast = entry.broadcast().toString();
        }

        if (entry.ip().protocol() == QAbstractSocket::IPv6Protocol) {
          ip = entry.ip().toString();
        }
      }

      gateway = this->getGateway();
      mac = interface.hardwareAddress();
    }
  }

  if (network)
    delete network;
}

QString NetworkMonitor::getGateway()
{
  QString pGateway;
  QProcess process;
  process.start("route -n");
  process.waitForFinished(-1); // will wait forever until finished

  QString stdout = process.readAllStandardOutput();
  QString stderr = process.readAllStandardError();

  if(!stdout.isEmpty()) {
    QString delimiterPattern("\n"); //delimitador de la cadena IP-PUERTO
    QStringList strGwInfo= stdout.split(delimiterPattern);
    QString strGwData=strGwInfo.at(3).trimmed();
    QStringList strGwList=strGwData.split("         ");
    QString strGateway= strGwList.at(1).trimmed();
    QStringList strGwListAux= strGateway.split("     ");
    pGateway= strGwListAux.at(0).trimmed();
  } else {
    pGateway = "UNKNOWN";
  }

  return pGateway;
}

