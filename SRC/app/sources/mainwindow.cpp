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
  this->flag = 0;
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

  updateInternetStatus();

#if defined(TEMPO) || defined(SNACK)
  updatePrinterStatus();
  if (printer->getStatus()) {
    printInitTicket();
  }
#endif
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
#if defined(TEMPO) || defined(SNACK)
  delete printer;
#endif
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
#if defined(TEMPO) || defined(SNACK)
  delete lbl2;
#endif
  delete lbl3;
  delete hbox;
  delete box;
  delete grid;
}

void MainWindow::closeApp()
{
  QApplication::quit();
  flag = 500;
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
#endif
#ifdef SNACK
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
#ifdef PRESENCIA
void MainWindow::match(QString userIdentifier, QString userName, QString userRut)
{
  lblOutputCounter = 30;

  if (userIdentifier == NULL && userName != NULL && userRut != NULL) {
    Utils::limitString(userName, 25);
    lblOutput->setText(tr("%1<br>Rut: %2<br><br><font size=4>Fuera de Hora</font>")
        .arg(userName)
        .arg(userRut));
  } else if (userIdentifier != NULL && userName != NULL && userRut != NULL) {
    Utils::limitString(userName, 25);
    lblOutput->setText(tr("%1<br>Rut: %2<br><br><font size=4>Acceso Concedido</font>")
        .arg(userName)
        .arg(userRut));
  } else {
    lblOutput->setText(tr("<h4>Acceso NO Concedido</h4><br><font size=4>Usuario no encontrado</font>"));
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

void MainWindow::enrollProgress(int importCount) // massive enroll
{
  DEBUG("Enroll thread progress..");

  if (importCount == -1) {
#ifdef TEMPO
    setFullScreen(tr("Comenzando proceso para<br>Carga Masiva.<br><br>Espere por favor..."));
#endif
#ifdef SNACK
    setFullScreen(tr("Importando servicios de<br>Casino. Este proceso puede<br>demorar un tiempo.<br><br>Espere por favor..."));
#endif
#ifdef PRESENCIA
    setFullScreen(tr("Importando turnos de<br>Presencia. Este proceso puede<br>demorar un tiempo.<br><br>Espere por favor..."));
#endif
  } else {
    lblOutput->setText(tr("Ejecutando Carga Masiva.<br><br>Usuarios Registrados:<br>%1<br><br>Espere por favor...").arg(importCount));
  }
}

void MainWindow::enrollFinished() // massive enroll
{
  DEBUG("Enroll thread finished enrolling users");
  startReboot(tr("Carga masiva efectuada<br>exitosamente.<br><br>Usuarios Registrados:<br>%1<br><br>Terminando procesos para<br>reinicio del sistema.<br><br>Espere por favor...").arg(idkit->getUserCount()));
}

void MainWindow::enrollError() // massive enroll
{
  DEBUG("Enroll thread finished with an error");
  LOG_ERROR("Error enrolling users...");
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
#ifdef PRESENCIA
    Utils::disableLeds();
#endif
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

    setFullScreen(tr("Actualizaciones Disponibles<br><br>Terminando procesos para<br>reinicio del sistema.<br><br>Espere por favor..."));
    startReboot(NULL);
  } else {
    DEBUG("NO UPDATES AVAILABLE");
  }
}

void MainWindow::updateRebootCountDown()
{
  rebootCountdownCounter -= 1;
  if (rebootCountdownCounter > 0) {
    lblOutput->setText(tr("Reiniciando sistema en...<br>%1 segundo(s)").arg(rebootCountdownCounter));
  } else if (rebootCountdownCounter == 0) {
    this->closeApp();
  } else {
    lblOutput->setText(tr("Error reiniciando sistema.<br>Por favor, reinicie<br>manualmente."));
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

#if defined(TEMPO) || defined(SNACK)
  lbl2 = new QLabel();
  lbl2->setAlignment(Qt::AlignCenter);
  lbl2->setStyleSheet("background-color: #333333;");
  QPixmap lbl2Pixmap(":/img/Resources/images/impresora_encendida.bmp");
  lbl2->setPixmap(lbl2Pixmap);
#endif

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
#if defined(TEMPO) || defined(SNACK)
  hbox->addWidget(lbl2, 0, Qt::AlignCenter);
#endif
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

#if defined(TEMPO) || defined(SNACK)
  printer = new PrinterSerial(settings);
  printer->init();
#endif
}

void MainWindow::initializeWorkerSensor()
{
  threadSensor = new QThread();
  workerSensor = new WorkerSensorMulti(settings);
  workerSensor->setVCOMWrapper(vcom);
  workerSensor->setIDKITWrapper(idkit);
#if defined(TEMPO) || defined(SNACK)
  workerSensor->setPrinterSerial(printer);
#endif
  workerSensor->setSQLiteManager(generaDB);
  workerSensor->moveToThread(threadSensor);

  connect(threadSensor, SIGNAL(started()), workerSensor, SLOT(mainLoop()));
  connect(workerSensor, SIGNAL(finished()), threadSensor, SLOT(quit()), Qt::DirectConnection);
  DEBUG("Starting thread in Thread %p", this->QObject::thread()->currentThreadId());
  threadSensor->start();
#ifdef TEMPO
  connect(workerSensor, SIGNAL(match(QString, QString, QString)), this, SLOT(match(QString, QString, QString)));
  connect(workerSensor, SIGNAL(buttonPressed(int, QString)), this, SLOT(buttonPressed(int, QString)));
#endif
#ifdef SNACK
  connect(workerSensor, SIGNAL(match(QString, QString, QString, QString, int)), this, SLOT(match(QString, QString, QString, QString, int)));
#endif
#ifdef PRESENCIA
  connect(workerSensor, SIGNAL(match(QString, QString, QString)), this, SLOT(match(QString, QString, QString)));
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
#if defined(SNACK) || defined(PRESENCIA)
  workerEnroller->setSQLiteManager(generaDB);
#endif

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

#if defined(TEMPO) || defined(SNACK)
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
#endif

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

void MainWindow::setFullScreen(QString msg)
{
  lblGeneraLogo->setVisible(false);
  lblEmpresaHolding->setVisible(false);
  lblDate->setVisible(false);
  lblTime->setVisible(false);
  lblMsg->setVisible(false);
#ifdef TEMPO
  enrollButton->setVisible(false);
#endif
  lbl1->setVisible(false);
#if defined(TEMPO) || defined(SNACK)
  lbl2->setVisible(false);
#endif
  lbl3->setVisible(false);
  lblOutput->setFixedHeight(320);
  lblOutput->setFixedWidth(240);
  lblOutput->setText(msg);
  qApp->processEvents();

  workerSensor->abort();
  threadSensor->wait();

  everySecondTimer->stop();
  everyHourTimer->stop();

  system("killall ntpd");
  system("killall updater");
  system("killall synchronizer");
  system("killall backup");
  system("sync; echo 3 > /proc/sys/vm/drop_caches");
}

void MainWindow::startReboot(QString msg)
{
  if (msg != NULL) {
    lblOutput->setText(msg);
    qApp->processEvents();
  }

  workerEnroller->abort();
  threadEnroller->wait();

  DEBUG("STARTING REBOOT TIMER...");
  QTimer *rebootTimer = new QTimer(this);
  rebootTimer->setInterval(1000);
  connect(rebootTimer, SIGNAL(timeout()), this, SLOT(updateRebootCountDown()));
  rebootTimer->start();
  updateRebootCountDown();
}

inline void MainWindow::updateDate(QDateTime now)
{
  static const QString DAYS[] = { "", "LUN", "MAR", "MIE", "JUE", "VIE", "SAB", "DOM" };
  static const QString MONTHS[] = {"", "ENERO", "FEBRERO", "MARZO", "ABRIL", "MAYO", "JUNIO", "JULIO", "AGOSTO", "SEPTIEMBRE", "OCTUBRE", "NOVIEMBRE", "DICIEMBRE" };

  if (lang == "en") {
    lblDate->setText(now.date().toString("ddd, dd MMMM yyyy"));
  } else {
    QDate currentDate = now.date();
    lblDate->setText(QString("%1, %2 %3 %4")
      .arg(DAYS[currentDate.dayOfWeek()])
      .arg(currentDate.day())
      .arg(MONTHS[currentDate.month()])
      .arg(currentDate.year()));
  }
}

