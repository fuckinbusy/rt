#ifndef _INC_RT
#define _INC_RT

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <sys/stat.h>

#include "rt_collections.h"
#include "rt_thread.h"

#define WIN32_LEAN_AND_MEAN
#define _WINUSER_
#define _WINGDI_
#define _IMM_
#define _WINCON_
#include <windows.h>
#include <TlHelp32.h>
#include <direct.h>
#include <shellapi.h>
#define RT_MAX_PATH MAX_PATH
#define RT_MAX_MODULE_NAME32 MAX_MODULE_NAME32

#ifdef RT_MALLOC
#   define RT_MALLOC malloc
#endif // RT_MALLOC

#ifdef RT_ASSERT
#   include <assert.h>
#   define RT_ASSERT assert
#endif // RT_ASSERT

#ifdef RT_FREE
#define RT_FREE free
#endif // RT_FREE

#ifndef nullptr
#   ifndef __cplusplus
#       define nullptr ((void*)0)
#   else
#       define nullptr nullptr
#   endif
#endif // nullptr

#define RT_DELIM_SIZE_LINE 36

#define RT_ARRAY_LEN(array) sizeof((array))/sizeof((*array))

typedef struct {
    DWORD processId;
    DWORD modId;
    PBYTE modBaseAddr;
    HMODULE hMod;
    CHAR name[RT_MAX_MODULE_NAME32 + 1];
    CHAR path[RT_MAX_PATH];
} RT_Module;

typedef struct {
    RT_Module *items;
    size_t size;
    size_t capacity;
} RT_ModuleList; // TODO: maybe change to smth simplier

typedef struct {
    DWORD processId;
    DWORD moduleId;
    DWORD parentProcessId;
    DWORD threadsCount;
    RT_ModuleList modules;
    CHAR name[RT_MAX_PATH];
} RT_Process;

typedef struct {
    RT_Process *items;
    size_t size;
    size_t capacity;
} RT_ProcessList;

bool rt_file_read(const char *path, RT_FileBuffer *buffer);
bool rt_file_write(const char *path, RT_FileBuffer *buffer);
size_t rt_file_hexdump(const char *path_in, const char *path_out);
size_t rt_file_get_size(const char *path);
bool rt_mkdir_if_not_exists(const char *path);
void rt_print_delim();
void rt_print_mainstat();
// size_t rt_mem_dump(const void *mem, size_t size, const char *path);

/* ---------- */
void rt_process_list_free(RT_ProcessList *list);
bool rt_process_list_enum(RT_ProcessList *list);
bool rt_module_list_enum(RT_ModuleList *list, DWORD processId);
void rt_process_list_dump(RT_ProcessList *list, FILE *out);


#endif // INC_RT