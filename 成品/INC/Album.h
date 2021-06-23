#ifndef _ALBUM_H_
#define _ALBUM_H_

#include "main.h"//由于这是依附性功能就不特定引头文件了

/*-------------------定义结构体---------------*/
//声明已经定义过的结构体
struct Lcd_Init;
struct Touch_val;
struct Filedir;
//自定义结构体
struct AUTO_ON_OFF
{
    int FileNum;//记录文件数量
    int * count;//记录现在播到哪张图片
};

#define START_URL   "/ZGT/System/Start/"//开启动画的存储路径
/*-------------------自定义函数---------------*/
void Album_Start(struct Touch_val * Touch,struct Lcd_Init * LCD,struct Filedir * File_Photo,struct Filedir * File_System
                ,int * count,int * Control_Num);//定义相册功能
void * Auto_Num(void * arg);//定义一个自动播放时计数的线程
void Start_System(struct Lcd_Init * LCD);//定义一个用于开机启动动画的功能
#endif
