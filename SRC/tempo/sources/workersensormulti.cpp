#include "workersensormulti.h"
#include "debug_macros.h"
#include "utils.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <linux/input.h>
#include <unistd.h>

#include <QThread>

WorkerSensorMulti::WorkerSensorMulti(QObject *parent) :
  QObject(parent)
{
  _abort = false;
  _interrupt = false;
}

void WorkerSensorMulti::setVCOMWrapper(VCOMWrapper *vcom) { this->vcom = vcom; }
void WorkerSensorMulti::setIDKITWrapper(IDKITWrapper *idkit) { this->idkit = idkit; }
void WorkerSensorMulti::setPrinterSerial(PrinterSerial *printer) { this->printer = printer; }
void WorkerSensorMulti::setSQLiteManager(EventsDB *manager) { this->eventsDB = manager; }

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
        rc = eventsDB->isUserIdentifiedOnLastMinute(QString(userIdentifier), typeInt);
        if (rc == 0) {
          emit error("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
        } else if (rc == 1) {
          emit match(NULL, QString(userName), QString(userRut));
        } else if (rc == 2) {
          emit buttonPressed(typeInt, QString(userName));
          
          if (printer->getStatus()) {
            printer->write_user(typeStr, QString(userIdentifier), QString(userName), QString(userRut), QString(userEmp));
          }
          
          rc = eventsDB->insertEvent(typeInt, userIdentifier, Utils::getCurrentUnixTimestamp(), 0);
          if (rc == 0) {
            emit error("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
          } else if (rc == 1) {
            LOG_INFO("Event added to events database...");
          } else if (rc == 2) {
            LOG_ERROR("ERROR adding event to events database...");
          } else {
            emit error("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
          }
        } else {
          emit error("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
        }
      } else {
        emit buttonPressed(0, QString(userName));
      }
    } else {
      emit match(NULL, NULL, NULL);
    }
  } else {
    emit error("Ha ocurrido un error<br>Por favor, notifique<br>a los encargados.");
  }

  if (compositeImage)
    delete [] compositeImage;
  emit identifierWorkDone();
}

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
        doEnroll();
        break;
      case Wait:
        // doWait();
        break;
    }
  }
}

bool WorkerSensorMulti::isButtonPressed(QString &typeStr, int &typeInt)
{
#ifdef HOST
  sleep(2);
  typeStr = "ENTRADA";
  typeInt = 1;
  emit buttonPressed(1, userName);
  return true;
#else
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
#endif
}
