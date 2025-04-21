/**
 * @file include/led.h
 * @brief LED control header file
 * @author SIYUAN
 * @date 2025/4/18
 */

#ifndef __LED_H
#define __LED_H

#include "threadpool.h"

class ColorLed;

/**
 * @brief: LED control data structure
 */
struct LedPwm
{
    int pin;
    int freq;
    int duty;
    ColorLed *led;
};

/**
 * @brief: LED control class
 * @note: This class is used to control the LED on the device
 */
class ColorLed
{
private:
    int m_red_pin;
    int m_grn_pin;
    int m_blue_pin;

public:
    ThreadPool *m_threadpool; //线程池对象

public:
    ColorLed(int m_red_pin, int m_grn_pin, int m_blue_pin);
    ~ColorLed();

    void setColor(int color[]);
    void setLedOff(void);

private:
    bool m_red_flag;    //控制红色灯是否打开的标志位
    bool m_grn_flag;    //控制绿色灯是否打开的标志位
    bool m_blue_flag;   //控制蓝色灯是否打开的标志位

private:
    void _set_pwm_off(int pin);
    static void _set_led_pwm(void* arg);
};

#endif