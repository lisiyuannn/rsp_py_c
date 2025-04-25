#ifndef __TM1637_H
#define __TM1637_H

/**
 * @file include/tm1637.h
 * @brief TM1637 7-segment display driver header file
 * @author SIYUAN
 * @date 2025/4/25
 * @details This file contains the function declarations and macros for controlling
 * the TM1637 7-segment display driver
 * @hardware: 共阳级数码管，灰度8级可调
 */

 #include <pthread.h>


class TM1637
{
private:
    int m_clk_pin; // CLK pin
    int m_dio_pin; // DIO pin
    unsigned char m_brightness; // Brightness level (0-7)
    bool m_display_on; // Display on/off flag
    unsigned char m_display_buff[4]; // Display data for 6 digits
    bool m_auto_increment; // Auto-increment flag

public:
    const unsigned char code[]={0xc0,0xf9,0xa4,0xb0,0x99,   
        0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};

public:
    TM1637(int clk_pin, int dio_pin);
    ~TM1637();
    
    void init(void);
    void set_brightness(int brightness);
    void set_display_on(void);
    void set_display_off(void);
    void set_auto_increment(bool auto_increment);
    void write_data_left(int data);
    void write_data_right(int data);

    void clear_display(void);
public:
    void _update_data(void);
    void _set_brightness(uint_t brightness);
    void _write_buffer(int addr, int data);
    void _set_display_on(bool display_on);
    void _set_auto_increment(bool auto_increment);
    void _start(void);
    void _stop(void);
    void _wait_ack(void);
    void _write_byte(unsigned char data);
    void _send_command(unsigned char cmd);

public:
    //设备锁，拿到锁之后才可以操作设备
    pthread_mutex_t m_mutex;

};

#endif