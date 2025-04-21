'''
    @BREAF: tm1637数码管驱动源码
    @AUTHOR: SIYUAN
    @DATE: 2025/4/14
    @LANGUAGE: PYTHON
'''
import RPi.GPIO as GPIO
import time

# 定义引脚
CLK = 21
DIO = 20
# 初始化GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setup(CLK, GPIO.OUT, initial=GPIO.LOW)
GPIO.setup(DIO, GPIO.OUT, initial=GPIO.LOW)
# 定义显示器的数字0-9
segdata = (0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f)

def start():
    # 设置端口为输出模式
    GPIO.setup((CLK, DIO), GPIO.OUT)
    # CLK为高电平时，DIO由高变低
    GPIO.output(CLK, GPIO.HIGH)
    GPIO.output(DIO, GPIO.HIGH)
    time.sleep(0.0002)
    GPIO.output(DIO, GPIO.LOW)

def stop():
    # 设置端口为输出模式
    GPIO.setup((CLK, DIO), GPIO.OUT)
    # CLK为高电平时，DIO由低变高
    GPIO.output(CLK, GPIO.LOW)
    time.sleep(0.0002)
    GPIO.output(DIO, GPIO.LOW)
    time.sleep(0.0002)
    GPIO.output(CLK, GPIO.HIGH)
    time.sleep(0.0002)
    GPIO.output(DIO, GPIO.HIGH)

# 等待应答
def wait_ack():
    # 设置引脚为输入模式
    GPIO.setup(DIO, GPIO.IN)
    # CLK先拉低，然后等待，再拉高，然后再拉低，DIO引脚会自动拉低
    GPIO.output(CLK, GPIO.LOW)
    time.sleep(0.0005)
    while(GPIO.input(DIO) == GPIO.HIGH):
        pass
    GPIO.output(CLK, GPIO.HIGH)
    time.sleep(0.0002)
    GPIO.output(CLK, GPIO.LOW)

# 发送字节
def write_byte(data):
    GPIO.setup((CLK, DIO), GPIO.OUT)
    for i in range(8):
        GPIO.output(CLK, GPIO.LOW)
        if((data>>i)&0x01):
            GPIO.output(DIO, GPIO.HIGH)
        else:
            GPIO.output(DIO, GPIO.LOW)

        time.sleep(0.0003)
        GPIO.output(CLK, GPIO.HIGH)
        time.sleep(0.0003)
        

# 写显示寄存器，地址自增
def display_inc():
    start()
    # 写0x40表示地址自增，0x44表示固定地址
    write_byte(0x40)
    wait_ack()
    stop()

    start()
    write_byte(0xC0)
    wait_ack()

    # 开始发送数据
    for i in range(10):
        write_byte(segdata[i])
        wait_ack()
    stop()


try:
    while True:
        display_inc()
        time.sleep(1)
        print("显示完成")

except KeyboardInterrupt:
    GPIO.cleanup()
    print("程序结束")

