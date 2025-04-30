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

    // 设置引脚模式
    pinMode(m_clk_pin, OUTPUT);
    pinMode(m_dio_pin, OUTPUT);

    set_display_on();
    delay(100);
    set_brightness(brightness_cmd::brightness_4);
    delay(100);
    set_display_on();

    // 初始化显示数据
    for (int i = 0; i < 10; ++i)
    {
        set_display_data(i*1000+i*100+i*10+i);
        flash_display();
        delay(1000);
    }
    
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

unsigned char TM1637::_set_buffer(unsigned char data, int addr)
{
    #ifdef DEBUG
    cout << "TM1637::_write_buffer()" << endl;
    #endif

    m_display_buff[addr] = data;

    return 0;
}

unsigned char TM1637::_get_buffer(int addr)
{
    //参数检查
    if(addr < 0 || addr > 3)
    {
        cout << "TM1637::_get_buffer() addr out of range" << endl;
        return -1;
    }
    return m_display_buff[addr];
}

unsigned char TM1637::_send_command(unsigned char cmd)
{
    unsigned char ack = 0;
    _start();
    _write_byte(cmd);
    ack = _wait_ack();
    _stop();
    delayMicroseconds(20000);
    return ack;
}

unsigned char TM1637::_set_auto_increment(bool auto_increment)
{
    if(auto_increment)
    {
        //开启地址自增
        return _send_command(0x40);
    }
    else
    {
        //使用固定地址
        return _send_command(0x44);
    }
}


/**用户函数 */
unsigned char TM1637::set_brightness(brightness_cmd brightness)
{

    return (_send_command((unsigned char)brightness) || _send_command(0x88));
    
}

unsigned char TM1637::set_brightness(int brightness)
{
    //检查数据亮度是否正确
    if(brightness < 1 || brightness > 8)
    {
        cout << "TM1637::set_brightness() brightness out of range" << endl;
        return 1;
    }
    else
    {
        auto cmd = static_cast<unsigned char>(
            (unsigned char)(brightness_cmd::brightness_1)
             + brightness - 1);

        return (_send_command(cmd) || _send_command(0x88));
    }
}


unsigned char TM1637::set_display_on(void)
{
    return _send_command(0x88);
}

unsigned char TM1637::set_display_off(void)
{
    return _send_command(0x80);
}


unsigned char TM1637::set_display_data(int data, int addr)
{
    //参数检查
    if(data < 0 || data > 9 || addr < 0 || addr > 3)
    {
        cout << "TM1637::set_display_data() data or addr out of range" << endl;
        return 1;
    }
    else
    
    return _set_buffer(this->m_code[data], addr);
}

unsigned char TM1637::set_display_data(int data)
{
    //参数检查
    if(data < 0 || data > 9999)
    {
        cout << "TM1637::set_display_data() data out of range" << endl;
        return 1;
    }
    else
    {
        int temp  = 0;
        for(int i = 0; i < 4; ++i)
        {
            temp = data % 10;
            _set_buffer(this->m_code[temp], i);
            data /= 10;
        }
    }
    return 0;
}

unsigned char TM1637::flash_display()
{
    int ret = 0;
    _set_auto_increment(true);
    //发送数据
    _start();
    _write_byte(m_addr[0]);
    ret |= _wait_ack();
    for(int i = 0; i < 4; ++i)
    {
        _write_byte(this->m_display_buff[i]);
        ret |= _wait_ack();
    }
    _stop();

    return ret;
}

unsigned char TM1637::clear_display()
{
    //清空显示数据
    set_display_data(0);
    //清空显示
    return flash_display();
}

unsigned char TM1637::set_colon(bool colon)
{
    unsigned ret = 0;

    //向固定地址写入
    if(colon)
    {
        ret = _set_buffer((m_display_buff[1]|0x80), 1);
    }
    else
    {
        ret = _set_buffer((m_display_buff[1]&0x7F), 1);
    }
    
    
    return ret;
}