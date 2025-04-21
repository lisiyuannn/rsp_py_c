#include "threadpool.h"


void* ThreadPool::_threadFunc(void *arg)
{
    NWORKER *worker = (NWORKER *)arg;
    worker->pool->_threadLoop(worker);
    return (void*)0;
}

/**
 * @brief 线程循环函数
 * @param void* arg 线程参数
 * @return void* 线程返回值
 * @note 线程循环函数，用于线程池中的线程循环执行任务，不断检查任务队列中是否有job
 */
void ThreadPool::_threadLoop(void* arg)
{
    NWORKER *worker = (NWORKER *)arg;
    while(1)
    {
        //查看任务队列
        pthread_mutex_lock(&m_jobs_mutex);
        while(m_jobs_list.size() == 0)
        {
            if(worker->terminate) break;
            //进入等待队列继续等待
            pthread_cond_wait(&m_jobs_cond, &m_jobs_mutex);
        }
        
        if(worker->terminate)
        {
            pthread_mutex_unlock(&m_jobs_mutex);
            break;
        }

        //从任务队列中取出任务
        NJOB *job = m_jobs_list.front();
        m_jobs_list.pop_front();    //将任务从队列中移除
        pthread_mutex_unlock(&m_jobs_mutex);    //释放锁

        m_free_workers--;
        worker->isWorking = 1;      //正在工作
        //执行job中的func
        job->function(job->user_arg);
        worker->isWorking = 0;      //工作完成
        m_free_workers++;

        free(job->user_arg);
        free(job);
    }
}

bool ThreadPool::_pushJob(NJOB* job)
{
    //获取任务队列锁
    pthread_mutex_lock(&m_jobs_mutex);
    int count = 0;
    //判断任务队列是否已满
    while(m_jobs_list.size() == m_max_jobs)
    {
        count++;
        pthread_mutex_unlock(&m_jobs_mutex);
        if(count > 100)
        {
            //超时
            return false;
        }
        usleep(1000);
        pthread_mutex_lock(&m_jobs_mutex);
    }
    //将任务加入任务队列
    m_jobs_list.push_back(job);
    //释放锁
    pthread_mutex_unlock(&m_jobs_mutex);
    //唤醒等待队列中的线程
    pthread_cond_signal(&m_jobs_cond);
    return true;
}

//面向用户添加任务
bool ThreadPool::addJob(void (*function)(void *arg), void *arg, int len)
{
    NJOB *job = (NJOB *)malloc(sizeof(NJOB));
    if(job == NULL)
    {
        perror("addJob: malloc job failed");
        return false;
    }

    memset(job, 0, sizeof(NJOB));
    job->user_arg = malloc(len);
    if(job->user_arg == NULL)
    {
        perror("addJob: malloc job->data failed");
        return false;
    }
    memcpy(job->user_arg, arg, len);
    job->function = function;

    //将任务加入任务队列
    if(!_pushJob(job))
    {
        perror("addJob: push job failed");
        return false;
    }
    return true;
}

ThreadPool::ThreadPool(int num_threads = 5, int max_jobs = 10)
    : m_sum_workers(num_threads), m_max_jobs(num_threads), 
    m_free_workers(0)
{
    //判断参数是否合理
    if(num_threads <= 0 || max_jobs <= 0)
    {
        perror("ThreadPool: num_threads or max_jobs is invalid");
    }

    //初始化任务队列
    if(pthread_cond_init(&m_jobs_cond, NULL) != 0)
        perror("ThreadPool: pthread_cond_init failed");
    
    //初始化锁
    if(pthread_mutex_init(&m_jobs_mutex, NULL)!= 0)
        perror("ThreadPool: pthread_mutex_init failed");

    //初始化线程池
    m_workers = new NWORKER[num_threads];
    if(m_workers == NULL)
    {
        perror("ThreadPool: malloc workers failed");
    }
    //初始化线程
    for(int i = 0; i < num_threads; ++i)
    {
        memset(&m_workers[i], 0, sizeof(NWORKER));
        m_workers[i].terminate = false;
        m_workers[i].isWorking = 0;
        m_workers[i].pool = this;
        //创建线程
        if(pthread_create(&m_workers[i].thread_id, NULL,
             _threadFunc, &m_workers[i]) != 0)
        {
            delete [] m_workers;
            perror("ThreadPool: pthread_create failed");
        }
        //设置分离属性
        if(pthread_detach(m_workers[i].thread_id)!= 0)
        {
            delete [] m_workers;
            perror("ThreadPool: pthread_detach failed");
        }
    }
}

ThreadPool::~ThreadPool()
{
    //销毁线程池
    for(int i = 0; i < m_sum_workers; ++i)
    {
        m_workers[i].terminate = true;
    }

    //唤醒所有线程
    pthread_mutex_lock(&m_jobs_mutex);
    pthread_cond_broadcast(&m_jobs_cond);
    pthread_mutex_unlock(&m_jobs_mutex);
    delete [] m_workers;
}

