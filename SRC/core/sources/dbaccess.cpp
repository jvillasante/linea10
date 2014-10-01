#include "dbaccess.h"
#include "debug_macros.h"
#include "utils.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QByteArray>
#include <QStringList>
#include <QFile>
#include <QTextStream>

EventsDB::EventsDB() {}

EventsDB::~EventsDB()
{
  if (this->db.isOpen()) {
    DEBUG("Closing Database...");
    this->db.close();
  }
}

bool EventsDB::init(const char *databaseName)
{
  bool dbExists = Utils::fileExists(databaseName);
  
  if (QSqlDatabase::contains("events")) {
    this->db = QSqlDatabase::database("events");
    this->db.setDatabaseName(databaseName);
  } else {
    this->db = QSqlDatabase::addDatabase("QSQLITE", "events");
    this->db.setDatabaseName(databaseName);
  }
  
  if(!this->db.open()) {
    LOG_ERROR("Failed to connect to %s: %s", databaseName, this->db.lastError().databaseText().toStdString().c_str());
    return false;
  }

  if (dbExists) {
    DEBUG("Database %s already exists. No need to create schema.", databaseName);
    return true;
  }
  
  LOG_INFO("Connected to %s database", databaseName);
  this->db.exec("PRAGMA auto_vacuum = FULL");
  this->db.exec("PRAGMA busy_timeout = 1000");
  this->db.exec("PRAGMA encoding = UTF-8");
  this->db.exec("PRAGMA shrink_memory");
  
  DEBUG("Creating schema on database %s", databaseName);
  QSqlQuery qry(this->db);
  if (!qry.exec("CREATE TABLE IF NOT EXISTS events ("
                "id INTEGER PRIMARY KEY,"   // identificador del evento
                "sense INTEGER,"            // sentido (entrada/salida)
                "ident VARCHAR(32),"        // identificador de la persona
                "date INTEGER,"             // fecha del evento (unixtimestamp)
                "synchronized INTEGER"      // boolean para saber si el evento ya fue enviado
                ");")) {
    LOG_ERROR("Query error: %s.", qry.lastError().databaseText().toStdString().c_str());
    qry.finish();
    return false;
  } else { 
    LOG_INFO("Schema created on database %s", databaseName);
    qry.finish();
    return true;
  }
}

int EventsDB::insertEvent(int sense, char *ident, int date, int synchronized)
{
  if (!this->db.isOpen()) { 
    DEBUG("Database events is not open");
    return 2;
  }

  // evento de salida es 0
  if (sense == 2) { sense = 0; }

  QSqlQuery qry(this->db);
  qry.prepare("INSERT INTO events(id, sense, ident, date, synchronized) VALUES(NULL, :f1, :f2, :f3, :f4)"); 
  qry.bindValue(":f1", sense);
  qry.bindValue(":f2", ident);
  qry.bindValue(":f3", date);
  qry.bindValue(":f4", synchronized);
  if (!qry.exec()) {
    LOG_ERROR("Error inserting event: %s", qry.lastError().text().toStdString().c_str());
    qry.finish();
    return 0;
  }

  LOG_INFO("Event %d inserted", qry.lastInsertId().toInt());
  qry.finish();
  return 1;
}

QSqlQuery *EventsDB::getEventsToSynchronize()
{
  QSqlQuery *qry = new QSqlQuery(this->db);

  if (!qry->exec("SELECT id, sense, ident, date FROM events WHERE synchronized = 0 LIMIT 20")) {
    LOG_ERROR("Query error: %s.", qry->lastError().databaseText().toStdString().c_str());
    qry->finish();
    delete qry;
    return NULL;
  } 

  DEBUG("Events selected... events ready to be synchronized...");
  return qry;
}

