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
    void close();

    QSqlQuery *getEventsToSynchronize();
    bool setEventSynchronized(int id);
    bool deleteEventsSyncronized();
    bool writeDatabaseToFile();

#ifdef TEMPO
    int insertEvent(int sense, char *ident, int date, int synchronized);
    int isUserIdentifiedOnLastMinute(QString identifier, int type);
#endif
#ifdef SNACK
    int insertEvent(int sense, char *ident, int date, int serviceId, int synchronized);
    int getServicesForUser(int userId, int day, int hour, QMap<int, ServiceDAO*> *services);
    int updateService(int userId, int serviceGroup);
    int truncateTables();
    int insertService(int id, QString name, int repetition);
    int insertSchedule(int id, int initHour, int endHour, int onLu, int onMa, int onMi, int onJu, int onVi, int onSa, int onDo);
    int insertScheduleService(int idSchedule, int idService);
    int insertPersonService(int idPerson, int idService, int serviceGroup);
#endif
#ifdef PRESENCIA
    int insertEvent(int sense, char *ident, int date, int synchronized);
    int truncateTables();
    int insertSchedule(int id, QString userIdentifier, int initHour, int endHour);
    bool isUserInSchedule(char *userIdentifier, int currentHour);
#endif

  private:
    QSqlDatabase db;
};

class ImportDB : public QObject
{
  Q_OBJECT

  public:
    bool init(const char *databaseName);
    void importDatabase(IDKITWrapper *idkit, GeneraDB *generaDb);

  private:
#ifdef TEMPO
    void importDatabaseTempo(IDKITWrapper *idkit, QSqlDatabase *importDb);
#endif
#ifdef SNACK
    void importDatabaseSnack(IDKITWrapper *idkit, QSqlDatabase *importDb, GeneraDB *generaDb);
#endif
#ifdef PRESENCIA
    void importDatabasePresencia(IDKITWrapper *idkit, QSqlDatabase *importDb, GeneraDB *generaDb);
#endif

  signals:
    void importProgress(int);
};

#endif
