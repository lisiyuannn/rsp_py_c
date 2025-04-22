#include <wiringPi.h>
#include <iostream>
#include <signal.h>
#include <unistd.h>

void sig_handler(int signo)
{
    if(signo == SIGINT)
    {
        std::cout << "SIGINT received, exiting..." << std::endl;
        digitalWrite(20, LOW); // 关闭LED
        pinMode(20, INPUT); // 恢复引脚为输入模式
        pullUpDnControl(20, PUD_OFF); // 关闭上拉/下拉电阻
        exit(0);
    }
}

using namespace std;

#define freq 100 // 频率
#define duty 10 // 占空比

int main()
{
    if(wiringPiSetupGpio() < 0)
    {
        perror("wiringPiSetup error: ");
        exit(1);
    }

    //登记信号处理函数
    signal(SIGINT, sig_handler);

    int led_pin = 21;
    pinMode(led_pin, OUTPUT);

    int period = 1000000 / freq; // 总周期（微秒）
    int high_time = period * duty / 100;
    int low_time = period - high_time;

    // while(1)
    // {
    //     digitalWrite(led_pin, HIGH);
    //     delayMicroseconds(high_time);
    //     digitalWrite(led_pin, LOW);
    //     delayMicroseconds(low_time);
    // }

    while(1)
    {
        digitalWrite(led_pin, HIGH);
        delayMicroseconds(high_time);
        digitalWrite(led_pin, LOW);
        delayMicroseconds(low_time);
    }
}   