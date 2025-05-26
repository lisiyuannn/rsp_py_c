#include "digital_disp.h"
#include <iostream>
#include <wiringPi.h>
#include <cstring>

using namespace std;

DigitalDisp::DigitalDisp(int pin_dio, int pin_clk)
    : pin_dio(pin_dio), pin_clk(pin_clk)
{
    cout << "DigitalDisp(int, int)" << endl;
    //设置引脚模式
    pinMode(this->pin_dio, OUTPUT);
    pinMode(this->pin_clk, OUTPUT);
}

DigitalDisp::~DigitalDisp()
{
    cout << "~DigitalDisp(int, int)" << endl;
    //恢复引脚
    pinMode(this->pin_dio, INPUT);
    pullUpDnControl(this->pin_dio, PUD_OFF); // 关闭上拉/下拉电阻
    pinMode(this->pin_clk, INPUT);
    pullUpDnControl(this->pin_clk, PUD_OFF); // 关闭上拉/下拉电阻
}

void DigitalDisp::start(void)
{
    // 设置端口为输出模式
    pinMode(this->pin_dio, OUTPUT);
    pinMode(this->pin_clk, OUTPUT);
    // CLK为高电平时，DIO由高变低
    digitalWrite(this->pin_clk, HIGH);
    digitalWrite(this->pin_dio, HIGH);
    delayMicroseconds(200);
    digitalWrite(this->pin_dio, LOW);
}

void DigitalDisp::stop(void)
{

    //设置端口为输出模式
    pinMode(this->pin_dio, OUTPUT);
    pinMode(this->pin_clk, OUTPUT);
    //CLK为高电平时，DIO由低变高
    digitalWrite(this->pin_clk, LOW);
    delayMicroseconds(200);
    digitalWrite(this->pin_dio, LOW);
    delayMicroseconds(200);
    digitalWrite(this->pin_clk, HIGH);
    delayMicroseconds(200);
    digitalWrite(this->pin_dio, HIGH);

}

void DigitalDisp::wait_ack(void)
{

    //设置端口为输入模式
    pinMode(this->pin_dio, INPUT);
    //CLK先拉低，然后等待，再拉高，然后再拉低，DIO引脚会自动拉低
    digitalWrite(this->pin_clk, LOW);
    delayMicroseconds(500);
    while(digitalRead(this->pin_dio) == HIGH)
    {
        delayMicroseconds(1);
    }
    //clk拉高
    digitalWrite(this->pin_clk, HIGH);
    delayMicroseconds(200);
    //clk拉低
    digitalWrite(this->pin_clk, LOW);
}

void DigitalDisp::write_byte(unsigned char data)
{
    //设置端口为输出模式
    pinMode(this->pin_dio, OUTPUT);
    pinMode(this->pin_clk, OUTPUT);
        //发送一个字节数据
        for (int i = 0; i < 8; ++i)
    {
        //CLK先拉低
        digitalWrite(this->pin_clk, LOW);
        if(data & 0x01)
        {
            //如果数据的最低位为1，DIO拉高
            digitalWrite(this->pin_dio, HIGH);
        }
        else
        {
            //如果数据的最低位为0，DIO拉低
            digitalWrite(this->pin_dio, LOW);
        }
        delayMicroseconds(3);
        data >> 1;
        //CLK拉高
        digitalWrite(this->pin_clk, HIGH);
        delayMicroseconds(3);
    }
}