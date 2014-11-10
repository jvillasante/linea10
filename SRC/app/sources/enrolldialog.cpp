#include "enrolldialog.h"

#include "debug_macros.h"
#include "utils.h"
#include "biobase64.h"

#include <QObject>
#include <unistd.h>
#include <QApplication>
#include <QXmlStreamReader>
#include <QStringList>
#include <QRgb>
#include <QColor>
#include <QPixmap>
#include <QVector>
#include <QImage>
#include <QByteArray>
#include <QBuffer>

EnrollDialog::EnrollDialog(QWidget *parent, QSettings *settings, WorkerSensorMulti *workerSensor) :
  QDialog(parent)
{
  DEBUG("EnrollDialog Constructor Called");
  this->settings = settings;
  this->rutNumber = "";
  this->screen = 1;
  
  soapHandlerPersona = new SoapHandler(this->settings);
  connect(soapHandlerPersona, SIGNAL(finished(QString)), this, SLOT(verificaPersonaFinished(QString)));
  connect(soapHandlerPersona, SIGNAL(error(QString)), this, SLOT(verificaPersonaError(QString)));
  
  soapHandlerEnroll = new SoapHandler(this->settings);
  connect(soapHandlerEnroll, SIGNAL(finished(QString)), this, SLOT(enrollFinished(QString)));
  connect(soapHandlerEnroll, SIGNAL(error(QString)), this, SLOT(enrollError(QString)));
  
  this->workerSensor = workerSensor;
  connect(workerSensor, SIGNAL(enrollWorkDone(uchar *, uchar *, int, int)), this, SLOT(enrollWorkDone(uchar *, uchar *, int, int)));
  
  this->setWindowOpacity(1.0);
  this->setWindowState(Qt::WindowFullScreen);
  this->resize(240, 320);
  
  initializeUI();
  
  closeTimer = new QTimer(this);
  closeTimer->setInterval(settings->value("timerCloseEnrollDialog", 60000).toInt());
  connect(closeTimer, SIGNAL(timeout()), this, SLOT(timerCloseDialog()));
  closeTimer->start();
}

EnrollDialog::~EnrollDialog()
{
  DEBUG("EnrollDialog Destructor Called");
  
  delete soapHandlerPersona;
  delete soapHandlerEnroll;
  delete closeTimer;
  
  delete grid;
  delete btnOk;
  delete btnCancel;
  
  // screen 1
  delete edtRut;
  delete lblMsg;
  delete btn1;
  delete btn2;
  delete btn3;
  delete btn4;
  delete btn5;
  delete btn6;
  delete btn7;
  delete btn8;
  delete btn9;
  delete btn0;
  delete btnHyphen;
  delete btnK;
  delete btnDelete;

  // screen 2
  delete lblEmpresa;
  delete lblName;
  delete lblMsgScreen2;
}

void EnrollDialog::verificaPersona()
{
  closeTimer->stop();
  
  const QString method = "VerificaPersonaM";
  const QString wsNamespace = settings->value("wsNamespace").toString();
  QMap<QString, QString> map;
  map["equSerial"] = settings->value("serialEquipo").toString();
  map["identificacion"] = rutNumber;
  const QString strSOAP = soapHandlerPersona->getXmlPacket(method, wsNamespace, map);

  const QUrl url(settings->value("wsVerificaPersonaURL").toString());
  soapHandlerPersona->fetch(method, wsNamespace, url, strSOAP);
}

void EnrollDialog::verificaPersonaFinished(QString response)
{
  bool rc;
  QString identifier, rut, name;
  
  closeTimer->start();
  
  CHECK(!response.contains("<title>Error en tiempo"), "Response error");
  
  rc = parseXmlPersona(response, identifier, rut, name);
  CHECK(rc == true, "Error parsing XML");

  if (identifier == "0") {
    DEBUG("User does not exists. identifier = %s", identifier.toStdString().c_str());
    setScreen2("", "", "");
    return;
  }
  
  DEBUG("User exists. identifier = %s", identifier.toStdString().c_str());
  setScreen2(identifier, rut, name);
  workerSensor->requestMethod(WorkerSensorMulti::Enroll);
  return;
  
error:
  setScreen2("", "", "");
  DEBUG("verificaPersona error...");
}

