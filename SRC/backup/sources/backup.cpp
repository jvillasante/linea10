#include "backup.h"

#include "debug_macros.h"
#include "utils.h"

#include <QCoreApplication>
#include <QTimer>
#include <QDateTime>
#include <QSqlQuery>
#include <QDir>
#include <QFileInfo>

void scheduleSlotAt(QObject *obj, const char *member, const QTime &at) {
  const int msecsPerDay = 24 * 60 * 60 * 1000;
  int msecs = QTime::currentTime().msecsTo(at);
  if (msecs < 0) msecs += msecsPerDay;
  QTimer::singleShot(msecs, obj, member);
}

Backup::Backup()
{
  settings = new QSettings("/mnt/jffs2/app.ini", QSettings::IniFormat);
  
  daysToKeepBackup = settings->value("keepEvents", 15).toInt();

  QString db = settings->value("eventsDB").toString();
  eventsDB = new EventsDB();
  eventsDB->init(db.toStdString().c_str());

  scheduleWork();
}

Backup::~Backup()
{
  delete settings;
  delete eventsDB;
}

void Backup::scheduleWork()
{
  scheduleSlotAt(this, SLOT(atMidnight()), QTime(0, 0));
}

void Backup::deleteOldFiles()
{
  QDateTime keepEvents = QDateTime::currentDateTime().addDays(daysToKeepBackup);
  QDir dir("/mnt/jffs2/backup");

  dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
  foreach(QFileInfo file, dir.entryInfoList()) {
    if (file.created() >= keepEvents) {
      if(dir.remove(file.path())) {
        DEBUG("Deleted /mnt/jffs2/backup/%s", file.absolutePath().toStdString().c_str());
      } else {
        DEBUG("Fail to Delete /mnt/jffs2/backup/%s", file.absolutePath().toStdString().c_str());
      }
    }
  } 
}

void Backup::atMidnight()
{
  // Do some work here...
  eventsDB->writeDatabaseToFile();
  deleteOldFiles();
  scheduleWork();
}

