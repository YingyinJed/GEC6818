#include "LED_BEEP.h"
/*
    函 数 名：LED_CONT_ON
    作    用：控制开发板的4个LED显示
    输入参数：无
    返    回：无
*/
void LED_CONT_ON()
{
    int fd = open("/dev/Led",O_RDWR);//打开设备下的LED，成功返回0
    if(fd < 0)
	{perror("Can not open /dev/LED\n");}
    ioctl(fd, LED1, 0);  //2ON
    ioctl(fd, LED2, 0);  //2ON
    ioctl(fd, LED3, 0);  //2ON
    ioctl(fd, LED4, 0);  //2ON
}
/*
    函 数 名：LED_CONT_OFF
    作    用：控制开发板的4个LED关闭
    输入参数：无
    返    回：无
*/
void LED_CONT_OFF()
{
    int fd = open("/dev/Led",O_RDWR);//打开设备下的LED，成功返回0
    if(fd < 0)
	{perror("Can not open /dev/LED\n");}
    ioctl(fd, LED1, 1);  //2OFF
    ioctl(fd, LED2, 1);  //2OFF
    ioctl(fd, LED3, 1);  //2OFF
    ioctl(fd, LED4, 1);  //2OFF
}
/*
    函 数 名：BEEP_Test
    作    用：测试蜂鸣器，让蜂鸣器响一声
    输入参数：无
    返    回：无
*/
void BEEP_Test()
{
    int fd = open("/dev/beep", O_RDWR);
    if(fd < 0){
		perror("open:");
	}
    ioctl(fd, 0, 1);                //BUZZER on	
    usleep(65535);
    ioctl(fd, 1, 1);                 //BUZZER off
    usleep(65535);	
	close(fd);
}
