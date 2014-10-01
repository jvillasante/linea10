#include <QCoreApplication>
#include <QString>

#include <csignal>
#include "debug_macros.h"
#include "firmwareupdater.h"

FirmwareUpdater *updater;

void cleanup()
{
  DEBUG("cleaning up...");

  if (updater) { delete updater; }
}

void signalHandler(int signum)
{
  DEBUG("Signal %d received!!!", signum);

  cleanup();
  exit(signum);
}


int main(int argc, char *argv[])
{
  signal(SIGINT, signalHandler);
  signal(SIGABRT, signalHandler);
  signal(SIGTERM, signalHandler);

  QCoreApplication app(argc, argv);

  DEBUG("Initializing...");
  updater = new FirmwareUpdater();

  int rc = app.exec();

  delete updater;

  return rc;
}
