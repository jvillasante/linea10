#include "mainwindow.h"

#include "unistd.h"
#include "debug_macros.h"
#include "utils.h"
#ifdef TEMPO
#include "enrolldialog.h"
#endif

#include <QSharedPointer>
#include <QApplication>
#include <QPixmap>
#include <QDate>
#include <QDateTime>
#include <QIcon>

MainWindow::MainWindow(QSettings *settings, QWidget *parent) :
  QWidget(parent)
{
  this->settings = settings;
  lang = settings->value("lang", "es").toString();

  lblOutputCounter = 0;
  rebootCountdownCounter = 10;

  initializeUI();
  initializeGeneraDatabase();
  initializeEverySecondTimer();
  initializeEveryHourTimer();
  initializeCore();
  initializeWorkerSensor();
  initializeWorkerEnroller();

  this->networkMonitor = new NetworkMonitor(settings);
#ifdef TEMPO
  this->soapHandler = new SoapHandler(settings);
  connect(this->soapHandler, SIGNAL(finished(QString)), this, SLOT(alarmasFinished(QString)));
  connect(this->soapHandler, SIGNAL(error(QString)), this, SLOT(alarmasError(QString)));
#endif

  updatePrinterStatus();
  updateInternetStatus();
  printInitTicket();
}

MainWindow::~MainWindow()
{
  workerSensor->abort();
  threadSensor->wait();
  delete threadSensor;
  delete workerSensor;

  workerEnroller->abort();
  threadEnroller->wait();
  delete threadEnroller;
  delete workerEnroller;

  delete everySecondTimer;
  delete everyHourTimer;
  delete idkit;
  delete vcom;
  delete printer;
  delete generaDB;
  delete networkMonitor;
#ifdef TEMPO
  delete soapHandler;
#endif

  delete lblGeneraLogo;
  delete lblEmpresaHolding;
  delete lblDate;
  delete lblTime;
  delete lblOutput;
  delete lblMsg;
#ifdef TEMPO
  delete enrollButton;
#endif
  delete lbl1;
  delete lbl2;
  delete lbl3;
  delete hbox;
  delete box;
  delete grid;
}

#ifdef TEMPO
void MainWindow::match(QString userIdentifier, QString userName, QString userRut)
{
  updatePrinterStatus();
  lblOutputCounter = 30;

  if (userIdentifier == NULL && userName != NULL && userRut != NULL) {
    Utils::limitString(userName, 25);
    lblOutput->setText(tr("%1<br>Rut: %2<br><br><font size=4>Usted ya marco...</font>")
        .arg(userName)
        .arg(userRut));
  } else if (userIdentifier != NULL && userName != NULL && userRut != NULL) {
    Utils::limitString(userName, 25);
    lblOutput->setText(tr("%1<br>Rut: %2<br><br><font size=4>Seleccione</font><br><font size=6>Entrada/Salida</font>")
        .arg(userName)
        .arg(userRut));
  } else {
    lblOutput->setText(tr("<h4>Usuario no encontrado</h4>"));
  }
}
#elif SNACK
void MainWindow::match(QString userIdentifier, QString userName, QString userRut, QString service, int servicesCount)
{
  updatePrinterStatus();
  lblOutputCounter = 30;

  if (userIdentifier == NULL && userName == NULL && userRut == NULL) {
    lblOutput->setText(tr("<h4>Usuario no encontrado</h4>"));
    return;
  }
  
  if (servicesCount == -2) {  // impresora sin papel o desconectada
    Utils::limitString(userName, 25);
    lblOutput->setText(tr("%1<br>Rut: %2<br><br><font size=4>Impresora sin papel<br>o desconectada.</font>")
        .arg(userName)
        .arg(userRut));
  } else if (servicesCount == -1) {  // Servicio ya dado
    Utils::limitString(userName, 25);
    lblOutput->setText(tr("%1<br>Rut: %2<br><br><font size=4>(%3)</font><br><font size=4>Servicio ya dado!</font>")
        .arg(userName)
        .arg(userRut)
        .arg(service));
  } else if (servicesCount == 0) {  // no tiene servicios
    Utils::limitString(userName, 25);
    lblOutput->setText(tr("%1<br>Rut: %2<br><br><font size=4>No tiene servicios!</font>")
        .arg(userName)
        .arg(userRut));
  } else if (servicesCount == 1) {  // tiene 1 servicio
    Utils::limitString(userName, 25);
    lblOutput->setText(tr("%1<br>Rut: %2<br><br><font size=4>%3</font>")
        .arg(userName)
        .arg(userRut)
        .arg(service));
  } else if (servicesCount > 1) {  // tiene multiples servicios
    Utils::limitString(userName, 25);
    lblOutput->setText(tr("%1<br>Rut: %2<br><br><font size=4>Múltiples Servicios...<br>Seleccione Uno</font>")
        .arg(userName)
        .arg(userRut));
  }
}
#endif

