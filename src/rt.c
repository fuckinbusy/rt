#include "rt.h"

void rt_print_delim()
{
    for (size_t i = 0; i < RT_DELIM_SIZE_LINE; ++i) {
        fputc('-', stdout);
    }
    fputc('\n', stdout);
}

void rt_print_mainstat()
{
    rt_print_delim();

    char file_name[RT_MAX_PATH];
    sprintf(file_name, "%s", __FILE__);
    
    size_t fs_bytes = rt_file_get_size(file_name);
    double fs_kb = (double)fs_bytes / 1024.0;
    double fs_mb = fs_kb / 1024.0;
    printf("%zu bytes | %.3f kb | %.3f mb\n", fs_bytes, fs_kb, fs_mb);

    rt_print_delim();
}

// size_t rt_mem_dump(const void *mem, size_t size, const char *path)
// {
//     if (!mem || size == 0) return 0;
    
//     const uint8_t *src = (uint8_t*)mem;
//     const char *file_path = path ? path : "rt_mem_dump";
    
//     FILE *file = fopen(file_path, "wb");
//     if (!file) return 0;

//     const int bytes_per_row = 16;
//     char line[256] = {0};
//     char *line_p = line;

//     size_t dc = 0;
//     for (size_t i = 0; i < size; i += bytes_per_row) {
//         line_p += sprintf(line, "%08zx | ", i);

//         int bytes_printed = 0;
//         for (size_t j = 0; j < bytes_per_row && i + j < size; ++j, ++dc) {
//             line_p += sprintf(line, "%02X ", src[i+j]);
//             bytes_printed++;
//         }

//         int padding = bytes_per_row - bytes_printed;
//         if (padding > 0) {
//             int spaces = padding * 3;
//             memset(line_p, ' ', spaces);
//             line_p += spaces;
//         }

//         *line_p++ = '\n';
//         *line_p = '\0';
//         line_p = line;
        
//         fputs(line, file);

//     }

//     fclose(file);
//     return dc;
// }

size_t rt_file_get_size(const char *path)
{
    struct stat file_stat;
    stat(path, &file_stat);
    return file_stat.st_size;
}

bool rt_file_read(const char *path, RT_FileBuffer *buffer)
{
    return rt_fbuffer_read_file(buffer, path);
}

bool rt_file_write(const char *path, RT_FileBuffer *buffer)
{
    return rt_fbuffer_write_file(buffer, path);
}

size_t rt_file_hexdump(const char *path_in, const char *path_out)
{
    if (!path_in) return 0;
    const char *out = path_out ? path_out : "dump";
    const int bytes_per_row = 16;
    char line[256] = {0};
    char *line_ptr = line;

    RT_FileBuffer fb = {0};
    rt_fbuffer_reserve(&fb, 0x100000); // try to reserve 1mb of space

    FILE *fdump = fopen(out, "wb");
    if (!fdump) return 0;

    // if failed to reserve, this function will allocate space anyway
    if (rt_fbuffer_read_file(&fb, path_in) == 0) return 0; // if failed, well, then RIP

    size_t dc = 0; // dumped bytes counter
    for (size_t i = 0; i < fb.size; i += bytes_per_row) {
        line_ptr += sprintf(line_ptr, "%08zx | ", i);

        int bytes_printed = 0; // printed bytes counter (in this row)
        for (int j = 0; j < bytes_per_row && i+j < fb.size; ++j, ++dc) { // hex
            line_ptr += sprintf(line_ptr, "%02X ", fb.data[i + j]);
            bytes_printed++;
        }

        int padding = bytes_per_row - bytes_printed; // padding to fill space
        if (padding > 0) {
            int spaces = padding * 3;
            memset(line_ptr, ' ', spaces);
            line_ptr += spaces;
        }

        line_ptr += sprintf(line_ptr, "| ");

        for (size_t j = 0; j < bytes_per_row && i+j < fb.size; ++j) { // ascii
            uint8_t c = fb.data[i + j];
            line_ptr += sprintf(line_ptr, "%c", (c >= 0x20 && c <= 0x7E) ? c : '.');
        }

        *line_ptr++ = '\n';
        *line_ptr = '\0';
        line_ptr = line;
        
        fputs(line, fdump);
    }

    fclose(fdump);
    return dc;
}

