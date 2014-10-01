#ifndef SOAPHANDLER_H
#define SOAPHANDLER_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QByteArray>
#include <QSettings>
#include <QMap>

class SoapHandler : public QObject
{
  Q_OBJECT
  public:
    explicit SoapHandler(QSettings *settings);
    ~SoapHandler();

    QString getXmlPacket(const QString &method, const QString &wsNamespace, QMap<QString, QString> &map);
    void fetch(const QString &method, const QString &wsNamespace, const QUrl &url, const QString &xmlData);
    void getMassiveLoad();

signals:
    void finished(QString);
    void error(QString);

    public slots:
      void namFinished(QNetworkReply *);

  private:
    QSettings *settings;
    QNetworkAccessManager *nam;
};

#endif // SOAPHANDLER_H
