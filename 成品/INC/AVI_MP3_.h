#ifndef _AVI_MP3__H_
#define _AVI_MP3__H_

#include "main.h"

/*-------------------宏定义---------------*/
//关于管道文件路径的宏定义
#define FIFOPATH_MP3    "/tmp/MP3"  //MP3的管道文件
#define FIFOPATH_AVI    "/tmp/AVI"  //AVI的管道文件
//关于MPLAYER的相关控制命令
#define MPLAYER_TIPS1   "mplayer -quiet -slave -input file="//MP3的MPLAYER开启前语
#define MPLAYER_TIPS2   "mplayer -quiet -slave -zoom -x 800 -y 400 -input file="//AVI的MPLAYER开启前语
#define MPLAYER_INIT_V  " -af volume=-30 "//设置MPLAYER开启的初始音量
/*-------------------定义结构体---------------*/
//声明已经定义过的结构体
struct Lcd_Init;
struct Touch_val;
struct Filedir;

/*-------------------自定义函数---------------*/
void Fifo_Init(char * FifoPath);//初始化创建对应路径下的管道文件
void WR_Fifo(char * FifoPath,char * command);//对目标管道文件进行写入操作
void* AVI_PLAY(void* AVIDir);//AVI线程执行函数
void AVI_PlayStart(struct Lcd_Init * LCD,struct Touch_val * Touch,struct Filedir * AVIFile,struct Filedir * SystemFile,
                    pthread_t AVI_pid,int * Control_Num);//AVI播放功能函数

void* MP3_PLAY(void* MP3Dir);//MP3线程执行函数
void MP3_PlayStart(struct Lcd_Init * LCD,struct Touch_val * Touch,struct Filedir * MP3File,struct Filedir * SystemFile,
                    pthread_t MP3_pid,int * Control_Num);//MP3播放功能函数
#endif
