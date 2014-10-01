#include <QCoreApplication>
#include <QString>

#include <csignal>
#include "debug_macros.h"
#include "backup.h"

Backup *backup;

void cleanup()
{
  DEBUG("cleaning up...");

  if (backup) { delete backup; }
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
  backup = new Backup();

  int rc = app.exec();

  delete backup;

  return rc;
}
