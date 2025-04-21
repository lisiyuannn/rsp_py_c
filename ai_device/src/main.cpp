#include "led.h"
#include <wiringPi.h>
#include <unistd.h>

#include <iostream>

using namespace std;


int main()
{
    if(wiringPiSetup() < 0)
    {
        perror("wiringPiSetup error: ");
        exit(1);
    }

    int color[3] = {255, 0, 0}; // 红色

    ColorLed led(16, 20, 21); // 假设引脚16、20、21分别对应红色、绿色和蓝色LED
    led.setColor(color);
    sleep(2); // 等待2秒
    color[0] = 0; color[1] = 255; color[2] = 0; // 绿色
    led.setColor(color);
    sleep(2); // 等待2秒
    color[0] = 0; color[1] = 0; color[2] = 255; // 绿色
    led.setColor(color);
    //全开
    sleep(2); // 等待2秒
    color[0] = 255; color[1] = 255; color[2] = 255; // 绿色
    led.setColor(color);

    sleep(2); // 等待2秒
    //全关
    led.setLedOff();
    sleep(2); // 等待2秒
    
    color[0] = 0; color[1] = 0; color[2] = 128; // 绿色
    led.setColor(color);


    return 0;
}