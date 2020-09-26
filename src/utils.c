#include "utils.h"

#include <sys/process.h>
#include <sys/tty.h>
#include <string.h>
#include <cell/fs/cell_fs_file_api.h>

int sys_dbg_process_write(uint64_t address, const void *data, size_t size)
{
    system_call_4(905, sys_process_getpid(), address, size, (uintptr_t)data);
    return_to_user_prog(int);
}

size_t get_file_size(char *filePath)
{
    int size = 0;
    CellFsStat fstat;
    CellFsErrno err = cellFsStat(filePath, &fstat);
    if (err != CELL_FS_SUCCEEDED)
    {
        return err;
    }
    return fstat.st_size;
}

void set_empty_deflated_data(char *buffer)
{
    int op[7];
    op[0] = 0x789C05B0;
    op[1] = 0xB1110000;
    op[2] = 0x04C48CC3;
    op[3] = 0x6869543A;
    op[4] = 0xF7BBE758;
    op[5] = 0xAEE75302;
    op[6] = 0x109802FE;
    sys_dbg_process_write((uintptr_t)buffer, &op, 7 * 4);
}

void hex_str_to_padded_hex_str(char *out, char *hexStr)
{
    char *outPtr;
    char *tmp = hexStr;

    if (!tmp || !out)
        return;

    if (tmp[1] == 'x')
        tmp += 2;

    int hexLen = strlen(tmp);

    if ((hexLen % 2) != 0) // must be even
    {
        hexLen++;
        out[0] = '0';
        outPtr = &out[1];
    }
    else outPtr = out;
    
    strcpy(outPtr, tmp);
}

void hex_str_to_buffer(char *out, char *hexStr, size_t hexLen)
{
    if (!out || !hexStr || hexLen < 1)
        return;

    size_t index = 0;
    while (index < (hexLen * 2))
    {
        char c = hexStr[index];
        int value = 0;
        if (c >= '0' && c <= '9')
            value = (c - '0');
        else if (c >= 'A' && c <= 'F')
            value = (10 + (c - 'A'));
        else if (c >= 'a' && c <= 'f')
            value = (10 + (c - 'a'));
        else
            return;

        out[(index / 2)] += (value << (((index + 1) % 2) * 4));
        index++;
    }
}

int hex_str_to_int32(char *hexStr, size_t hexLen)
{
    char out[4];
    memset(out, 0, 4);
    int start = (4 - hexLen);
    if (start < 0)
        start = 0;

    hex_str_to_buffer((char *)(out + start), hexStr, hexLen);

    return *(int *)(&out);
}
