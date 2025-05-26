#include "wiringPi.h"
#include "dht11.h"
#include <iostream>
#include <cstring>

using namespace std;

/* 主机发送开始信号*/
void dht11_start(void)
{
    //GPIO改为输出模式
    pinMode(DHT_DATA_PIN, OUTPUT);

    //先拉高30us
    DHT_HIGH;
    delayMicroseconds(30);
    
    //拉低电平至少18ms
    DHT_LOW;
    delayMicroseconds(18000);

    //再拉高20-40us
    DHT_HIGH;
    delayMicroseconds(25);

    pinMode(DHT_DATA_PIN, INPUT);
    pullUpDnControl(DHT_DATA_PIN, PUD_UP);
}


/* 获取一个字节数据*/
unsigned char dht11_rec_byte(void)
{
    unsigned char data;
    for(int i = 0; i < 8; ++i)
    {
        data <<= 1;
        while(DHT_READ_DATA == LOW);
        delayMicroseconds(30);
        
        if(DHT_READ_DATA == HIGH)
        {
            data |= 1;
            //等待高电平结束
            while(DHT_READ_DATA == HIGH);
        }
        //一位读取完毕
    }
    return data;
}


/* 开始获取数据*/
void dht11_rec_data(unsigned char dht_data[])
{

    unsigned char check;
    int sum = 0;

    memset(dht_data, 0, 4);

    //主机发送信号
    dht11_start();

    //等待从机相应
    delayMicroseconds(5);

    while(DHT_READ_DATA == HIGH)
    {
        //cout << "while(DHT_READ_DATA == HIGH)" << endl;
    }

    if(DHT_READ_DATA == LOW)
    {

        while(DHT_READ_DATA == LOW);
        while(DHT_READ_DATA == HIGH);

        //程序到这里开始准备接收数据
        for(int i = 0; i < 4; ++i)
        {
            dht_data[i] = dht11_rec_byte();
            sum += dht_data[i];
        }
        sum %= 256;
        check = dht11_rec_byte();

        delayMicroseconds(55);
        //数据接收完毕
        pinMode(DHT_DATA_PIN, OUTPUT);
        //拉高电平表示空闲
        DHT_HIGH;

        if(sum != check)
        {
            memset(dht_data, 0, 4);    
            cout << "校验失败" << endl;
        }

        cout << "temp get done" << endl;
    }
}


void dht11_over(void)
{
    digitalWrite(DHT_DATA_PIN, LOW);
    delayMicroseconds(1000);
    pinMode(DHT_DATA_PIN, INPUT);
}
