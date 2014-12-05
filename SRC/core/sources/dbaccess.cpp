#include "dbaccess.h"
#include "debug_macros.h"
#include "utils.h"
#include "dao_service.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QByteArray>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QMapIterator>
#include <QVariant>

GeneraDB::GeneraDB() {}

GeneraDB::~GeneraDB()
{
  if (this->db.isOpen()) {
    DEBUG("Closing Database...");
    this->db.close();
  }
}

bool GeneraDB::init(const char *databaseName)
{
  bool dbExists = Utils::fileExists(databaseName);
  
  if (QSqlDatabase::contains("genera")) {
    this->db = QSqlDatabase::database("genera");
    this->db.setDatabaseName(databaseName);
  } else {
    this->db = QSqlDatabase::addDatabase("QSQLITE", "genera");
    this->db.setDatabaseName(databaseName);
  }
  
  if(!this->db.open()) {
    LOG_ERROR("Failed to connect to %s: %s", databaseName, this->db.lastError().databaseText().toStdString().c_str());
    return false;
  }
  
  LOG_INFO("Connected to %s database", databaseName);
  this->db.exec("PRAGMA auto_vacuum = FULL");
  this->db.exec("PRAGMA busy_timeout = 1000");
  this->db.exec("PRAGMA encoding = UTF-8");
  this->db.exec("PRAGMA shrink_memory");

  if (dbExists) {
    DEBUG("Database %s already exists. No need to create schema.", databaseName);
    return true;
  }
  
  DEBUG("Creating schema on database %s", databaseName);

#ifdef TEMPO
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
#elif SNACK
  QSqlQuery qry(this->db);
  if (!qry.exec("CREATE TABLE IF NOT EXISTS events ("
                "id INTEGER PRIMARY KEY,"   // identificador del evento
                "sense INTEGER,"            // sentido (entrada/salida)
                "ident VARCHAR(32),"        // identificador de la persona
                "date INTEGER,"             // fecha del evento (unixtimestamp)
                "service INTEGER,"          // identificador del servicio dado
                "synchronized INTEGER"      // boolean para saber si el evento ya fue enviado
                ");")) {
    LOG_ERROR("Query error: %s.", qry.lastError().databaseText().toStdString().c_str());
    qry.finish();
    return false;
  } else { 
    qry.finish();
  }
  
  if (!qry.exec("CREATE TABLE IF NOT EXISTS services ("
                "id INTEGER PRIMARY KEY,"       // identificador del servicio
                "name VARCHAR(100),"            // nombre del servicio
                "repetition INTEGER DEFAULT 0"  // indica si el servicio se puede repetir o no (0, 1)
                ");")) {
    LOG_ERROR("Query error: %s.", qry.lastError().databaseText().toStdString().c_str());
    qry.finish();
    return false;
  } else { 
    qry.finish();
  }
  
  if (!qry.exec("CREATE TABLE IF NOT EXISTS persons_services ("
                "id_person INTEGER NOT NULL,"                 // identificador de persona
                "id_service INTEGER NOT NULL,"                // identificador de servicio
                "last_served_timestamp INTEGER DEFAULT 0,"    // grupo al que pertenece el servicio
                "service_group INTEGER DEFAULT 0,"            // timestamp de servicio dado
                "PRIMARY KEY (id_person, id_service)"
                ");")) {
    LOG_ERROR("Query error: %s.", qry.lastError().databaseText().toStdString().c_str());
    qry.finish();
    return false;
  } else { 
    qry.finish();
  }
  
  if (!qry.exec("CREATE TABLE IF NOT EXISTS schedules ("
                "id INTEGER PRIMARY KEY,"              // identificador del horario
                "init_hour VARCHAR(4) DEFAULT 0,"      // hora de inicio (0800 para las 08:00 horas)
                "end_hour VARCHAR(4) DEFAULT 0,"       // hora de terminacion (1500 para las 15:00 horas)
                "on_lu INTEGER DEFAULT 0,"             // dias del servicio activo (1 o 0)
                "on_ma INTEGER DEFAULT 0,"
                "on_mi INTEGER DEFAULT 0,"
                "on_ju INTEGER DEFAULT 0,"
                "on_vi INTEGER DEFAULT 0,"
                "on_sa INTEGER DEFAULT 0,"
                "on_do INTEGER DEFAULT 0"
                ");")) {
    LOG_ERROR("Query error: %s.", qry.lastError().databaseText().toStdString().c_str());
    qry.finish();
    return false;
  } else { 
    qry.finish();
  }
  
  if (!qry.exec("CREATE TABLE IF NOT EXISTS schedules_services ("
                "id_schedule INTEGER NOT NULL,"        // identificador del horario
                "id_service INTEGER NOT NULL,"         // identificador de servicio
                "PRIMARY KEY (id_schedule, id_service)"
                ");")) {
    LOG_ERROR("Query error: %s.", qry.lastError().databaseText().toStdString().c_str());
    qry.finish();
    return false;
  } else { 
    qry.finish();
  }
  
  LOG_INFO("Schema created on database %s", databaseName);
  return true;
#endif
  return false;
}

