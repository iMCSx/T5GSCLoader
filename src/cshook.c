#include "cshook.h"
#include "utils.h"
#include "inttypes.h"

#include <sys/process.h>
#include <stdio.h>
#include <cstring>

#define CS_HOOK_CREATE_ERROR(num, err) (0xC5000000 | (num << 16) | (err & 0x0000FFFF));

uint32_t hooksInfoCount = 0;
cs_hook_info hooksInfo[MAX_HOOKS_FUNCTIONS];
__attribute__((section(".text#"))) cs_hook_prehook prehooks[MAX_HOOKS_FUNCTIONS];
__attribute__((section(".text#"))) cs_hook_trampoline trampolines[MAX_HOOKS_FUNCTIONS];

int cs_hook_process_write(uint64_t address, const void *data, size_t size)
{
    int res = sys_dbg_process_write(address, data, size);

    if (res == ENOSYS || res != CELL_OK)
    {
        /*
            // Put here a different methods to write to process in case of CEX console.
            // memcpy is not the best way since the function will crash if your firmware hasn't page restrictions patched (some cobra fw should support it).
            // I keep commented this since i have a dex console, free to you to make your own.
            
            memcpy((void*)(uintptr_t)address, data, size);
        */
        return -1;
    }
    return CELL_OK;
}

int cs_hook_create_prehook(cs_hook_info *hookInfo)
{
    if (!hookInfo)
        return -1;
    if (hookInfo->type != CS_HOOK_TYPE_CTR)
        return -2;

    int op[PREHOOK_INSTRUCTIONS_COUNT];
    op[0] = 0x7C0802A6;
    op[1] = 0xF8010020;
    op[2] = 0xF8410028;
    op[3] = 0x3D800000 + (((int)hookInfo->detour >> 16) & 0xFFFF);
    op[4] = 0x618C0000 + ((int)hookInfo->detour & 0xFFFF);
    op[5] = 0x804C0004;
    op[6] = 0x818C0000;
    op[7] = 0x7D8903A6;
    op[8] = 0x4E800421;
    op[9] = 0xE8410028;
    op[10] = 0xE8010020;
    op[11] = 0x7C0803A6;
    op[12] = 0x4E800020;

    if (cs_hook_process_write((uintptr_t)&prehooks[hookInfo->index], &op, PREHOOK_INSTRUCTIONS_COUNT * 4) != CELL_OK)
        return -3;

    return 0;
}

int cs_hook_create_trampoline(cs_hook_info *hookInfo)
{
    if (!hookInfo)
        return -1;
    if (hookInfo->type != CS_HOOK_TYPE_CTR)
        return -2;

    int op[TRAMPOLINE_INSTRUCTIONS_COUNT];
    op[0] = 0x39800000;
    op[1] = 0x658C0000 + ((((int)hookInfo->source->function + 16) >> 16) & 0xFFFF);
    op[2] = 0x618C0000 + (((int)hookInfo->source->function + 16) & 0xFFFF);
    op[3] = 0x7D8903A6;
    op[4] = *(int *)(((int)hookInfo->source->function) + 0);
    op[5] = *(int *)(((int)hookInfo->source->function) + 4);
    op[6] = *(int *)(((int)hookInfo->source->function) + 8);
    op[7] = *(int *)(((int)hookInfo->source->function) + 12);
    op[8] = 0x4E800420;

    hookInfo->trampoline_opd.function = (void *)&trampolines[hookInfo->index];
    hookInfo->trampoline_opd.toc = hookInfo->source->toc;
    *hookInfo->tramp = &hookInfo->trampoline_opd;

    if (cs_hook_process_write((uintptr_t)hookInfo->trampoline_opd.function, &op, TRAMPOLINE_INSTRUCTIONS_COUNT * 4) != CELL_OK)
        return -3;

    return 0;
}

int cs_hook_resolve_import_opd(cs_hook_info *hookInfo)
{
    if (!hookInfo)
        return -1;
    if (hookInfo->type != CS_HOOK_TYPE_IMPORT)
        return -2;

    hookInfo->opd_import[0] = *(int *)(((int)hookInfo->source->function) + 4);
    hookInfo->opd_import[1] = *(int *)(((int)hookInfo->source->function) + 8);

    opd32 *import_opd = *(opd32 **)(hookInfo->opd_import[0] << 16 | hookInfo->opd_import[1] & 0xFFFF);
    hookInfo->trampoline_opd.function = import_opd->function;
    hookInfo->trampoline_opd.toc = import_opd->toc;

    *hookInfo->tramp = &hookInfo->trampoline_opd;
    return 0;
}

