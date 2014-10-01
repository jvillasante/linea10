#include "mainwindow.h"

#include "unistd.h"
#include "debug_macros.h"
#include "utils.h"
#include "enrolldialog.h"

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
  initializeEventsDatabase();
  initializeEverySecondTimer();
  initializeEveryHourTimer();
  initializeCore();
  initializeWorkerSensor();
  initializeWorkerEnroller();

  this->networkMonitor = new NetworkMonitor(settings);
  this->soapHandler = new SoapHandler(settings);
  connect(this->soapHandler, SIGNAL(finished(QString)), this, SLOT(alarmasFinished(QString)));
  connect(this->soapHandler, SIGNAL(error(QString)), this, SLOT(alarmasError(QString)));
  
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
  delete eventsDB;
  delete networkMonitor;
  delete soapHandler;

  delete lblGeneraLogo;
  delete lblEmpresaHolding;
  delete lblDate;
  delete lblTime;
  delete lblOutput;
  delete lblMsg;
  delete enrollButton;
  delete lbl1;
  delete lbl2;
  delete lbl3;
  delete hbox;
  delete box;
  delete grid;
}

void MainWindow::match(QString userIdentifier, QString userName, QString userRut)
{
  updatePrinterStatus();

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

void MainWindow::buttonPressed(int button, QString userName)
{
  Utils::limitString(userName, 25);
  
  if (button == 0) {
    lblOutput->setText(tr("%1<br><br><font size=6>TIMEOUT</font>").arg(userName));
  } else  if (button == 1) {
    lblOutput->setText(tr("%1<br><br><font size=6>ENTRADA</font>").arg(userName));
  } else if (button == 2) {
    lblOutput->setText(tr("%1<br><br><font size=6>SALIDA</font>").arg(userName));
  }
}

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

void MainWindow::error(QString msg)
{
  lblOutputCounter = 5;
  lblOutput->setText(msg);
}

void MainWindow::updateEverySecond()
{
  // Cambiar el texto del label mensaje al pasar 5 segundos
  if (lblOutputCounter > 0 && (--lblOutputCounter) == 0) {
    lblOutput->setText(tr("Coloque el dedo<br>en el sensor..."));
  }

  // cambiar la hora cada segundo
  QDateTime now = QDateTime::currentDateTime();
  lblTime->display(now.toString("hh:mm:ss"));

  // solo cambiar la fecha si estamos inicializando o si la hora es 0 (12 de la mannana)
  if ((lblDate->text().isEmpty()) || (now.time().hour() == 0)) {
    updateDate(now);
  }
}

void MainWindow::updateEveryHour()
{
  // Delete syncronized events
  // int time = settings->value("timeEvents", 15).toInt();
  // if (!eventsDB->deleteEventsSyncronized(time)) {
  //   DEBUG("Unable to delete events syncronized...");
  // }
  
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
    enrollButton->setVisible(false);
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
      settings->value("empresaHolding", "GENERA S.A.").toString() + "\n" +
      settings->value("fwVersion", "").toString());
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

  enrollButton = new QPushButton();
  enrollButton->setFlat(true);
  enrollButton->setStyleSheet("background-color: #019934;"
                              "border: none;");
  enrollButton->setIcon(QIcon(":/img/Resources/images/huella_enrolamiento.bmp"));
  enrollButton->setIconSize(QSize(48, 50));
  connect(enrollButton, SIGNAL(clicked()), this, SLOT(enrollButtonPressed()));

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

  hbox->addWidget(enrollButton, 0, Qt::AlignCenter);
  hbox->addWidget(lbl1, 0, Qt::AlignCenter);
  hbox->addWidget(lbl2, 0, Qt::AlignCenter);
  hbox->addWidget(lbl3, 0, Qt::AlignCenter);

  box->setLayout(hbox);
  grid->addWidget(box, 11, 0, 2, 8);

  setStyleSheet("background:rgb(0,101,153);");
  setLayout(grid);
}

