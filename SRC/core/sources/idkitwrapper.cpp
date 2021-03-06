#include "idkitwrapper.h"

#include "debug_macros.h"
#include "cstring"

#ifndef NDEBUG
#include <time.h>
#endif

static unsigned char license[84] = {
  0x49, 0x43, 0x5f, 0x4c, 0x02, 0x00, 0x44, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x49, 0x30, 0x30, 0x35, 0x32, 0x33, 0x36, 0x32, 0x31, 0x34,
  0x32, 0x31, 0x37, 0x33, 0x38, 0x30, 0x00, 0x00, 0x00, 0x00, 0x49, 0x44,
  0x45, 0x4e, 0x54, 0x59, 0x54, 0x45, 0x43, 0x48, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x50, 0xc3, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2b, 0xeb, 0xc8, 0x95,
  0xfd, 0x8f, 0xac, 0x66, 0xce, 0xe3, 0x6e, 0x88, 0x4a, 0x52, 0x3a, 0x9a
};

IDKITWrapper::IDKITWrapper(QSettings *settings)
{
  this->settings = settings;
  isOpen = false;
}

IDKITWrapper::~IDKITWrapper()
{
  if (isOpen) {
    IEngine_TerminateModule();
    isOpen = false;
    LOG_INFO("IDKIT is now closed");
  }
}

void IDKITWrapper::close()
{
  if (isOpen) {
    IEngine_TerminateModule();
    isOpen = false;
    LOG_INFO("IDKIT is now closed");
  }
}

bool IDKITWrapper::init()
{
  int rc;
  int userLimit;

  CHECK(isOpen == false, "IDKIT already oppened.");

  rc = IEngine_InitWithLicense(license, 84);
  CHECK_IDKIT(rc, "IEngine_InitWithLicense");

  rc = IEngine_SetParameter(CFG_STORE_IMAGES, 0);
  CHECK_IDKIT(rc, "IEngine_SetParameter");

  rc = IEngine_SetParameter(CFG_SIMILARITY_THRESHOLD, 12300);
  CHECK_IDKIT(rc, "IEngine_SetParameter");

  rc = IEngine_SetParameter(CFG_IDENTIFICATION_SPEED, 7);
  CHECK_IDKIT(rc, "IEngine_SetParameter");

  rc = IEngine_SetParameter(CFG_MAX_ROTATION, 50);
  CHECK_IDKIT(rc, "IEngine_SetParameter");

  // Prints out maximum number of users in the database This limit is defined in your license
  userLimit = 0;
  rc = IEngine_GetUserLimit(&userLimit);
  CHECK_IDKIT(rc, "IEngine_GetUserLimit");
  DEBUG("Maximum permitted number of users in the database: %d.", userLimit);

  // Opens iengine.db flat file database. If iengine.db does not exist, an empty database is created from scratch
  rc = IEngine_Connect((char *) settings->value("iengineDB", "Resources/iengine.db").toString().toStdString().c_str());
  CHECK_IDKIT(rc, "IEngine_Connect");
  DEBUG("Database file %s selected.", settings->value("iengineDB", "NO DATABASE SELECTED FROM CONFIGURATIONS!").toString().toStdString().c_str());

#ifndef NDEBUG
  int memoryUsage;
  rc = IEngine_GetMemoryUsage(&memoryUsage);
  CHECK_IDKIT(rc, "IEngine_GetMemoryUsage");
  DEBUG("IDKIT Memory Usage: %d", memoryUsage);
#endif

  isOpen = true;
  LOG_INFO("IDKITWrapper::init OK");
  return true;

error:
  isOpen = false;
  LOG_ERROR("IDKITWrapper::init FAIL");
  return false;
}

