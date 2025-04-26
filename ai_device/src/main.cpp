#include "led.h"
#include "main.h"
#include <wiringPi.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include "tm1637.h"

using namespace std;




int main()
{
    //BCM引脚
    if(wiringPiSetupGpio() < 0)
    {
        perror("wiringPiSetup error: ");
        exit(1);
    }

    TM1637 tm(21, 20);
    tm.init();

    tm._start();
    tm._write_byte(0x40);
    tm._wait_ack();
    tm._stop();

    tm._start();
    tm._write_byte(0xc0);
    tm._wait_ack();

    for(int i = 0; i < 10; i++)
    {
        tm._write_byte(tm.code[i]);
        tm._wait_ack();
    }

    tm._stop();
    

    return 0;
}