void EnrollDialog::verificaPersonaError(QString error)
{
  closeTimer->start();
  UNUSED(error);
  setScreen2("", "", "");
  DEBUG("soaphandler error: %s", error.toStdString().c_str());
}

void EnrollDialog::enrollWorkDone(uchar *compositeImage, uchar *templateImage, int width, int height)
{
  if (compositeImage == NULL) { return; }

  DEBUG("EnrollWorkDone!!!!!");

  uchar imagio[width][height];
  int count = 0;
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      imagio[i][j] = compositeImage[count];
      count++;
    }
  } 
  delete [] compositeImage;
  
  uchar templateio[2048];
  for (int i = 0; i < 2048; i++) {
    templateio[i] = templateImage[i];
  }
  delete [] templateImage;

  // Mostrando imagen en pantalla
  QVector<QRgb> colorTable;
  for (int i = 0; i < 256; i++) { colorTable.push_back(QColor(i, i, i).rgb()); };
  QImage image((uchar *) imagio, width, height, QImage::Format_Indexed8); // was incorrect
  image.setColorTable(colorTable);

  QImage scaled_image(image.scaled(240, 200, Qt::KeepAspectRatio));

  lblName->setVisible(false);
  grid->removeWidget(grid->itemAtPosition(0, 0)->widget());
  grid->removeWidget(grid->itemAtPosition(1, 0)->widget());
  grid->removeWidget(grid->itemAtPosition(2, 0)->widget());
  
  grid->addWidget(lblEmpresa, 0, 0, 1, 4, Qt::AlignCenter);
  grid->addWidget(lblMsgScreen2, 1, 0, 4, 4, Qt::AlignCenter);
  
  btnOk->setVisible(true);
  lblMsgScreen2->setPixmap(QPixmap::fromImage(scaled_image));
  lblEmpresa->setText(tr("Aceptar para enrolar"));

  update();
  qApp->processEvents();

  // Creando imagenes para llamada a servicio web
  biobase64* base64help = new biobase64();
  QByteArray templateArray((char*) &templateio[0]);
  QByteArray imageArray((char*) &imagio[0]);
  imgRawB64 = base64help->base64_encode((const uchar *) &imagio[0], (unsigned int) sizeof(imagio));
  template1 = base64help->base64_encode((const uchar *) &templateio[0], (unsigned int) sizeof(templateio));

  //convirtiendo la imagen a jpg
  QByteArray im;
  QBuffer bufferJpeg(&im);
  bufferJpeg.open(QIODevice::WriteOnly);
  if (!image.save(&bufferJpeg, "PNG")) {
    DEBUG("Error saving image to buffer");
  }
  bufferJpeg.close();

  int pimgsize = 0;
  pimgsize = bufferJpeg.data().length();
  DEBUG("jpg size: %d", pimgsize);
  unsigned char pimgaux[pimgsize];
  for(int e = 0;e < pimgsize; e++) { pimgaux[e]= (unsigned char) bufferJpeg.data().at(e); }

  imgbaseJpg64 = base64help->base64_encode(pimgaux, (unsigned int) sizeof(pimgaux));
  DEBUG("jpg size: %d", imgbaseJpg64.size());
}

void EnrollDialog::enroll(QString &identificacion, QString &imgRawB64, QString &imgJpgB64, QString &tmp1)
{
  const QString method = "enrollM";
  const QString wsNamespace = settings->value("wsNamespace").toString();
  QMap<QString, QString> map;
  map["equSerial"] = settings->value("serialEquipo").toString();
  map["identificacion"] = identificacion;
  map["imgRawB64_1"] = imgRawB64;
  map["imgJpgB64_1"] = imgJpgB64;
  map["templateLumiB64_1"] = tmp1;
  const QString strSOAP = soapHandlerEnroll->getXmlPacket(method, wsNamespace, map);

  const QUrl url(settings->value("wsEnrollURL").toString());
  soapHandlerEnroll->fetch(method, wsNamespace, url, strSOAP);
}