bool rt_mkdir_if_not_exists(const char *path)
{
    int result = mkdir(path);
    if (result < 0) {
        if (errno == EEXIST) {
            fprintf(stderr, "Directory `%s` already exists\n", path);
            return true;
        }
        fprintf(stderr, "Couldn't create directory `%s`: %s\n", path, strerror(errno));
        return false;
    }
    return true;
}

// void rt_process_list_free(RT_ProcessList *list)
// {
//     if (!list) return;
//     for (size_t i = 0; i < list->size; ++i) {
//             rt_list_free(&list->items[i].modules);
//         }
//     rt_list_free(list);
// }

// bool rt_module_list_enum(RT_ModuleList *list, DWORD processId)
// {
//     if (!list || processId == 0) return false;

//     HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);
//     if (snap == INVALID_HANDLE_VALUE) {
//         fprintf(stderr, "Cannot snapshot modules for PID %lu (error: %lu)\n", GetLastError());
//         return false;
//     }

//     MODULEENTRY32 me = { .dwSize = sizeof(MODULEENTRY32) };
//     if (!Module32First(snap, &me)) {
//         CloseHandle(snap);
//         return false;
//     }

//     do {
//         if(!rt_list_expand_if_needed(list, sizeof(RT_Module))) {
//             CloseHandle(snap);
//             return false;
//         }
//         RT_Module module = {
//             .hMod = me.hModule,
//             .modBaseAddr = me.modBaseAddr,
//             .modId = me.th32ModuleID,
//             .name = {0},
//             .path = {0},
//             .processId = me.th32ProcessID
//         };
//         strcpy(module.name, me.szModule);
//         strcpy(module.path, me.szExePath);
//         list->items[list->size++] = module;
//     } while (Module32Next(snap, &me));

//     CloseHandle(snap);
//     return true;
// }

// bool rt_process_list_enum(RT_ProcessList *list)
// {
//     if (!list) return false;

//     HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//     if (snap == INVALID_HANDLE_VALUE) return false;
//     PROCESSENTRY32 pe = { .dwSize = sizeof(PROCESSENTRY32) };
//     if (!Process32First(snap, &pe)) {
//         CloseHandle(snap);
//         return false;
//     }
//     do {
//         if(!rt_list_expand_if_needed(list, sizeof(RT_Process))) {
//             CloseHandle(snap);
//             return false;
//         }
        
//         RT_ModuleList modList = {0};
//         rt_module_list_enum(&modList, pe.th32ProcessID);
        
//         RT_Process process = {
//             .processId = pe.th32ProcessID,
//             .moduleId = pe.th32ModuleID,
//             .name = {0},
//             .threadsCount = pe.cntThreads,
//             .parentProcessId = pe.th32ParentProcessID,
//             .modules = modList
//         };
//         strcpy(process.name, pe.szExeFile);
//         list->items[list->size++] = process;
//     } while (Process32Next(snap, &pe));
//     // TODO fix this function // it has a lot of critical bugs rn
//     CloseHandle(snap);
//     return true;
// }

void rt_process_list_dump(RT_ProcessList *list, FILE *out)
{
    if (!list || !list->items || list->size == 0) return;
    if (!out) out = stdout;

    fprintf(out, "=== Process List (%zu total) ===\n", list->size);
    for (size_t i = 0; i < list->size; ++i) {
        RT_Process *proc = &list->items[i];
        fprintf(out, "[%3zu] PID: %-5lu | PPID: %-5lu | Threads: %-3lu | Name: %s\n", 
               i, proc->processId, proc->parentProcessId, proc->threadsCount, proc->name);

        if (!proc->modules.items || proc->modules.size == 0) {
            fprintf(out, "      |--- [No modules loaded or access denied]\n");
            continue;
        }

        RT_ModuleList *modList = &proc->modules;
        for (size_t j = 0; j < modList->size; ++j) {
            RT_Module *mod = &proc->modules.items[j];
            fprintf(out, "      |--- [%02zu] %s (0x%p)\n", j, mod->name, mod->modBaseAddr);
        }

        rt_print_delim();
    }
}