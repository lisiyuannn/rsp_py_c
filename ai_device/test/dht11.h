/**
  * @AUTHER: SIYUAN
  * @DATA: 2025/4/16
  * @BREAF: DHT11 driver
  */

#ifndef __DHT11_H
#define __DHT11_H
#include "wiringPi.h"

#define DHT_DATA_PIN 21

#define DHT_HIGH digitalWrite(DHT_DATA_PIN, HIGH)
#define DHT_LOW digitalWrite(DHT_DATA_PIN, LOW)
#define DHT_READ_DATA digitalRead(DHT_DATA_PIN)

void dht11_start(void);
void dht11_rec_data(unsigned char dht_data[]);
unsigned char dht11_rec_byte(void);
void dht11_over(void);

#endif
