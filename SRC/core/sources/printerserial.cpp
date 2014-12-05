#include "printerserial.h"

#include <debug_macros.h>
#include <unistd.h>
#include "qextserialenumerator.h"
#include "utils.h"

#include <QByteArray>
#include <QDateTime>
#include <QDate>

PrinterSerial::PrinterSerial(QSettings *settings)
{
  this->settings = settings;
  this->lang = this->settings->value("lang", "es").toString();
  this->serialPort = NULL;
  this->connectedStatus = true;
}

PrinterSerial::~PrinterSerial()
{
  if (this->serialPort != NULL) {
    if (this->serialPort->isOpen()) {
      this->serialPort->close();
    }

    delete this->serialPort;
    LOG_INFO("Printer Serial is now closed");
  }
}

bool PrinterSerial::init()
{
  if (this->serialPort == NULL) {
    this->serialPort = new QextSerialPort("/dev/ttyS1", QextSerialPort::EventDriven);
    this->serialPort->setBaudRate(BAUD9600);
    this->serialPort->setDataBits(DATA_8);
    this->serialPort->setParity(PAR_NONE);
    this->serialPort->setFlowControl(FLOW_OFF);
    this->serialPort->setStopBits(STOP_1);
    this->serialPort->setTimeout(500);
    this->serialPort->waitForReadyRead(100);

    if (this->serialPort->open(QIODevice::ReadWrite) == true) {
#ifndef NDEBUG
      QObject::connect(this->serialPort, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
#endif
      DEBUG("PrinterSerial: Listening for data on /dev/ttyS1");

      this->selectPrinter();

      LOG_INFO("PrinterSerial::init OK");
      return true;
    } else {
      DEBUG("PrinterSerial: Device failed to open");
      LOG_ERROR("PrinterSerial::init FAIL");
      return false;
    }
  } else {
    this->serialPort->close();
    return false;
  }
}

void PrinterSerial::write_hello(QString mac, QString ip, QString gateway, QString masc, QString broadcast)
{
  QByteArray bytes;
  QString serial = settings->value("serialEquipo").toString();
  QString type = settings->value("tipoEquipo").toString();
  QString empresa = settings->value("empresaHolding").toString();
  QString lang = settings->value("lang").toString();
  lang = (lang == "en") ? "EN" : "ES";

  fontCompressed();

  bytes.append(tr("SERIAL: ").toAscii()).append(serial.toAscii()).append("\r\n");
  bytes.append(tr("TECNOLOGIA: BIOMETRIA DACTILAR").toAscii()).append("\r\n");
  bytes.append(tr("SENSORES BIOMETRICOS: ").toAscii()).append(QString("1\r\n").toAscii());
  bytes.append(tr("MODO BIOMETRIA: IDENTIFICACION NO NATIVA").toAscii()).append("\r\n");
  bytes.append(tr("LENGUAJE: ").toAscii()).append(lang.toAscii()).append("\r\n");
  bytes.append(tr("TIPO DE EQUIPO: ").toAscii()).append(type.toAscii()).append("\r\n");
  bytes.append(tr("EMPRESA: ").toAscii()).append(empresa.toAscii()).append("\r\n");
  bytes.append(tr("ETHERNET MAC: ").toAscii()).append(mac.toAscii()).append("\r\n");
  bytes.append(tr("ETHERNET IP: ").toAscii()).append(ip.toAscii()).append("\r\n");
  bytes.append(tr("ETHERNET GATEWAY: ").toAscii()).append(gateway.toAscii()).append("\r\n");
  bytes.append(tr("ETHERNET MASC: ").toAscii()).append(masc.toAscii()).append("\r\n");
  bytes.append(tr("ETHERNET BROADCASTING: ").toAscii()).append(broadcast.toAscii()).append("\r\n");
  
  QDateTime now = Utils::getCurrentTimestamp();
  if (lang == "EN") {
    bytes.append(QString("Date :%1 Time: %2\r\n").arg(getDate(now)).arg(now.toString("hh:mm")).toAscii());
  } else {
    bytes.append(QString("Fecha :%1 Hora: %2\r\n").arg(getDate(now)).arg(now.toString("hh:mm")).toAscii());
  }
  
  if (this->writeBytes(bytes)) {
    LOG_INFO("PrinterSerial::write_hello OK");
    this->send_cut();
  } else {
    LOG_INFO("PrinterSerial::write_hello FAIL");
  }
}

#ifdef TEMPO
void PrinterSerial::write_user(QString type, QString userIdentifier, QString userName, QString userRut, QString userEmp)
{
  QByteArray bytes;

  fontCompressed();
  boldOn();
  alignCenter();

  if (!userEmp.isEmpty())
    bytes.append(QString(userEmp).toAscii());
  else
    bytes.append(settings->value("empresaHolding", "Banco de Chile").toString().toAscii());
  bytes.append("\r\n");
  bytes.append(QString("-- ").toAscii());
  bytes.append(type.toAscii());
  bytes.append(QString(" --").toAscii());
  bytes.append("\r\n\r\n");

  if (this->writeBytes(bytes)) {
    boldOff();
    alignLeft();
    bytes.clear();

    QDateTime now = Utils::getCurrentTimestamp();
    
    if (lang == "en") {
      bytes.append(QString("Date   :%1 Time: %2\r\n").arg(getDate(now)).arg(now.toString("hh:mm")).toAscii());
    } else {
      bytes.append(QString("Fecha  :%1 Hora: %2\r\n").arg(getDate(now)).arg(now.toString("hh:mm")).toAscii());
    }

    bytes.append(QString(tr("Ident  :")).toAscii());
    bytes.append(QString(userIdentifier).toAscii());
    bytes.append(QString("\r\n").toAscii());

    bytes.append(QString(tr("RUT    :")).toAscii());
    bytes.append(QString(userRut).toAscii());
    bytes.append(QString("\r\n").toAscii());

    bytes.append(QString(tr("Nombre :")).toAscii());
    bytes.append(QString(userName).toAscii());
    bytes.append(QString("\r\n").toAscii());

    if (this->writeBytes(bytes)) {
      LOG_INFO("PrinterSerial::write_user OK");
      this->send_cut();
    } else {
      LOG_ERROR("PrinterSerial::write_user FAIL");
    }
  } else {
    LOG_ERROR("PrinterSerial::write_user FAIL");
  }
}
#elif SNACK
void PrinterSerial::write_user(QString service, QString userIdentifier, QString userName, QString userRut, 
    QString userEmp, QString userCentroCosto)
{
  QByteArray bytes;

  fontCompressed();
  boldOn();
  alignCenter();

  if (!userEmp.isEmpty())
    bytes.append(QString(userEmp).toAscii());
  else
    bytes.append(settings->value("empresaHolding", "Banco de Chile").toString().toAscii());
  bytes.append("\r\n");
  bytes.append(QString("-- ").toAscii());
  bytes.append(service.toAscii());
  bytes.append(QString(" --").toAscii());
  bytes.append("\r\n\r\n");

  if (this->writeBytes(bytes)) {
    boldOff();
    alignLeft();
    bytes.clear();

    QDateTime now = Utils::getCurrentTimestamp();
    
    if (lang == "en") {
      bytes.append(QString("Date   :%1 Time: %2\r\n").arg(getDate(now)).arg(now.toString("hh:mm")).toAscii());
    } else {
      bytes.append(QString("Fecha  :%1 Hora: %2\r\n").arg(getDate(now)).arg(now.toString("hh:mm")).toAscii());
    }

    bytes.append(QString(tr("Ident  :")).toAscii());
    bytes.append(QString(userIdentifier).toAscii());
    bytes.append(QString("\r\n").toAscii());

    bytes.append(QString(tr("RUT    :")).toAscii());
    bytes.append(QString(userRut).toAscii());
    bytes.append(QString("\r\n").toAscii());

    bytes.append(QString(tr("Nombre :")).toAscii());
    bytes.append(QString(userName).toAscii());
    bytes.append(QString("\r\n").toAscii());
    
    bytes.append(QString(tr("C.Costo:")).toAscii());
    bytes.append(QString(userCentroCosto).toAscii());
    bytes.append(QString("\r\n").toAscii());

    if (this->writeBytes(bytes)) {
      LOG_INFO("PrinterSerial::write_user OK");
      this->send_cut();
    } else {
      LOG_ERROR("PrinterSerial::write_user FAIL");
    }
  } else {
    LOG_ERROR("PrinterSerial::write_user FAIL");
  }
}
#endif

void PrinterSerial::setStatus(bool status) { this->connectedStatus = status; }
bool PrinterSerial::getStatus() { return this->connectedStatus; }

bool PrinterSerial::getRealTimePaperStatus(QString &msg)
{
  QByteArray bytes("\x10\x04\x04\x00", 4);
  msg = "";

  int sizedata = bytes.size();
  int i = this->serialPort->write(bytes, sizedata);
  this->serialPort->flush();

  if (i == sizedata) {
    DEBUG("PrinterSerial::getRealTimePaperStatus Write OK");
    char pData[2] = {0};
    // sleep(2);
    int e = this->serialPort->read(&pData[0], 1);
    
    if (e == 1) {
      DEBUG("PrinterSerial::getRealTimePaperStatus Leyendo Estado del PapeAl");
      switch (pData[0]) {
        case 18:  // tiene papel 12 hex
          msg = "Impresora con papel";
          DEBUG("Impresora con papel");
          return true;
          break;
        case 114: // no tiene papel 72 hex
          msg = "Impresora no tiene papel o Tapa abierta";
          DEBUG("Impresora no tiene papel o tapa abierta");
          return false;
          break;
        default:
          msg = "Error Impresora";
          DEBUG("Error Impresora");
          return false;
          break;
      }
    } else {
      msg = "Fallo leyendo Estado del Papel";
      DEBUG("Fallor leyendo Estado del Papel");
      return false;
    }
  } else {
    msg = "Fallo Comando Estado del Papel";
    DEBUG("Fallo Comando Estado del Papel");
    return false;
  }
  
  return false;
}

bool PrinterSerial::getRealTimeStatus(QString &msg)
{
  QByteArray bytes("\x10\x04\x01\x00", 4);
  msg = "";

  int sizedata = bytes.size();
  int i = this->serialPort->write(bytes, sizedata);
  this->serialPort->flush();

  if (i == sizedata) {
    DEBUG("PrinterSerial::getRealTimeStatus Write OK");
    char pData[2] = {0};
    int e = this->serialPort->read(&pData[0], 1);

    if (e == 1) {
      DEBUG("PrinterSerial::getRealTimeStatus Leyendo estado de la impresora");
      switch(pData[0]) {
        case 22:  // ONLINE 16 hex
          msg = "Impresora Online";
          DEBUG("Impresora Online");
          return true;
        case 30:  // OFFLINE 1E hex
          msg = "Impresora Offline";
          DEBUG("Impresora Offline");
          return false;
        default:
          msg = "Printer Error";
          DEBUG("Printer Error");
          return false;
      }
    } else {
      msg = "Fallo leyendo Estado de Impresora";
      DEBUG("Fallo leyendo Estado de Impresora");
      return false;
    }
  } else {
    msg = "Fallo Comando Estado de Impresora";
    DEBUG("Fallo Comando Estado de Impresora");
    return false;
  }
  
  return false;
}

bool PrinterSerial::getRealTimeOfflineStatus(QString &msg)
{
  QByteArray bytes("\x10\x04\x02\x00", 4);
  msg = "";

  int sizedata = bytes.size();
  int i = this->serialPort->write(bytes, sizedata);
  this->serialPort->flush();
  
  if (i == sizedata) {
    DEBUG("PrinterSerial::getRealTimeOfflineStatus Write OK");
    char pData[2] = {0};
    int e = this->serialPort->read(&pData[0], 1);

    if (e == 1) {
      DEBUG("Leyendo Offline Status");
      switch (pData[0]) {
        case 18:  // 12 hex normal
          msg = "Impresora OK";
          DEBUG("Impresora OK");
          return true;
          break;
        case 114: // 72 hex impresora sin papel
          msg = "Impresora sin papel";
          DEBUG("Impresora sin papel");
          return true;
          break;
        case 90:  // 5A hex presionando feed button con tapa cerrada y con papel
          msg = "Presionando Boton Alimentacion";
          DEBUG("Presionando Boton Alimentacion");
          return true;
          break;
        case 86:  // 56 hex tapa de impresora abierta con papel dentro
          msg = "Levantada Tapa Impresora";
          DEBUG("Levantada Tapa Impresora");
          return true;
          break;
        case 122:  // 7A hex impresora sin papel presionando feed button
          msg = "Impresora Sin Papel, Boton Alimentacion";
          DEBUG("Impresora Sin Papel, Boton Alimentacion");
          return true;
          break;
        case 118:  // 76 hex impresora sin papel con tapa abierta
          msg = "Impresora sin Papel con Tapa Abierta";
          DEBUG("Impresora sin Papel con Tapa Abierta");
          return true;
          break;
        case 62:  // 3E hex sin papel con tapa abierta presionando feed button
          msg = "Impresora sin Papel con Tapa Abierta, Boton Alimentacion";
          DEBUG("Impresora sin Papel con Tapa Abierta, Boton Alimentacion");
          return true;
          break;
        default:
          msg = "Error offline command";
          DEBUG("Error offline command");
          return false;
          break;
      }
    } else {
      msg = "Fallo leyendo offline status";
      DEBUG("Fallo leyendo offline status");
      return false;
    }
  } else {
    msg = "Fallo comando offline status";
    DEBUG("Fallo comando offline status");
    return false;
  }

  return false;
}

void PrinterSerial::onReadyRead()
{
  QByteArray bytes;
  int availableBytes = this->serialPort->bytesAvailable();
  bytes.resize(availableBytes);
  this->serialPort->read(bytes.data(), bytes.size());

  DEBUG("PrinterSerial: bytes read: %d", bytes.size());
  DEBUG("PrinterSerial: data read: %s", QString::fromAscii(bytes).toStdString().c_str());
}

bool PrinterSerial::writeBytes(QByteArray bytes)
{
  int size = bytes.size();

  if (size > 0) {
    int rc = this->serialPort->write(bytes, size);
    this->serialPort->flush();

    if (rc == size) {
      return true;
    } else {
      return false;
    }
  }

  return false;
}

bool PrinterSerial::send_cut()
{
  QByteArray bytes("\x1D\x56\x42\x00", 4);  // hex(42) = decimal(66)

  if (this->writeBytes(bytes)) {
    DEBUG("PrinterSerial: Send Cut OK");
    return true;
  } else {
    DEBUG("PrinterSerial: Send Cut FAIL");
    return false;
  }
}

bool PrinterSerial::selectPrinter()
{
  QByteArray bytes("\x1B\x3D\x01\x00", 4);

  if (this->writeBytes(bytes)) {
    DEBUG("PrinterSerial: Select Printer OK");
    return true;
  } else {
    DEBUG("PrinterSerial: Select Printer FAIL");
    return false;
  }
}

bool PrinterSerial::boldOn()
{
  QByteArray bytes("\x1B\x45\xFF\x00", 4);

  if (this->writeBytes(bytes)) {
    DEBUG("PrinterSerial: BOLD ON OK");
    return true;
  } else {
    DEBUG("PrinterSerial: BOLD ON FAIL");
    return false;
  }
}

bool PrinterSerial::boldOff()
{
  QByteArray bytes("\x1B\x45\x00\x00", 4);

  if (this->writeBytes(bytes)) {
    DEBUG("PrinterSerial: BOLD OFF OK");
    return true;
  } else {
    DEBUG("PrinterSerial: BOLD OFF FAIL");
    return false;
  }
}

bool PrinterSerial::alignLeft()
{
  QByteArray bytes("\x1B\x61\x00\x00", 4);

  if (this->writeBytes(bytes)) {
    DEBUG("PrinterSerial: ALIGN LEFT OK");
    return true;
  } else {
    DEBUG("PrinterSerial: ALIGN LEFT FAIL");
    return false;
  }
}

bool PrinterSerial::alignCenter()
{
  QByteArray bytes("\x1B\x61\x01\x00", 4);

  if (this->writeBytes(bytes)) {
    DEBUG("PrinterSerial: ALIGN CENTER OK");
    return true;
  } else {
    DEBUG("PrinterSerial: ALIGN CENTER FAIL");
    return false;
  }
}

bool PrinterSerial::alignRight()
{
  QByteArray bytes("\x1B\x61\x02\x00", 4);

  if (this->writeBytes(bytes)) {
    DEBUG("PrinterSerial: ALIGN RIGHT OK");
    return true;
  } else {
    DEBUG("PrinterSerial: ALIGN RIGHT FAIL");
    return false;
  }
}

bool PrinterSerial::fontStandard()
{
  QByteArray bytes("\x1B\x21\x00\x00", 4);

  if (this->writeBytes(bytes)) {
    DEBUG("PrinterSerial: FONT STANDARD OK");
    return true;
  } else {
    DEBUG("PrinterSerial: FONT STANDARD FAIL");
    return false;
  }
}

bool PrinterSerial::fontCompressed()
{
  QByteArray bytes("\x1B\x4D\x01\x00", 4);

  if (this->writeBytes(bytes)) {
    DEBUG("PrinterSerial: FONT COMPRESSED OK");
    return true;
  } else {
    DEBUG("PrinterSerial: FONT COMPRESSED FAIL");
    return false;
  }
}

QString PrinterSerial::getDate(QDateTime now)
{
  QString strDate;
  QDate currentDate = now.date();

  if (lang == "en") {
    strDate = currentDate.toString("ddd, dd/MM/yyyy");
  } else {
    QString strdayweek = "";

    switch (currentDate.dayOfWeek()) {
      case 1:
        strdayweek = "LUN";
        break;
      case 2:
        strdayweek = "MAR";
        break;
      case 3:
        strdayweek = "MIE";
        break;
      case 4:
        strdayweek = "JUE";
        break;
      case 5:
        strdayweek = "VIE";
        break;
      case 6:
        strdayweek = "SAB";
        break;
      case 7:
        strdayweek = "DOM";
        break;
      default:
        strdayweek = "ERROR";
        break;
    }

    strDate = strdayweek + ", " + now.toString("dd/MM/yyyy");
  }

  return strDate;
}