#ifdef TEMPO
void MainWindow::buttonPressed(int button, QString userName)
{
  Utils::limitString(userName, 25);
  lblOutputCounter = 30;
  
  if (button == 0) {
    lblOutput->setText(tr("%1<br><br><font size=6>TIMEOUT</font>").arg(userName));
  } else  if (button == 1) {
    lblOutput->setText(tr("%1<br><br><font size=6>ENTRADA</font>").arg(userName));
  } else if (button == 2) {
    lblOutput->setText(tr("%1<br><br><font size=6>SALIDA</font>").arg(userName));
  }
}
#endif

void MainWindow::enrollFinished() // massive enroll
{
  DEBUG("Enroll thread finished enrolling users");
  lblMsg->setText(tr("Usuarios Registrados: %1").arg(idkit->getUserCount()));
}

void MainWindow::enrollError() // massive enroll
{
  DEBUG("Enroll thread finished with an error");
  LOG_ERROR("Error enrolling users...");
}

void MainWindow::enrollProgress(int importCount) // massive enroll
{
  DEBUG("Enroll thread progress..");
  lblMsg->setText(tr("Usuarios Registrados: %1").arg(importCount));
}

void MainWindow::identifierWorkDone()
{
  lblOutputCounter = 5;
  updateInternetStatus();
  workerSensor->requestMethod(WorkerSensorMulti::Identify);
}

#ifdef TEMPO
void MainWindow::enrollButtonPressed()  // enroll on board
{
  workerSensor->requestMethod(WorkerSensorMulti::Wait);

  EnrollDialog *dlg = new EnrollDialog(NULL, settings, workerSensor);
  connect(dlg, SIGNAL(enrollDialogClosed(QString)), this, SLOT(enrollDialogClosed(QString)));
  dlg->show();
}

void MainWindow::enrollDialogClosed(QString msg)
{
  lblOutputCounter = 5;
  lblOutput->setText(msg);
}
#endif

void MainWindow::message(QString msg)
{
  lblOutputCounter = 30;
  lblOutput->setText(msg);
}

void MainWindow::updateEverySecond()
{
  // Cambiar el texto del label mensaje al pasar 5 segundos
  if (lblOutputCounter > 0 && (--lblOutputCounter) == 0) {
    lblOutput->setText(tr("Coloque el dedo<br>en el sensor..."));
  }

  // cambiar la hora cada segundo
  QDateTime now = Utils::getCurrentTimestamp();
  lblTime->display(now.toString("hh:mm:ss"));

  updateDate(now);
}