#if defined(TEMPO) || defined(PRESENCIA)
bool IDKITWrapper::registerUserFromTemplate(unsigned char *tpl, char *userIdentifier, char *userName,
    char *userRut, char *userEmp)
{
  int rc;
  IENGINE_USER user;

  // Initializing user structure
  user = IEngine_InitUser();
  if (user == NULL) {
    LOG_ERROR("IEngine_InitUser memory error.");
    goto error;
  }

  {
    rc = IEngine_ImportUserTemplate(user, FORMAT_ICS, tpl);
    CHECK_IDKIT(rc, "IEngine_ImportUserTemplate");

    rc = IEngine_SetStringTag(user, "identifier", userIdentifier);
    CHECK_IDKIT(rc, "IEngine_SetStringTag (identifier)");

    rc = IEngine_SetStringTag(user, "name", userName);
    CHECK_IDKIT(rc, "IEngine_SetStringTag (name)");

    rc = IEngine_SetStringTag(user, "rut", userRut);
    CHECK_IDKIT(rc, "IEngine_SetStringTag (rut)");

    rc = IEngine_SetStringTag(user, "empresa", userEmp);
    CHECK_IDKIT(rc, "IEngine_SetStringTag (empresa)");

    // Register user
    int userId;
    rc = IEngine_RegisterUser(user, &userId);
    CHECK_IDKIT(rc, "IEngine_RegisterUser");
    DEBUG("User %d: %s registered", userId, userName);

    // Free user
    rc = IEngine_FreeUser(user);
    CHECK_IDKIT(rc, "IEngine_FreeUser");

    DEBUG("IDKITWrapper::registerUserFromTemplate OK");
    return true;
  }

error:
  if (user) {
    IEngine_ClearTag(user, "identifier");
    IEngine_ClearTag(user, "name");
    IEngine_ClearTag(user, "rut");
    IEngine_ClearTag(user, "empresa");
    IEngine_FreeUser(user);
  }
  LOG_ERROR("IDKITWrapper::registerUserFromTemplate FAIL");
  return false;
}
#endif

#ifdef SNACK
bool IDKITWrapper::registerUserFromTemplateSnack(unsigned char *tpl, int userId, char *userIdentifier, char *userName,
    char *userRut, char *userEmp, int userRepeticion, char *userCentroCosto)
{
  int rc;
  IENGINE_USER user;

  // Initializing user structure
  user = IEngine_InitUser();
  if (user == NULL) {
    LOG_ERROR("IEngine_InitUser memory error.");
    goto error;
  }

  {
    rc = IEngine_ImportUserTemplate(user, FORMAT_ICS, tpl);
    CHECK_IDKIT(rc, "IEngine_ImportUserTemplate");

    rc = IEngine_SetStringTag(user, "identifier", userIdentifier);
    CHECK_IDKIT(rc, "IEngine_SetStringTag (identifier)");

    rc = IEngine_SetStringTag(user, "name", userName);
    CHECK_IDKIT(rc, "IEngine_SetStringTag (name)");

    rc = IEngine_SetStringTag(user, "rut", userRut);
    CHECK_IDKIT(rc, "IEngine_SetStringTag (rut)");

    rc = IEngine_SetStringTag(user, "empresa", userEmp);
    CHECK_IDKIT(rc, "IEngine_SetStringTag (empresa)");

    rc = IEngine_SetIntTag(user, "repeticion", userRepeticion);
    CHECK_IDKIT(rc, "IEngine_SetStringTag (repeticion)");

    rc = IEngine_SetStringTag(user, "centro_costo", userCentroCosto);
    CHECK_IDKIT(rc, "IEngine_SetStringTag (centro_costo)");

    // Register user
    rc = IEngine_RegisterUserAs(user, userId);
    CHECK_IDKIT(rc, "IEngine_RegisterUserAs");
    DEBUG("User %d: %s registered", userId, userName);

    // Free user
    rc = IEngine_FreeUser(user);
    CHECK_IDKIT(rc, "IEngine_FreeUser");

    DEBUG("IDKITWrapper::registerUserFromTemplate OK");
    return true;
  }

error:
  if (user) {
    IEngine_ClearTag(user, "identifier");
    IEngine_ClearTag(user, "name");
    IEngine_ClearTag(user, "rut");
    IEngine_ClearTag(user, "empresa");
    IEngine_ClearTag(user, "repeticion");
    IEngine_ClearTag(user, "centro_costo");
    IEngine_FreeUser(user);
  }
  LOG_ERROR("IDKITWrapper::registerUserFromTemplate FAIL");
  return false;
}
#endif

