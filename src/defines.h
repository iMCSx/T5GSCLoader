#ifndef DEFINES_H
#define DEFINES_H

#include <stdbool.h>

// Macros to automate naming/setting variables for natives/hooks
#define t5nd(ret_type, name, args)    \
    typedef ret_type(*name##_t) args; \
    uint64_t name##_opd;              \
    name##_t name;

#define t5ni(name)                                                                                                    \
    name##_opd = isMultiplayer ? ((uint64_t)T5_##name << 32) | T5_TOC : ((uint64_t)T5_##name##_ZM << 32) | T5_TOC_ZM; \
    name = (name##_t) & name##_opd;

#define t5nhd(ret_type, name, args)   \
    typedef ret_type(*name##_t) args; \
    uint64_t name##_opd;              \
    name##_t name;                    \
    name##_t name##_Trampoline;       \
    ret_type name##_Hook args;

#define t5nhi(name)                                                                                                   \
    name##_opd = isMultiplayer ? ((uint64_t)T5_##name << 32) | T5_TOC : ((uint64_t)T5_##name##_ZM << 32) | T5_TOC_ZM; \
    name = (name##_t) & name##_opd;                                                                                   \
    name##_Trampoline = 0;

#define t5o(name) (isMultiplayer ? T5_##name : T5_##name##_ZM)

#define MAX_GSC_COUNT 100

#define T5INFO "[T5] Info: "
#define T5WARNING "[T5] Warning: "
#define T5ERROR "[T5] ERROR: "

#define SCRIPTS_PATH "/dev_hdd0/tmp/T5GSCLoader"

// Game structs
typedef union DvarValue
{
    bool enabled;
    int integer;
    uint32_t unsignedInt;
    int64_t integer64;
    uint64_t unsignedInt64;
    float value;
    float vector[4];
    const char *string;
    char color[4];
} DvarValue;

typedef enum scriptInstance_t
{
    SCRIPTINSTANCE_SERVER = 0,
    SCRIPTINSTANCE_CLIENT = 1,
    SCRIPT_INSTANCE_MAX = 2
} scriptInstance_t;

typedef enum dvarType_t
{
    DVAR_TYPE_BOOL = 0x0,
    DVAR_TYPE_FLOAT = 0x1,
    DVAR_TYPE_FLOAT_2 = 0x2,
    DVAR_TYPE_FLOAT_3 = 0x3,
    DVAR_TYPE_FLOAT_4 = 0x4,
    DVAR_TYPE_INT = 0x5,
    DVAR_TYPE_ENUM = 0x6,
    DVAR_TYPE_STRING = 0x7,
    DVAR_TYPE_COLOR = 0x8,
    DVAR_TYPE_INT64 = 0x9,
    DVAR_TYPE_LINEAR_COLOR_RGB = 0xA,
    DVAR_TYPE_COLOR_XYZ = 0xB,
    DVAR_TYPE_COUNT = 0xC
} dvarType_t;

typedef struct dvar_s
{
    const char *name;
    const char *description;
    int hash;
    unsigned int flags;
    dvarType_t type;
    bool modified;
    bool loadedFromSaveGame;
    DvarValue current;
    DvarValue latched;
    DvarValue reset;
    DvarValue saved;
    char domain[10];
    struct dvar_s *hashNext;
} dvar_s;

typedef struct scrVarPub_t
{
    char _unsafe[0x38];
    int checksum;
    int entId;
    int entFieldName;
    char *programHunkUser;
    char *programBuffer;
    char *endScriptBuffer;
    char _unsafe2[0x0C];
} scrVarPub_t; // 0x58

typedef struct scrCompilePub_t
{
    char _unsafe[0x20030];
    int programLen;
    char _unsafe2[0x1004];
} scrCompilePub_t; // 0x21038

typedef struct RawFile
{
    char *name;
    int len;
    char *buffer;
} RawFile;

typedef enum XAssetType
{
    ASSET_TYPE_RAWFILE = 0x26
} XAssetType;

typedef union XAssetHeader
{
    struct RawFile *rawFile;
    void *data;
} XAssetHeader;

typedef struct XAsset
{
    enum XAssetType type;
    union XAssetHeader header;
} XAsset;

typedef struct XAssetEntry
{
    XAsset asset;
    char zoneIndex;
    bool inuse;
    uint16_t nextHash;
    uint16_t nextOverride;
    uint16_t usageFrame;
    char margin[0x10];
} XAssetEntry;

typedef union XAssetEntryPoolEntry
{
    struct XAssetEntry entry;
    union XAssetEntryPoolEntry *next;
} XAssetEntryPoolEntry;

// Customs
typedef struct InflateData
{
    char *deflatedBuffer;
    char *hunkMemoryBuffer;
    char _unsafe[0x18];
} InflateData; // 0x20? (unknown structure, ps3 only)

typedef struct opd32
{
    void *function;
    int toc;
} opd32;
typedef struct opd32 *popd32;

typedef struct scrChecksum_t
{
    int checksum;
    int programLen;
    int substract;
} scrChecksum_t; // 0xC (unknown struct not in pdb)

typedef struct RawFileData
{
    char name[100];
    int inflatedSize;
    int size;
    char buffer[0x20];
} RawFileData;

typedef struct GSCLoaderRawfile
{
    XAssetEntry entry;
    RawFile asset;
    RawFileData data;
} GSCLoaderRawfile;

typedef struct GSCLoader
{
    char currentModName[256];
    GSCLoaderRawfile rawFiles[MAX_GSC_COUNT];
} GSCLoader;

#endif