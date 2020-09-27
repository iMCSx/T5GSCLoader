#ifndef CSHOOK_H
#define CSHOOK_H

/*
    iMCSx's Simple Hook Library
*/

#include "defines.h"

#define TRAMPOLINE_INSTRUCTIONS_COUNT 9
#define PREHOOK_INSTRUCTIONS_COUNT 13
#define MAX_HOOKS_FUNCTIONS 100

#define cs_hook_install(name, type) cs_hook_create((popd32)name, (popd32)name##_Hook, (popd32*)&name##_Trampoline, true, type)

typedef struct cs_hook_trampoline
{
    char opcodes[TRAMPOLINE_INSTRUCTIONS_COUNT * 4];
} cs_hook_trampoline;

typedef struct cs_hook_prehook
{
    char opcodes[PREHOOK_INSTRUCTIONS_COUNT * 4];
} cs_hook_prehook;

typedef enum cs_hook_type
{
    CS_HOOK_TYPE_CTR,
    CS_HOOK_TYPE_IMPORT
} cs_hook_type;

typedef struct cs_hook_info
{
    popd32 source;
    popd32 detour;
    popd32 *tramp;
    opd32 trampoline_opd;
    uint32_t index;
    cs_hook_type type;
    int opd_import[2];
} cs_hook_info;

cs_hook_info *cs_hook_get_info_from_source(popd32 source);
int cs_hook_create(popd32 source, popd32 detour, popd32 *trampoline, bool enable, cs_hook_type type);
int cs_hook_enable(popd32 source);
int cs_hook_disable(popd32 source);

#endif