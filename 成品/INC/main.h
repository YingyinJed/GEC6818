#ifndef _MAIN_H_
#define _MAIN_H_
/*--------------------------头文件引用--------------------------*/
//系统头文件
#include <stdlib.h>
#include <pthread.h>        //引用线程相关函数
#include <unistd.h>
#include <stdio.h>
#include <sys/select.h>
#include <stdlib.h>
#include <time.h>
//自定义头文件
#include "System.h"     //毫秒级延时
#include "Filedir.h"    //目录检索
#include "Touch.h"      //触摸屏
#include "LCD_display.h"//LCD显示
#include "Album.h"      //相册功能
#include "GuaPhoto.h"   //刮图功能
#include "AVI_MP3_.h"   //AVI+MP3功能

#include "Camera.h"     //引入摄像头相关功能
#include "api_v4l2.h"

#include "LED_BEEP.h"   //引入控制LED和BEEP的相关头文件

//引入OneNet平台相关头文件
#include "EdpKit.h"
#include "ConnectOneNet.h"
/*--------------------------头文件引用--------------------------*/

/*--------------------------自定义宏定义--------------------------*/
//需要目录检索的目录
#define SYSTEM_PHOTO_DIR    "/mnt/udisk/ZGT/System/"    //系统图片文件目录 
#define BMP_PHOTO_DIR       "/mnt/udisk/ZGT/Picture/"   //BMP图片文件目录
#define MP3_DIR             "/mnt/udisk/ZGT/MP3/"
#define AVI_DIR             "/mnt/udisk/ZGT/AVI/"
/*--------------------------自定义宏定义--------------------------*/

void* AVI_PLAY(void* AVIDir);

#endif
