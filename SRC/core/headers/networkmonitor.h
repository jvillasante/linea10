#ifndef NETWORKMONITOR_H
#define NETWORKMONITOR_H

#include <QObject>
#include <QSettings>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>

class NetworkMonitor : public QObject
{
  Q_OBJECT
    
  public:
    explicit NetworkMonitor(QSettings *settings);
    ~NetworkMonitor();

  public:
    bool pingServer();
    void getNetworkInfo();
    void getNetworkInfo(QString &mac, QString &ip, QString &gateway, QString &masc, QString &broadcast);
  
  private:
    QSettings *settings;
    QTcpSocket *socket;

    QString wsIp;
    int wsPort;

  private:
    QString getGateway();
};

#endif // NETWORKMONITOR_H