void EnrollDialog::enrollFinished(QString response)
{
  bool rc;
  QString identificacion, serial, resultado;
  
  CHECK(!response.contains("<title>Error en tiempo"), "Response error");
  
  rc = parseXmlEnroll(response, identificacion, serial, resultado);
  CHECK(rc == true, "Error parsing XML");

  DEBUG("*******Identificacion: %s", identificacion.toStdString().c_str());

  if (resultado == "-1") {
    DEBUG("WS Error. identificacion = %s", identificacion.toStdString().c_str());
    closeDialog(tr("Error procesando..."));
    return;
  }

  if (resultado == "0") {
    DEBUG("Fingerprint already exists. identificacion = %s", identificacion.toStdString().c_str());
    closeDialog(tr("Huella ya existe..."));
    return;
  }
  
  DEBUG("User fingerprint enrolled. identificacion = %s", identificacion.toStdString().c_str());
  closeDialog(tr("Huella enrolada..."));
  return;
  
error:
  closeDialog(tr("Error procesando..."));
  DEBUG("enroll error...");
}

void EnrollDialog::enrollError(QString error)
{
  UNUSED(error);
  DEBUG("soaphandler error: %s", error.toStdString().c_str());
  closeDialog(tr("Error procesando..."));
}

void EnrollDialog::btn1Clicked() { closeTimer->stop(); rutNumber.append("1"); edtRut->setText(rutNumber); closeTimer->start(); }
void EnrollDialog::btn2Clicked() { closeTimer->stop(); rutNumber.append("2"); edtRut->setText(rutNumber); closeTimer->start(); }
void EnrollDialog::btn3Clicked() { closeTimer->stop(); rutNumber.append("3"); edtRut->setText(rutNumber); closeTimer->start(); }
void EnrollDialog::btn4Clicked() { closeTimer->stop(); rutNumber.append("4"); edtRut->setText(rutNumber); closeTimer->start(); }
void EnrollDialog::btn5Clicked() { closeTimer->stop(); rutNumber.append("5"); edtRut->setText(rutNumber); closeTimer->start(); }
void EnrollDialog::btn6Clicked() { closeTimer->stop(); rutNumber.append("6"); edtRut->setText(rutNumber); closeTimer->start(); }
void EnrollDialog::btn7Clicked() { closeTimer->stop(); rutNumber.append("7"); edtRut->setText(rutNumber); closeTimer->start(); }
void EnrollDialog::btn8Clicked() { closeTimer->stop(); rutNumber.append("8"); edtRut->setText(rutNumber); closeTimer->start(); }
void EnrollDialog::btn9Clicked() { closeTimer->stop(); rutNumber.append("9"); edtRut->setText(rutNumber); closeTimer->start(); }
void EnrollDialog::btn0Clicked() { closeTimer->stop(); rutNumber.append("0"); edtRut->setText(rutNumber); closeTimer->start(); }
void EnrollDialog::btnHyphenClicked() { closeTimer->stop(); rutNumber.append("-"); edtRut->setText(rutNumber); closeTimer->start(); }
void EnrollDialog::btnKClicked() { closeTimer->stop(); rutNumber.append("K"); edtRut->setText(rutNumber); closeTimer->start(); }
void EnrollDialog::btnDeleteClicked() { closeTimer->stop(); rutNumber.chop(1); edtRut->setText(rutNumber); closeTimer->start(); }

