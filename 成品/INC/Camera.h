#ifndef __CAMERA_H__
#define __CAMERA_H__

#define LCD_WIDTH  			800
#define LCD_HEIGHT 			480
#define FB_SIZE				(LCD_WIDTH * LCD_HEIGHT * 4)
#include "main.h"

//以下属于网络通信需要引用的头文件
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
/*--------------------------自定义宏定义--------------------------*/
#define url_pic "/mnt/udisk/ZGT/Camera/pic/"//定义删除PIC缓存的命令
#define url_video "/mnt/udisk/ZGT/Camera/video/"//删除缓存区域的video文件 
#define JPGToVideo "ffmpeg -f image2 -i "//执行命令将捕捉的JPG转化为AVI
#define AVI_PLAYplace   "mplayer -slave -quiet -input file=/tmp/AVI -geometry 80:0 -zoom -x 640 -y 480 "//AVI的播放命令
                                                                    //定义播放位置和播放大小
#define PHOTO_url   "/mnt/udisk/ZGT/Camera/Photo/"//定义拍摄后的照片的存储路径
//定义从手机接收到的命令应该转化为的数字值
#define CONT_INIT           0   //定义一个用于清零的控制数字         清0标志位
#define MUSIC_PLAY          1   //定义命令MUSIC_PLAY的控制数字      音乐播放
#define GET_VIDEO           2   //定义命令GET_VIDEO的控制数字       获得摄像头数据
#define BACK                3   //定义命令BACK的控制数字            返回
#define ENTER_KUGOU         4   //定义ENTER_KUGOU的控制数字         打开MP3播放器
#define MUSIC_STOP_CONT     5   //定义MUSIC_STOP_CONT的控制数字     播放暂停/继续
#define MUSIC_PREV          6   //定义MUSIC_PREV的控制数字          播放上一首
#define MUSIC_NEXT          7   //定义MUSIC_NEXT的控制数字          播放下一首
#define LED_ON              8   //定义LED_ON 的控制数字             开灯
#define LED_OFF             9   //定义LED_OFF的控制数字             关灯
#define BEEP                10  //定义BEEP的控制数字                蜂鸣器
/*-------------------定义结构体---------------*/
//声明已经定义过的结构体
struct Lcd_Init;
struct Touch_val;
struct Filedir;
struct FrameBuffer;//定义一个摄像头数据相关的结构体指针并为其分配内存
unsigned long file_size_get(const char *pfile_path);//给出文件大小
int lcd_draw_jpg(unsigned int x,unsigned int y,const char *pjpg_path,char *pjpg_buf,unsigned int jpg_buf_size,unsigned int jpg_half
                ,struct Lcd_Init * LCD);//在LCD上显示JPG文件
int lcd_draw_jpg_in_jpg(unsigned int x,unsigned int y,const char *pjpg_path,char *pjpg_buf,unsigned int jpg_buf_size,struct Lcd_Init * LCD);

void Camera_Start(struct Lcd_Init *LCD,struct Touch_val * Touch,struct Filedir * SystemFile,pthread_t Camera_pid
                    ,pthread_t AVI_pid,struct Filedir * AVICameraDir,struct Filedir * PhoCameraDir,int * Control_Num);//开启摄像头功能
void * Camera_AVI(void * arg);//摄像到的JPG内容转化为AVI的线程

//以下属于网络通信相关函数
void Phone_Network(struct Lcd_Init *LCD,struct Touch_val * Touch,int * Control_Num);//启动手机控制网络的服务
void *real_time_jpg();//定义一个在摄像头功能中给手机发送摄像头信息的线程
void *OneNetReacv(void *arg);//定义一个用于OneNet接收信息的线程
void * Send_OneNet(void * arg);//定义一个用于OneNet发送信息的线程
void OneNet_Start(int * Control_Num);
#endif