void MainWindow::updateEveryHour()
{
  if (Utils::fileExists("/usr/local/firmware.tar.gz") && Utils::fileExists("/usr/local/firmware.tar.gz.md5")) {
    DEBUG("THERE ARE UPDATES AVAILABLE... VERIFYING MD5...");

    if (!Utils::verifyMd5("/usr/local/firmware.tar.gz.md5")) {
      DEBUG("FIRMWARE MD5 FAIL... REMOVING UPDATE FILES...");
      Utils::removeFile("/usr/local/firmware.tar.gz");
      Utils::removeFile("/usr/local/firmware.tar.gz.md5");
      return;
    }

    DEBUG("UPDATES READY... KILLING PROCESSES...");

    lblGeneraLogo->setVisible(false);
    lblEmpresaHolding->setVisible(false);
    lblDate->setVisible(false);
    lblTime->setVisible(false);
    lblMsg->setVisible(false);
#ifdef TEMPO
    enrollButton->setVisible(false);
#endif
    lbl1->setVisible(false);
    lbl2->setVisible(false);
    lbl3->setVisible(false);
    lblOutput->setFixedHeight(320);
    lblOutput->setFixedWidth(240);
    lblOutput->setText(tr("Actualizaciones Disponibles<br>Terminando procesos para<br>reinicio del sistema...<br><br>Espere por favor..."));
    qApp->processEvents();

    workerSensor->abort();
    threadSensor->wait();

    // disconnect(workerEnroller, SIGNAL(enrollProgressSignal(int)), this, SLOT(enrollProgress(int)));
    workerEnroller->abort();
    threadEnroller->wait();

    everySecondTimer->stop();
    everyHourTimer->stop();

    DEBUG("STARTING REBOOT TIMER...");
    QTimer *rebootTimer = new QTimer(this);
    rebootTimer->setInterval(1000);
    connect(rebootTimer, SIGNAL(timeout()), this, SLOT(updateRebootCountDown()));
    rebootTimer->start();
    updateRebootCountDown();
  } else {
    DEBUG("NO UPDATES AVAILABLE");
  }
}

void MainWindow::updateRebootCountDown()
{
  rebootCountdownCounter -= 1;

  lblOutput->setText(tr("Actualizaciones disponibles<br>Reiniciando sistema en...<br>%1 segundo(s)").arg(rebootCountdownCounter));
  if (rebootCountdownCounter == 0) {
    Utils::reboot();
  }
}

