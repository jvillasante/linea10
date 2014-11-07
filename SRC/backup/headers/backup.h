#ifndef BACKUP_H
#define BACKUP_H

#include <QObject>
#include <QSettings>
#include <QTimer>
#include <QDateTime>

#include "debug_macros.h"
#include "dbaccess.h"

class Backup : public QObject
{
  Q_OBJECT
  public:
    explicit Backup();
    ~Backup();

  public slots:
    void doWork();

  private:
    int daysToKeepBackup;
    void deleteOldFiles();
    
    QSettings *settings;
    GeneraDB *generaDB;
    QTimer *timer;
};

#endif // BACKUP_H
