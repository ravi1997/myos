#ifndef _TASK_HPP_
#define _TASK_HPP_ 1

#include <common//types.hpp>

typedef enum
{
    PROCESS, THREAD, VM86
} taskType_t;

struct task
{
    taskType_t       type;           // Indicates whether it is a thread or a process
    uint32_t         pid;            // Process ID
    uint32_t         esp;            // Stack pointer
    uint32_t         ss;             // Stack segment
    pageDirectory_t* pageDirectory;  // Page directory
    void*            kernelStack;    // Kernel stack location
    void*            FPUptr;         // Pointer to FPU data
    list_t*          threads;        // All threads owned by this tasks - deleted if this task is exited
    task_t*          parent;         // Task that created this thread (only used for threads)
    event_queue_t*   eventQueue;     // 0 if no event handling enabled. Points to queue otherwise.
    uint8_t          privilege;      // Access privilege
    struct ipc_node* ipc_node;       // IPC node of the current task
    struct ipc_node* ipc_working;    // Working IPC node of the current task

    // Information needed by scheduler
    //uint16_t         priority; // Indicates how often this task gets the CPU
    blocker_t        blocker;  // Object indicating reason and duration of blockade

    // Task specific graphical output settings
    console_t*       console; // Console used by this task
    uint8_t          attrib;  // Color

    // Information needed for cleanup
    bool    speaker;
    list_t* files;
    list_t* folders;
    list_t* heaps;

    // comment for debug purpose
    char comment[21];
};


#endif
