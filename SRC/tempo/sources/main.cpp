#include "mainwindow.h"
#include "debug_macros.h"

#include <csignal>
#include <QSettings>
#include <QTranslator>
#include <QTextCodec>
#include <QApplication>
#include <QDesktopWidget>

#include <QMap>
#include <QMapIterator>

#include "utils.h"
#include "dbaccess.h"

MainWindow *window;
QSettings *settings;

void signalHandler(int signum)
{
  DEBUG("Signal %d received!!!", signum);

  if (settings) { delete settings; }
  if (window)   { delete window; }

  DEBUG("Bye Bye...");
  fflush(stdout);
  fflush(stderr);
  exit(signum);
}

void configure(const char *configDb) 
{
  QMap<QString, QString> configMap; 
  
  ConfigDB db;
  db.init(configDb); 
  db.getOldConfigs(configMap);
  
  settings->setValue("serieImpresora", configMap["serie_impresora"]);
  QString cliente = configMap["cliente"];
  QString clienteUpdate = configMap["cliente"].replace("HORUS9", "Update");
  
  settings->setValue("empresaHolding", configMap["empresa_holding"]);
  settings->setValue("serialEquipo", configMap["serial_equipo"]);
  settings->setValue("identificadorEquipo", configMap["identificador_equipo"]);
  settings->setValue("wsIP", configMap["ws_ip"]);
  settings->setValue("wsPort", configMap["ws_port"]);
  
  settings->setValue("wsCargaMasivaURL", QString("http://%1:%2/%3/WS_HORUS9.asmx?WSDL")
      .arg(configMap["ws_ip"]).arg(configMap["ws_port"]).arg(cliente));
  settings->setValue("wsFirmwareUpdateURL", QString("http://%1:%2/%3/UpdateLinea9.asmx")
      .arg(configMap["ws_ip"]).arg(configMap["ws_port"]).arg(clienteUpdate));
  settings->setValue("wsSincronizacionURL", QString("http://%1:%2/%3/WS_HORUS9.asmx?op=SincronizacionMarcasM")
      .arg(configMap["ws_ip"]).arg(configMap["ws_port"]).arg(cliente));
  settings->setValue("wsVerificaPersonaURL", QString("http://%1:%2/%3/WS_HORUS9.asmx?op=VerificaPersonaM")
      .arg(configMap["ws_ip"]).arg(configMap["ws_port"]).arg(cliente));
  settings->setValue("wsEnrollURL", QString("http://%1:%2/%3/WS_HORUS9.asmx?op=enrollM")
      .arg(configMap["ws_ip"]).arg(configMap["ws_port"]).arg(cliente));
  settings->setValue("wsAlarmasURL", QString("http://%1:%2/%3/WS_HORUS9.asmx?op=alarma")
      .arg(configMap["ws_ip"]).arg(configMap["ws_port"]).arg(cliente));
}

int main(int argc, char *argv[])
{
  signal(SIGINT, signalHandler);
  signal(SIGABRT, signalHandler);
  signal(SIGTERM, signalHandler);

  QApplication app(argc, argv);
  app.setOrganizationName("Genera");
  app.setApplicationName("Tempo10");

  QString appPath = QApplication::applicationDirPath();
  
  QString src = appPath + "/Resources/settings/app.ini";
  if (Utils::fileExists("/mnt/jffs2/Gen_Config.sql")) {
    const QString dest = "/mnt/jffs2/app.ini";
    Utils::moveFile(src, dest);
    settings = new QSettings(dest, QSettings::IniFormat);

    configure("/mnt/jffs2/Gen_Config.sql");
    Utils::removeFile("/mnt/jffs2/Gen_Config.sql");
  } else {
    QSettings newSettings(src, QSettings::IniFormat);
    QString fwVersion = newSettings.value("fwVersion").toString();
    
    settings = new QSettings("/mnt/jffs2/app.ini", QSettings::IniFormat);
    settings->setValue("fwVersion", fwVersion);
    if (!settings->contains("keepEvents")) { settings->setValue("keepEvents", 15); }
    if (settings->contains("eventsDB"))    { settings->remove("eventsDB"); }
    if (!settings->contains("generaDB"))   { settings->setValue("generaDB", "/mnt/jffs2/genera.db"); }
  } 
  settings->sync();
  
  QString lang = settings->value("lang", "es").toString();
  DEBUG("Language is: %s.", lang.toStdString().c_str());

  QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
  QTranslator translator;
  if (lang == "es") {
    if (translator.load("tempo_es.qm", appPath + "/Resources/languages")) {
      app.installTranslator(&translator);
    } else {
      DEBUG("Unable to install tempo_es.qm translation.");
    }
  } else {
    if (translator.load("tempo_en.qm", appPath + "/Resources/languages")) {
      app.installTranslator(&translator);
    } else {
      DEBUG("Unable to install tempo_en.qm translation.");
    }
  }
  
  DEBUG("****************************************************************************");
  DEBUG("serialEquipo: %s", settings->value("serialEquipo").toString().toStdString().c_str());
  DEBUG("identificadorEquipo: %s", settings->value("identificadorEquipo").toString().toStdString().c_str());
  DEBUG("wsIp: %s", settings->value("wsIP").toString().toStdString().c_str());
  DEBUG("wsPort: %s", settings->value("wsPort").toString().toStdString().c_str());
  DEBUG("wsCargaMasivaURL: %s", settings->value("wsCargaMasivaURL").toString().toStdString().c_str());
  DEBUG("wsFirmwareUpdateURL: %s", settings->value("wsFirmwareUpdateURL").toString().toStdString().c_str());
  DEBUG("wsSincronizacionURL: %s", settings->value("wsSincronizacionURL").toString().toStdString().c_str());
  DEBUG("wsVerificaPersonaURL: %s", settings->value("wsVerificaPersonaURL").toString().toStdString().c_str());
  DEBUG("wsEnrollURL: %s", settings->value("wsEnrollURL").toString().toStdString().c_str());
  DEBUG("wsAlarmasURL: %s", settings->value("wsAlarmasURL").toString().toStdString().c_str());
  DEBUG("****************************************************************************");

  int WIDTH = 240;
  int HEIGHT = 320;
  int screenWidth, screenHeight;
  int x, y;

  window = new MainWindow(settings);
  QDesktopWidget *desktop = QApplication::desktop();

  screenWidth = desktop->width();
  screenHeight = desktop->height();

  x = (screenWidth - WIDTH) / 2;
  y = (screenHeight - HEIGHT) / 2;
  
  window->resize(WIDTH, HEIGHT);
  window->move(x, y);
  window->setWindowOpacity(1.0);
  window->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

  window->showFullScreen();

  int rc = app.exec();

  if (settings) { delete settings; }
  if (window)   { delete window; }

  DEBUG("Bye Bye...");
  fflush(stdout);
  fflush(stderr);
  return rc;
}
