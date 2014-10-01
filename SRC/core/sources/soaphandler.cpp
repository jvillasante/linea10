#include "soaphandler.h"
#include "debug_macros.h"

#include <QMapIterator>

SoapHandler::SoapHandler(QSettings *settings)
{
  this->settings = settings;
  nam = new QNetworkAccessManager(this);

  connect(nam, SIGNAL(finished(QNetworkReply *)), SLOT(namFinished(QNetworkReply *)));
}

SoapHandler::~SoapHandler()
{
  if (nam) {
    delete nam;
  }
}

QString SoapHandler::getXmlPacket(const QString &method, const QString &wsNamespace, QMap<QString, QString> &map)
{
  QString strSOAP;
  strSOAP.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>") ;
  strSOAP.append("<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">");

  strSOAP.append("<soap:Body>");
  strSOAP.append("<").append(method).append(" xmlns=\"").append(wsNamespace).append("\">");

  QMapIterator<QString, QString> it(map);
  while (it.hasNext()) {
    it.next();
    strSOAP.append("<").append(it.key()).append(">").append(it.value()).append("</").append(it.key()).append(">");
  }

  strSOAP.append("</").append(method).append(">");
  strSOAP.append("</soap:Body>");
  strSOAP.append("</soap:Envelope>");

  return strSOAP;
}

void SoapHandler::fetch(const QString &method, const QString &wsNamespace, const QUrl &url, const QString &xmlData)
{
  if (!url.isValid()) {
    emit error(QString("URL %1 is not valid").arg(url.toString()));
    return;
  }

  DEBUG("Fetching SOAP service");
  DEBUG("URL: %s", url.toString().toStdString().c_str());
  DEBUG("HOST: %s", url.host().toStdString().c_str());
  DEBUG("PATH: %s", url.path().toStdString().c_str());
  DEBUG("PORT: %d", url.port());
  DEBUG("DATA: %s", xmlData.toStdString().c_str());

  QByteArray arr;
  arr.append(xmlData);

  QString soapAction = wsNamespace + method;
  DEBUG("SoapAction %s", soapAction.toStdString().c_str());

  QNetworkRequest request;
  request.setUrl(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(QString("text/xml;charset=utf-8")));
  request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(qulonglong(arr.size())));
  request.setRawHeader("Host", url.host().toAscii());
  request.setRawHeader("SOAPAction", soapAction.toAscii());
  request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QVariant(int(QNetworkRequest::AlwaysNetwork)));

  nam->post(request, arr);
  LOG_INFO("Request sent to service %s... Waiting response...", method.toStdString().c_str());
}

void SoapHandler::namFinished(QNetworkReply *response)
{
  LOG_INFO("Request to SOAP service finished...");

  if (response->error() == QNetworkReply::NoError) {
    QByteArray dataResult = response->readAll();
    QString resultData(dataResult);
    emit finished(resultData);
    LOG_INFO("Response OK");
    DEBUG("DATA: %s", resultData.toStdString().c_str());
  } else {
    emit error(response->errorString());
    LOG_ERROR("Response FAIL");
    DEBUG("ERROR: %s", response->errorString().toStdString().c_str());
  }

  response->deleteLater();
}
