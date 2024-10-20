//
// Created by wenshen on 24-10-10.
//

#include "timer.h"

#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

void init_timer(Timer *timer, const unsigned time_slice) {
    if (timer) {
        timer->time_slice = time_slice;
        timer->system_time = 0;
        timer->elasped_time = 0;
    }
}

void start_time(Timer *timer, Task_ *task) {
    if (!timer || !task)
        return;

    task->scheduler_sys_time = timer->system_time; //更新调度时间

    srand(task->seed); //设置盐值

    //如果time 当前还存在时间片 并待执行的任务还没有完成
    for (int i = 0; i < timer->time_slice && task->status != COMPLETED; ++i) {
        if (task->remaining_cpu_time == 0) {
            //如果任务剩余时间没有 则证明任务已经被执行完了
            task->status = COMPLETED;
        } else if (task->remaining_io_time > 0) {
            task->remaining_io_time--;
        } else {
            //检查是否击中IO触发
            float progress = (float) i / timer->time_slice;
            bool in_trigger = (task->io_trigger_points[0] == -1 && task->io_trigger_points[1] == -1)
                              || (task->io_trigger_points[0] <= progress && task->io_trigger_points[1] >= progress);
            // 击中IO
            if (in_trigger) {
                float random_value = (float) rand() / RAND_MAX;

                if (random_value < task->cpu_io_ratio) {
                    printf("Task %d performs I/O at time slice %d\n", task->t_id, i);
                    int io_time_random = rand() % 50; // 如果发生了IO 会随即生成一个IO时间
                    task->remaining_io_time = io_time_random;
                    task->status = WAITING;
                    task->io_total_time += io_time_random;
                }
            } else {
                task->status = RUNNING;
                // CPU 执行
                if (task->remaining_cpu_time > 0)
                    task->remaining_cpu_time--;
                task->cpu_use_total_time++;
                task->time_slice_used++;
                printf("Task %d uses CPU at time slice %d,remaining_time %d \n", task->t_id, i,
                       task->remaining_cpu_time);
            }
        }

        timer->elasped_time++;
        timer->system_time++;
    }
}
