#ifndef INNOVATRICS_IDKIT_H
#define INNOVATRICS_IDKIT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#define IDKIT_API __declspec( dllimport )
#else
#define IDKIT_API
#endif

#define CRYPT_KEY_LENGTH 32
#define MAX_MINUTIAE_COUNT 256
#define MAX_CRITICAL_POINTS_COUNT 16
#define IPLUGIN_DESCRIPTION_MAX_LEN 256


// === Types definition === //

typedef enum
{
    UNKNOWN_FINGER = 0,
    RIGHT_THUMB = 1,
    RIGHT_INDEX = 2,
    RIGHT_MIDDLE = 3,
    RIGHT_RING = 4,
    RIGHT_LITTLE = 5,
    LEFT_THUMB = 6,
    LEFT_INDEX = 7,
    LEFT_MIDDLE = 8,
    LEFT_RING = 9,
    LEFT_LITTLE = 10
} IENGINE_FINGER_POSITION;

typedef enum
{
    FORMAT_BMP = 0,
    FORMAT_PNG = 1,
    FORMAT_JPG = 2,
    FORMAT_GIF = 3,
    FORMAT_TIF = 4,
    FORMAT_WSQ = 5,
    FORMAT_JPEG2K=6
} IENGINE_IMAGE_FORMAT;

typedef enum
{
    FORMAT_ICS = 1,
    FORMAT_ANSI = 2,
	FORMAT_ISO = 3,
    FORMAT_ANSI_PLUS = 4,
    FORMAT_ISO_PLUS = 5
} IENGINE_TEMPLATE_FORMAT;

typedef enum
{
    CFG_BEST_CANDIDATES_COUNT = 0,
    CFG_SIMILARITY_THRESHOLD = 1,
    CFG_SCANNER_TYPE = 2,
    CFG_RESOLUTION_DPI = 3,
    CFG_MAX_ROTATION = 4,
    CFG_STORE_IMAGES = 5,
    CFG_IDENTIFICATION_SPEED = 6,
	CFG_NETWORK_COMPRESSION = 7,
    CFG_LOG_LEVEL = 8,
    CFG_MIN_MATCHING_FINGERPRINTS = 9,
    CFG_MAX_TEMPLATE_SIZE = 10,
	CFG_JPEG2K_COMPRESSION_RATIO = 11,
	CFG_WSQ_BITRATE = 12,
	CFG_DB_IMAGE_FORMAT = 13,
	CFG_LOAD_IMAGES = 14,
	CFG_MAX_ODBC_LOADING_THREADS = 15,
	CFG_ICS_TEMPLATE_VERSION = 16,
	CFG_EXTRACT_CRITICAL_POINTS = 17
} IENGINE_CONFIG;

typedef enum
{
    MODE_GENERAL = 0,
    MODE_ROLLED = 6
} IENGINE_SCANNER_TYPE;

typedef enum
{
	REGISTRATION_LOCAL = 0,
	REGISTRATION_EVERYWHERE = 1,
	REGISTRATION_SERVER_ONLY = 2
} IENGINE_PLUGIN_LOCATION;

typedef void *IENGINE_USER;
typedef void *IENGINE_COLLECTION;
typedef void *IENGINE_CONNECTION;

typedef struct
{
    unsigned int Major;
    unsigned int Minor;
} IENGINE_VERSION;

typedef struct
{
    unsigned char angle;
    unsigned short x;
    unsigned short y;
    unsigned char type;
} IENGINE_MINUTIAE, *IENGINE_MINUTIAE_PTR;

typedef struct
{
	unsigned char angle;
	unsigned short x;
	unsigned short y;
	unsigned char type;
} IENGINE_CRITICAL_POINT, *IENGINE_CRITICAL_POINT_PTR;

// === Initialization and Termination functions ===//

IDKIT_API int IEngine_InitModule( );
IDKIT_API int IEngine_InitWithLicense( unsigned char *license, int length );
IDKIT_API int IEngine_InitClient( char * connectionString );
IDKIT_API int IEngine_TerminateModule( );


// === Database Connections === //