void MainWindow::initializeUI()
{
  lblGeneraLogo = new QLabel();
  lblGeneraLogo->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
  lblGeneraLogo->setStyleSheet("margin-left: 5px;");
  QPixmap lblGeneraLogoPixmap(":/img/Resources/images/genera_logo.png");
  lblGeneraLogo->setPixmap(lblGeneraLogoPixmap);
  lblEmpresaHolding = new QLabel(
      settings->value("empresaHolding", "GENERA S.A.").toString() + "\n" + settings->value("fwVersion", "").toString());

  lblEmpresaHolding->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
  lblEmpresaHolding->setStyleSheet("font-size: 16px;"
      "color: #FFFFFF;"
      "font-style: normal;"
      "font-weight: italic;");

  lblDate = new QLabel();
  lblDate->setAlignment(Qt::AlignCenter);
  lblDate->setStyleSheet("font-size: 14px;"
      "color: #00FF00;"
      "font-style: normal;"
      "padding-top: 10px;");

  lblTime = new QLCDNumber(8);
  lblTime->setLineWidth(2);
  lblTime->setFrameStyle(QFrame::NoFrame | QFrame::Plain);
  lblTime->setSegmentStyle(QLCDNumber::Flat);
  lblTime->setStyleSheet("color: #00FF00;"
      "text-align: center;"
      "padding-bottom: 10px;");

  lblOutput = new QLabel(tr("Coloque el dedo<br>en el sensor..."));
  lblOutput->setAlignment(Qt::AlignCenter);
  lblOutput->setMinimumHeight(135);
  lblOutput->setMinimumWidth(240);
  lblOutput->setTextFormat(Qt::RichText);
  lblOutput->setStyleSheet("font-size: 16px;"
      "padding-top: 10px;"
      "background-color: #333333;"
      "color: #FFFFFF;"
      "border-radius: 4px;"
      "border 1px solid #000000;"
      "font-style: normal;"
      "margin-left: 8px;");

  lblMsg = new QLabel();
  lblMsg->setAlignment(Qt::AlignCenter);
  lblMsg->setStyleSheet("font-size: 12px;"
      "color: #00FF00;"
      "font-style: normal;");

#ifdef TEMPO
  enrollButton = new QPushButton();
  enrollButton->setFlat(true);
  enrollButton->setStyleSheet("background-color: #019934;"
                              "border: none;");
  enrollButton->setIcon(QIcon(":/img/Resources/images/huella_enrolamiento.bmp"));
  enrollButton->setIconSize(QSize(48, 50));
  connect(enrollButton, SIGNAL(clicked()), this, SLOT(enrollButtonPressed()));
#endif

  lbl1 = new QLabel();
  lbl1->setAlignment(Qt::AlignCenter);
  lbl1->setStyleSheet("background-color: #333333;");
  QPixmap lbl1Pixmap(":/img/Resources/images/internet_habilitada.bmp");
  lbl1->setPixmap(lbl1Pixmap);

  lbl2 = new QLabel();
  lbl2->setAlignment(Qt::AlignCenter);
  lbl2->setStyleSheet("background-color: #333333;");
  QPixmap lbl2Pixmap(":/img/Resources/images/impresora_encendida.bmp");
  lbl2->setPixmap(lbl2Pixmap);

  lbl3 = new QLabel();
  lbl3->setAlignment(Qt::AlignCenter);
  lbl3->setStyleSheet("background-color: #333333;");
  QPixmap lbl3Pixmap(":/img/Resources/images/huella_habilitada.bmp");
  lbl3->setPixmap(lbl3Pixmap);

  grid = new QGridLayout(this);
  grid->setSpacing(0);
  grid->setMargin(0);
  grid->setContentsMargins(0, 0, 0, 0);

  // widget, row, col, rowSpan, colSpan
  grid->addWidget(lblGeneraLogo, 0, 0, 1, 4, Qt::AlignLeft);
  grid->addWidget(lblEmpresaHolding, 0, 4, 1, 4, Qt::AlignRight);
  grid->addWidget(lblDate, 1, 0, 1, 8, Qt::AlignCenter);
  grid->addWidget(lblTime, 2, 0, 2, 8);
  grid->addWidget(lblOutput, 4, 0, 6, 8, Qt::AlignCenter);
  grid->addWidget(lblMsg, 10, 0, 1, 8, Qt::AlignCenter);

  box = new QWidget();
  box->setStyleSheet("border-radius: 4px;"
      "border 1px solid #000000;"
      "background-color: #333333;"
      "margin-left: 8px;");
  hbox = new QHBoxLayout();
  hbox->setSpacing(0);
  hbox->setMargin(0);
  hbox->setContentsMargins(0, 0, 0, 0);

#ifdef TEMPO
  hbox->addWidget(enrollButton, 0, Qt::AlignCenter);
#endif
  hbox->addWidget(lbl1, 0, Qt::AlignCenter);
  hbox->addWidget(lbl2, 0, Qt::AlignCenter);
  hbox->addWidget(lbl3, 0, Qt::AlignCenter);

  box->setLayout(hbox);
  grid->addWidget(box, 11, 0, 2, 8);

  setStyleSheet("background:rgb(0,101,153);");
  setLayout(grid);
}

void MainWindow::initializeGeneraDatabase()
{
  QString db = settings->value("generaDB").toString();
  generaDB = new GeneraDB();
  generaDB->init(db.toStdString().c_str());
}

void MainWindow::initializeEverySecondTimer()
{
  everySecondTimer = new QTimer(this);
  everySecondTimer->setInterval(settings->value("timerReloj", 1000).toInt());
  connect(everySecondTimer, SIGNAL(timeout()), this, SLOT(updateEverySecond()));
  everySecondTimer->start();
}

