//
// Created by wenshen on 24-10-10.
//

#ifndef TIMER_H
#define TIMER_H
#include "task_.h"
#include <stdint.h>

typedef struct Timer Timer;

struct Timer {
    unsigned time_slice;
    unsigned elasped_time;
    uint64_t system_time;
};

void init_timer(Timer *timer, unsigned time_slice);

void start_time(Timer *timer, Task_ *task);


#define reset_timer(t) if((t)) (t)->elasped_time = 0;
#define advance_system_time(t,ints) if((t)) (t)->system_time += (ints);
#define is_time_slice_complete(t)  (t) && (t) -> elasped_time >= (t)->time_slice
#endif //TIMER_H
