#ifndef __THREADPOOL_H
#define __THREADPOOL_H

#include <iostream>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <list>
using namespace std;

class ThreadPool;

/**
 * @breaf: 任务结构体
 * @note: 任务结构体包含一个函数指针和一个参数指针
 */
struct NJOB
{
    void (*function)(void *arg);    //任务函数
    void *user_arg;                 //任务参数
};

/**
 * @breaf: 线程池中的每个线程
 * @note: 线程池中的每个线程都包含一个线程id，一个是否终止的标志，一个是否在工作中的标志和一个指向线程池的指针
 */
struct NWORKER
{
    pthread_t thread_id;    //线程id
    bool terminate;         //是否终止
    int isWorking;          //是否在工作
    ThreadPool *pool;       //所属线程池
};

/**
 * @breaf: 线程池类
 * @note: 线程池类包含一个线程数组，一个任务队列和一些其他变量
 */
class ThreadPool
{
private:
    NWORKER *m_workers;    //线程池
    //NJOB *m_jobs;          //任务队列

public:
    //构造函数，一共创建num_threads个线程
    ThreadPool(int num_threads, int max_jobs);
    //销毁线程池    
    ~ThreadPool();
    //面向用户的添加任务
    bool addJob(void (*function)(void *arg), void *arg, int len);

private:
    //向线程池中加入任务
    bool _pushJob(NJOB *job);
    //线程函数
    static void *_threadFunc(void *arg);
    //线程循环
    void _threadLoop(void* arg);

private:
    list<NJOB*> m_jobs_list;
    int m_max_jobs;      //最大任务数
    int m_sum_workers;    //线程数总和
    int m_free_workers;   //空闲任务数

    pthread_cond_t m_jobs_cond;     //任务条件变量
    pthread_mutex_t m_jobs_mutex;   //任务互斥量
};

#endif


