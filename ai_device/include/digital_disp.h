/**
 * @file include/led.h
 * @author: SIYUAN
 * @date: 2025-4-23
 * @brief: Tm1637 digital display driver header file
 * @details: This file contains the function declarations and macros for controlling
 */

#ifndef __DIGITAL_DISP_H
#define __DIGITAL_DISP_H

class DigitalDisp
{
private:
    int pin_dio;
    int pin_clk;

public:
    DigitalDisp(int pin_dio, int pin_clk);
    ~DigitalDisp();

    void start(void);
    void stop(void);
    void wait_ack(void);
    void write_byte(unsigned char data);
    void display_inc(void);
};

#endif