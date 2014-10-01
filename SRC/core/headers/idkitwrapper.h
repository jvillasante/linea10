#ifndef IDKITWRAPPER_H
#define IDKITWRAPPER_H

#include "idkit.h"
#include <QSettings>

class IDKITWrapper
{
  public:
    explicit IDKITWrapper(QSettings *settings);
    ~IDKITWrapper();

    bool init();
    void close();
    bool registerUserFromRawImage(unsigned char *rawImage, int width, int height, char *userIdentifier, char *userName, char *userRut);
    bool registerUserFromTemplate(unsigned char *tpl, char *userIdentifier, char *userName, char *userRut);
    bool matchFromRawImage(unsigned char *rawImage, int width, int height, char *userIdentifier, char *userName, char *userRut);
    int getUserCount();
    bool clearDatabase();

  private:
    bool isOpen;
    QSettings *settings;
};

#endif // IDKITWRAPPER_H
