#include "led.h"
#include "main.h"
#include "threadpool.h"
#include <wiringPi.h>
#include <iostream>
#include <cstring>

using namespace std;

/**
 * @brief 构造函数，用于初始化 ColorLed 类的对象。
 * 
 * 该构造函数接收三个整数参数，分别代表红色、绿色和蓝色 LED 灯的引脚编号，并使用成员初始化列表对相应的成员变量进行初始化。
 * 同时，将运行标志 `run_flag` 初始化为 `false`。
 * 
 * @param m_red_pin 红色 LED 灯的引脚编号。
 * @param m_grn_pin 绿色 LED 灯的引脚编号。
 * @param m_blue_pin 蓝色 LED 灯的引脚编号。
 */
ColorLed::ColorLed(int m_red_pin, int m_grn_pin, int m_blue_pin, ThreadPool* pool)
    : m_red_pin(m_red_pin), m_grn_pin(m_grn_pin), m_blue_pin(m_blue_pin), m_threadpool(pool),
    m_red_flag(false), m_grn_flag(false), m_blue_flag(false)
{
    #ifdef DEBUG
    cout << "ColorLed(int, int, int)" << endl;
    #endif
    //设置引脚模式
    pinMode(this->m_blue_pin, OUTPUT);
    pinMode(this->m_grn_pin, OUTPUT);
    pinMode(this->m_red_pin, OUTPUT);

    digitalWrite(this->m_blue_pin, LOW);
    digitalWrite(this->m_grn_pin, LOW);
    digitalWrite(this->m_red_pin, LOW);
}


/**
 * @brief 析构函数，用于销毁 ColorLed 类的对象。
 * 
 * 该析构函数在 ColorLed 类的对象被销毁时调用，主要功能是恢复 LED 灯对应引脚的状态，
 * 将引脚模式设置为输入模式，并关闭引脚的上拉/下拉电阻，同时将运行标志 `run_flag` 置为 `false`。
 */
ColorLed::~ColorLed()
{
    #ifdef DEBUG
    cout << "~ColorLed(int, int, int)" << endl;
    #endif

    m_red_flag = false;
    m_grn_flag = false;
    m_blue_flag = false;

    //等待 LED 灯熄灭
    delayMicroseconds(20000);

    //恢复引脚
    pinMode(this->m_blue_pin, INPUT);
    pullUpDnControl(this->m_blue_pin, PUD_OFF); // 关闭上拉/下拉电阻
    pinMode(this->m_grn_pin, INPUT);
    pullUpDnControl(this->m_grn_pin, PUD_OFF); // 关闭上拉/下拉电阻
    pinMode(this->m_red_pin, INPUT);
    pullUpDnControl(this->m_red_pin, PUD_OFF); // 关闭上拉/下拉电阻

}


void ColorLed::_set_pwm_off(int pin)
{
    if(pin == this->m_blue_pin) m_blue_flag = false;
    else if(pin == this->m_grn_pin) m_grn_flag = false;
    else if(pin == this->m_red_pin) m_red_flag = false;
}

void ColorLed::_set_led_pwm(void* arg)
{
    LedPwm *led_pwm = (LedPwm *)arg;

    #ifdef DEBUG
    cout << "_set_led_pwm start" << endl;
    #endif

    bool *pin_flag = nullptr;
    if(led_pwm->pin == led_pwm->led->m_blue_pin) pin_flag = &led_pwm->led->m_blue_flag;
    else if(led_pwm->pin == led_pwm->led->m_grn_pin) pin_flag = &led_pwm->led->m_grn_flag;
    else if(led_pwm->pin == led_pwm->led->m_red_pin) pin_flag = &led_pwm->led->m_red_flag;

    int period = 1000000 / led_pwm->freq; // 总周期（微秒）
    #ifdef DEBUG
    cout << "led_pwm->duty: " << led_pwm->duty << endl;
    #endif
    int high_time = period * led_pwm->duty / 100;
    int low_time = period - high_time;

    #ifdef DEBUG
    cout << "high_time: " << high_time << endl;
    cout << "low_time: " << low_time << endl;
    #endif

    //软件模拟pwm
    while(*pin_flag)
    {
        digitalWrite(led_pwm->pin, HIGH);
        delayMicroseconds(high_time);
        digitalWrite(led_pwm->pin, LOW);
        delayMicroseconds(low_time);
    }

    #ifdef DEBUG
    cout << "_set_led_pwm end" << endl;
    #endif

}

void ColorLed::setLedOff(void)
{
    //关闭软件pwm
    _set_pwm_off(this->m_blue_pin);
    _set_pwm_off(this->m_grn_pin);
    _set_pwm_off(this->m_red_pin);

    //关闭 LED 灯
    digitalWrite(this->m_blue_pin, LOW);
    digitalWrite(this->m_grn_pin, LOW);
    digitalWrite(this->m_red_pin, LOW);
}


void ColorLed::setColor(int color[])
{
    int pin = 0;
    int freq = 0;
    int duty = 0;
    //处理红色通道
    pin = this->m_red_pin;
    freq = 100;
    duty = (int)((color[0])*(float)(100.0/255.0)); //将颜色值转换为0-100之间的值
    LedPwm led_pwm = {pin, freq, duty, this};     //可以使用局部变量，addjob会深拷贝一份
    this->m_red_flag = true; //设置标志位
    this->m_threadpool->addJob(_set_led_pwm, (void*)&led_pwm, sizeof(led_pwm));

    //处理绿色通道
    pin = this->m_grn_pin;
    freq = 100;
    duty = (int)((color[1])*(float)(100.0/255.0)); //将颜色值转换为0-100之间的值
    led_pwm = {pin, freq, duty, this};     //可以使用局部变量，addjob会深拷贝一份
    this->m_grn_flag = true; //设置标志位
    this->m_threadpool->addJob(_set_led_pwm, (void*)&led_pwm, sizeof(led_pwm));

    //处理蓝色通道
    pin = this->m_blue_pin;
    freq = 100;
    duty = (int)((color[2])*(float)(100.0/255.0)); //将颜色值转换为0-100之间的值
    led_pwm = {pin, freq, duty, this};     //可以使用局部变量，addjob会深拷贝一份
    this->m_blue_flag = true; //设置标志位
    this->m_threadpool->addJob(_set_led_pwm, (void*)&led_pwm, sizeof(led_pwm));
}