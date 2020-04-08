#ifndef _SYNCHRONISATION_HPP_
#define _SYNCHRONISATION_HPP_ 1

#include<common//types.hpp>
#include <kernel//system//tasking//scheduler.hpp>

typedef struct
{
    task_t*  blocker; // Task that is blocking the mutex
    uint32_t blocks;  // Indicates whether this mutex is blocked at the moment or not. -> You have to call unlock as often as lock to unblock mutex.
} mutex_t;


#endif
