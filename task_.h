//
// Created by wenshen on 24-10-9.
//

#ifndef TASK__H
#define TASK__H
#include <time.h>

typedef struct Task_ Task_;

typedef enum {
    INIT,
    READY,
    RUNNING,
    WAITING,
    COMPLETED,
} TaskStatus;

struct Task_ {
    int t_id;
    // 剩余时间 remaining_time 支持 -1 表明进入了无限循环状态（"坏"孩子任务）
    int remaining_cpu_time;
    int queue_level; // 当前优先队列的级别
    TaskStatus status; // 任务状态
    int cpu_use_total_time; // 占用 CPU 的总时长（不包含 IO 时间）
    int remaining_io_time; //如果发生来IO 此为io剩余的时间
    int io_total_time; // IO 等待总时长
    int time_slice_used; // 当前时间片已使用的时间
    int time_slice; // 当前任务的时间片长度
    int priority_boost_count;

    // 用于 IO 触发的随机性（特定时间片）
    // 支持外部指定和随机性给定一个随机率，在真正执行时是否击中
    float cpu_io_ratio; // CPU/IO 使用的概率比例

    float io_trigger_points[2]; // IO 触发位置（比例），(NULL, NULL) 表示完全随机

    unsigned int seed; // 随机种子
    //支持 PPE(周期性优先级提升) 用于上次执行的sys_time
    time_t scheduler_sys_time;
    //任务准备好被调度器调度的时间 随机性
    int arrival_time;
};

Task_ *create_task(int queue_level, int time_slice, unsigned int seed, float cpu_io_ratio, float trigger_point_1,
                   float trigger_point_2, int remaining_time);

void destroy_(Task_ *task);

#define down(t)        \
do {      \
if ((t) != NULL && (t)->queue_level > 0) { \
--((t)->queue_level); \
} \
} while (0)


#define up(t)        \
do {      \
if ((t) != NULL) { \
++((t)->queue_level); \
} \
} while (0)

#endif //TASK__H