void MainWindow::initializeEveryHourTimer()
{
  everyHourTimer = new QTimer(this);
  everyHourTimer->setInterval(settings->value("timerActualizacionEfectiva", 3600000).toInt());
  connect(everyHourTimer, SIGNAL(timeout()), this, SLOT(updateEveryHour()));
  everyHourTimer->start();
}

void MainWindow::initializeCore()
{
  vcom = new VCOMWrapper(settings);
  vcom->setupConnection();

  idkit = new IDKITWrapper(settings);
  idkit->init();

  int userCount = idkit->getUserCount();
  if (userCount > 0) {
    lblMsg->setText(tr("Usuarios Registrados: %1").arg(idkit->getUserCount()));
  } else {
    lblMsg->setText(tr("No hay usuarios registrados"));
  }

  printer = new PrinterSerial(settings);
  printer->init();
}

void MainWindow::initializeWorkerSensor()
{
  threadSensor = new QThread();
  workerSensor = new WorkerSensorMulti();
  workerSensor->setVCOMWrapper(vcom);
  workerSensor->setIDKITWrapper(idkit);
  workerSensor->setPrinterSerial(printer);
  workerSensor->setSQLiteManager(generaDB);
  workerSensor->moveToThread(threadSensor);

  connect(threadSensor, SIGNAL(started()), workerSensor, SLOT(mainLoop()));
  connect(workerSensor, SIGNAL(finished()), threadSensor, SLOT(quit()), Qt::DirectConnection);
  DEBUG("Starting thread in Thread %p", this->QObject::thread()->currentThreadId());
  threadSensor->start();
#ifdef TEMPO
  connect(workerSensor, SIGNAL(match(QString, QString, QString)), this, SLOT(match(QString, QString, QString)));
  connect(workerSensor, SIGNAL(buttonPressed(int, QString)), this, SLOT(buttonPressed(int, QString)));
#elif SNACK
  connect(workerSensor, SIGNAL(match(QString, QString, QString, QString, int)), this, SLOT(match(QString, QString, QString, QString, int)));
#endif
  connect(workerSensor, SIGNAL(identifierWorkDone()), this, SLOT(identifierWorkDone()));
  connect(workerSensor, SIGNAL(message(QString)), this, SLOT(message(QString)));

  workerSensor->requestMethod(WorkerSensorMulti::Identify);
}

void MainWindow::initializeWorkerEnroller()
{
  threadEnroller = new QThread();
  workerEnroller = new WorkerEnroller(settings, idkit);
  workerEnroller->moveToThread(threadEnroller);

  connect(workerEnroller, SIGNAL(enrollFinished()), this, SLOT(enrollFinished()));
  connect(workerEnroller, SIGNAL(enrollError()), this, SLOT(enrollError()));
  connect(workerEnroller, SIGNAL(enrollProgressSignal(int)), this, SLOT(enrollProgress(int)));
  connect(workerEnroller, SIGNAL(workRequested()), threadEnroller, SLOT(start()));
  connect(threadEnroller, SIGNAL(started()), workerEnroller, SLOT(doWork()));
  connect(workerEnroller, SIGNAL(finished()), threadEnroller, SLOT(quit()), Qt::DirectConnection);

  // To avoid having two threads running simultaneously, the previous thread is aborted.
  workerEnroller->abort();
  threadEnroller->wait(); // If the thread is not running, this will immediately return.
  workerEnroller->requestWork();
}

void MainWindow::printInitTicket()
{
  QString mac, ip, gateway, masc, broadcast;
  networkMonitor->getNetworkInfo(mac, ip, gateway, masc, broadcast);
  
  printer->write_hello(mac, ip, gateway, masc, broadcast);
}