#ifdef TEMPO
int GeneraDB::insertEvent(int sense, char *ident, int date, int synchronized)
{
  if (!this->db.isOpen()) { 
    DEBUG("Database genera is not open");
    return 1;
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
    return 1;
  }

  LOG_INFO("Event %d inserted", qry.lastInsertId().toInt());
  qry.finish();
  return 0;
}
#elif SNACK
int GeneraDB::insertEvent(int sense, char *ident, int date, int serviceId, int synchronized)
{
  if (!this->db.isOpen()) { 
    DEBUG("Database genera is not open");
    return 1;
  }

  QSqlQuery qry(this->db);
  qry.prepare("INSERT INTO events(id, sense, ident, date, service, synchronized) VALUES(NULL, :f1, :f2, :f3, :f4, :f5)"); 
  qry.bindValue(":f1", sense);
  qry.bindValue(":f2", ident);
  qry.bindValue(":f3", date);
  qry.bindValue(":f4", serviceId);
  qry.bindValue(":f5", synchronized);
  if (!qry.exec()) {
    LOG_ERROR("Error inserting event: %s", qry.lastError().text().toStdString().c_str());
    qry.finish();
    return 1;
  }

  LOG_INFO("Event %d inserted", qry.lastInsertId().toInt());
  qry.finish();
  return 0;
}
#endif

QSqlQuery *GeneraDB::getEventsToSynchronize()
{
  QSqlQuery *qry = new QSqlQuery(this->db);

#ifdef TEMPO
  if (!qry->exec("SELECT id, sense, ident, date FROM events WHERE synchronized = 0 LIMIT 20")) {
    LOG_ERROR("Query error: %s.", qry->lastError().databaseText().toStdString().c_str());
    qry->finish();
    delete qry;
    return NULL;
  } 
#elif SNACK
  if (!qry->exec("SELECT id, sense, ident, date, service FROM events WHERE synchronized = 0 LIMIT 20")) {
    LOG_ERROR("Query error: %s.", qry->lastError().databaseText().toStdString().c_str());
    qry->finish();
    delete qry;
    return NULL;
  } 
#endif

  DEBUG("Genera selected... events ready to be synchronized...");
  return qry;
}

bool GeneraDB::setEventSynchronized(int id)
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

bool GeneraDB::deleteEventsSyncronized()
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

