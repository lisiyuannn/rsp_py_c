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
 #include <main.h>

 enum class brightness_cmd:unsigned char
 {
     brightness_1 = 0x80,
     brightness_2 = 0x81,
     brightness_3 = 0x82,
     brightness_4 = 0x83,
     brightness_5 = 0x84,
     brightness_6 = 0x85,
     brightness_7 = 0x86,
     brightness_8 = 0x87
 };


class TM1637
{
private:
    int m_clk_pin; // CLK pin
    int m_dio_pin; // DIO pin
    unsigned char m_brightness; // Brightness level (0-7)
    bool m_display_on; // Display on/off flag
    unsigned char m_display_buff[4]; // Display data for 6 digits
    bool m_auto_increment; // Auto-increment flag
    unsigned char m_addr[4] = {0xc0, 0xc1, 0xc2, 0xc3}; // Address for each digit

public:
    

public:
    const unsigned char m_code[16]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,
        0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};

public:
    TM1637(int clk_pin, int dio_pin);
    ~TM1637(void);
    
    void tm1637_init(void);
    
    /**测试完成 */
    unsigned char set_brightness(brightness_cmd brightness);
    unsigned char set_brightness(int brightness);
    unsigned char set_display_on(void);
    unsigned char set_display_off(void);
    unsigned char clear_display(void);
    unsigned char set_display_data(int data, int addr);
    unsigned char set_display_data(int data);
    unsigned char flash_display();
    
    
    /** */
    //冒号开关
    unsigned char set_colon(bool colon);
    

    
public:
    /**测试完成 */
    void _start(void);
    void _stop(void);
    unsigned char _wait_ack(void);
    void _write_byte(unsigned char data);
    unsigned char _set_auto_increment(bool auto_increment);
    unsigned char _set_buffer(unsigned char data, int addr);
    unsigned char _get_buffer(int addr);
    unsigned char _send_command(unsigned char cmd);
    /** */
    

public:
    //设备锁，拿到锁之后才可以操作设备
    pthread_mutex_t m_mutex;

};

#endif