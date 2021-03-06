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

#if defined(TEMPO) || defined(PRESENCIA)
    bool registerUserFromTemplate(unsigned char *tpl, char *userIdentifier, char *userName, char *userRut,
        char *userEmp);
    bool matchFromRawImage(unsigned char *rawImage, int width, int height, char *userIdentifier, char *userName,
        char *userRut, char *userEmp);
#endif
#ifdef SNACK
    bool registerUserFromTemplateSnack(unsigned char *tpl, int userId, char *userIdentifier, char *userName,
        char *userRut, char *userEmp, int userRepeticion, char *userCentroCosto);
    bool matchFromRawImageSnack(unsigned char *rawImage, int width, int height, int *userId, char *userIdentifier,
        char *userName, char *userRut, char *userEmp, int *repeticion, char *userCentroCosto);
#endif

    int getUserCount();
    bool clearDatabase();

  private:
    bool isOpen;
    QSettings *settings;
};

#endif // IDKITWRAPPER_H
