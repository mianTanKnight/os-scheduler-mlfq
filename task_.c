//
// Created by wenshen on 24-10-9.
//

#include "task_.h"

#include <time.h>

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

int id_generator = 1;

void destroy_(Task_ *task) {
    if (NULL != task)
        free(task);
}

Task_ *create_task(const int queue_level, const int time_slice, const unsigned int seed, const float cpu_io_ratio,
                   const float trigger_point_1,
                   const float trigger_point_2,
                   const int remaining_time) {
    Task_ *task = malloc(sizeof(Task_));
    if (!task) {
        printf("Failed to allocate memory for task.\n");
        return NULL;
    }
    task->arrival_time = rand() % 50;
    task->remaining_cpu_time = remaining_time != -1 ? task->remaining_cpu_time = rand() % 100 + 10 : -1;
    // 随机生成剩余时间，模拟任务大小
    task->t_id = id_generator++;
    task->queue_level = queue_level;
    task->time_slice = time_slice;
    task->seed = seed;
    task->cpu_io_ratio = cpu_io_ratio;
    task->io_trigger_points[0] = trigger_point_1;
    task->io_trigger_points[1] = trigger_point_2;
    task->status = INIT;
    task->cpu_use_total_time = -1;
    task->remaining_io_time = -1;
    task->io_total_time = -1;
    task->priority_boost_count = 0;
    task->scheduler_sys_time = -1;
    // 输出任务信息日志
    printf("Task Created: ID=%d, Arrival Time=%d, Queue Level=%d, Time Slice=%d, Remaining Time=%d, CPU/IO Ratio=%.2f"
           ", Trigger Points=[%.2f, %.2f],SysSchedulerTime: %ld\n",
           task->t_id, task->arrival_time, task->queue_level, task->time_slice, task->remaining_cpu_time,
           task->cpu_io_ratio,
           task->io_trigger_points[0], task->io_trigger_points[1], task->scheduler_sys_time);
    return task;
}


void execute_task(Task_ *task) {
    if (!task) return;

    srand(task->seed);
    for (int i = 0; i < task->time_slice; ++i) {
        float progress = (float) i / task->time_slice;

        float p_1 = task->io_trigger_points[0];
        float p_2 = task->io_trigger_points[1];

        bool in_trigger = (p_1 == -1 && p_2 == -1) ||
                          (p_1 == -1 && progress <= p_2) ||
                          (p_2 == -1 && progress >= p_1) ||
                          (progress >= p_1 && progress <= p_2);

        if (in_trigger) {
            float random_value = (float) rand() / RAND_MAX;

            if (random_value < task->cpu_io_ratio) {
                printf("Task %d performs I/O at time slice %d\n", task->t_id, i);
                task->status = WAITING;
                // 模拟 I/O 操作...
                task->io_total_time += 1; // 累加 I/O 时间
                return; // 当前时间片结束，进入 I/O 等待状态
            }
        }

        printf("Task %d uses CPU at time slice %d\n", task->t_id, i);
        task->cpu_use_total_time++;
        task->time_slice_used++;

        if (task->remaining_cpu_time != -1) {
            task->remaining_cpu_time--;
            if (task->remaining_cpu_time <= 0) {
                task->status = COMPLETED;
                printf("Task %d is completed\n", task->t_id);
                break;
            }
        }
    }
}

/***********************************************************Test***************************************************************************************/

// void test_task_init(Task_ **tasks, const unsigned size) {
//     printf("Task Init Test \n");
//
//     for (int i = 0; i < size; i++) {
//         Task_ *task = create_task(8, 10, rand(), 0.5f, 0.2f, 0.8f, 50);
//         if (task) {
//             printf(
//                 "Task %d: Queue Level: %d, Time Slice: %d, Remaining Time: %d, CPU/IO Ratio: %.2f, Trigger Points: [%.2f, %.2f]\n",
//                 task->t_id, task->queue_level, task->time_slice, task->remaining_time, task->cpu_io_ratio,
//                 task->io_trigger_points[0], task->io_trigger_points[1]);
//             tasks[i] = task;
//         } else {
//             printf("Error creating task %d\n", i);
//         }
//     }
// }
//
//
// void test_multitask_schedule(Task_ **tasks, unsigned size, unsigned count) {
//     for (int i = 0; i < count; ++i) {
//         printf("=== Scheduling Round %d ===\n", i);
//         for (int j = 0; j < size; ++j) {
//             if (tasks[j] && tasks[j]->status != COMPLETED) {
//                 execute_task(tasks[j]);
//             }
//         }
//     }
// }

// void test_bad_task_execution() {
//     Task_ *bad_task = create_task(8, 10, rand(), 0.1f, -1, -1, -1);
//     Task_ *normal_task = create_task(8, 10, rand(), 0.7, 0.2, 0.5, 30);
//
//
//     for (int round = 0; round < 5; round++) {
//         printf("=== Scheduling Round %d ===\n", round);
//
//         if (bad_task && bad_task->status != COMPLETED) {
//             execute_task(bad_task);
//             if (bad_task->status == COMPLETED) {
//                 destroy_(bad_task);
//                 bad_task = NULL;
//             }
//         }
//
//         if (normal_task && normal_task->status != COMPLETED) {
//             execute_task(normal_task);
//             if (normal_task->status == COMPLETED) {
//                 destroy_(normal_task);
//                 normal_task = NULL;
//             }
//         }
//     }
// }


// int main() {
// test_bad_task_execution();
// unsigned c = 5;
// Task_ **tasks = malloc(sizeof(Task_ *) * c);
// for (int i = 0; i < c; ++i)
//     tasks[i] = NULL;
//
// test_task_init(tasks, c);
// printf("Task Init Test \n");
//
// test_multitask_schedule(tasks, c, 3);
//
// for (int i = 0; i < c; ++i) {
//     if (tasks[i] && tasks[i]->status == COMPLETED) {
//         printf("Task %d is completed\n", tasks[i]->t_id);
//         destroy_(tasks[i]);
//         tasks[i] = NULL;
//     }
// }
// }
