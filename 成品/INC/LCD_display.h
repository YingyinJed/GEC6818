#ifndef _LCD_DISPLAY_H_
#define _LCD_DISPLAY_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

/*-------------------宏定义-----------------*/
//与硬件和像素点配置相关的宏定义
#define     LCD_PATH         "/dev/fb0"  //LCD文件路径
#define     W                800         //LCD屏幕宽
#define     H                480         //LCD屏幕高
#define     LCD_SIZE         W*H*4       //LCD像素点
#define     BMP_SIZE         W*H*3       //BMP显示像素点
//关于显示方向的宏定义
#define     DISPLAY_NODIR       0//定义无方位的显示值
#define     DISPLAY_LEFT        1//定义从左到右的显示值
#define     DISPLAY_RIGHT       2//定义从右到左的显示值
#define     DISPLAY_UP          3//定义从上到下的显示值
#define     DISPLAY_DOWN        4//定义从下到上的显示值   
//宏定义PPT特效的显示速度定义横向的速度
#define     VERY_FAST   8
#define     FAST        4
#define     SLOW        2
#define     VERY_SLOW   1
#define     NO_SPEED    0
/*-------------------定义结构体---------------*/
struct Lcd_Init
{
    int fd_lcd;//LCD文件的文件编号
    int* p_lcd;//LCD显示的内存指针
};

/*-------------------定义函数-----------------*/
void LCD_Init(struct Lcd_Init * LCD);//初始化LCD结构体获得LCD文件编号和LCD映射指针
void LCD_BMPDisplay(struct Lcd_Init * LCD,char * PhotoPath,int Display_Type,int Display_Speed);//以上下左右平移和立即显示的模式显示BMP图片
void LCD_bmp_X_Y(struct Lcd_Init* LCD,char * PhotoPath,int Touch_x,int Touch_y);//在特定的坐标位置开始散开播放BMP图片
void LCD_X_Y(struct Lcd_Init* LCD,char * PhotoPath,int Touch_x,int Touch_y);//在特定的位置显示那一小部分的图片内容
#endif