bool EventsDB::setEventSynchronized(int id)
{
  QSqlQuery qry(this->db);
  qry.prepare("UPDATE events SET synchronized = 1 WHERE id = :f1"); 
  qry.bindValue(":f1", id);

  if (!qry.exec()) {
    LOG_ERROR("Error setting synchronized event: %s", qry.lastError().text().toStdString().c_str());
    qry.finish();
    return false;
  }

  DEBUG("Event %d marked as synchronized.", id);
  qry.finish();
  return true;
}

bool EventsDB::deleteEventsSyncronized()
{
  QSqlQuery qry(this->db);

  if (!qry.exec("DELETE FROM events WHERE synchronized = 1")) {
    LOG_ERROR("Error deleting synchronized events: %s", qry.lastError().text().toStdString().c_str());
    qry.finish();
    return false;
  }

  DEBUG("Synchronized events deleted...");
  qry.finish();
  return true;
}

bool EventsDB::writeDatabaseToFile()
{
  int eventsCount = 0;
  QString filename = QDate::currentDate().toString("'/mnt/jffs2/backup/events_'yyyy_MM_dd'.bak'");
  QDateTime eventDateTime;
  QSqlQuery qry(this->db);

  QFile file(filename);
  bool fileOpened = false;
  bool fileIsNew = false;
  if (Utils::fileExists(filename.toStdString().c_str())) {
    fileOpened = file.open(QIODevice::Append | QIODevice::Text);
    fileIsNew = false;
  } else {
    fileOpened = file.open(QIODevice::WriteOnly | QIODevice::Text);
    fileIsNew = true;
  }

  if (fileOpened) {
    QTextStream stream(&file);
    if (fileIsNew)
      stream << "#sentido;identificator;fecha;sincronizado (Timestamp: " << QDate::currentDate().toString("yyyy/MM/dd hh:mm:ss") << ")\n\n";

    if (!qry.exec("SELECT sense, ident, date, synchronized FROM events WHERE synchronized = 1")) {
      LOG_ERROR("Query error: %s.", qry.lastError().databaseText().toStdString().c_str());
      file.close();
      return false;
    } 
    
    DEBUG("Selected. Beginnning to write synchronized events to file: %s.", filename.toStdString().c_str());
    while(qry.next()) {
      int sentido = qry.value(0).toInt();
      QString ident = qry.value(1).toString();
      int unixDate = qry.value(2).toInt();
      int synchronized = qry.value(3).toInt();
      
      Utils::getFromUnixTimestamp(eventDateTime, unixDate);
      stream << QString("%1;%2;%3;%4").arg(sentido).arg(ident).arg(eventDateTime.toString("yyyy-MM-dd hh:mm")).arg(synchronized) << "\n";
      eventsCount += 1;
    }

    file.close();
    DEBUG("%d events written to file: %s.", eventsCount, filename.toStdString().c_str());
    if (eventsCount > 0) this->deleteEventsSyncronized();
    return true;
  } else {
    if (fileIsNew)
      LOG_ERROR("Error creating new file %s", filename.toStdString().c_str());
    else
      LOG_ERROR("Error opening file %s", filename.toStdString().c_str());

    return false;
  }
}

int EventsDB::isUserIdentifiedOnLastMinute(QString identifier, int type)
{
  int count = 0;
  QDateTime eventDateTime;

  // Salida...
  if (type == 2) { type = 0; }
  
  QSqlQuery qry(this->db);
  qry.prepare("SELECT id, date FROM events WHERE ident = :f1 AND sense = :f2 ORDER BY id DESC LIMIT 1");
  qry.bindValue(":f1", identifier);
  qry.bindValue(":f2", type);
  if (!qry.exec()) {
    LOG_ERROR("Error selecting event: %s", qry.lastError().text().toStdString().c_str());
    qry.finish();
    return 0;
  }
  
  while(qry.next()) {
    Utils::getFromUnixTimestamp(eventDateTime, qry.value(1).toInt());
    count += 1;
  }

  if (count != 1) {
    qry.finish();
    return 2;
  }
  
  if (eventDateTime.addSecs(60) >= QDateTime::currentDateTime()) {
    qry.finish();
    return 1;
  } else {
    qry.finish();
    return 2;
  }
}