IDKIT_API IENGINE_CONNECTION IEngine_InitConnection();
IDKIT_API int IEngine_SelectConnection(IENGINE_CONNECTION connection);
IDKIT_API int IEngine_CloseConnection(IENGINE_CONNECTION connection);
IDKIT_API int IEngine_Connect( char * databaseFile);


// === Configuration Functions === //

IDKIT_API int IEngine_SetParameter( IENGINE_CONFIG parameter, int value );
IDKIT_API int IEngine_GetParameter( IENGINE_CONFIG parameter, int *value );
IDKIT_API int IEngine_SetCryptKey(unsigned char cryptKey[CRYPT_KEY_LENGTH]);
IDKIT_API IENGINE_VERSION IEngine_GetVersion();
IDKIT_API int IEngine_GetVersionInfo( IENGINE_VERSION * version );
IDKIT_API const char * IEngine_GetProductString();


// === Database Management Related Functions === //

IDKIT_API int IEngine_ClearDatabase();

IDKIT_API int IEngine_RegisterUser(IENGINE_USER user, int * userID );
IDKIT_API int IEngine_RegisterUserAs(IENGINE_USER user, int userID );
IDKIT_API int IEngine_UpdateUser( IENGINE_USER user, int userID );
IDKIT_API int IEngine_RemoveUser( int userID );

IDKIT_API int IEngine_GetUser( IENGINE_USER user, int userID );
IDKIT_API int IEngine_GetUserCount(int *userCount);
IDKIT_API int IEngine_GetMemoryUsage(int * memoryUsage);
IDKIT_API int IEngine_GetUserLimit(int *userLimit);
IDKIT_API int IEngine_GetUserIDs(int *userIDs,int maxCount);
IDKIT_API int IEngine_GetAllUserIDs(IENGINE_COLLECTION collection);
IDKIT_API int IEngine_GetUserIDsByQuery(IENGINE_COLLECTION collection, const char * query);


// === User Related Functions === //

IDKIT_API IENGINE_USER IEngine_InitUser();
IDKIT_API int IEngine_CopyUser( IENGINE_USER srcUser, IENGINE_USER dstUser, bool withImages);
IDKIT_API int IEngine_ClearUser( IENGINE_USER user);
IDKIT_API int IEngine_FreeUser( IENGINE_USER user);

IDKIT_API int IEngine_AddFingerprint( IENGINE_USER user, IENGINE_FINGER_POSITION fingerPosition, unsigned char * fingerprintImage);
IDKIT_API int IEngine_AddFingerprintEx( IENGINE_USER user, IENGINE_FINGER_POSITION fingerPosition, unsigned char * fingerprintImage, int imgSize);
IDKIT_API int IEngine_AddFingerprintRAW( IENGINE_USER user, IENGINE_FINGER_POSITION position, unsigned char * rawImage, int width, int height);
IDKIT_API int IEngine_AddFingerprintFromFile( IENGINE_USER user, IENGINE_FINGER_POSITION fingerPosition, const char * filename);
IDKIT_API int IEngine_AddFingerprintFromUser( IENGINE_USER user, IENGINE_USER fromUser, int fromIndex, bool withImage );
IDKIT_API int IEngine_SetCustomData( IENGINE_USER user, unsigned char *data, int length );
IDKIT_API int IEngine_SetFingerprint( IENGINE_USER user, int fingerprintIndex, IENGINE_FINGER_POSITION fingerPosition, unsigned char * fingerprintImage);
IDKIT_API int IEngine_SetFingerprintEx( IENGINE_USER user, int fingerprintIndex, IENGINE_FINGER_POSITION fingerPosition, unsigned char * fingerprintImage, int imgSize);
IDKIT_API int IEngine_SetFingerprintRAW( IENGINE_USER user, int fingerprintIndex, IENGINE_FINGER_POSITION fingerPosition, unsigned char * rawImage, int width, int height );
IDKIT_API int IEngine_SetFingerprintFromFile( IENGINE_USER user, int fingerprintIndex, IENGINE_FINGER_POSITION fingerPosition, const char * filename);
IDKIT_API int IEngine_SetFingerprintFromUser( IENGINE_USER user, int index, IENGINE_USER fromUser, int fromIndex, bool withImage );
IDKIT_API int IEngine_SetFingerPosition( IENGINE_USER user, int fingerprintIndex, IENGINE_FINGER_POSITION fingerPosition);
IDKIT_API int IEngine_RemoveFingerprint( IENGINE_USER user, int fingerprintIndex);

