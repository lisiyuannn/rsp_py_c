#include "threadpool.h"

#include <iostream>

using namespace std;
void testFunc(void* arg)
{
    int *p = (int*)arg;
    cout << "testFunc: " << (int)(*p) << endl;
}

int main()
{
    ThreadPool* pool = new ThreadPool(10, 100);
    for(int i = 0; i < 1000; ++i)
    {
        pool->addJob(testFunc, (void*)&i, sizeof(i));
    
    }
}