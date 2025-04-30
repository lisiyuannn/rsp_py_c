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
    //tm.tm1637_init();

    tm.tm1637_init();

    tm.set_colon(true);
    tm.flash_display();
    delay(1000);
    
    tm.set_display_data(1);
    tm.flash_display();
    delay(1000);
    tm.set_display_data(22);
    tm.flash_display();
    delay(1000);
    tm.set_display_data(333);
    tm.flash_display();
    delay(1000);
    tm.set_display_data(4444);
    tm.flash_display();
    delay(1000);

    tm.set_display_data(14, 0);
    tm.flash_display();
    delay(1000);
    tm.set_display_data(13, 1);
    tm.set_colon(true);
    tm.flash_display();
    delay(1000);

    tm.clear_display();
    delay(1000);
    tm.set_display_off();
    

    return 0;
}