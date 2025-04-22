#include <wiringPi.h>
#include <iostream>
using namespace std;
#include <signal.h>
#include <unistd.h>



void sig_handler(int signo)
{
    if(signo == SIGINT)
    {
        std::cout << "SIGINT received, exiting..." << std::endl;
        digitalWrite(21, LOW); // 关闭LED
        pinMode(21, INPUT); // 恢复引脚为输入模式
        pullUpDnControl(21, PUD_OFF); // 关闭上拉/下拉电阻
        digitalWrite(20, LOW); // 关闭LED
        pinMode(20, INPUT); // 恢复引脚为输入模式
        pullUpDnControl(20, PUD_OFF); // 关闭上拉/下拉电阻
        digitalWrite(16, LOW); // 关闭LED
        pinMode(16, INPUT); // 恢复引脚为输入模式
        pullUpDnControl(16, PUD_OFF); // 关闭上拉/下拉电阻
        exit(0);
    }
}

int main()
{
    if(wiringPiSetupGpio() < 0)
    {
        perror("wiringPiSetup error: ");
        exit(1);
    }

    signal(SIGINT, sig_handler);

    pinMode(21, OUTPUT);
    pinMode(20, OUTPUT);
    pinMode(16, OUTPUT);

    digitalWrite(21, HIGH);
    digitalWrite(20, HIGH);
    digitalWrite(16, HIGH);
    while(1)
    {
        sleep(2);
    }

    

    return 0;
}
