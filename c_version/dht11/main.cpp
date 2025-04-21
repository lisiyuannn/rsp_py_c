#include "dht11.h"
#include <iostream>
#include <csignal>

using namespace std;

// 信号处理函数
void signal_handler(int signal)
{
    if(signal == SIGINT)
    {
        cout << endl;
        dht11_over();
        std::cout << "Received SIGINT, exiting..." << std::endl;
        exit(0);
    }
}

int main()
{
    // 注册信号处理函数
    signal(SIGINT, signal_handler);

    if(wiringPiSetupGpio() < 0)
    {
        perror("wiringPiSetup error: ");
        exit(1);
    }
    
    unsigned char temp[4];

    while(1)
    {
        dht11_rec_data(temp);
        std::cout << "湿度: " << (int)temp[0] << " %RH" << std::endl;
        std::cout << "温度: " << (int)temp[2] << " °C" << std::endl;

        delayMicroseconds(5000000);
    }
    
    return 0;
}
