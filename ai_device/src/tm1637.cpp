#include "tm1637.h"
#include "main.h"
#include <wiringPi.h>
#include <iostream>
#include <cstring>
#include <mutex>

using namespace std;



/**
 * @brief TM1637类的构造函数，用于初始化TM1637模块的相关参数和引脚模式。
 * @param clk_pin 时钟引脚编号。
 * @param dio_pin 数据输入输出引脚编号。
 */
TM1637::TM1637(int clk_pin, int dio_pin)
    : m_clk_pin(clk_pin), m_dio_pin(dio_pin), m_brightness(0x88),
      m_display_on(false), m_auto_increment(false)
{
    #ifdef DEBUG
    cout << "TM1637(int, int)" << endl;
    #endif

    // 初始化显示数据
    memset(m_display_buff, 0, sizeof(m_display_buff));

    //初始化锁
    pthread_mutex_init(&this->m_mutex, NULL);
}

TM1637::~TM1637()
{
    #ifdef DEBUG
    cout << "~TM1637(int, int)" << endl;
    #endif
    // 恢复引脚
    pinMode(m_clk_pin, INPUT);
    pullUpDnControl(m_clk_pin, PUD_OFF); // 关闭上拉/下拉电阻
    pinMode(m_dio_pin, INPUT);
    pullUpDnControl(m_dio_pin, PUD_OFF); // 关闭上拉/下拉电阻

    //销毁锁
    pthread_mutex_destroy(&m_mutex);
}

void TM1637::tm1637_init()
{
    #ifdef DEBUG
    cout << "TM1637::init()" << endl;
    #endif

    // 拿锁
    while(pthread_mutex_lock(&m_mutex) != 0)
    {
        static int retry_count = 0;
        retry_count++;
        delayMicroseconds(10000);
        if(retry_count > 500)
        {
            perror("TM1637::init() lock failed");
            return;
        }
    }

    // 设置引脚模式
    pinMode(this->m_clk_pin, OUTPUT);
    pinMode(this->m_dio_pin, OUTPUT);
    // 设置引脚电平
    digitalWrite(this->m_clk_pin, LOW);
    digitalWrite(this->m_dio_pin, LOW);

    // 开启数码管

    for(int i = 0; i < 10; ++i)
    {
        _write_buffer(0, i);
        _write_buffer(1, i);
        _write_buffer(2, i);
        _write_buffer(3, i);
        delayMicroseconds(1000000);
    }
    memset(m_display_buff, 0, sizeof(m_display_buff));
    
    #ifdef DEBUG
    cout << "TM1637::init() success" << endl;
    #endif

    // 释放锁
    pthread_mutex_unlock(&m_mutex);
}

void TM1637::_start()
{
    #ifdef DEBUG
    cout << "TM1637::_start()" << endl;
    #endif

    // 设置端口为输出模式
    pinMode(this->m_clk_pin, OUTPUT);
    pinMode(this->m_dio_pin, OUTPUT);
    // CLK为高电平时，DIO由高变低
    digitalWrite(this->m_clk_pin, HIGH);
    delayMicroseconds(2);
    digitalWrite(this->m_dio_pin, HIGH);
    delayMicroseconds(2);
    digitalWrite(this->m_dio_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(this->m_clk_pin, LOW);
    delayMicroseconds(2);
}

void TM1637::_stop()
{
    #ifdef DEBUG
    cout << "TM1637::_stop()" << endl;
    #endif

    //设置端口为输出模式
    pinMode(this->m_dio_pin, OUTPUT);
    pinMode(this->m_clk_pin, OUTPUT);
    //CLK为高电平时，DIO由低变高
    digitalWrite(this->m_clk_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(this->m_dio_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(this->m_clk_pin, HIGH);
    delayMicroseconds(2);
    digitalWrite(this->m_dio_pin, HIGH);
    delayMicroseconds(2);
}

unsigned char TM1637::_wait_ack()
{
    //设置端口为输入模式
    pinMode(this->m_dio_pin, INPUT);
    //CLK先拉低，然后等待，再拉高，然后再拉低，DIO引脚会自动拉低
    digitalWrite(this->m_clk_pin, LOW);
    delayMicroseconds(5);
    while(digitalRead(this->m_dio_pin) == HIGH)
    {
        static int retry_count = 0;
        retry_count++;
        if(retry_count > 50)
        {
            perror("TM1637::_wait_ack() timeout");
            return 1;
        }
        delayMicroseconds(1);
    }
    //clk拉高
    digitalWrite(this->m_clk_pin, HIGH);
    delayMicroseconds(2);
    //clk拉低
    digitalWrite(this->m_clk_pin, LOW);
    pinMode(this->m_dio_pin, OUTPUT);

    return 0;
}

void TM1637::_write_byte(unsigned char data)
{
    //设置端口为输出模式
    pinMode(this->m_dio_pin, OUTPUT);
    pinMode(this->m_clk_pin, OUTPUT);
    //发送一个字节数据
    for (int i = 0; i < 8; ++i)
    {
        //CLK先拉低
        digitalWrite(this->m_clk_pin, LOW);
        if(data & 0x01)
        {
            //如果数据的最低位为1，DIO拉高
            digitalWrite(this->m_dio_pin, HIGH);
        }
        else
        {
            //如果数据的最低位为0，DIO拉低
            digitalWrite(this->m_dio_pin, LOW);
        }
        delayMicroseconds(3);
        data >>= 1;
        //CLK拉高
        digitalWrite(this->m_clk_pin, HIGH);
        delayMicroseconds(3);
    }
}

void TM1637::_write_buffer(int addr, int data)
{
    #ifdef DEBUG
    cout << "TM1637::_write_buffer()" << endl;
    #endif

    if(addr < 0 || addr > 3)
    {
        perror("TM1637::_write_buffer() addr out of range");
        return;
    }

    if(data >= 10 || data < 0)
    {
        perror("TM1637::_write_buffer() data out of range");
        return;
    }
    //将int类型数据转换为unsigned char类型数据
    this->m_display_buff[addr] = this->code[data];
}

unsigned char TM1637::_send_command(unsigned char cmd)
{
    unsigned char ack = 0;
    _start();
    _write_byte(cmd);
    ack = _wait_ack();
    _stop();
    return ack;
}


/**用户函数 */
unsigned char TM1637::set_brightness(unsigned char brightness)
{
    //检查数据亮度是否正确
    if(brightness < (unsigned char)(brightness_cmd::brightness_1) || 
        brightness > (unsigned char)(brightness_cmd::brightness_8))
    {
        cout << "TM1637::set_brightness() brightness out of range" << endl;
        return 1;
    }
    else
    {
        _send_command(brightness);
    }
}