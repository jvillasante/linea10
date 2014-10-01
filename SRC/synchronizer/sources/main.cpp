#include <QCoreApplication>
#include <QString>

#include <csignal>
#include "debug_macros.h"
#include "synchronizer.h"

Synchronizer *synchronizer;

void cleanup()
{
  DEBUG("cleaning up...");

  if (synchronizer) { delete synchronizer; }
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
  synchronizer = new Synchronizer();

  int rc = app.exec();

  delete synchronizer;

  return rc;
}