#if defined(TEMPO) || defined(PRESENCIA)
bool IDKITWrapper::matchFromRawImage(unsigned char *rawImage, int width, int height, char *userIdentifier,
    char *userName, char *userRut, char *userEmp)
{
  int rc, bmpLength, userID, score;
  bool result = false;
  IENGINE_USER user;

  // Initializing user structure
  user = IEngine_InitUser();
  if (user == NULL) {
    LOG_ERROR("IEngine_InitUser memory error.");
    goto error;
  }

  rc = IEngine_ConvertRawImage2Bmp(rawImage, width, height, NULL, &bmpLength);
  CHECK_IDKIT(rc, "IEngine_ConvertRawImage2Bmp (First Call)");

  { // get rid of lint error by creating a new scope
    unsigned char bmpImage[bmpLength];
    memset(&bmpImage[0], 0x00, bmpLength);
    rc = IEngine_ConvertRawImage2Bmp(rawImage, width, height, &bmpImage[0], &bmpLength);
    CHECK_IDKIT(rc, "IEngine_ConvertRawImage2Bmp (Second Call)");

    // Add unknown fingerprint to user structure.
    // The index of unknown fingerprint in user structure will be 0, as it is
    // the first fingerprint added to this structure
    rc = IEngine_AddFingerprint(user, UNKNOWN_FINGER, bmpImage);
    CHECK_IDKIT(rc, "IEngine_AddFingerprint");

    rc = IEngine_FindUser(user, &userID, &score);
    CHECK_IDKIT(rc, "IEngine_FindUser");

    if (userID > 0)  {
      DEBUG("User already registered in the database with registration ID: %d.", userID);
      DEBUG("(Detected similarity score: %d)", score);

      // Load user data into user
      rc = IEngine_GetUser(user, userID);
      CHECK_IDKIT(rc, "IEngine_GetUser");

      int tagLength = 0;
      rc = IEngine_GetStringTag(user, "identifier", NULL, &tagLength);
      CHECK_IDKIT(rc, "IEngine_GetStringTag (identifier - First Call)");
      char identifier[tagLength];
      rc = IEngine_GetStringTag(user, "identifier", &identifier[0], &tagLength);
      CHECK_IDKIT(rc, "IEngine_GetStringTag (identifier - Second Call)");

      tagLength = 0;
      rc = IEngine_GetStringTag(user, "name", NULL, &tagLength);
      CHECK_IDKIT(rc, "IEngine_GetStringTag (name - First Call)");
      char name[tagLength];
      rc = IEngine_GetStringTag(user, "name", &name[0], &tagLength);
      CHECK_IDKIT(rc, "IEngine_GetStringTag (name - Second Call)");

      tagLength = 0;
      rc = IEngine_GetStringTag(user, "rut", NULL, &tagLength);
      CHECK_IDKIT(rc, "IEngine_GetStringTag (rut - First Call)");
      char rut[tagLength];
      rc = IEngine_GetStringTag(user, "rut", &rut[0], &tagLength);
      CHECK_IDKIT(rc, "IEngine_GetStringTag (rut - Second Call)");

      tagLength = 0;
      rc = IEngine_GetStringTag(user, "empresa", NULL, &tagLength);
      CHECK_IDKIT(rc, "IEngine_GetStringTag (empresa - First Call)");
      char empresa[tagLength];
      rc = IEngine_GetStringTag(user, "empresa", &empresa[0], &tagLength);
      CHECK_IDKIT(rc, "IEngine_GetStringTag (empresa - Second Call)");

      strcpy(userIdentifier, identifier);
      strcpy(userName, name);
      strcpy(userRut, rut);
      strcpy(userEmp, empresa);

      LOG_INFO("User found in the database.");
      result = true;
    } else {
      // if no user with the same fingerprint is present in the database,
      // userID (and score) is set to 0 by IEngine_FindUser function
      LOG_INFO("User not found in the database.");
      result = false;
    }
  }

  rc = IEngine_FreeUser(user);
  CHECK_IDKIT(rc, "IEngine_FreeUser");

  LOG_INFO("IDKITWrapper::matchFromRawImage OK");
  return result;

error:
  if (user) { IEngine_FreeUser(user); }

  LOG_ERROR("IDKITWrapper::matchFromRawImage FAIL");
  return false;
}
#endif

