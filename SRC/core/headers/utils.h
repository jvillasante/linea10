#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QDateTime>

namespace Utils
{
  void limitString(QString &aString, int maxLength);
  QString dateFormat(const QString &format);

  void reboot();

  bool verifyFileSize(QString &theFile, int expectedSize);
  bool wgetCall(QString &dest, QString &source);
  void removeFile(const QString &theFile);
  bool verifyMd5(const QString &theFileMd5);
  bool moveFile(QString &source, const QString &destDir);
  uint getCurrentUnixTimestamp();
  void getFromUnixTimestamp(QDateTime &dateTime, uint unixTimeStamp);

  bool fileExists(const char *name);

  char rutVerifyDigit(unsigned rut);
}

#endif // UTILS_H
