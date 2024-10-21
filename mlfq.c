//
// Created by wenshen on 24-10-10.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "mlfq.h"

#include <math.h>


// register 会进入register_queue，将 node 直接封装为新节点并入队
bool register_(MLFQ *mlfq, Node_ *node) {
    if (!mlfq || !node) {
        fprintf(stderr, "Error: Invalid MLFQ or Node provided to register_.\n");
        return false;
    }

    // 直接将原始节点封装为新的注册节点
    Node_ *register_node = create_node(node);
    if (!register_node) {
        fprintf(stderr, "Error: Failed to create register node.\n");
        return false;
    }

    // 将新创建的注册节点加入到 register_queue
    enqueue(&mlfq->register_queue, register_node);
    return true;
}


bool insert_(MLFQ *mlfq, Node_ *node) {
    if (!mlfq || !node)
        return false;
    Task_ *task = node->data;
    if (!task)
        return false;

    if (task->queue_level < 0 || task->queue_level > MAX_QUEUE_SIZE - 1) {
        printf("Invalid task level %d\n", task->queue_level);
        return false;
    }
    Queue *queue = &mlfq->queues[task->queue_level];
    enqueue(queue, node);
    return true;
}

//准确的是说 从mlfq 中移除 通常是在task 完成时,但注意并不清理node 而是在"特定"时间清除
void remove_(MLFQ *mlfq, Node_ *node) {
    if (!mlfq || !node)
        return;
    Task_ *task = node->data;
    Queue *queue = &mlfq->queues[task->queue_level];
    remove_node(queue, node);
}


void mlfq_scheduler_do(MLFQ *mlfq, Timer *timer) {
    for (int level = MAX_QUEUE_SIZE - 1; level >= 0; level--) {
        Queue *queue = &mlfq->queues[level];
        if (QUEUE_EMPTY(queue))
            continue;
        //时间片轮询的支持 dequeue会把当前node 弹出队列 如果之后再次入队(当前或其他队列) 都使用的是尾插
        Node_ *node = dequeue(queue);
        Task_ *task = node->data;
        start_time(timer, task);

        if (task->status == COMPLETED) {
            printf("Inof: Task completed of scheduler.\n");
        }else if (task->status == WAITING || level == 0) {
            printf("Info: Task re_enqueue with id %d, level %d\n", task->t_id, level);
            insert_(mlfq, node); //重新入队
        } else if ((task->status = RUNNING)) {
            down(task);
            insert_(mlfq, node); //重新入队
            printf("Info: Task Tier Down with id %d, level %d \n", task->t_id, level - 1);
        }
        break;
    }

    //Periodic priority enhancement
    // for (int level = 0; level <= MAX_QUEUE_SIZE - 1; level++) {
    //     // =号 排除最高优先级的队列
    //     Node_ *lp_node = mlfq->queues[level].front;
    //     while (lp_node) {
    //         Task_ *lp_task = lp_node->data;
    //         if (lp_task && timer->system_time - lp_task->scheduler_sys_time > 5) {
    //             remove_(mlfq, lp_node); //先从当前的队列移除
    //             up(lp_task);
    //             insert_(mlfq, lp_node); //升级
    //             printf("Info : PPE TaskId: %d, UpLevel %d,task_exec_systime: %ld ,Timer_systime: %ld \n",
    //                    lp_task->t_id, lp_task->queue_level
    //                    , timer->system_time, lp_task->scheduler_sys_time);
    //             return; //一次之处理一个
    //         }
    //         lp_node = lp_node->next;
    //     }
    // }
}


void mlfq_scheduler(MLFQ *mlfq, Timer *timer) {
    if (!mlfq || !timer)
        exit(-1);

    Node_ *node_r = mlfq->register_queue.front; // 获取注册队列的节点

    while (node_r) {
        Node_ *next_r = node_r->next;
        Node_ *task_node = node_r->data; // 获取实际的任务节点
        Task_ *task = task_node->data; // 通过任务节点获取任务数据

        printf( //log
            "Task Check_: ID=%d, Arrival Time=%d, Queue Level=%d, Time Slice=%d, Cpu_Remaining_Time=%d, CPU/IO Ratio=%.2f, Trigger Points=[%.2f, %.2f],"
            "remaining_io_time = %d,SysSchedulerTime: %ld\n",
            task->t_id, task->arrival_time, task->queue_level, task->time_slice, task->remaining_cpu_time,
            task->cpu_io_ratio,
            task->io_trigger_points[0], task->io_trigger_points[1], task->remaining_io_time, task->scheduler_sys_time);

        if (task->status == INIT && timer->system_time >= task->arrival_time) {
            printf("Task %d arrives at time %lu and is added to Queue Level %d\n", task->t_id, timer->system_time,
                   task->queue_level);
            task->status = READY;
            insert_(mlfq, task_node);
        } else {
            // 检查任务是否完成 并清理资源 或者说 mlfq并没有清理资源只有移除的权限 这里就是"特定"
            if (task->status == COMPLETED) {
                printf("Info: Task COMPLETED Id %d \n", task->t_id);
                free_node(task_node);
                remove_node(&mlfq->register_queue, node_r); //从register_queue中移除
                free(node_r); //清理register_node
            }
        }
        node_r = next_r;
    }
    mlfq_scheduler_do(mlfq, timer);
    reset_timer(timer);
    advance_system_time(timer, 1);
}


void free_mlfq(MLFQ *mlfq) {
    if (!mlfq)
        return;
    free(mlfq);
}


// /*********************************************Test***********************************************************************/


void test_bad_task() {
    MLFQ *mlfq = malloc(sizeof(MLFQ));
    Timer timer;
    init_timer(&timer, 5);
    //典型的坏“孩子”任务  在时间片最后一刻执行io 而且几率是100% 剩余时间是 -1
    Task_ *bad_task = create_task(MAX_QUEUE_SIZE - 1, 5, rand(), 1, 0.9f, 0.9f, -1);
    Node_ *node = create_node(bad_task);
    register_(mlfq, node);

    int round = 0;
    while (!ALL_DOWN_(mlfq)) {
        printf("\n=== Scheduling Round %d ===\n", round++);
        mlfq_scheduler(mlfq, &timer);
    }
    free_mlfq(mlfq);
}

void test_mlfq() {
    MLFQ *mlfq = malloc(sizeof(MLFQ));
    Timer timer;
    init_timer(&timer, 5);

    int n_tasks = 5;
    for (int i = 0; i < n_tasks; i++) {
        Task_ *task = create_task(MAX_QUEUE_SIZE - 1, 5, rand(), 0.1f, 0.2f, 0.7f, 50);
        Node_ *node = create_node(task);
        register_(mlfq, node);
    }

    float random_ration = 0.01;

    // scheduler
    int round = 0;
    while (!ALL_DOWN_(mlfq)) {
        printf("\n=== Scheduling Round %d ===\n", round++);
        mlfq_scheduler(mlfq, &timer);
        // 在调度过程中的随机任务
        float random_value = (float) rand() / RAND_MAX;
        if (random_value <= random_ration) {
            printf("Info: Random_Task -");
            Task_ *task = create_task(MAX_QUEUE_SIZE - 1, 5, rand(), 0.1f, 0.2f, 0.7f, 50);
            Node_ *node = create_node(task);
            register_(mlfq, node);
        }
    }
    free_mlfq(mlfq);
}
