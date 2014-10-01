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
	CFG_WSQ_BITRATE = 12
} IENGINE_CONFIG;

typedef enum
{
    MODE_GENERAL = 0,
    MODE_ROLLED = 6
} IENGINE_SCANNER_TYPE;

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
IDKIT_API int IEngine_CopyUser(IENGINE_USER srcUser, IENGINE_USER dstUser, bool withImages);
IDKIT_API int IEngine_ClearUser(IENGINE_USER user);
IDKIT_API int IEngine_FreeUser(IENGINE_USER user);

IDKIT_API int IEngine_AddFingerprint( IENGINE_USER user, IENGINE_FINGER_POSITION fingerPosition, unsigned char * fingerprintImage);
IDKIT_API int IEngine_AddFingerprintFromFile(IENGINE_USER user, IENGINE_FINGER_POSITION fingerPosition, const char * filename);
IDKIT_API int IEngine_SetCustomData( IENGINE_USER user, unsigned char *data, int length );
IDKIT_API int IEngine_SetFingerprint( IENGINE_USER user, int fingerprintIndex, IENGINE_FINGER_POSITION fingerPosition, unsigned char * fingerprintImage);
IDKIT_API int IEngine_SetFingerprintFromFile( IENGINE_USER user, int fingerprintIndex, IENGINE_FINGER_POSITION fingerPosition, const char * filename);
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
IDKIT_API int IEngine_ConvertRawImage2Bmp(unsigned char *rawImage, int width, int height, unsigned char * bmpImage, int *length);


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
#define IENGINE_E_INCONSISTENTSIZE  1134
#define IENGINE_E_BADTEMPLATE       1135
#define IENGINE_E_QUERYSYNTAX       1136
#define IENGINE_E_BADCRYPTKEY       1140
#define IENGINE_E_SSL               1141
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


// === Deprecated Symbols === //

#ifdef _MSC_VER
#pragma deprecated( IEngine_GetVersion )
#pragma deprecated( IEngine_GetUserIDs )
#endif


#ifdef __cplusplus
}
#endif

#endif // INNOVATRICS_IDKIT_H
