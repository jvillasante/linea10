#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QDateTime>

namespace Utils
{
  void limitString(QString &aString, int maxLength);

  void reboot();

  bool verifyFileSize(QString &theFile, int expectedSize);
  bool wgetCall(QString &dest, QString &source);
  void removeFile(const QString &theFile);
  bool verifyMd5(const QString &theFileMd5);
  bool moveFile(QString &source, const QString &destDir);
  
  QString dateFormat(const QString &format);
  QDateTime getCurrentTimestamp();
  QDateTime getCurrentTimestampUtc();
  uint getCurrentUnixTimestamp();
  uint getCurrentUnixTimestampUtc();
  void getFromUnixTimestamp(QDateTime &dateTime, uint unixTimeStamp);

  bool fileExists(const char *name);

  char rutVerifyDigit(unsigned rut);

  bool isNtpRunning();

  void getCurrentDateTimeForSnack(int result[]);
}

#endif // UTILS_H