bool GeneraDB::writeDatabaseToFile()
{
  int eventsCount = 0;
  QString filename = QDate::currentDate().toString("'/mnt/jffs2/backup/events_'yyyy_MM_dd'.bak'");
  QDateTime eventDateTime;

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
    QSqlQuery qry(this->db);
    QTextStream stream(&file);

#ifdef TEMPO
    if (fileIsNew)
      stream << "#sentido;identificador;fecha;sincronizado (Timestamp: " << Utils::getCurrentTimestamp().toString("yyyy/MM/dd hh:mm:ss") << ")\n\n";

    if (!qry.exec("SELECT sense, ident, date, synchronized FROM events WHERE synchronized = 1")) {
      LOG_ERROR("Query error: %s.", qry.lastError().databaseText().toStdString().c_str());
      qry.finish();
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
      stream << QString("%1;%2;%3;%4").arg(sentido).arg(ident).arg(eventDateTime.toString("yyyy-MM-dd hh:mm:ss")).arg(synchronized) << "\n";
      eventsCount += 1;
    }
#elif SNACK
    if (fileIsNew)
      stream << "#sentido;identificador;fecha;servicio;sincronizado (Timestamp: " << Utils::getCurrentTimestamp().toString("yyyy/MM/dd hh:mm:ss") << ")\n\n";

    if (!qry.exec("SELECT sense, ident, date, service, synchronized FROM events WHERE synchronized = 1")) {
      LOG_ERROR("Query error: %s.", qry.lastError().databaseText().toStdString().c_str());
      qry.finish();
      file.close();
      return false;
    } 
    
    DEBUG("Selected. Beginnning to write synchronized events to file: %s.", filename.toStdString().c_str());
    while(qry.next()) {
      int sentido = qry.value(0).toInt();
      QString ident = qry.value(1).toString();
      int unixDate = qry.value(2).toInt();
      int service = qry.value(3).toInt();
      int synchronized = qry.value(4).toInt();
      
      Utils::getFromUnixTimestamp(eventDateTime, unixDate);
      stream << QString("%1;%2;%3;%4;%5").arg(sentido).arg(ident).arg(eventDateTime.toString("yyyy-MM-dd hh:mm:ss")).arg(service).arg(synchronized) << "\n";
      eventsCount += 1;
    }
#endif

    qry.finish();
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

#ifdef TEMPO
int GeneraDB::isUserIdentifiedOnLastMinute(QString identifier, int type)
{
  int count = 0;
  uint dbTimestamp = 0;

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
  
  while (qry.next()) {
    dbTimestamp = qry.value(1).toInt();
    count += 1;
  }

  if (count != 1) {
    qry.finish();
    return 2;
  }

  DEBUG("*********************************************************");
  DEBUG("DB %d      : ", dbTimestamp);
  DEBUG("DB + 60 %d : ", dbTimestamp + 60);
  DEBUG("CURRENT %d : ", Utils::getCurrentUnixTimestamp());
  DEBUG("*********************************************************");
  
  if ((dbTimestamp + 60) >= Utils::getCurrentUnixTimestamp()) {
    qry.finish();
    return 1;
  } else {
    qry.finish();
    return 2;
  }
}
#endif

#ifdef SNACK
int GeneraDB::getServicesForUser(int userId, int day, int hour, QMap<int, ServiceDAO*> *services)
{
  int count = 0;

  QString sql = "";
  switch (day) {
    case 1:
      sql = "SELECT DISTINCT s.id AS id, s.name AS name, s.repetition AS repetition, sh.init_hour AS init_hour,"
        " sh.end_hour AS end_hour, ps.service_group AS service_group, ps.last_served_timestamp AS last_served"
        " FROM services s"
        " INNER JOIN persons_services ps ON s.id = ps.id_service"
        " INNER JOIN schedules_services ss ON s.id = ss.id_service"
        " INNER JOIN schedules sh ON ss.id_schedule = sh.id"
        " WHERE ps.id_person = :f1 AND sh.on_lu = 1 AND"
        " CASE WHEN CAST(sh.init_hour AS INTEGER) <= CAST(sh.end_hour AS INTEGER)"
        " THEN CAST(sh.init_hour AS INTEGER) <= :f2 AND CAST(sh.end_hour AS INTEGER) >= :f3"
        " ELSE"
        "   (CAST(sh.init_hour AS INTEGER) <= :f4 AND CAST(sh.end_hour AS INTEGER) <= :f5)"
        "   OR"
        "   (CAST(sh.init_hour AS INTEGER) >= :f6 AND CAST(sh.end_hour AS INTEGER) >= :f7)"
        " END";
      break;
    case 2:
      sql = "SELECT DISTINCT s.id AS id, s.name AS name, s.repetition AS repetition, sh.init_hour AS init_hour,"
        " sh.end_hour AS end_hour, ps.service_group AS service_group, ps.last_served_timestamp AS last_served"
        " FROM services s"
        " INNER JOIN persons_services ps ON s.id = ps.id_service"
        " INNER JOIN schedules_services ss ON s.id = ss.id_service"
        " INNER JOIN schedules sh ON ss.id_schedule = sh.id"
        " WHERE ps.id_person = :f1 AND sh.on_ma = 1 AND"
        " CASE WHEN CAST(sh.init_hour AS INTEGER) <= CAST(sh.end_hour AS INTEGER)"
        " THEN CAST(sh.init_hour AS INTEGER) <= :f2 AND CAST(sh.end_hour AS INTEGER) >= :f3"
        " ELSE"
        "   (CAST(sh.init_hour AS INTEGER) <= :f4 AND CAST(sh.end_hour AS INTEGER) <= :f5)"
        "   OR"
        "   (CAST(sh.init_hour AS INTEGER) >= :f6 AND CAST(sh.end_hour AS INTEGER) >= :f7)"
        " END";
      break;
    case 3:
      sql = "SELECT DISTINCT s.id AS id, s.name AS name, s.repetition AS repetition, sh.init_hour AS init_hour,"
        " sh.end_hour AS end_hour, ps.service_group AS service_group, ps.last_served_timestamp AS last_served"
        " FROM services s"
        " INNER JOIN persons_services ps ON s.id = ps.id_service"
        " INNER JOIN schedules_services ss ON s.id = ss.id_service"
        " INNER JOIN schedules sh ON ss.id_schedule = sh.id"
        " WHERE ps.id_person = :f1 AND sh.on_mi = 1 AND"
        " CASE WHEN CAST(sh.init_hour AS INTEGER) <= CAST(sh.end_hour AS INTEGER)"
        " THEN CAST(sh.init_hour AS INTEGER) <= :f2 AND CAST(sh.end_hour AS INTEGER) >= :f3"
        " ELSE"
        "   (CAST(sh.init_hour AS INTEGER) <= :f4 AND CAST(sh.end_hour AS INTEGER) <= :f5)"
        "   OR"
        "   (CAST(sh.init_hour AS INTEGER) >= :f6 AND CAST(sh.end_hour AS INTEGER) >= :f7)"
        " END";
      break;
    case 4:
      sql = "SELECT DISTINCT s.id AS id, s.name AS name, s.repetition AS repetition, sh.init_hour AS init_hour,"
        " sh.end_hour AS end_hour, ps.service_group AS service_group, ps.last_served_timestamp AS last_served"
        " FROM services s"
        " INNER JOIN persons_services ps ON s.id = ps.id_service"
        " INNER JOIN schedules_services ss ON s.id = ss.id_service"
        " INNER JOIN schedules sh ON ss.id_schedule = sh.id"
        " WHERE ps.id_person = :f1 AND sh.on_ju = 1 AND"
        " CASE WHEN CAST(sh.init_hour AS INTEGER) <= CAST(sh.end_hour AS INTEGER)"
        " THEN CAST(sh.init_hour AS INTEGER) <= :f2 AND CAST(sh.end_hour AS INTEGER) >= :f3"
        " ELSE"
        "   (CAST(sh.init_hour AS INTEGER) <= :f4 AND CAST(sh.end_hour AS INTEGER) <= :f5)"
        "   OR"
        "   (CAST(sh.init_hour AS INTEGER) >= :f6 AND CAST(sh.end_hour AS INTEGER) >= :f7)"
        " END";
      break;
    case 5:
      sql = "SELECT DISTINCT s.id AS id, s.name AS name, s.repetition AS repetition, sh.init_hour AS init_hour,"
        " sh.end_hour AS end_hour, ps.service_group AS service_group, ps.last_served_timestamp AS last_served"
        " FROM services s"
        " INNER JOIN persons_services ps ON s.id = ps.id_service"
        " INNER JOIN schedules_services ss ON s.id = ss.id_service"
        " INNER JOIN schedules sh ON ss.id_schedule = sh.id"
        " WHERE ps.id_person = :f1 AND sh.on_vi = 1 AND"
        " CASE WHEN CAST(sh.init_hour AS INTEGER) <= CAST(sh.end_hour AS INTEGER)"
        " THEN CAST(sh.init_hour AS INTEGER) <= :f2 AND CAST(sh.end_hour AS INTEGER) >= :f3"
        " ELSE"
        "   (CAST(sh.init_hour AS INTEGER) <= :f4 AND CAST(sh.end_hour AS INTEGER) <= :f5)"
        "   OR"
        "   (CAST(sh.init_hour AS INTEGER) >= :f6 AND CAST(sh.end_hour AS INTEGER) >= :f7)"
        " END";
      break;
    case 6:
      sql = "SELECT DISTINCT s.id AS id, s.name AS name, s.repetition AS repetition, sh.init_hour AS init_hour,"
        " sh.end_hour AS end_hour, ps.service_group AS service_group, ps.last_served_timestamp AS last_served"
        " FROM services s"
        " INNER JOIN persons_services ps ON s.id = ps.id_service"
        " INNER JOIN schedules_services ss ON s.id = ss.id_service"
        " INNER JOIN schedules sh ON ss.id_schedule = sh.id"
        " WHERE ps.id_person = :f1 AND sh.on_sa = 1 AND"
        " CASE WHEN CAST(sh.init_hour AS INTEGER) <= CAST(sh.end_hour AS INTEGER)"
        " THEN CAST(sh.init_hour AS INTEGER) <= :f2 AND CAST(sh.end_hour AS INTEGER) >= :f3"
        " ELSE"
        "   (CAST(sh.init_hour AS INTEGER) <= :f4 AND CAST(sh.end_hour AS INTEGER) <= :f5)"
        "   OR"
        "   (CAST(sh.init_hour AS INTEGER) >= :f6 AND CAST(sh.end_hour AS INTEGER) >= :f7)"
        " END";
      break;
    case 7:
      sql = "SELECT DISTINCT s.id AS id, s.name AS name, s.repetition AS repetition, sh.init_hour AS init_hour,"
        " sh.end_hour AS end_hour, ps.service_group AS service_group, ps.last_served_timestamp AS last_served"
        " FROM services s"
        " INNER JOIN persons_services ps ON s.id = ps.id_service"
        " INNER JOIN schedules_services ss ON s.id = ss.id_service"
        " INNER JOIN schedules sh ON ss.id_schedule = sh.id"
        " WHERE ps.id_person = :f1 AND sh.on_do = 1 AND"
        " CASE WHEN CAST(sh.init_hour AS INTEGER) <= CAST(sh.end_hour AS INTEGER)"
        " THEN CAST(sh.init_hour AS INTEGER) <= :f2 AND CAST(sh.end_hour AS INTEGER) >= :f3"
        " ELSE"
        "   (CAST(sh.init_hour AS INTEGER) <= :f4 AND CAST(sh.end_hour AS INTEGER) <= :f5)"
        "   OR"
        "   (CAST(sh.init_hour AS INTEGER) >= :f6 AND CAST(sh.end_hour AS INTEGER) >= :f7)"
        " END";
      break;
    default:
      LOG_ERROR("Wrong Day %d", day);
      return 0;
  }

  QSqlQuery qry(this->db);
  qry.prepare(sql);
  qry.bindValue(":f1", userId);
  qry.bindValue(":f2", hour);
  qry.bindValue(":f3", hour);
  qry.bindValue(":f4", hour);
  qry.bindValue(":f5", hour);
  qry.bindValue(":f6", hour);
  qry.bindValue(":f7", hour);
  if (!qry.exec()) {
    LOG_ERROR("Error selecting services: %s", qry.lastError().text().toStdString().c_str());
    qry.finish();
    return 0;
  }
  
  while(qry.next()) {
    int service_id = qry.value(0).toInt();
    QString service_name = qry.value(1).toString();
    int service_repetition = qry.value(2).toInt();
    int service_init_hour = qry.value(3).toInt();
    int service_end_hour = qry.value(4).toInt();
    int service_group = qry.value(5).toInt();
    int service_timestamp = qry.value(6).toInt();

    ServiceDAO *dao = new ServiceDAO();
    dao->id = service_id;
    dao->name = service_name;
    dao->repetition = (service_repetition == 0) ? false : true;
    dao->initHour = service_init_hour;
    dao->endHour = service_end_hour;
    dao->group = service_group;
    dao->lastServed = service_timestamp;
    services->insert(service_id, dao);
    
    count += 1;
  }
  
  qry.finish();
  return count;
}

int GeneraDB::updateService(int userId, int serviceGroup)
{
  if (!this->db.isOpen()) { 
    DEBUG("Database genera is not open");
    return 1;
  }

  QSqlQuery qry(this->db);
  qry.prepare("UPDATE persons_services SET last_served_timestamp = :f1 WHERE id_person = :f2 AND service_group = :f3"); 
  qry.bindValue(":f1", Utils::getCurrentUnixTimestamp());
  qry.bindValue(":f2", userId);
  qry.bindValue(":f3", serviceGroup);
  if (!qry.exec()) {
    LOG_ERROR("Error updating service: %s", qry.lastError().text().toStdString().c_str());
    qry.finish();
    return 1;
  }

  LOG_INFO("Service updated");
  qry.finish();
  return 0;
}

#endif

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
  UNUSED(idkit);
  QSqlDatabase db = QSqlDatabase::database("import");
  
#ifdef TEMPO
  importDatabaseTempo(idkit, &db);
#elif SNACK
  importDatabaseSnack(idkit, &db);
#endif
}