void EnrollDialog::btnOkClicked()
{
  if (screen == 1) {
    int rut;
    QString digit;
    char calculatedDigit;

    if (rutNumber.isEmpty()) {
      lblMsg->setText(tr("RUT Vacío"));
      return;
    }

    // if (rutNumber.length() != 10) {
    //   lblMsg->setText(tr("RUT Inválido"));
    //   return;
    // }

    QStringList splitted = rutNumber.split("-");
    if (splitted.length() != 2) {
      lblMsg->setText(tr("RUT Inválido"));
      return;
    }

    rut = splitted.at(0).trimmed().toInt();
    digit = splitted.at(1).trimmed();
    calculatedDigit = Utils::rutVerifyDigit((unsigned) rut); 

    DEBUG("**************************************************************");
    DEBUG("rut: %d", rut);
    DEBUG("digit: %s", digit.toStdString().c_str());
    DEBUG("calculatedDigit: %s", QString(calculatedDigit).toStdString().c_str());
    DEBUG("**************************************************************");
    
    if (digit != QString(calculatedDigit)) {
      lblMsg->setText(tr("RUT Inválido"));
      return;
    }

    verificaPersona();
  } else if (screen == 2) {
    DEBUG("Comenzando el proceso de enrolamiento");
    btnOk->setVisible(false);
    btnCancel->setVisible(false);
    lblEmpresa->setText(tr("Enrolando... Espere..."));
    update();
    qApp->processEvents();
    
    enroll(rutNumber, imgRawB64, imgbaseJpg64, template1);
  } else {  // error
    closeDialog(tr("Error procesando..."));
  }
}

void EnrollDialog::btnCancelClicked()
{
  closeDialog(tr("Operacion cancelada..."));
}

void EnrollDialog::timerCloseDialog()
{
  closeDialog(tr("Enrolamiento Timeout..."));
}

void EnrollDialog::closeDialog(QString msg)
{
  rutNumber = "";
  emit enrollDialogClosed(msg);
  sleep(1);
  close();
}

void EnrollDialog::closeEvent(QCloseEvent *event)
{
  UNUSED(event);
  
  disconnect(soapHandlerPersona, SIGNAL(finished(QString)), this, SLOT(verificaPersonaFinished(QString)));
  disconnect(soapHandlerPersona, SIGNAL(error(QString)), this, SLOT(verificaPersonaError(QString)));
  disconnect(soapHandlerEnroll, SIGNAL(finished(QString)), this, SLOT(enrollFinished(QString)));
  disconnect(soapHandlerEnroll, SIGNAL(error(QString)), this, SLOT(enrollError(QString)));
  
  disconnect(workerSensor, SIGNAL(enrollWorkDone(uchar *, uchar *, int, int)), this, SLOT(enrollWorkDone(uchar *, uchar *, int, int)));
  workerSensor->requestMethod(WorkerSensorMulti::Identify);
  this->deleteLater();
}

