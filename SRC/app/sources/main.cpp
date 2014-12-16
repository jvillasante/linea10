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

#include <stdlib.h>
#include <unistd.h>
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
#ifdef TEMPO
  app.setApplicationName("Tempo10");
#elif SNACK
  app.setApplicationName("Snack10");
#endif
  
  QString appPath = QApplication::applicationDirPath();
  
  QString src = appPath + "/Resources/settings/app.ini";
  if (Utils::fileExists("/mnt/jffs2/Gen_Config.sql")) {
    const QString dest = "/mnt/jffs2/app.ini";
    Utils::copyFile(src, dest);
    settings = new QSettings(dest, QSettings::IniFormat);

    configure("/mnt/jffs2/Gen_Config.sql");
    Utils::removeFile("/mnt/jffs2/Gen_Config.sql");
  } else {
    QSettings newSettings(src, QSettings::IniFormat);
    settings = new QSettings("/mnt/jffs2/app.ini", QSettings::IniFormat);
    
    if (settings->contains("key1"))        { settings->remove("key1"); }
    if (settings->contains("key2"))        { settings->remove("key2"); }
    if (settings->contains("key3"))        { settings->remove("key3"); }
    if (settings->contains("key4"))        { settings->remove("key4"); }
    if (settings->contains("key5"))        { settings->remove("key5"); }
    if (settings->contains("key6"))        { settings->remove("key6"); }
    if (settings->contains("key7"))        { settings->remove("key7"); }
    if (settings->contains("key8"))        { settings->remove("key8"); }
    if (settings->contains("key9"))        { settings->remove("key9"); }
    if (settings->contains("key10"))       { settings->remove("key10"); }
    if (settings->contains("eventsDB"))    { settings->remove("eventsDB"); }
    
    if (!settings->contains("keepEvents")) { settings->setValue("keepEvents", 15); }
    if (!settings->contains("generaDB"))   { settings->setValue("generaDB", "/mnt/jffs2/genera.db"); }
    if (!settings->contains("ntpIP"))      { settings->setValue("ntpIP", newSettings.value("ntpIP").toString()); }
    settings->setValue("fw", newSettings.value("fw").toString());
    settings->setValue("fwVersion", newSettings.value("fwVersion").toString());
  } 
  settings->sync();
  
  QString lang = settings->value("lang", "es").toString();
  DEBUG("Language is: %s.", lang.toStdString().c_str());

  QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
  QTranslator translator;
  if (lang == "es") {
    if (translator.load("app_es.qm", appPath + "/Resources/languages")) {
      app.installTranslator(&translator);
    } else {
      DEBUG("Unable to install app_es.qm translation.");
    }
  } else {
    if (translator.load("app_en.qm", appPath + "/Resources/languages")) {
      app.installTranslator(&translator);
    } else {
      DEBUG("Unable to install app_en.qm translation.");
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

  if (window) {
    DEBUG("MainWindow Flag: %d", window->flag);
    if (window->flag == 500) {
      if (settings) { delete settings; }
      if (window)   { delete window; }
      
      DEBUG("Bye Bye...");
      fflush(stdout);
      fflush(stderr);
      execl("/sbin/reboot", "reboot", NULL);
      return 0;
    }
  }

  if (settings) { delete settings; }
  if (window)   { delete window; }

  DEBUG("Bye Bye...");
  fflush(stdout);
  fflush(stderr);
  return rc;
}
