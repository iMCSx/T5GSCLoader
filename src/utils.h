#ifndef UTILS_H
#define UTILS_H

#include <sys/prx.h>
#include <types.h>

// Fixing the define only to remove the annoying intellisense issue..
#ifdef SYS_MODULE_INFO_SECTION
#undef SYS_MODULE_INFO_SECTION
#define SYS_MODULE_INFO_SECTION __attribute__((section(SYS_MODINFO_SECTION_NAME ", \"a\",@progbits#")))
#endif

int sys_dbg_process_write(uint64_t address, const void *data, size_t size);
size_t get_file_size(char *filePath);
void set_empty_deflated_data(char *buffer);
void hex_str_to_padded_hex_str(char *out, char *hexStr);
void hex_str_to_buffer(char *out, char *hexStr, size_t hexLen);
int hex_str_to_int32(char *hexStr, size_t hexLen);

#endif /* UTILS_H */