#ifndef DAO_SERVICE_H
#define DAO_SERVICE_H

#include <QString>
#include <QMap>
#include <QMapIterator>

class ServiceDAO
{
  public:
    ServiceDAO();
    ~ServiceDAO();
    int id;
    QString name;
    bool repetition;
    int initHour;
    int endHour;
    int group;
    int lastServed;
};

#endif // DAO_SERVICE