void EnrollDialog::initializeUI()
{
  grid = new QGridLayout();
  
  lblMsg = new QLabel(tr("Introduzca su RUT"));
  lblMsg->setAlignment(Qt::AlignCenter);
  lblMsg->setStyleSheet("font-size: 12px;"
      "color: #FFFFFF;"
      "text-align: center;"
      "font-style: normal;"
      "font-weight: bold;");

  edtRut = new QLineEdit();
  edtRut->resize(100, 50);
  edtRut->setStyleSheet("background:rgb(0,102,153);"
      "color:rgb(255,255,255);"
      "font-size:18px;"
      "padding-right: 10px;");
  
  btnDelete = new QPushButton();
  btnDelete->resize(50, 50);
  btnDelete->setFlat(true);
  btnDelete->setStyleSheet("background-image: url(:/img/Resources/images/borrar.bmp);"
      "background-repeat: no repeat;"
      "background-position: center center;"
      "color:rgb(255,255,255);"
      "font-size:18px");
  connect(btnDelete, SIGNAL(clicked()), this, SLOT(btnDeleteClicked()));
  
  btn1 = new QPushButton();
  btn1->resize(50, 50);
  btn1->setFlat(true);
  btn1->setStyleSheet("background-image: url(:/img/Resources/images/1.bmp);"
      "background-repeat: no repeat;"
      "background-position: center center;"
      "color:rgb(255,255,255);"
      "font-size:18px");
  connect(btn1, SIGNAL(clicked()), this, SLOT(btn1Clicked()));

  btn2 = new QPushButton();
  btn2->resize(50, 50);
  btn2->setFlat(true);
  btn2->setStyleSheet("background-image: url(:/img/Resources/images/2.bmp);"
      "background-repeat: no repeat;"
      "background-position: center center;"
      "color:rgb(255,255,255);"
      "font-size:18px");
  connect(btn2, SIGNAL(clicked()), this, SLOT(btn2Clicked()));
  
  btn3 = new QPushButton();
  btn3->resize(50, 50);
  btn3->setFlat(true);
  btn3->setStyleSheet("background-image: url(:/img/Resources/images/3.bmp);"
      "background-repeat: no repeat;"
      "background-position: center center;"
      "color:rgb(255,255,255);"
      "font-size:18px");
  connect(btn3, SIGNAL(clicked()), this, SLOT(btn3Clicked()));
  
  btn4 = new QPushButton();
  btn4->resize(50, 50);
  btn4->setFlat(true);
  btn4->setStyleSheet("background-image: url(:/img/Resources/images/4.bmp);"
      "background-repeat: no repeat;"
      "background-position: center center;"
      "color:rgb(255,255,255);"
      "font-size:18px");
  connect(btn4, SIGNAL(clicked()), this, SLOT(btn4Clicked()));
  
  btn5 = new QPushButton();
  btn5->resize(50, 50);
  btn5->setFlat(true);
  btn5->setStyleSheet("background-image: url(:/img/Resources/images/5.bmp);"
      "background-repeat: no repeat;"
      "background-position: center center;"
      "color:rgb(255,255,255);"
      "font-size:18px");
  connect(btn5, SIGNAL(clicked()), this, SLOT(btn5Clicked()));
  
  btn6 = new QPushButton();
  btn6->resize(50, 50);
  btn6->setFlat(true);
  btn6->setStyleSheet("background-image: url(:/img/Resources/images/6.bmp);"
      "background-repeat: no repeat;"
      "background-position: center center;"
      "color:rgb(255,255,255);"
      "font-size:18px");
  connect(btn6, SIGNAL(clicked()), this, SLOT(btn6Clicked()));
  
  btn7 = new QPushButton();
  btn7->resize(50, 50);
  btn7->setFlat(true);
  btn7->setStyleSheet("background-image: url(:/img/Resources/images/7.bmp);"
      "background-repeat: no repeat;"
      "background-position: center center;"
      "color:rgb(255,255,255);"
      "font-size:18px");
  connect(btn7, SIGNAL(clicked()), this, SLOT(btn7Clicked()));
  
  btn8 = new QPushButton();
  btn8->resize(50, 50);
  btn8->setFlat(true);
  btn8->setStyleSheet("background-image: url(:/img/Resources/images/8.bmp);"
      "background-repeat: no repeat;"
      "background-position: center center;"
      "color:rgb(255,255,255);"
      "font-size:18px");
  connect(btn8, SIGNAL(clicked()), this, SLOT(btn8Clicked()));
  
  btn9 = new QPushButton();
  btn9->resize(50, 50);
  btn9->setFlat(true);
  btn9->setStyleSheet("background-image: url(:/img/Resources/images/9.bmp);"
      "background-repeat: no repeat;"
      "background-position: center center;"
      "color:rgb(255,255,255);"
      "font-size:18px");
  connect(btn9, SIGNAL(clicked()), this, SLOT(btn9Clicked()));
  
  btn0 = new QPushButton();
  btn0->resize(50, 50);
  btn0->setFlat(true);
  btn0->setStyleSheet("background-image: url(:/img/Resources/images/0.bmp);"
      "background-repeat: no repeat;"
      "background-position: center center;"
      "color:rgb(255,255,255);"
      "font-size:18px");
  connect(btn0, SIGNAL(clicked()), this, SLOT(btn0Clicked()));
  
  btnHyphen = new QPushButton();
  btnHyphen->resize(50, 50);
  btnHyphen->setFlat(true);
  btnHyphen->setStyleSheet("background-image: url(:/img/Resources/images/guion.bmp);"
      "background-repeat: no repeat;"
      "background-position: center center;"
      "color:rgb(255,255,255);"
      "font-size:18px");
  connect(btnHyphen, SIGNAL(clicked()), this, SLOT(btnHyphenClicked()));
  
  btnK = new QPushButton();
  btnK->resize(50, 50);
  btnK->setFlat(true);
  btnK->setStyleSheet("background-image: url(:/img/Resources/images/k.bmp);"
      "background-repeat: no repeat;"
      "background-position: center center;"
      "color:rgb(255,255,255);"
      "font-size:18px");
  connect(btnK, SIGNAL(clicked()), this, SLOT(btnKClicked()));
  
  btnOk = new QPushButton();
  btnOk->resize(50, 50);
  btnOk->setFlat(true);
  btnOk->setStyleSheet("background-image: url(:/img/Resources/images/aceptar.bmp);"
      "background-repeat: no repeat;"
      "background-position: center center;"
      "color:rgb(255,255,255);"
      "font-size:18px");
  connect(btnOk, SIGNAL(clicked()), this, SLOT(btnOkClicked()));
  
  btnCancel = new QPushButton();
  btnCancel->resize(50, 50);
  btnCancel->setFlat(true);
  btnCancel->setStyleSheet("background-image: url(:/img/Resources/images/cancelar.bmp);"
      "background-repeat: no repeat;"
      "background-position: center center;"
      "color:rgb(255,255,255);"
      "font-size:18px");
  connect(btnCancel, SIGNAL(clicked()), this, SLOT(btnCancelClicked()));
  
  lblEmpresa = new QLabel();
  lblEmpresa->setAlignment(Qt::AlignCenter);
  lblEmpresa->setStyleSheet("font-size: 18px;"
                            "color: #FFFFFF;"
                            "font-style: normal;");
  
  lblName= new QLabel();
  lblName->setAlignment(Qt::AlignCenter);
  lblName->setStyleSheet("font-size: 18px;"
                         "color: #FFFFFF;"
                         "font-style: normal;");
  
  lblMsgScreen2= new QLabel();
  lblMsgScreen2->setAlignment(Qt::AlignCenter);
  lblMsgScreen2->setStyleSheet("font-size: 18px;"
                               "color: #FFFFFF;"
                               "font-style: normal;");

  // widget, row, col, rowSpan, colSpan
  grid->addWidget(lblMsg, 0, 0, 1, 4, Qt::AlignCenter);
  grid->addWidget(edtRut, 1, 0, 1, 3, Qt::AlignCenter);
  grid->addWidget(btnDelete, 1, 3, 1, 1, Qt::AlignCenter);

  grid->addWidget(btn1, 2, 0, 1, 1, Qt::AlignCenter);
  grid->addWidget(btn2, 2, 1, 1, 1, Qt::AlignCenter);
  grid->addWidget(btn3, 2, 2, 1, 1, Qt::AlignCenter);

  grid->addWidget(btn4, 3, 0, 1, 1, Qt::AlignCenter);
  grid->addWidget(btn5, 3, 1, 1, 1, Qt::AlignCenter);
  grid->addWidget(btn6, 3, 2, 1, 1, Qt::AlignCenter);
  grid->addWidget(btnHyphen, 3, 3, 1, 1, Qt::AlignCenter);
  
  grid->addWidget(btn7, 4, 0, 1, 1, Qt::AlignCenter);
  grid->addWidget(btn8, 4, 1, 1, 1, Qt::AlignCenter);
  grid->addWidget(btn9, 4, 2, 1, 1, Qt::AlignCenter);
  grid->addWidget(btnK, 4, 3, 1, 1, Qt::AlignCenter);

  grid->addWidget(btnCancel, 5, 0, 1, 1, Qt::AlignCenter);
  grid->addWidget(btn0, 5, 1, 1, 1, Qt::AlignCenter);
  grid->addWidget(btnOk, 5, 2, 1, 1, Qt::AlignCenter);
  
  setStyleSheet("background:rgb(0,101,153);");
  setLayout(grid);
}

