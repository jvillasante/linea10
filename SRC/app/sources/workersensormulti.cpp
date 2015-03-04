#include "workersensormulti.h"
#include "debug_macros.h"
#include "utils.h"

#ifdef SNACK
#include "dao_service.h"
#endif

#include <cstdio>
#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <linux/input.h>
#include <unistd.h>

#include <QThread>
#include <QMap>
#include <QMapIterator>
#include <QList>

#ifdef SNACK
#define EXE_GREP "/bin/grep"
#define COMMAND_STR_DEVICES EXE_GREP " EV /proc/bus/input/devices | " EXE_GREP " -nE '120013'"

#define KEY_RELEASE 0
#define KEY_PRESS 1
#define KEY_KEEPING_PRESSED 2
#endif

WorkerSensorMulti::WorkerSensorMulti(QObject *parent) :
  QObject(parent)
{
  _abort = false;
  _interrupt = false;

#ifdef SNACK
  memset(keypadDevice, 0, sizeof(char) * 20);
  int deviceIndex = determineInputDeviceIndex();
  if (deviceIndex < 0) {
    LOG_ERROR("Can't determine keybord, setting it to: /dev/input/event2");
    sprintf(keypadDevice, "/dev/input/event%d", 2);
  } else {
    sprintf(keypadDevice, "/dev/input/event%d", deviceIndex);
  }
  DEBUG("KEYPAD DEVICE: %s", keypadDevice);
#endif
}

void WorkerSensorMulti::setVCOMWrapper(VCOMWrapper *vcom) { this->vcom = vcom; }
void WorkerSensorMulti::setIDKITWrapper(IDKITWrapper *idkit) { this->idkit = idkit; }
#if defined(TEMPO) || defined(SNACK)
void WorkerSensorMulti::setPrinterSerial(PrinterSerial *printer) { this->printer = printer; }
#endif
void WorkerSensorMulti::setSQLiteManager(GeneraDB *manager) { this->generaDB = manager; }

void WorkerSensorMulti::requestMethod(WorkerSensorMulti::Method method)
{
  DEBUG("Request worker sensor method %d in Thread %p", method, thread()->currentThreadId());
  QMutexLocker locker(&mutex);
  _interrupt = true;
  _method = method;
  condition.wakeOne();
}

void WorkerSensorMulti::abort()
{
  DEBUG("Request worker sensor aborting in thread %p", thread()->currentThreadId());
  QMutexLocker locker(&mutex);
  _abort = true;
  condition.wakeOne();
}

void WorkerSensorMulti::doIdentify()
{
  DEBUG("Starting identify in thread %p", thread()->currentThreadId());

#ifdef TEMPO
  doIdentifyTempo();
#endif
#ifdef SNACK
  doIdentifySnack();
#endif
#ifdef PRESENCIA
  doIdentifyPresencia();
#endif
}