cs_hook_info *cs_hook_get_info_from_source(popd32 source)
{
    for (int i = 0; i < MAX_HOOKS_FUNCTIONS; i++)
        if (hooksInfo[i].source == source)
            return &hooksInfo[i];
            
    return 0;
}

int cs_hook_enable(popd32 source)
{
    cs_hook_info *hookInfo = cs_hook_get_info_from_source(source);
    if (hookInfo == 0)
        return -1;

    if (hookInfo->type == CS_HOOK_TYPE_CTR)
    {
        int op[4];
        op[0] = 0x3D800000 + (((int)&prehooks[hookInfo->index] >> 16) & 0xFFFF);
        op[1] = 0x618C0000 + ((int)&prehooks[hookInfo->index] & 0xFFFF);
        op[2] = 0x7D8903A6;
        op[3] = 0x4E800420;
        if (cs_hook_process_write((uintptr_t)hookInfo->source->function, &op, 4 * 4) != CELL_OK)
            return -2;
    }
    else if (hookInfo->type == CS_HOOK_TYPE_IMPORT)
    {
        int op[2];
        op[0] = 0x658C0000 + (((int)&hookInfo->detour >> 16) & 0xFFFF);
        op[1] = 0x818C0000 + ((int)&hookInfo->detour & 0xFFFF);
        if (cs_hook_process_write((uintptr_t)hookInfo->source->function + 4, &op, 4 * 2) != CELL_OK)
            return -2;
    }
    return 0;
}

int cs_hook_disable(popd32 source)
{
    cs_hook_info *hookInfo = cs_hook_get_info_from_source(source);
    if (hookInfo == 0)
        return -1;

    if (hookInfo->type == CS_HOOK_TYPE_CTR)
    {
        int op[4];
        op[0] = *(int *)(&trampolines[hookInfo->index] + 4 * 4);
        op[1] = *(int *)(&trampolines[hookInfo->index] + 5 * 4);
        op[2] = *(int *)(&trampolines[hookInfo->index] + 6 * 4);
        op[3] = *(int *)(&trampolines[hookInfo->index] + 7 * 4);
        if (cs_hook_process_write((uintptr_t)hookInfo->source->function, &op, 4 * 4) != CELL_OK)
            return -2;
    }
    else if (hookInfo->type == CS_HOOK_TYPE_IMPORT)
    {
        if (cs_hook_process_write((uintptr_t)hookInfo->source->function + 4, &hookInfo->opd_import, 4 * 2) != CELL_OK)
            return -2;
    }
    return 0;
}

int cs_hook_create(popd32 source, popd32 detour, popd32 *trampoline, bool enable, cs_hook_type type)
{
    int err = 0;
    if (cs_hook_get_info_from_source(source) != 0)
    {
        return 1;
    }

    cs_hook_info *hookInfo = &hooksInfo[hooksInfoCount];

    hookInfo->source = source;
    hookInfo->detour = detour;
    hookInfo->tramp = trampoline;
    hookInfo->index = hooksInfoCount;
    hookInfo->type = type;
    hooksInfoCount++;

    if (type == CS_HOOK_TYPE_CTR)
    {
        if ((err = cs_hook_create_prehook(hookInfo) < 0))
        {
            return CS_HOOK_CREATE_ERROR(1, err);
        }
        if ((err = cs_hook_create_trampoline(hookInfo) < 0))
        {
            return CS_HOOK_CREATE_ERROR(2, err);
        }
    }
    else if (type == CS_HOOK_TYPE_IMPORT)
    {
        if ((err = cs_hook_resolve_import_opd(hookInfo) < 0))
        {
            return CS_HOOK_CREATE_ERROR(3, err);
        }
    }

    if (enable)
    {
        if (cs_hook_enable(hookInfo->source) < 0)
        {
            return CS_HOOK_CREATE_ERROR(4, err);
        }
    }
    return 0;
}