void EnrollDialog::setScreen2(QString identifier, QString rut, QString name)
{
  screen = 2;

  lblEmpresa->setText(settings->value("empresaHolding", "Genera S. A.").toString());

  if (identifier.isEmpty()) {
    lblName->setText(tr("Usuario no encontrado"));
  } else {
    lblName->setText(tr("%1\n%2\n%3").arg(identifier).arg(rut).arg(name));
  }
  
  if (identifier.isEmpty()) {
    lblMsgScreen2->setText("");
  } else {
    lblMsgScreen2->setText(tr("Coloque el dedo\nen el sensor para\ncapturar huella..."));
  }
  
  grid->removeWidget(grid->itemAtPosition(0, 0)->widget());
  grid->removeWidget(grid->itemAtPosition(1, 0)->widget());
  grid->removeWidget(grid->itemAtPosition(1, 3)->widget());
  grid->removeWidget(grid->itemAtPosition(2, 0)->widget());
  grid->removeWidget(grid->itemAtPosition(2, 1)->widget());
  grid->removeWidget(grid->itemAtPosition(2, 2)->widget());
  grid->removeWidget(grid->itemAtPosition(3, 0)->widget());
  grid->removeWidget(grid->itemAtPosition(3, 1)->widget());
  grid->removeWidget(grid->itemAtPosition(3, 2)->widget());
  grid->removeWidget(grid->itemAtPosition(3, 3)->widget());
  grid->removeWidget(grid->itemAtPosition(4, 0)->widget());
  grid->removeWidget(grid->itemAtPosition(4, 1)->widget());
  grid->removeWidget(grid->itemAtPosition(4, 2)->widget());
  grid->removeWidget(grid->itemAtPosition(4, 3)->widget());
  grid->removeWidget(grid->itemAtPosition(5, 0)->widget());
  grid->removeWidget(grid->itemAtPosition(5, 1)->widget());
  grid->removeWidget(grid->itemAtPosition(5, 2)->widget());
  
  edtRut->setVisible(false);
  lblMsg->setVisible(false);
  btn1->setVisible(false);
  btn2->setVisible(false);
  btn3->setVisible(false);
  btn4->setVisible(false);
  btn5->setVisible(false);
  btn6->setVisible(false);
  btn7->setVisible(false);
  btn8->setVisible(false);
  btn9->setVisible(false);
  btn0->setVisible(false);
  btnHyphen->setVisible(false);
  btnK->setVisible(false);
  btnDelete->setVisible(false);
  btnOk->setVisible(false);
  
  // widget, row, col, rowSpan, colSpan
  grid->addWidget(lblEmpresa, 0, 0, 1, 4, Qt::AlignCenter);
  grid->addWidget(lblName, 1, 0, 1, 4, Qt::AlignCenter);
  grid->addWidget(lblMsgScreen2, 2, 0, 3, 4, Qt::AlignCenter);
  grid->addWidget(btnCancel, 5, 0, 1, 1, Qt::AlignCenter);
  grid->addWidget(btnOk, 5, 3, 1, 1, Qt::AlignCenter);
}