bool ImportDB::init(const char *databaseName)
{
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "import");
  db.setDatabaseName(databaseName);

  if(!db.open()) {
    LOG_ERROR("Failed to connect to %s: %s", databaseName, db.lastError().databaseText().toStdString().c_str());
    return false;
  }

  LOG_INFO("Connected to %s database", databaseName);
  return true;
}

void ImportDB::importDatabase(IDKITWrapper *idkit)
{
  QSqlDatabase db = QSqlDatabase::database("import");
  
  int importCount = 0;
  QSqlQuery qry(db);

  // identificador, nombre, rut, template
  if (!qry.exec("SELECT p.identificadorper, "
        "p.nombreper || ' ' || p.apppaternoper || ' ' || p.appmaternoper, "
        "p.rutper || '-' ||  p.rutdvper,  h.template "
        "FROM gen_persona p INNER JOIN gen_huella_dactilar h ON p.idper = h.idper")) {
    LOG_ERROR("Query error: %s.", qry.lastError().databaseText().toStdString().c_str());
  } else {
    DEBUG("Selected. Beginnning to import data.");

    while(qry.next()) {
      QString identifier = qry.value(0).toString();
      Utils::limitString(identifier, 32);

      QString name = qry.value(1).toString();
      Utils::limitString(name, 100);

      QString rut = qry.value(2).toString();
      Utils::limitString(rut, 15);

      QString tpl = qry.value(3).toString();
      QByteArray bytes = tpl.toUtf8();
      QByteArray decoded = bytes.fromBase64(bytes);

      // bool registerUserFromTemplate(unsigned char *tpl, const char *userIdentifier, const char *userName, const char *userRut);
      if (idkit->registerUserFromTemplate(reinterpret_cast<unsigned char *>(decoded.data()), (char *) identifier.toStdString().c_str(),
            (char *) name.toStdString().c_str(), (char *) rut.toStdString().c_str())) {
        importCount += 1;
      }

      if (importCount % 10 == 0) {
        emit importProgress(importCount);
      }
    }
  }
}

bool ConfigDB::init(const char *databaseName)
{ 
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "config");
  db.setDatabaseName(databaseName);

  if(!db.open()) {
    LOG_ERROR("Failed to connect to %s: %s", databaseName, db.lastError().databaseText().toStdString().c_str());
    return false;
  }

  LOG_INFO("Connected to %s database", databaseName);
  return true;
}

bool ConfigDB::getOldConfigs(QMap<QString, QString> &configMap)
{
  QSqlDatabase db = QSqlDatabase::database("config");
  
  QSqlQuery qry(db);

  if (!qry.exec("SELECT paramconfig, valueconfig FROM gen_config_equipo")) {
    LOG_ERROR("Query error: %s.", qry.lastError().databaseText().toStdString().c_str());
    return false;
  } else {
    DEBUG("Selected. Beginnning to generate data.");

    while(qry.next()) {
      QString param = qry.value(0).toString();
      QString value = qry.value(1).toString();

      if (param == "SERIAL_EQUIPO") {
        configMap["serial_equipo"] = value;
      } else if (param == "IDENTIFICADOR_EQUIPO") {
        configMap["identificador_equipo"] = value;
      } else if (param == "CLIENTE") {
        configMap["cliente"] = value;
      } else if (param == "SERIE_IMPRESORA") {
        configMap["serie_impresora"] = value;
      } else if (param == "WS_MARCAS") {
        QStringList pieces = value.split("-");
        configMap["ws_ip"] = pieces.at(0);
        configMap["ws_port"] = pieces.at(1);
      } else {
        continue;
      }
    }
    
    return true;
  }
}
