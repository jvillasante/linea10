#include "backup.h"

#include "debug_macros.h"
#include "utils.h"

#include <QCoreApplication>
#include <QTimer>
#include <QDateTime>
#include <QSqlQuery>
#include <QDir>
#include <QFileInfo>
#include <QTime>

Backup::Backup()
{
  settings = new QSettings("/mnt/jffs2/app.ini", QSettings::IniFormat);

  daysToKeepBackup = settings->value("keepEvents", 15).toInt();

  QString db = settings->value("generaDB").toString();
  generaDB = new GeneraDB();
  generaDB->init(db.toStdString().c_str());

  timer = new QTimer(this);
  timer->setInterval(3600000);
  // timer->setInterval(60000);
  connect(timer, SIGNAL(timeout()), this, SLOT(doWork()));
  timer->start();
}

Backup::~Backup()
{
  delete timer;
  delete settings;
  delete generaDB;
}

void Backup::deleteOldFiles()
{
  QDateTime keepEvents = Utils::getCurrentTimestamp().addDays(-daysToKeepBackup);
  QDir dir("/mnt/jffs2/backup");

  dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
  foreach(QFileInfo file, dir.entryInfoList()) {
    if (file.created() <= keepEvents) {
      if(dir.remove(file.absoluteFilePath())) {
        DEBUG("Deleted %s", file.absoluteFilePath().toStdString().c_str());
      } else {
        DEBUG("Fail to Delete %s", file.absoluteFilePath().toStdString().c_str());
      }
    }
  }
}

void Backup::doWork()
{
  QDateTime currentDateTime = Utils::getCurrentTimestamp();
  QTime currentTime = currentDateTime.time();

  if (currentTime.hour() == 23) {
    DEBUG("Starting backup process...");
    generaDB->writeDatabaseToFile();
    deleteOldFiles();
    DEBUG("Backup process done...");
  }
}

