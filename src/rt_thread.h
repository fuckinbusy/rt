#ifndef _INC_RT_THREAD
#define _INC_RT_THREAD

#include <windows.h>
#include <stdbool.h>

#ifdef STILL_ACTIVE
#   define RT_THREAD_STATE_RUNNING STILL_ACTIVE
#else
#   define RT_THREAD_STATE_RUNNING (unsigned long)0x00000103L
#endif
#define RT_THREAD_STATE_STOPPED 0
#define RT_THREAD_STATE_INTERRUPTED 1
#define RT_THREAD_STATE_UNKNOWN 2

/* Threading section */
typedef struct _RT_Thread {
    HANDLE handle;
    void *param;
    DWORD (*thread_func)(void *param);
    DWORD state;
} RT_Thread;

void rt__thread_free(RT_Thread *thread, int thread_status);
bool rt_thread_create(RT_Thread *thread, void *param, DWORD (*thread_func)(void *param));
bool rt_thread_join(RT_Thread *thread);
bool rt_thread_detach(RT_Thread *thread);
bool rt_thread_join_all(RT_Thread *threads, size_t count);
bool rt_thread_detach_all(RT_Thread *threads, size_t count);

static inline void rt_thread_sleep(DWORD ms)
{
    Sleep(ms);
}

static inline DWORD rt_thread_get_state(RT_Thread *thread)
{
    return (thread) ? thread->state : RT_THREAD_STATE_STOPPED;
}

static inline bool rt_thread_is_running(RT_Thread *thread)
{
    return (thread && thread->state == RT_THREAD_STATE_RUNNING);
}

#endif // _INC_RT_THREAD