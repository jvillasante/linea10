#ifndef BACKUP_H
#define BACKUP_H

#include <QObject>
#include <QSettings>

#include "debug_macros.h"
#include "dbaccess.h"

class Backup : public QObject
{
  Q_OBJECT
  public:
    explicit Backup();
    ~Backup();

  public slots:
    void atMidnight();

  private:
    int daysToKeepBackup;
    void scheduleWork();
    void deleteOldFiles();
    
    QSettings *settings;
    EventsDB *eventsDB;
};

#endif // BACKUP_H
