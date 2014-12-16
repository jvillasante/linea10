#include "utils.h"
#include "debug_macros.h"
#include <sys/stat.h>

#include <QString>
#include <QProcess>
#include <QFile>
#include <QDateTime>
#include <QDate>
#include <QStringList>

namespace Utils
{
  void limitString(QString &aString, int maxLength)
  {
    if (aString.length() <= maxLength)
      return;

    aString.truncate(maxLength - 4);
    aString.append("...");
  }

  void reboot()
  {
    QProcess rebootProcess;
    DEBUG("rebooting ..................................");
    rebootProcess.start("/sbin/reboot");
    rebootProcess.waitForFinished(-1);
  }

  bool wgetCall(QString &dest, QString &source)
  {
    QString wgetCommand = QString("wget -O %1 %2").arg(dest).arg(source);
    DEBUG("Sending command %s", wgetCommand.toStdString().c_str());

    QProcess wgetProcess;
    wgetProcess.start(wgetCommand);
    wgetProcess.waitForFinished(-1);

    QString wgetStdout = wgetProcess.readAllStandardOutput();
    QString wgetStderr = wgetProcess.readAllStandardError();
    DEBUG("wget stdout: %s", wgetStdout.toStdString().c_str());
    DEBUG("wget stderr: %s", wgetStderr.toStdString().c_str());

    if (wgetStderr.contains(QString("server returned error"))) {
      DEBUG("wget command FAIL");  // TODO - what to do here
      return false;
    }

    DEBUG("wget command OK");
    return true;
  }

  bool verifyFileSize(QString &theFile, int expectedSize)
  {
    int actualSize = 0;
    QFile file(theFile);
    if (file.open(QIODevice::ReadOnly)){
      actualSize = file.size();
      file.close();
    }

    if (actualSize != expectedSize) {
      DEBUG("File %s with problems...", theFile.toStdString().c_str());
      DEBUG("This is most likely because problems with your internet connection");
      DEBUG("Original Size: %d", expectedSize);
      DEBUG("Received Size: %d", actualSize);
      return false;
    }

    DEBUG("%s size OK", theFile.toStdString().c_str());
    return true;
  }

  void removeFile(const QString &theFile)
  {
    QString removeCommand = QString("rm -rf %1").arg(theFile);
    DEBUG("Sending command %s", removeCommand.toStdString().c_str());

    QProcess removeProcess;
    removeProcess.start(removeCommand);
    removeProcess.waitForFinished(-1);

    DEBUG("delete %s OK", theFile.toStdString().c_str());
  }

  bool verifyMd5(const QString &theFileMd5)
  {
    QString md5Command = QString("md5sum -c %1").arg(theFileMd5);
    DEBUG("Sending command %s", md5Command.toStdString().c_str());

    QProcess md5Process;
    md5Process.start(md5Command);
    md5Process.waitForFinished(-1);

    QString md5Stdout = md5Process.readAllStandardOutput();
    QString md5Stderr = md5Process.readAllStandardError();
    DEBUG("md5sum stdout: %s", md5Stdout.toStdString().c_str());
    DEBUG("md5sum stderr: %s", md5Stderr.toStdString().c_str());

    if (md5Stdout.contains(QString("OK"))) {
      DEBUG("md5sum command OK");
      return true;
    }

    DEBUG("md5sum command FAIL");
    return false;
  }
  
  bool copyFile(QString &source, const QString &destDir)
  {
    QString copyCommand = QString("cp -f %1 %2").arg(source).arg(destDir);
    DEBUG("Sending command %s", copyCommand.toStdString().c_str());

    QProcess copyProcess;
    copyProcess.start(copyCommand);
    copyProcess.waitForFinished(-1);

    QString copyStdout = copyProcess.readAllStandardOutput();
    QString copyStderr = copyProcess.readAllStandardError();
    DEBUG("cp stdout: %s", copyStdout.toStdString().c_str());
    DEBUG("cp stderr: %s", copyStderr.toStdString().c_str());

    if (copyStdout.contains(QString("No such file"))) {
      DEBUG("cp command FAIL");
      return false;
    }

    DEBUG("cp command OK");
    return true;
  }

  bool moveFile(QString &source, const QString &destDir)
  {
    QString mvCommand = QString("mv -f %1 %2").arg(source).arg(destDir);
    DEBUG("Sending command %s", mvCommand.toStdString().c_str());

    QProcess mvProcess;
    mvProcess.start(mvCommand);
    mvProcess.waitForFinished(-1);

    QString mvStdout = mvProcess.readAllStandardOutput();
    QString mvStderr = mvProcess.readAllStandardError();
    DEBUG("mv stdout: %s", mvStdout.toStdString().c_str());
    DEBUG("mv stderr: %s", mvStderr.toStdString().c_str());

    if (mvStdout.contains(QString("No such file"))) {
      DEBUG("mv command FAIL");
      return false;
    }

    DEBUG("mv command OK");
    return true;
  }

  bool fileExists(const char *name)
  {
    struct stat buffer;
    return (stat(name, &buffer) == 0);
  }
  
  QString dateFormat(const QString &format)
  {
    QDateTime now = getCurrentTimestamp();
    return now.date().toString(format);
  }
  
  QDateTime getCurrentTimestamp()
  {
    return QDateTime::currentDateTime();
  }
  
  QDateTime getCurrentTimestampUtc()
  {
    return QDateTime::currentDateTimeUtc();
  }
  
  uint getCurrentUnixTimestamp()
  {
    QDateTime now = getCurrentTimestamp();
    return now.toTime_t();
  }
  
  uint getCurrentUnixTimestampUtc()
  {
    QDateTime now = getCurrentTimestampUtc();
    return now.toTime_t();
  }
  
  void getFromUnixTimestamp(QDateTime &dateTime, uint unixTimeStamp)
  {
    dateTime.setTime_t(unixTimeStamp);
  }
  
  void getCurrentDateTimeForSnack(int result[])
  {
    QDateTime now = getCurrentTimestamp();
    QDate currentDate = now.date();
    
    int day = currentDate.dayOfWeek();
    int time = now.toString("hhmm").toInt();
    result[0] = day;
    result[1] = time;
  }
  
  char rutVerifyDigit(unsigned rut)
  {
    unsigned factor, sum, digit;
    unsigned rest, result;
    
    for (factor = 2, sum = 0; rut > 0; factor++) {
      digit = rut % 10;
      rut /= 10;
      sum += digit * factor;

      if (factor >= 7) factor = 1;
    } 

    rest = sum % 11;
    result = 11 - rest;

    DEBUG("result: %d", result);

    if (result < 10) return result + '0';
    else if (result == 10) return 'K';
    else return '0';
  }
}