bool EnrollDialog::parseXmlPersona(QString &response, QString &identifier, QString &rut, QString &name)
{
  QXmlStreamReader xml(response);

  QString idPer, rutStr, rutDv, nombre, apPaterno, apMaterno;

  while (!xml.atEnd() && !xml.hasError()) {
    QXmlStreamReader::TokenType token = xml.readNext();

    if (token == QXmlStreamReader::StartDocument) { continue; }

    if (token == QXmlStreamReader::StartElement) {
      if (xml.name() == "idPer") {
        token = xml.readNext();
        if(xml.tokenType() == QXmlStreamReader::Characters) {
          DEBUG("idPer: %s", xml.text().toString().toStdString().c_str());
          idPer = xml.text().toString();
          continue;
        }
      }

      if (xml.name() == "rut") {
        token = xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::Characters) {
          DEBUG("rut: %s", xml.text().toString().toStdString().c_str());
          rutStr = xml.text().toString();
          continue;
        }
      }

      if (xml.name() == "rutDv") {
        token = xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::Characters) {
          DEBUG("rutDv: %s", xml.text().toString().toStdString().c_str());
          rutDv = xml.text().toString();
          continue;
        }
      }
      
      if (xml.name() == "nombre") {
        token = xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::Characters) {
          DEBUG("nombre: %s", xml.text().toString().toStdString().c_str());
          nombre = xml.text().toString();
          continue;
        }
      }
      
      if (xml.name() == "apPaterno") {
        token = xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::Characters) {
          DEBUG("apPaterno: %s", xml.text().toString().toStdString().c_str());
          apPaterno = xml.text().toString();
          continue;
        }
      }
      
      if (xml.name() == "apMaterno") {
        token = xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::Characters) {
          DEBUG("apMaterno: %s", xml.text().toString().toStdString().c_str());
          apMaterno = xml.text().toString();
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
  
  identifier = idPer;
  rut = rutStr + "-" + rutDv;
  name = nombre + " " + apPaterno + " " + apMaterno;

  if (identifier.isEmpty() || rut.isEmpty() || name.isEmpty()) {
    DEBUG("WS Error. identifier: %s, rut: %s, name: %s", identifier.toStdString().c_str(), rut.toStdString().c_str(), name.toStdString().c_str());
    return false;
  }

  DEBUG("WS OK. identifier: %s, rut: %s, name: %s", identifier.toStdString().c_str(), rut.toStdString().c_str(), name.toStdString().c_str());
  return true;
}