#ifdef SNACK
bool IDKITWrapper::matchFromRawImageSnack(unsigned char *rawImage, int width, int height, int *userId,
    char *userIdentifier, char *userName, char *userRut, char *userEmp, int *repeticion, char *userCentroCosto)
{
  int rc, bmpLength, userID, score;
  bool result = false;
  IENGINE_USER user;

  // Initializing user structure
  user = IEngine_InitUser();
  if (user == NULL) {
    LOG_ERROR("IEngine_InitUser memory error.");
    goto error;
  }

  rc = IEngine_ConvertRawImage2Bmp(rawImage, width, height, NULL, &bmpLength);
  CHECK_IDKIT(rc, "IEngine_ConvertRawImage2Bmp (First Call)");

  { // get rid of lint error by creating a new scope
    unsigned char bmpImage[bmpLength];
    memset(&bmpImage[0], 0x00, bmpLength);
    rc = IEngine_ConvertRawImage2Bmp(rawImage, width, height, &bmpImage[0], &bmpLength);
    CHECK_IDKIT(rc, "IEngine_ConvertRawImage2Bmp (Second Call)");

    // Add unknown fingerprint to user structure.
    // The index of unknown fingerprint in user structure will be 0, as it is
    // the first fingerprint added to this structure
    rc = IEngine_AddFingerprint(user, UNKNOWN_FINGER, bmpImage);
    CHECK_IDKIT(rc, "IEngine_AddFingerprint");

    rc = IEngine_FindUser(user, &userID, &score);
    CHECK_IDKIT(rc, "IEngine_FindUser");

    if (userID > 0)  {
      DEBUG("User already registered in the database with registration ID: %d.", userID);
      DEBUG("(Detected similarity score: %d)", score);

      // Load user data into user
      rc = IEngine_GetUser(user, userID);
      CHECK_IDKIT(rc, "IEngine_GetUser");

      int tagLength = 0;
      rc = IEngine_GetStringTag(user, "identifier", NULL, &tagLength);
      CHECK_IDKIT(rc, "IEngine_GetStringTag (identifier - First Call)");
      char identifier[tagLength];
      rc = IEngine_GetStringTag(user, "identifier", &identifier[0], &tagLength);
      CHECK_IDKIT(rc, "IEngine_GetStringTag (identifier - Second Call)");

      tagLength = 0;
      rc = IEngine_GetStringTag(user, "name", NULL, &tagLength);
      CHECK_IDKIT(rc, "IEngine_GetStringTag (name - First Call)");
      char name[tagLength];
      rc = IEngine_GetStringTag(user, "name", &name[0], &tagLength);
      CHECK_IDKIT(rc, "IEngine_GetStringTag (name - Second Call)");

      tagLength = 0;
      rc = IEngine_GetStringTag(user, "rut", NULL, &tagLength);
      CHECK_IDKIT(rc, "IEngine_GetStringTag (rut - First Call)");
      char rut[tagLength];
      rc = IEngine_GetStringTag(user, "rut", &rut[0], &tagLength);
      CHECK_IDKIT(rc, "IEngine_GetStringTag (rut - Second Call)");

      tagLength = 0;
      rc = IEngine_GetStringTag(user, "empresa", NULL, &tagLength);
      CHECK_IDKIT(rc, "IEngine_GetStringTag (empresa - First Call)");
      char empresa[tagLength];
      rc = IEngine_GetStringTag(user, "empresa", &empresa[0], &tagLength);
      CHECK_IDKIT(rc, "IEngine_GetStringTag (empresa - Second Call)");

      rc = IEngine_GetIntTag(user, "repeticion", repeticion);
      CHECK_IDKIT(rc, "IEngine_GetIntTag (repeticion)");

      tagLength = 0;
      rc = IEngine_GetStringTag(user, "centro_costo", NULL, &tagLength);
      CHECK_IDKIT(rc, "IEngine_GetStringTag (centro_costo - First Call)");
      char centroCosto[tagLength];
      rc = IEngine_GetStringTag(user, "centro_costo", &centroCosto[0], &tagLength);
      CHECK_IDKIT(rc, "IEngine_GetStringTag (centro_costo - Second Call)");

      strcpy(userIdentifier, identifier);
      strcpy(userName, name);
      strcpy(userRut, rut);
      strcpy(userEmp, empresa);
      *userId = userID;
      strcpy(userCentroCosto, centroCosto);

      LOG_INFO("User found in the database.");
      result = true;
    } else {
      // if no user with the same fingerprint is present in the database,
      // userID (and score) is set to 0 by IEngine_FindUser function
      LOG_INFO("User not found in the database.");
      result = false;
    }
  }

  rc = IEngine_FreeUser(user);
  CHECK_IDKIT(rc, "IEngine_FreeUser");

  LOG_INFO("IDKITWrapper::matchFromRawImage OK");
  return result;

error:
  if (user) { IEngine_FreeUser(user); }

  LOG_ERROR("IDKITWrapper::matchFromRawImage FAIL");
  return false;
}
#endif

int IDKITWrapper::getUserCount()
{
  int userCount = 0;
  int rc;

  rc = IEngine_GetUserCount(&userCount);
  CHECK_IDKIT(rc, "IEngine_GetUserCount");

  LOG_INFO("IDKITWrapper::getUserCount OK");
  return userCount;

error:
  LOG_ERROR("IDKITWrapper::getUserCount FAIL");
  return -1;
}

bool IDKITWrapper::clearDatabase()
{
  int rc;

  rc = IEngine_ClearDatabase();
  CHECK_IDKIT(rc, "IEngine_ClearDatabase");

  LOG_INFO("IDKITWrapper::clearDatabase OK");
  return true;

error:
  LOG_ERROR("IDKITWrapper::clearDatabase FAIL");
  return false;
}