#ifdef TEMPO
void ImportDB::importDatabaseTempo(IDKITWrapper *idkit, QSqlDatabase *db)
{
  int importCount = 0;
  QSqlQuery qry(*db);

  if (!qry.exec("SELECT p.identificadorper, "
        "p.nombreper || ' ' || p.apppaternoper || ' ' || p.appmaternoper, "
        "p.rutper || '-' ||  p.rutdvper,  h.template, e.empnombre "
        "FROM gen_persona p "
        "INNER JOIN gen_huella_dactilar h ON p.idper = h.idper "
        "INNER JOIN gen_empresa e ON p.idemp = e.idemp")) {
    LOG_ERROR("Query error: %s.", qry.lastError().databaseText().toStdString().c_str());
    qry.finish();
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

      QString emp = qry.value(4).toString();
      Utils::limitString(emp, 32);

      if (idkit->registerUserFromTemplate(reinterpret_cast<unsigned char *>(decoded.data()), (char *) identifier.toStdString().c_str(),
            (char *) name.toStdString().c_str(), (char *) rut.toStdString().c_str(), (char *) emp.toStdString().c_str())) {
        importCount += 1;
      }

      if (importCount % 10 == 0) {
        emit importProgress(importCount);
      }
    }
    qry.finish();
  }
}
#endif