#ifdef TEMPO
void WorkerSensorMulti::doIdentifyTempo()
{
  bool abort;
  bool interrupt;

  mutex.lock(); abort = _abort; interrupt = _interrupt; mutex.unlock();
  if (abort || interrupt) { DEBUG("Interrupting Identify in thread %p", thread()->currentThreadId()); return; }

  uchar *compositeImage;
  uchar templateImage[2048];
  uint width;
  uint height;
  uint templateSize;
  int spoof;
  int rc;

  char userIdentifier[32];
  char userName[100];
  char userRut[15];
  char userEmp[32];

  width = this->vcom->getImageWidth();
  height = this->vcom->getImageHeight();
  compositeImage = new uchar[width * height];

  memset(compositeImage, 0, sizeof(uchar) * width * height);
  memset(userIdentifier, 0, sizeof(char) * 32);
  memset(userName, 0, sizeof(char) * 100);
  memset(userRut, 0, sizeof(char) * 8);
  memset(userEmp, 0, sizeof(char) * 32);

  bool stop = false;
  while (stop == false) {
    rc = vcom->capture(compositeImage, width, height, templateImage, templateSize, spoof, 1, 0);
    mutex.lock(); abort = _abort; interrupt = _interrupt; mutex.unlock();
    if (abort || interrupt) {
      DEBUG("Interrupting Identify in thread %p", thread()->currentThreadId());
      if (compositeImage) { delete [] compositeImage; }
      return;
    }
    if (rc != 1) { stop = true; }
  }

  if (rc == 0) {
    if (idkit->matchFromRawImage(compositeImage, width, height, &userIdentifier[0], &userName[0], &userRut[0], &userEmp[0])) {
      emit match(QString(userIdentifier), QString(userName), QString(userRut));

      QString typeStr;
      int typeInt;
      if (isButtonPressed(typeStr, typeInt)) {
        rc = generaDB->isUserIdentifiedOnLastMinute(QString(userIdentifier), typeInt);
        if (rc == 0) {
          emit message("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
        } else if (rc == 1) {
          emit match(NULL, QString(userName), QString(userRut));
        } else if (rc == 2) {
          emit buttonPressed(typeInt, QString(userName));

          if (printer->getStatus()) {
            printer->write_user(typeStr, QString(userIdentifier), QString(userName), QString(userRut), QString(userEmp));
          }

          rc = generaDB->insertEvent(typeInt, userIdentifier, Utils::getCurrentUnixTimestamp(), 0);
          if (rc == 0) {
            LOG_INFO("Event added to events database...");
          } else if (rc == 1) {
            emit message("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
          } else {
            emit message("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
          }
        } else {
          emit message("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
        }
      } else {
        emit buttonPressed(0, QString(userName));
      }
    } else {
      emit match(NULL, NULL, NULL);
    }
  } else {
    emit message("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
  }

  if (compositeImage)
    delete [] compositeImage;
  emit identifierWorkDone();
}
#endif

#ifdef PRESENCIA
void WorkerSensorMulti::doIdentifyPresencia()
{
  bool abort;
  bool interrupt;

  mutex.lock(); abort = _abort; interrupt = _interrupt; mutex.unlock();
  if (abort || interrupt) { DEBUG("Interrupting Identify in thread %p", thread()->currentThreadId()); return; }

  uchar *compositeImage;
  uchar templateImage[2048];
  uint width;
  uint height;
  uint templateSize;
  int spoof;
  int rc;

  char userIdentifier[32];
  char userName[100];
  char userRut[15];
  char userEmp[32];

  width = this->vcom->getImageWidth();
  height = this->vcom->getImageHeight();
  compositeImage = new uchar[width * height];

  memset(compositeImage, 0, sizeof(uchar) * width * height);
  memset(userIdentifier, 0, sizeof(char) * 32);
  memset(userName, 0, sizeof(char) * 100);
  memset(userRut, 0, sizeof(char) * 8);
  memset(userEmp, 0, sizeof(char) * 32);

  bool stop = false;
  while (stop == false) {
    rc = vcom->capture(compositeImage, width, height, templateImage, templateSize, spoof, 1, 0);
    mutex.lock(); abort = _abort; interrupt = _interrupt; mutex.unlock();
    if (abort || interrupt) {
      DEBUG("Interrupting Identify in thread %p", thread()->currentThreadId());
      if (compositeImage) { delete [] compositeImage; }
      return;
    }
    if (rc != 1) { stop = true; }
  }

  if (rc == 0) {
    if (idkit->matchFromRawImage(compositeImage, width, height, &userIdentifier[0], &userName[0], &userRut[0], &userEmp[0])) {
      emit match(QString(userIdentifier), QString(userName), QString(userRut));
      DEBUG("User: %s", userName);

      // QString typeStr;
      // int typeInt;
      // if (isButtonPressed(typeStr, typeInt)) {
        // rc = generaDB->isUserIdentifiedOnLastMinute(QString(userIdentifier), typeInt);
        // if (rc == 0) {
          // emit message("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
        // } else if (rc == 1) {
          // emit match(NULL, QString(userName), QString(userRut));
        // } else if (rc == 2) {
          // emit buttonPressed(typeInt, QString(userName));

          // if (printer->getStatus()) {
            // printer->write_user(typeStr, QString(userIdentifier), QString(userName), QString(userRut), QString(userEmp));
          // }

          // rc = generaDB->insertEvent(typeInt, userIdentifier, Utils::getCurrentUnixTimestamp(), 0);
          // if (rc == 0) {
            // LOG_INFO("Event added to events database...");
          // } else if (rc == 1) {
            // emit message("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
          // } else {
            // emit message("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
          // }
        // } else {
          // emit message("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
        // }
      // } else {
        // emit buttonPressed(0, QString(userName));
      // }
    } else {
      emit match(NULL, NULL, NULL);
    }
  } else {
    emit message("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
  }

  if (compositeImage)
    delete [] compositeImage;
  emit identifierWorkDone();
}
#endif

#ifdef SNACK
void WorkerSensorMulti::doIdentifySnack()
{
  bool abort;
  bool interrupt;

  mutex.lock(); abort = _abort; interrupt = _interrupt; mutex.unlock();
  if (abort || interrupt) { DEBUG("Interrupting Identify in thread %p", thread()->currentThreadId()); return; }

  uchar *compositeImage;
  uchar templateImage[2048];
  uint width;
  uint height;
  uint templateSize;
  int spoof;
  int rc;

  width = this->vcom->getImageWidth();
  height = this->vcom->getImageHeight();
  compositeImage = new uchar[width * height];
  memset(compositeImage, 0, sizeof(uchar) * width * height);

  bool stop = false;
  while (stop == false) {
    rc = vcom->capture(compositeImage, width, height, templateImage, templateSize, spoof, 1, 0);
    mutex.lock(); abort = _abort; interrupt = _interrupt; mutex.unlock();
    if (abort || interrupt) {
      DEBUG("Interrupting Identify in thread %p", thread()->currentThreadId());
      if (compositeImage) { delete [] compositeImage; }
      return;
    }
    if (rc != 1) { stop = true; }
  }

  if (rc == 0) {
    char userIdentifier[32], userName[100], userRut[15], userEmp[32], userCentroCosto[32];
    int userId, repeticion;

    memset(userIdentifier, 0, sizeof(char) * 32);
    memset(userName, 0, sizeof(char) * 100);
    memset(userRut, 0, sizeof(char) * 8);
    memset(userEmp, 0, sizeof(char) * 32);
    memset(userCentroCosto, 0, sizeof(char) * 32);

    if (idkit->matchFromRawImageSnack(compositeImage, width, height, &userId, &userIdentifier[0], &userName[0],
          &userRut[0], &userEmp[0], &repeticion, &userCentroCosto[0])) {
      int datetime[2];  // hora actual: [2,1330] (martes, 13:30 horas)
      Utils::getCurrentDateTimeForSnack(datetime);

      QMap<int, ServiceDAO*> services;
      int count = generaDB->getServicesForUser(userId, datetime[0], datetime[1], &services);
      DEBUG("User %d (%s) has %d service(s) on [%d, %d]", userId, userIdentifier, count, datetime[0], datetime[1]);

      if (count <= 0) {
        emit match(userIdentifier, userName, userRut, NULL, 0);
      } else if (count == 1) {
        ServiceDAO *service = NULL;
        QMap<int, ServiceDAO*>::iterator it;
        for (it = services.begin(); it != services.end(); ++it)
          service = it.value();

        if ((repeticion == 1) || (service->repetition == 1)) {
          giveService(service, userId, userIdentifier, userName, userRut, userEmp, userCentroCosto);
        } else {
          QDateTime lastServed;
          Utils::getFromUnixTimestamp(lastServed, service->lastServed);
          QDateTime current = Utils::getCurrentTimestamp();

          if (lastServed.date() >= current.date()) { // servicio ya dado
            emit match(userIdentifier, userName, userRut, service->name, -1);
          } else { // servicio no dado aun
            giveService(service, userId, userIdentifier, userName, userRut, userEmp, userCentroCosto);
          }
        }
      } else {
        emit match(userIdentifier, userName, userRut, NULL, count);
        int numberEntered = waitForKeyboard();
        DEBUG("Number Entered: %d", numberEntered);

        if (numberEntered == -1) {
          LOG_ERROR("Error on keyboard input");
          emit message("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
        } else if (numberEntered == -2) {
          emit message("<font size=6>TIMEOUT</font>");
        } else if (numberEntered == -3) {
          emit message("<font size=4>Operacion Cancelada</font>");
        } else if (numberEntered > 0) {
          if (services.contains(numberEntered)) {
            ServiceDAO *service = services[numberEntered];
            ServiceDAO *lastServedServiceInGroup = getLastServedFromGroup(services, service);
            DEBUG("Service: %s", service->name.toStdString().c_str());
            DEBUG("Last served service in group: %s", lastServedServiceInGroup->name.toStdString().c_str());

            if ((repeticion == 1) || (service->repetition == 1)) {
              giveService(service, userId, userIdentifier, userName, userRut, userEmp, userCentroCosto);
            } else {
              QDateTime lastServed;
              Utils::getFromUnixTimestamp(lastServed, lastServedServiceInGroup->lastServed);
              QDateTime current = Utils::getCurrentTimestamp();

              if (lastServed.date() >= current.date()) { // servicio ya dado
                emit match(userIdentifier, userName, userRut, service->name, -1);
              } else { // servicio no dado aun
                giveService(service, userId, userIdentifier, userName, userRut, userEmp, userCentroCosto);
              }
            }
          } else {
            DEBUG("Usted no posse el servicio: %d", numberEntered);
            emit message("Usted no posee el<br>servicio solicitado.");
          }
        } else {
          LOG_ERROR("Error on keyboard input");
          emit message("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
        }
      }

      // delete ServiceDAO from map
      QMap<int, ServiceDAO*>::iterator it;
      for (it = services.begin(); it != services.end(); ++it)
        delete it.value();
    } else {
      emit match(NULL, NULL, NULL, NULL, 0);
    }
  } else {
    emit message("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
  }

  if (compositeImage)
    delete [] compositeImage;
  emit identifierWorkDone();
}

void WorkerSensorMulti::giveService(ServiceDAO *service, int userId, char *userIdentifier, char *userName,
    char *userRut, char *userEmp, char *userCentroCosto)
{
  int rc;

  emit match(userIdentifier, userName, userRut, service->name, 1);

  if (!printer->getStatus()) {
    emit match(userIdentifier, userName, userRut, service->name, -2);
    return;
  }

  printer->write_user(service->name, QString(userIdentifier), QString(userName), QString(userRut),
      QString(userEmp), QString(userCentroCosto));

  rc = generaDB->updateService(userId, service->group);
  if (rc == 0) {
    LOG_INFO("Service updated...");
  } else if (rc == 1) {
    emit message("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
  } else {
    emit message("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
  }

  rc = generaDB->insertEvent(1, userIdentifier, Utils::getCurrentUnixTimestamp(), service->id, 0);
  if (rc == 0) {
    LOG_INFO("Event added to events database...");
  } else if (rc == 1) {
    emit message("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
  } else {
    emit message("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
  }
}
#endif

#ifdef TEMPO
void WorkerSensorMulti::doEnroll()
{
  DEBUG("Starting enroll in thread %p", thread()->currentThreadId());
  bool abort;
  bool interrupt;

  mutex.lock(); abort = _abort; interrupt = _interrupt; mutex.unlock();
  if (abort || interrupt) { DEBUG("Interrupting Enroll in thread %p", thread()->currentThreadId()); return; }

  uchar *compositeImage;
  uchar *templateImage;
  uint width;
  uint height;
  uint templateSize;
  int spoof;
  int rc;

  width = this->vcom->getImageWidth();
  height = this->vcom->getImageHeight();
  compositeImage = new uchar[width * height];
  templateImage = new uchar[2048];

  memset(templateImage, 0, 2048);
  memset(compositeImage, 0, sizeof(uchar) * width * height);

  bool stop = false;
  while (stop == false) {
    rc = vcom->capture(compositeImage, width, height, templateImage, templateSize, spoof, 1, 1);
    mutex.lock(); abort = _abort; interrupt = _interrupt; mutex.unlock();
    if (abort || interrupt) {
      DEBUG("Interrupting Enroll in thread %p", thread()->currentThreadId());
      if (compositeImage) { delete [] compositeImage; }
      if (templateImage) { delete [] templateImage; }
      return;
    }
    if (rc != 1) { stop = true; }
  }

  if (rc == 0) {
    DEBUG("capture image for enroll OK");
    emit enrollWorkDone(compositeImage, templateImage, width, height);
  } else {
    DEBUG("capture image for enroll FAIL");
    emit enrollWorkDone(NULL, NULL, 0, 0);

    if (compositeImage) delete [] compositeImage;
    if (templateImage) delete [] templateImage;
  }
}
#endif

void WorkerSensorMulti::doWait()
{
  DEBUG("Starting worker wait in thread %p", thread()->currentThreadId());
  bool abort;
  bool interrupt;

  while (true) {
    mutex.lock(); abort = _abort; interrupt = _interrupt; mutex.unlock();
    if (abort || interrupt) { DEBUG("Interrupting Wait in thread %p", thread()->currentThreadId()); return; }

    DEBUG("WAITING...");
    sleep(1);
  }
}

void WorkerSensorMulti::mainLoop()
{
  DEBUG("Starting worker mainLoop in thread %p", thread()->currentThreadId());

  while (true) {
    mutex.lock();
    if (!_interrupt && !_abort) {
      condition.wait(&mutex);
    }
    _interrupt = false;

    if (_abort) {
      DEBUG("Aborting worker mainLoop in thread %p", thread()->currentThreadId());
      mutex.unlock();
      emit finished();
      return;
    }

    Method method = _method;
    mutex.unlock();

    switch(method) {
      case Identify:
        doIdentify();
        break;
      case Enroll:
#ifdef TEMPO
        doEnroll();
#endif
        break;
      case Wait:
        // doWait();
        break;
    }
  }
}

#ifdef TEMPO
bool WorkerSensorMulti::isButtonPressed(QString &typeStr, int &typeInt)
{
  int fd;
  struct input_event event;
  ssize_t bytesRead;

  int ret;
  struct timeval tv;
  fd_set readfds;

  mutex.lock(); bool abort = _abort; mutex.unlock();
  if (abort) { DEBUG("Aborting worker sensor process in Thread %p on buttons Handler", thread()->currentThreadId()); return false; }

  typeStr = "";
  typeInt = -1;
  fd = open("/dev/input/event0", O_RDONLY);
  CHECK(fd != -1, "Error opening /dev/input/event0 for reading");
  DEBUG("/dev/input/event0 oppened for reading");

  /* Wait on fd for input */
  FD_ZERO(&readfds);
  FD_SET(fd, &readfds);

  /* Wait up to five seconds */
  tv.tv_sec = 5;
  tv.tv_usec = 0;

repeat:
  ret = select(fd + 1, &readfds, NULL, NULL, &tv);
  if (ret == -1) {
    LOG_ERROR("select call on /dev/input/event0: an error ocurred");
    goto error;
  } else if (!ret) {
    DEBUG("select call on /dev/input/event0: TIMEOUT");
    if (fd) { if (close(fd) != 0) { LOG_ERROR("Error closing /dev/input/event0"); } }
    DEBUG("/dev/input/event0 closed because of TIMEOUT");
    return false;
  }

  /* File descriptor is now ready */
  if (FD_ISSET(fd, &readfds)) {
    bytesRead = read(fd, &event, sizeof(struct input_event));
    CHECK(bytesRead != -1, "Read input error: call to read returned -1.");
    CHECK(bytesRead == sizeof(struct input_event), "Read input error: bytes read is not an input_event.");

    switch (event.code) {
      case BTN_5:
        DEBUG("IN Button Pressed");
        typeStr = tr("ENTRADA");
        typeInt = 1;
        break;
      case BTN_6:
        DEBUG("OUT Button Pressed");
        typeStr = tr("SALIDA");
        typeInt = 2;
        break;
      default:
        // DEBUG("UNKNOWN Button Pressed");
        goto repeat;
    }
  }

  if ((typeInt != -1) && !typeStr.isEmpty()) {
    if (fd) { if (close(fd) != 0) { LOG_ERROR("Error closing /dev/input/event0"); } }
    DEBUG("/dev/input/event0 closed because button %s was pressed", typeStr.toStdString().c_str());
    return true;
  } else {
    if (fd) { if (close(fd) != 0) { LOG_ERROR("Error closing /dev/input/event0"); } }
    DEBUG("/dev/input/event0 closed because UNKNOWN button was pressed");
    return false;
  }

error:
  if (fd) { if (close(fd) != 0) { LOG_ERROR("Error closing /dev/input/event0"); } }
  DEBUG("/dev/input/event0 closed in error handler");
  return false;
}
#endif
#ifdef SNACK
ServiceDAO *WorkerSensorMulti::getLastServedFromGroup(QMap<int, ServiceDAO*> services, ServiceDAO *service)
{
  ServiceDAO *result = NULL;
  QMap<int, ServiceDAO*>::iterator it;
  for (it = services.begin(); it != services.end(); ++it) {
    if (it.value()->group == service->group) {
      if (it.value()->lastServed > service->lastServed) {
        result = it.value();
      }
    }
  }

  return (result == NULL) ? service : result;
}

std::string WorkerSensorMulti::execute(const char* cmd)
{
  FILE* pipe = popen(cmd, "r");
  if (!pipe) {
    LOG_ERROR("Pipe error");
    return NULL;
  }
  char buffer[128];
  std::string result = "";
  while(!feof(pipe))
    if(fgets(buffer, 128, pipe) != NULL)
      result += buffer;
  pclose(pipe);
  DEBUG("%s: %s", cmd, result.c_str());
  return result;
}

int WorkerSensorMulti::determineInputDeviceIndex()
{
  // extract input number from /proc/bus/input/devices (I don't know how to do it better. If you have an idea, please let me know.)
  std::string output = execute(COMMAND_STR_DEVICES);

  int index = atoi(output.c_str()) - 1;
  return index;
}

int WorkerSensorMulti::waitForKeyboard()
{
  int number = 0;
  bool done = false;
  bool ignoreEvent = false;

  int fd;
  struct input_event event;
  ssize_t bytesRead;

  int ret;
  struct timeval tv;
  fd_set readfds;

  mutex.lock(); bool abort = _abort; mutex.unlock();
  if (abort) { DEBUG("Aborting worker sensor process in Thread %p on buttons Handler", thread()->currentThreadId()); return -1; }

  fd = open(keypadDevice, O_RDONLY);
  CHECK(fd != -1, "Error opening keyboard for reading");
  DEBUG("%s oppened for reading", keypadDevice);

  /* Wait on fd for input */
  FD_ZERO(&readfds);
  FD_SET(fd, &readfds);

  /* Wait up to five seconds */
  tv.tv_sec = 10;
  tv.tv_usec = 0;

repeat:
  ret = select(fd + 1, &readfds, NULL, NULL, &tv);
  if (ret == -1) {
    LOG_ERROR("select call on %s: an error ocurred", keypadDevice);
    goto error;
  } else if (!ret) {
    DEBUG("select call on %s: TIMEOUT", keypadDevice);
    if (fd) { if (close(fd) != 0) { LOG_ERROR("Error closing %s", keypadDevice); } }
    DEBUG("%s closed because of TIMEOUT", keypadDevice);
    return -2;
  }

  /* File descriptor is now ready */
  if (FD_ISSET(fd, &readfds)) {
    bytesRead = read(fd, &event, sizeof(struct input_event));
    CHECK(bytesRead != -1, "Read input error: call to read returned -1.");
    CHECK(bytesRead == sizeof(struct input_event), "Read input error: bytes read is not an input_event.");

    if (event.type != EV_KEY) goto repeat;

    // * generates another event, let's ignore it right here
    if (ignoreEvent) { ignoreEvent = false; goto repeat; }

    if ((event.value == KEY_PRESS) || (event.value == KEY_KEEPING_PRESSED)) {
      switch (event.code) {
        case 2:
          DEBUG("1 Pressed");
          if (number < 9999) {
            number = number * 10 + 1;
            emit message("<font size=6>Teclado<br>" + QString::number(number) + "</font>");
          }
          break;
        case 3:
          DEBUG("2 Pressed");
          if (number < 9999) {
            number = number * 10 + 2;
            emit message("<font size=6>Teclado<br>" + QString::number(number) + "</font>");
          }
          break;
        case 4:
          DEBUG("3 Pressed");
          if (number < 9999) {
            number = number * 10 + 3;
            emit message("<font size=6>Teclado<br>" + QString::number(number) + "</font>");
          }
          break;
        case 5:
          DEBUG("4 Pressed");
          if (number < 9999) {
            number = number * 10 + 4;
            emit message("<font size=6>Teclado<br>" + QString::number(number) + "</font>");
          }
          break;
        case 6:
          DEBUG("5 Pressed");
          if (number < 9999) {
            number = number * 10 + 5;
            emit message("<font size=6>Teclado<br>" + QString::number(number) + "</font>");
          }
          break;
        case 7:
          DEBUG("6 Pressed");
          if (number < 9999) {
            number = number * 10 + 6;
            emit message("<font size=6>Teclado<br>" + QString::number(number) + "</font>");
          }
          break;
        case 8:
          DEBUG("7 Pressed");
          if (number < 9999) {
            number = number * 10 + 7;
            emit message("<font size=6>Teclado<br>" + QString::number(number) + "</font>");
          }
          break;
        case 9:
          DEBUG("8 Pressed");
          if (number < 9999) {
            number = number * 10 + 8;
            emit message("<font size=6>Teclado<br>" + QString::number(number) + "</font>");
          }
          break;
        case 10:
          DEBUG("9 Pressed");
          if (number < 9999) {
            number = number * 10 + 9;
            emit message("<font size=6>Teclado<br>" + QString::number(number) + "</font>");
          }
          break;
        case 11:
          DEBUG("0 Pressed");
          if ((number > 0) && (number < 9999)) {
            number = number * 10 + 0;
            emit message("<font size=6>Teclado<br>" + QString::number(number) + "</font>");
          }
          break;
        case 42: // TODO: Ver codigo del *
          DEBUG("* Pressed");
          ignoreEvent = true;
          break;
        case 52:
          DEBUG(". Pressed");
          break;
        case 1:
          DEBUG("Cancel Pressed");
          done = true;
          number = 0;
          break;
        case 14:
          DEBUG("Clear Pressed");
          number = 0;
          emit message("<font size=6>Teclado<br></font>");
          break;
        case 57:
          DEBUG("Blank Pressed");
          break;
        case 28:
          DEBUG("Confirm Pressed");
          if (number > 0) {
            done = true;
          }
          break;
        default:
          DEBUG("UNKNOWN Pressed");
          break;
      }
    }
  }

  if (!done) {
    goto repeat;
  } else {
    if (number > 0) {
      if (fd) { if (close(fd) != 0) { LOG_ERROR("Error closing %s", keypadDevice); } }
      return number;
    } else {
      if (fd) { if (close(fd) != 0) { LOG_ERROR("Error closing %s", keypadDevice); } }
      return -3;
    }
  }

error:
  if (fd) { if (close(fd) != 0) { LOG_ERROR("Error closing %s", keypadDevice); } }
  DEBUG("%s closed in error handler", keypadDevice);
  return -1;
}
#endif
