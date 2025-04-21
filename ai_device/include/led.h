/**
 * @file include/led.h
 * @brief LED control header file
 * @author SIYUAN
 * @date 2025/4/18
 */

#ifndef __LED_H
#define __LED_H

enum class LedColor 
{
    OFF,    // 默认为 0
    RED,    // 默认为 1
    GREEN,  // 默认为 2
    ORANGE  // 默认为 3
};


/**
 * @brief: LED control class
 * @note: This class is used to control the LED on the device
 */
class TricolourLed
{
public:
    int m_red_pin;
    int m_grn_pin;
    int m_org_pin;
public:
    TricolourLed(int m_red_pin, int m_grn_pin, int m_org_pin);
    ~TricolourLed();

    void setColor(LedColor color);

private:
    void _set_led_red(void);
    void _set_led_grn(void);
    void _set_led_org(void);
    void _set_led_off(void);

};

#endif