#ifdef SNACK
void ImportDB::importDatabaseSnack(IDKITWrapper *idkit, QSqlDatabase *db)
{
<<<<<<< HEAD
  UNUSED(idkit);
  UNUSED(db);
  DEBUG("Not implemented yet...");
=======
  int importCount = 0;
  QSqlQuery qry(*importDb);
  
  int rc = generaDb->truncateTables();
  if (rc != 0) {
    LOG_ERROR("Error truncating genera tables.");
    return;
  }

  if (!qry.exec("SELECT s.id_service, s.name, s.repetition FROM services s")) {
    LOG_ERROR("Query error: %s.", qry.lastError().databaseText().toStdString().c_str());
    qry.finish();
    return;
  } else {
    while (qry.next()) {
      int service_id = qry.value(0).toInt();
      QString service_name = qry.value(1).toString();
      int service_repetition = qry.value(2).toInt();

      generaDb->insertService(service_id, service_name, service_repetition);
    }
    
    qry.finish();
  }
  
  if (!qry.exec("SELECT s.id_schedule, s.init_hour, s.end_hour, s.on_lu, s.on_ma, s.on_mi, s.on_ju, s.on_vi, s.on_sa, s.on_do FROM schedules s")) {
    LOG_ERROR("Query error: %s.", qry.lastError().databaseText().toStdString().c_str());
    qry.finish();
    return;
  } else {
    while (qry.next()) {
      int schedule_id = qry.value(0).toInt();
      int init_hour = qry.value(1).toInt();
      int end_hour = qry.value(2).toInt();
      int onLu = qry.value(3).toInt();
      int onMa = qry.value(4).toInt();
      int onMi = qry.value(5).toInt();
      int onJu = qry.value(6).toInt();
      int onVi = qry.value(7).toInt();
      int onSa = qry.value(8).toInt();
      int onDo = qry.value(9).toInt();

      generaDb->insertSchedule(schedule_id, init_hour, end_hour, onLu, onMa, onMi, onJu, onVi, onSa, onDo);
    }
    
    qry.finish();
  }
  
  if (!qry.exec("SELECT s.id_schedule, s.id_service FROM schedules_services s")) {
    LOG_ERROR("Query error: %s.", qry.lastError().databaseText().toStdString().c_str());
    qry.finish();
    return;
  } else {
    while (qry.next()) {
      int schedule_id = qry.value(0).toInt();
      int service_id = qry.value(1).toInt();

      generaDb->insertScheduleService(schedule_id, service_id);
    }
    
    qry.finish();
  }
  
  if (!qry.exec("SELECT p.id_person, p.id_service, p.service_group FROM persons_services p")) {
    LOG_ERROR("Query error: %s.", qry.lastError().databaseText().toStdString().c_str());
    qry.finish();
    return;
  } else {
    while (qry.next()) {
      int person_id = qry.value(0).toInt();
      int service_id = qry.value(1).toInt();
      int service_group = qry.value(2).toInt();

      generaDb->insertPersonService(person_id, service_id, service_group);
    }
    
    qry.finish();
  }

  if (!qry.exec("SELECT p.cas_idper, p.cas_identificadorper, "
        "p.cas_nombreper || ' ' || p.cas_apppaternoper || ' ' || p.cas_appmaternoper, "
        "p.cas_rutper || '-' ||  p.cas_rutdvper, p.cas_repeticion, p.cas_codigocentrocosto, "
        "h.template, e.cas_empnombre "
        "FROM gen_cas_persona p "
        "INNER JOIN gen_cas_huella_dactilar h ON p.cas_idper = h.idper "
        "INNER JOIN gen_cas_empresa e ON p.cas_idemp = e.cas_idemp")) {
    LOG_ERROR("Query error: %s.", qry.lastError().databaseText().toStdString().c_str());
    qry.finish();
  } else {
    DEBUG("Selected. Beginnning to import data.");

    while(qry.next()) {
      int userId = qry.value(0).toInt();
      
      QString identifier = qry.value(1).toString();
      Utils::limitString(identifier, 32);

      QString name = qry.value(2).toString();
      Utils::limitString(name, 100);

      QString rut = qry.value(3).toString();
      Utils::limitString(rut, 15);

      int repeticion = qry.value(4).toInt();

      QString centroCosto = qry.value(5).toString();
      Utils::limitString(centroCosto, 32);

      QString tpl = qry.value(6).toString();
      QByteArray bytes = tpl.toUtf8();
      QByteArray decoded = bytes.fromBase64(bytes);

      QString emp = qry.value(7).toString();
      Utils::limitString(emp, 32);

      if (idkit->registerUserFromTemplateSnack(reinterpret_cast<unsigned char *>(decoded.data()), userId, 
            (char *) identifier.toStdString().c_str(), (char *) name.toStdString().c_str(), 
            (char *) rut.toStdString().c_str(), (char *) emp.toStdString().c_str(), repeticion,
            (char *) centroCosto.toStdString().c_str())) {
        importCount += 1;
      }

      if (importCount % 10 == 0) {
        emit importProgress(importCount);
      }
    }
    qry.finish();
  }
>>>>>>> 9bf309d... snack10 v0.8 almost ready
}
#endif

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
    qry.finish();
    return false;
  } else {
    DEBUG("Selected. Beginnning to generate data.");

    while(qry.next()) {
      QString param = qry.value(0).toString();
      QString value = qry.value(1).toString();

      if (param == "EMPRESA_HOLDING_NOMBRE") {
        configMap["empresa_holding"] = value;
      } else if (param == "SERIAL_EQUIPO") {
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
    
    qry.finish();
    return true;
  }
}