bool EnrollDialog::parseXmlEnroll(QString &response, QString &identificador, QString &serial, QString &resultado)
{
  QXmlStreamReader xml(response);

  while (!xml.atEnd() && !xml.hasError()) {
    QXmlStreamReader::TokenType token = xml.readNext();

    if (token == QXmlStreamReader::StartDocument) { continue; }

    if (token == QXmlStreamReader::StartElement) {
      if (xml.name() == "identificacion") {
        token = xml.readNext();
        if(xml.tokenType() == QXmlStreamReader::Characters) {
          DEBUG("identificacion: %s", xml.text().toString().toStdString().c_str());
          identificador = xml.text().toString();
          continue;
        }
      }

      if (xml.name() == "equSerial") {
        token = xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::Characters) {
          DEBUG("equSerial: %s", xml.text().toString().toStdString().c_str());
          serial = xml.text().toString();
          continue;
        }
      }

      if (xml.name() == "resultado") {
        token = xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::Characters) {
          DEBUG("resultado: %s", xml.text().toString().toStdString().c_str());
          resultado = xml.text().toString();
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
  
  if (identificador.isEmpty() || serial.isEmpty() || resultado.isEmpty()) {
    DEBUG("WS FAIL. identificador: %s, serial: %s, resultado: %s", identificador.toStdString().c_str(), serial.toStdString().c_str(), resultado.toStdString().c_str());
    return false;
  }

  DEBUG("WS OK. identificador: %s, serial: %s, resultado: %s", identificador.toStdString().c_str(), serial.toStdString().c_str(), resultado.toStdString().c_str());
  return true;
}
