#ifndef DBACCESS_H
#define DBACCESS_H

#include <QObject>
#include <QSqlDatabase>
#include <QMap>
#include "idkitwrapper.h"

class EventsDB : public QObject
{
  Q_OBJECT
    
  public:
    explicit EventsDB();
    ~EventsDB();

    bool init(const char *databaseName);

    QSqlQuery *getEventsToSynchronize();
    int insertEvent(int sense, char *ident, int date, int synchronized);
    bool setEventSynchronized(int id);
    int isUserIdentifiedOnLastMinute(QString identifier, int type);
    bool deleteEventsSyncronized();
    bool writeDatabaseToFile();

  private:
    QSqlDatabase db;
};

class ImportDB : public QObject
{
  Q_OBJECT
    
  public:
    bool init(const char *databaseName);
    void importDatabase(IDKITWrapper *idkit);

  signals:
    void importProgress(int);
};

class ConfigDB : public QObject
{
  Q_OBJECT
    
  public:
    bool init(const char *databaseName);
    bool getOldConfigs(QMap<QString, QString> &configMap);
};

#endif