inline void MainWindow::updatePrinterStatus()
{
  QString paper;
  bool printerStatus = printer->getRealTimeStatus(paper);
  if (printerStatus) {
    printer->setStatus(true);
    QPixmap lbl2Pixmap(":/img/Resources/images/impresora_encendida.bmp");
    lbl2->setPixmap(lbl2Pixmap);

#ifdef TEMPO
    const QString method = "alarma";
    const QString wsNamespace = settings->value("wsNamespace").toString();
    QMap<QString, QString> map;
    map["equSerial"] = settings->value("serialEquipo").toString();
    map["codigoAlarma"] = "2";
    map["estado"] = "0";
    map["fechHorAlarma"] = Utils::getCurrentTimestamp().toString("yyyy-MM-dd hh:mm");
    const QString strSOAP = soapHandler->getXmlPacket(method, wsNamespace, map);

    const QUrl url(settings->value("wsAlarmasURL").toString());
    soapHandler->fetch(method, wsNamespace, url, strSOAP);
#endif
  } else {
    printer->setStatus(false);
    QPixmap lbl2Pixmap(":/img/Resources/images/impresora_sinpapel.bmp");
    lbl2->setPixmap(lbl2Pixmap);

#ifdef TEMPO
    const QString method = "alarma";
    const QString wsNamespace = settings->value("wsNamespace").toString();
    QMap<QString, QString> map;
    map["equSerial"] = settings->value("serialEquipo").toString();
    map["codigoAlarma"] = "2";
    map["estado"] = "1";
    map["fechHorAlarma"] = Utils::getCurrentTimestamp().toString("yyyy-MM-dd hh:mm");
    const QString strSOAP = soapHandler->getXmlPacket(method, wsNamespace, map);

    const QUrl url(settings->value("wsAlarmasURL").toString());
    soapHandler->fetch(method, wsNamespace, url, strSOAP);
#endif
  }
}

inline void MainWindow::updateInternetStatus()
{
  if (this->networkMonitor->pingServer()) {
    QPixmap lbl1Pixmap(":/img/Resources/images/internet_habilitada.bmp");
    lbl1->setPixmap(lbl1Pixmap);
  } else {
    QPixmap lbl1Pixmap(":/img/Resources/images/internet_deshabilitada.bmp");
    lbl1->setPixmap(lbl1Pixmap);
  }
}

#ifdef TEMPO
void MainWindow::alarmasFinished(QString response)
{
  DEBUG("Response: %s", response.toStdString().c_str());
  UNUSED(response);
}

void MainWindow::alarmasError(QString error)
{
  DEBUG("Error: %s", error.toStdString().c_str());
  UNUSED(error);
}
#endif

inline void MainWindow::updateDate(QDateTime now)
{
  QString strDate;
  QDate currentDate = now.date();

  if (lang == "en") {
    strDate = currentDate.toString("ddd, dd MMMM yyyy");
  } else {
    switch (currentDate.dayOfWeek()) {
      case 1:
        strDate = "LUN";
        break;
      case 2:
        strDate = "MAR";
        break;
      case 3:
        strDate = "MIE";
        break;
      case 4:
        strDate = "JUE";
        break;
      case 5:
        strDate = "VIE";
        break;
      case 6:
        strDate = "SAB";
        break;
      case 7:
        strDate = "DOM";
        break;
      default:
        strDate = "ERROR";
        break;
    }
    
    strDate += ", " + now.toString("dd") + " ";

    switch(currentDate.month()) {
      case 1:
        strDate += "ENERO";
        break;
      case 2:
        strDate += "FEBRERO";
        break;
      case 3:
        strDate += "MARZO";
        break;
      case 4:
        strDate += "ABRIL";
        break;
      case 5:
        strDate += "MAYO";
        break;
      case 6:
        strDate += "JUNIO";
        break;
      case 7:
        strDate += "JULIO";
        break;
      case 8:
        strDate += "AGOSTO";
        break;
      case 9:
        strDate += "SEPTIEMBRE";
        break;
      case 10:
        strDate += "OCTUBRE";
        break;
      case 11:
        strDate += "NOVIEMBRE";
        break;
      case 12:
        strDate += "DICIEMBRE";
        break;
      default:
        strDate += "ERROR";
        break;
    }
    
    strDate += " " + now.toString("yyyy");
  }

  lblDate->setText(strDate);
}
