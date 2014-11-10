#ifndef DBACCESS_H
#define DBACCESS_H

#include <QObject>
#include <QSqlDatabase>
#include <QMap>
#include "idkitwrapper.h"
#include "dao_service.h"

class GeneraDB : public QObject
{
  Q_OBJECT
    
  public:
    explicit GeneraDB();
    ~GeneraDB();

    bool init(const char *databaseName);

    QSqlQuery *getEventsToSynchronize();
    bool setEventSynchronized(int id);
    bool deleteEventsSyncronized();
    bool writeDatabaseToFile();

#ifdef TEMPO
    int insertEvent(int sense, char *ident, int date, int synchronized);
    int isUserIdentifiedOnLastMinute(QString identifier, int type);
#elif SNACK
    int insertEvent(int sense, char *ident, int date, int serviceId, int synchronized);
    int getServicesForUser(int userId, int day, int hour, QMap<int, ServiceDAO*> *services);   
    int updateService(int userId, int serviceGroup);
#endif

  private:
    QSqlDatabase db;
};

class ImportDB : public QObject
{
  Q_OBJECT
    
  public:
    bool init(const char *databaseName);
    void importDatabase(IDKITWrapper *idkit);

  private:
#ifdef TEMPO
    void importDatabaseTempo(IDKITWrapper *idkit, QSqlDatabase *db);
#elif SNACK
    void importDatabaseSnack(IDKITWrapper *idkit, QSqlDatabase *db);
#endif

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
