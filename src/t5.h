#ifndef T5_H
#define T5_H

#include <stdarg.h>
#include <stdbool.h>
#include <types.h>

#include <cell/spurs/lfqueue.h>

#include "defines.h"

bool isMultiplayer;
char scriptPath[255];
int modHandle;

GSCLoader loader;
scrChecksum_t checksums[3];
scrVarPub_t *scrVarPub;
scrCompilePub_t *scrCompilePub;

t5nd(XAssetHeader *, DB_FindXAssetHeader, (XAssetHeader *header, XAssetType type, const char *name, bool errorIfMissing, int waitTime));
t5nd(XAssetEntryPoolEntry*, DB_LinkXAssetEntry, (XAssetEntry *newEntry, int allowOverride));
t5nd(dvar_s*, Dvar_FindVar, (const char *name));
t5nd(int, Scr_GetFunctionHandle, (scriptInstance_t inst, const char *scriptName, const char *functionName));
t5nd(unsigned short, Scr_ExecThread, (scriptInstance_t inst, int handle, int paramcount));
t5nd(void, Scr_FreeThread, (unsigned short handle, scriptInstance_t inst));
t5nd(int, Scr_GetNumParam, (int scriptInstance));
t5nd(char*, Scr_GetString, (unsigned int index, scriptInstance_t scriptInstance));

t5nhd(int, cellSpursLFQueuePushBody, (CellSpursLFQueue *lfqueue, const void *buffer, unsigned int isBlocking));
t5nhd(int, Scr_LoadScript, (scriptInstance_t inst, const char *scriptName));
t5nhd(void, Scr_GetChecksum, (scrChecksum_t *vmChecksum, scriptInstance_t inst));
t5nhd(void, Scr_LoadGameType, (void));
t5nhd(popd32, Scr_GetFunction, (const char **pName, int *type));

GSCLoaderRawfile *get_loader_rawfile_from_deflated_buffer(char *inflatedBuffer);
void get_or_create_mod_path(char *path);
bool create_assets_from_scripts(char *path);
int init_game();

#endif