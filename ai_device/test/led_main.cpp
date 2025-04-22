#include "led.h"
#include <wiringPi.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>

using namespace std;

// void sigHandler(int signo)
// {
//     if (signo == SIGINT)
//     {
//         cout << "Caught SIGINT, exiting..." << endl;
//         exit(0);
//     }
// }


int main()
{
    //BCM引脚
    if(wiringPiSetupGpio() < 0)
    {
        perror("wiringPiSetup error: ");
        exit(1);
    }

    // signal(SIGINT, sigHandler); // 注册信号处理函数

    int color[3] = {20, 255, 255}; // 红色

    ThreadPool* pool = new ThreadPool(10, 100); // 创建线程池对象
    if(pool == NULL)
    {
        perror("ThreadPool: malloc pool failed");
        exit(1);
    }

    ColorLed led(16, 20, 21, pool); // 假设引脚16、20、21分别对应红色、绿色和蓝色LED
    led.setColor(color);
    
    sleep(10); // 等待10秒钟
    led.setLedOff(); // 关闭LED灯

    // 关闭线程池
    delete pool;
    

    return 0;
}