IDKIT_API int IEngine_GetCustomData( IENGINE_USER user, unsigned char *data, int *length );
IDKIT_API int IEngine_GetFingerprintImage( IENGINE_USER user, int fingerprintIndex, IENGINE_IMAGE_FORMAT format,unsigned char * fingerprintImage, int * length );
IDKIT_API int IEngine_SaveFingerprintImage( IENGINE_USER user, int fingerprintIndex, IENGINE_IMAGE_FORMAT format,char *filename);
IDKIT_API int IEngine_GetFingerPosition( IENGINE_USER user, int fingerprintIndex, IENGINE_FINGER_POSITION *fingerPosition);
IDKIT_API int IEngine_GetFingerprintQuality(IENGINE_USER user,int fingerprintIndex, int *quality);
IDKIT_API int IEngine_GetFingerprintCount( IENGINE_USER user, int *fingerprintCount);

IDKIT_API int IEngine_SaveMinutiaeImage( IENGINE_USER user, int fingerprintIndex, IENGINE_IMAGE_FORMAT format,char *filename);
IDKIT_API int IEngine_GetMinutiaeImage( IENGINE_USER user, int fingerprintIndex, IENGINE_IMAGE_FORMAT format,unsigned char * minutiaeImage, int * length );
IDKIT_API int IEngine_GetMinutiaePoints( IENGINE_USER user, int fingerprintIndex, int *minutiaeCount, IENGINE_MINUTIAE minutiae[MAX_MINUTIAE_COUNT] );
IDKIT_API int IEngine_GetDeltasAndCores( IENGINE_USER user, int fingerprintIndex, int * criticalPointsCount, IENGINE_CRITICAL_POINT criticalPoints[MAX_CRITICAL_POINTS_COUNT] );


// === User Related Functions : Extended Use === //

IDKIT_API int IEngine_ExportUserTemplate(IENGINE_USER user, IENGINE_TEMPLATE_FORMAT format, unsigned char *templateData, int *length);
IDKIT_API int IEngine_ImportUserTemplate(IENGINE_USER user, IENGINE_TEMPLATE_FORMAT format,unsigned char *templateData);
IDKIT_API int IEngine_AttachFingerprintImage(IENGINE_USER user, int fingerprintIndex, unsigned char * fingerprintImage);
IDKIT_API int IEngine_SerializeUser( IENGINE_USER user, bool serializeImages, unsigned char * buffer, int * length );
IDKIT_API int IEngine_DeserializeUser( IENGINE_USER user, unsigned char * buffer );


// === User Related Functions : Tags === //

IDKIT_API int IEngine_SetStringTag(IENGINE_USER user, const char * name, const char * value);
IDKIT_API int IEngine_GetStringTag(IENGINE_USER user, const char * name, char * value, int * length);
IDKIT_API int IEngine_SetIntTag(IENGINE_USER user, const char * name, int value);
IDKIT_API int IEngine_GetIntTag(IENGINE_USER user, const char * name, int * value);
IDKIT_API int IEngine_ClearTag(IENGINE_USER user, const char * name);
IDKIT_API int IEngine_HasTag(IENGINE_USER user, const char * name, int * present);
IDKIT_API int IEngine_GetTagCount(IENGINE_USER user, int * count);
IDKIT_API int IEngine_GetTagName(IENGINE_USER user, int offset, char * name, int * length);


// === User Collection Functions === //

IDKIT_API IENGINE_COLLECTION IEngine_InitCollection();
IDKIT_API int IEngine_FreeCollection(IENGINE_COLLECTION collection);
IDKIT_API int IEngine_GetCollectionSize(IENGINE_COLLECTION collection, int * count);
IDKIT_API int IEngine_GetCollectionIDs(IENGINE_COLLECTION collection, int * ids, int count);


// === Conversion Functions === //

