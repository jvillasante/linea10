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

int main(int argc, char *argv[])
{
  signal(SIGINT, signalHandler);
  signal(SIGABRT, signalHandler);
  signal(SIGTERM, signalHandler);

  QApplication app(argc, argv);
  app.setOrganizationName("Genera");
#ifdef TEMPO
  app.setApplicationName("Tempo10");
#endif
#ifdef SNACK
  app.setApplicationName("Snack10");
#endif
#ifdef PRESENCIA
  app.setApplicationName("Presencia10");
#endif

  QString appPath = QApplication::applicationDirPath();

  QString configPath = "/mnt/jffs2/app.ini";
  settings = new QSettings(configPath, QSettings::IniFormat);

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