void MainWindow::initializeEventsDatabase()
{
  QString db = settings->value("eventsDB").toString();
  eventsDB = new EventsDB();
  eventsDB->init(db.toStdString().c_str());
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
  // The thread and the worker are created in the constructor so it is always safe to delete them.
  threadSensor = new QThread();
  workerSensor = new WorkerSensorMulti();
  workerSensor->setVCOMWrapper(vcom);
  workerSensor->setIDKITWrapper(idkit);
  workerSensor->setPrinterSerial(printer);
  workerSensor->setSQLiteManager(eventsDB);
  workerSensor->moveToThread(threadSensor);

  connect(threadSensor, SIGNAL(started()), workerSensor, SLOT(mainLoop()));
  connect(workerSensor, SIGNAL(finished()), threadSensor, SLOT(quit()), Qt::DirectConnection);
  DEBUG("Starting thread in Thread %p", this->QObject::thread()->currentThreadId());
  threadSensor->start();
  connect(workerSensor, SIGNAL(match(QString, QString, QString)), this, SLOT(match(QString, QString, QString)));
  connect(workerSensor, SIGNAL(buttonPressed(int, QString)), this, SLOT(buttonPressed(int, QString)));
  connect(workerSensor, SIGNAL(identifierWorkDone()), this, SLOT(identifierWorkDone()));
  connect(workerSensor, SIGNAL(error(QString)), this, SLOT(error(QString)));

  workerSensor->requestMethod(WorkerSensorMulti::Identify);
}

void MainWindow::initializeWorkerEnroller()
{
  // The thread and the worker are created in the constructor so it is always safe to delete them.
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

    const QString method = "alarma";
    const QString wsNamespace = settings->value("wsNamespace").toString();
    QMap<QString, QString> map;
    map["equSerial"] = settings->value("serialEquipo").toString();
    map["codigoAlarma"] = "2";
    map["estado"] = "0";
    map["fechHorAlarma"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");
    const QString strSOAP = soapHandler->getXmlPacket(method, wsNamespace, map);

    const QUrl url(settings->value("wsAlarmasURL").toString());
    soapHandler->fetch(method, wsNamespace, url, strSOAP);
  } else {
    printer->setStatus(false);
    QPixmap lbl2Pixmap(":/img/Resources/images/impresora_sinpapel.bmp");
    lbl2->setPixmap(lbl2Pixmap);

    const QString method = "alarma";
    const QString wsNamespace = settings->value("wsNamespace").toString();
    QMap<QString, QString> map;
    map["equSerial"] = settings->value("serialEquipo").toString();
    map["codigoAlarma"] = "2";
    map["estado"] = "1";
    map["fechHorAlarma"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");
    const QString strSOAP = soapHandler->getXmlPacket(method, wsNamespace, map);

    const QUrl url(settings->value("wsAlarmasURL").toString());
    soapHandler->fetch(method, wsNamespace, url, strSOAP);
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

inline void MainWindow::updateDate(QDateTime now)
{
  QString strDate;
  QDate currentDate = now.date();

  if (lang == "en") {
    strDate = currentDate.toString("ddd, dd MMMM yyyy");
  } else {
    QString strmonth = "";
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

    switch(currentDate.month()) {
      case 1:
        strmonth = "ENERO";
        break;
      case 2:
        strmonth = "FEBRERO";
        break;
      case 3:
        strmonth = "MARZO";
        break;
      case 4:
        strmonth = "ABRIL";
        break;
      case 5:
        strmonth = "MAYO";
        break;
      case 6:
        strmonth = "JUNIO";
        break;
      case 7:
        strmonth = "JULIO";
        break;
      case 8:
        strmonth = "AGOSTO";
        break;
      case 9:
        strmonth = "SEPTIEMBRE";
        break;
      case 10:
        strmonth = "OCTUBRE";
        break;
      case 11:
        strmonth = "NOVIEMBRE";
        break;
      case 12:
        strmonth = "DICIEMBRE";
        break;
      default:
        strmonth = "ERROR";
        break;
    }

    strDate = strdayweek + ", " + now.toString("dd") + " " + strmonth + " " + now.toString("yyyy");
  }

  lblDate->setText(strDate);
}