IDKIT_API int IEngine_ConvertBmp2RawImage(unsigned char *bmpImage, unsigned char * rawImage, int *width, int *height);
IDKIT_API int IEngine_ConvertBmp2RawImageEx(unsigned char *bmpImage, int bmpSize,unsigned char * rawImage, int *width, int *height);
IDKIT_API int IEngine_ConvertRawImage2Bmp(unsigned char *rawImage, int width, int height, unsigned char * bmpImage, int *length);
IDKIT_API int IEngine_ConvertImage(unsigned char *inImage, int inLength, IENGINE_IMAGE_FORMAT format, unsigned char *outImage, int *outLength);


// === Identification Functions === //

IDKIT_API int IEngine_FindUser( IENGINE_USER user, int * userID, int * score );
IDKIT_API int IEngine_FindFingerprint( IENGINE_USER user, int fingerprintIndex,int * userID, int *bestIndex, int * score );
IDKIT_API int IEngine_FindUserInSelection(IENGINE_USER user,int selectionCount,int *selectedUserIDs,int *userID,int *score);
IDKIT_API int IEngine_FindFingerprintInSelection(IENGINE_USER user, int index,int selectionCount,int *selectedUserIDs,int * userID, int *bestIndex, int *score);
IDKIT_API int IEngine_FindUserInMemory(IENGINE_USER user, int recordCount, unsigned char **recordsInMemory, int *userID, int *score);
IDKIT_API int IEngine_FindFingerprintInMemory(IENGINE_USER user, int index, int recordCount, unsigned char **recordsInMemory, int *userID, int *bestIndex, int *score);
IDKIT_API int IEngine_FindUserByQuery(IENGINE_USER user, const char * query, int * uids, int * scores);
IDKIT_API int IEngine_FindFingerprintByQuery(IENGINE_USER user, int index, const char * query, int * uids, int * bestIndices, int * scores);


// === Verification Functions === //

IDKIT_API int IEngine_MatchUser(IENGINE_USER user, int userID,int * score );
IDKIT_API int IEngine_MatchFingerprint(IENGINE_USER user, int fingerprintIndex, int userID,int *bestIndex,int * score );
IDKIT_API int IEngine_MatchUsers(IENGINE_USER user, IENGINE_USER user2,int * score );
IDKIT_API int IEngine_MatchFingerprints(IENGINE_USER user, int fingerprintIndex, IENGINE_USER user2,int fingerprintIndex2,int * score );


// === Error Handling === //

IDKIT_API const char * IEngine_GetErrorMsg( int errcode );
IDKIT_API int IEngine_SetLogFile( const char * filename );

// == NEW == //

IDKIT_API int IEngine_GetHardwareId( char * hwId, int *length);
IDKIT_API int IEngine_GetLicenseInformation( char *hwId, int *hwId_length, char *clientId, int *clientId_length, int *userLimit, int *clientLimit, int *expYear, int *expMonth, int *expDay, char *licFileLocation );

IDKIT_API int IEngine_CharacterizeScore(int score, int fingersCount, int databaseSize, int speedLevel, double *rllr, double *far, double *frr);

// UNDOCUMENTED:
// EXTERNAL PLUGINS:
IDKIT_API int IEngine_RegisterPlugin(const char* pluginPath, const char* pluginName, IENGINE_PLUGIN_LOCATION registerLocation, int * pluginUID);
IDKIT_API int IEngine_UnregisterPlugin(int pluginUID, IENGINE_PLUGIN_LOCATION unregisterLocation);
IDKIT_API int IEngine_GetPluginInfo(int pluginUID, unsigned char* majorVersion, unsigned char* minorVersion, int* reservedInfo, int* implementedFunctions, char* description);
// functional only if plugin is registered locally
IDKIT_API int IEngine_ExtractPluginTemplate(int pluginUID, const char* inputData, int length, char* templateData, int* tLength, int reserved);

