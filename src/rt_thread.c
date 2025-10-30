#include "rt_thread.h"

void rt__thread_free(RT_Thread *thread, int thread_status)
{
    GetExitCodeThread(thread->handle, &thread->state);
    CloseHandle(thread->handle);
    thread->handle = NULL;
    thread->thread_func = NULL;
    thread->param = NULL;
    thread->state = thread_status;
}

bool rt_thread_create(RT_Thread *thread, void *param, DWORD (*thread_func)(void *param))
{
    if (!(thread && thread_func)) return false;

    thread->handle = CreateThread(
        NULL,
        0,
        thread_func,
        param,
        0,
        NULL
    );
    if (!thread->handle) return false;

    thread->param = param;
    thread->thread_func = thread_func;
    thread->state = RT_THREAD_STATE_RUNNING;

    return true;
}

bool rt_thread_join(RT_Thread *thread)
{
    if (!thread) return false;

    WaitForSingleObject(thread->handle, INFINITE);
    rt__thread_free(thread, RT_THREAD_STATE_STOPPED);

    return true;
}

bool rt_thread_detach(RT_Thread *thread)
{
    if (!thread) return false;
    
    rt__thread_free(thread, RT_THREAD_STATE_INTERRUPTED); // detach does not wait for thread to finish
    
    return true;
}

bool rt_thread_join_all(RT_Thread *threads, size_t count)
{
    if (!threads || count == 0) return false;
    
    for (size_t i = 0; i < count; ++i) {
        RT_Thread *thread = &threads[i];
        WaitForSingleObject(thread->handle, INFINITE);
        rt__thread_free(thread, RT_THREAD_STATE_STOPPED);
    }
    
    return true;
}

bool rt_thread_detach_all(RT_Thread *threads, size_t count)
{
    if (!threads || count == 0) return false;
    
    for (size_t i = 0; i < count; ++i) {
        rt__thread_free(&(threads[i]), RT_THREAD_STATE_INTERRUPTED);
    }
    
    return true;
}