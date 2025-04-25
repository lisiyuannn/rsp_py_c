#include "led.h"
#include "main.h"
#include <wiringPi.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include "digital_disp.h"

using namespace std;




int main()
{
    //BCM引脚
    if(wiringPiSetupGpio() < 0)
    {
        perror("wiringPiSetup error: ");
        exit(1);
    }

    DigitalDisp disp(21, 20);
    

    return 0;
}