IDKIT_API int IEngine_AddPluginTemplate( IENGINE_USER user, int pluginUID, unsigned char templateType, unsigned char bioPosition, unsigned char templateQuality, unsigned char extractorMajorVersion, unsigned char extractorMinorVersion, unsigned char * templateData, int length );
IDKIT_API int IEngine_SetPluginTemplate( IENGINE_USER user, int templateIndex, int pluginUID, unsigned char templateType, unsigned char bioPosition, unsigned char templateQuality, unsigned char extractorMajorVersion, unsigned char extractorMinorVersion, unsigned char * templateData, int length );
IDKIT_API int IEngine_GetPluginTemplate( IENGINE_USER user, int templateIndex, int * pluginUID, unsigned char * templateType, unsigned char * bioPosition, unsigned char * templateQuality, unsigned char * extractorMajorVersion, unsigned char* extractorMinorVersion, unsigned char* templateData, int * length );
IDKIT_API int IEngine_RemovePluginTemplate( IENGINE_USER user, int templateIndex );
IDKIT_API int IEngine_GetPluginTemplateCount( IENGINE_USER user, int* templateCount );

IDKIT_API int IEngine_GetFingerprintClass(unsigned char* fingerprintImage, int length, int *fingerprintClass);

// === Error Messages === //

#define IENGINE_E_NOERROR           0
#define IENGINE_E_BADPARAM          1101
#define IENGINE_E_NOFINGERPRINT     1102
#define IENGINE_E_DBOPEN            1111
#define IENGINE_E_DBFAILED          1112
#define IENGINE_E_DBACCESSDENIED	1113
#define IENGINE_E_BLANKIMAGE        1114
#define IENGINE_E_BADIMAGE          1115
#define IENGINE_E_INIT              1116
#define IENGINE_E_FILE              1117
#define IENGINE_E_BADUSER           1118
#define IENGINE_E_BADINDEX          1119
#define IENGINE_E_MEMORY            1120
#define IENGINE_E_NULLPARAM         1121
#define IENGINE_E_OTHER             1122
#define IENGINE_E_NOIMAGE           1123
#define IENGINE_E_INTERNAL          1124
#define IENGINE_E_NONEXISTINGID     1125
#define IENGINE_E_DUPLICATEID       1126
#define IENGINE_E_BADUSERID         1127
#define IENGINE_E_DBFULL            1128
#define IENGINE_E_BADLICENSE        1129
#define IENGINE_E_EXPIREDLICENSE    1130
#define IENGINE_E_MISSINGDLL        1131
#define IENGINE_E_BADFORMAT         1132
#define IENGINE_E_BADVALUE          1133
#define IENGINE_E_INCONSISTENTSIZE  1134 // Deprecated
#define IENGINE_E_BADTEMPLATE       1135
#define IENGINE_E_QUERYSYNTAX       1136
#define IENGINE_E_BADCRYPTKEY       1140
#define IENGINE_E_SSL               1141
#define IENGINE_E_USERFULL			1142
#define IENGINE_E_DBMISSINGTABLE	1143
#define IENGINE_E_DBBADVERSION		1144
#define IENGINE_E_SRVFAILED         1201
#define IENGINE_E_CONSTR            1202
#define IENGINE_E_CONTYPE           1203
#define IENGINE_E_NOTCONNECTED      1204
#define IENGINE_E_MAXCLIENTS        1205
#define IENGINE_E_NONODE            1206 // Deprecated
#define IENGINE_E_NODEDISCONNECTED  1207 // Deprecated
#define IENGINE_E_TEMPORARY         1208
#define IENGINE_E_PROTOCOL          1209
#define IENGINE_E_PASSWORD          1210
#define IENGINE_E_NOTSUPPORTED      1211

#define IENGINE_E_PLUGIN_CANNOT_LOAD		1300
#define IENGINE_E_PLUGIN_UNKNOWN		1301
#define IENGINE_E_PLUGIN_NOT_FOUND		1302
#define IENGINE_E_PLUGIN_DUPLICATEID		1303
#define IENGINE_E_PLUGIN_UID_INCONSISTENCY	1304

// === Deprecated Symbols === //

#ifdef _MSC_VER
#pragma deprecated( IEngine_GetVersion )
#pragma deprecated( IEngine_GetUserIDs )
#endif


#ifdef __cplusplus
}
#endif

#endif // INNOVATRICS_IDKIT_H
