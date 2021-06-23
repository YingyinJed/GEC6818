#include "AVI_MP3_.h"
void * thr_exit;//定义一个空指针用于退出线程
int count_MP3 = 0;//定义一个用于记录现在MP3播到哪的数字
int count_AVI = 0;//定义一个用于记录现在AVI播到哪的数字
int AVI_pid_Num = 1;//定义一个控制AVI线程的数字,初始化为1让它自动播放下一首
int MP3_pid_Num = 1;//定义一个控制MP3线程的数字，初始化为1让它自动播放下一首
/*
    函数名  ：Fifo_Init
    作  用  ：查看管道文件是否存在，如果不存在则创建管道文件
    输入参数：fifopath为管道的创建路径
    返 回 值：无
*/
void Fifo_Init(char * FifoPath)
{
    // 检查管道文件是否存在
    if (access(FifoPath,F_OK))
    {
         // 若不存在则创建管道文件创建管道文件   mkfifo( );
        if(mkfifo(FifoPath,0666))
        {perror("mkfifo  error ");} 
    }
}
/*
    函数名  ：Mk_Fifo
    作  用  ：在相应的管道文件内写入相应的命令
    输入参数：fifopath为管道的创建路径，command为写入管道的内容
    返 回 值：无
*/
void WR_Fifo(char * FifoPath,char * command)
{
    //打开管道文件   open（）；
    int fd_fifo = open( FifoPath , O_RDWR); // 如果管道只有读者、写者将阻塞在open函数中 ， 使用O_RDWR 则可以避免该阻塞出现
    if (-1 == fd_fifo)
    {perror("open fifo error");}
    //读取、写入数据 read/write
    int ret_val = write(fd_fifo , command, strlen(command) );
    if (ret_val <= 0 )
    {perror("write error");}
    //关闭管道文件
    close(fd_fifo);
}
/*
    函数名  ：AVI_PlayStart
    作  用  ：进入AVI播放功能
    输入参数：LCD       结构体指针
             Touch     触摸屏结构体指针
             AVIFile    AVI文件目录结构体指针
             SystemFile 系统文件目录结构体指针
             AVI_pid    在主函数声明的相关线程
    返 回 值：无
*/
void AVI_PlayStart(struct Lcd_Init * LCD,struct Touch_val * Touch,struct Filedir * AVIFile,struct Filedir * SystemFile,
                    pthread_t AVI_pid,int * Control_Num)
{
    LCD_bmp_X_Y(LCD,SystemFile->FilePath[AVI_STOP_NUM],Touch->x,Touch->y);//进入AVI播放界面
    Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
    (* Control_Num) = CONT_INIT;//清除标志位
    pthread_create(&AVI_pid,NULL,AVI_PLAY,(void *)AVIFile);//配置AVI相关线程
    sleep(1);//延时一小会
    WR_Fifo(FIFOPATH_AVI,"pause\n");//配置完后先输入暂停命令让它不要播放
    int flag_AVI = 0;//定义一个播放AVI的标志位
    while (1)
    {
        usleep(2000);//延时一小会
        if ((Touch->x > 335 && Touch->x < 450 && Touch->y > 370 && Touch->Touch_leave == 1)
            ||((* Control_Num) == MUSIC_STOP_CONT))//暂停/播放
        {
            if (flag_AVI == 0)//如果视频没有开始播放
            {
                flag_AVI = 1;//改变播放状态
                LCD_BMPDisplay(LCD,SystemFile->FilePath[AVI_PLAY_NUM],DISPLAY_NODIR,NO_SPEED);//LCD显示视频播放界面
            }else
            {
                flag_AVI = 0;//改变播放状态
                LCD_BMPDisplay(LCD,SystemFile->FilePath[AVI_STOP_NUM],DISPLAY_NODIR,NO_SPEED);//LCD显示视频暂停界面
            }
            (* Control_Num) = CONT_INIT;//清除标志位
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            printf("pause\n");//打印命令
            WR_Fifo(FIFOPATH_AVI,"pause\n");//执行控制命令
        }
        else if ((Touch->x > 530 && Touch->x < 620 && Touch->y > 380 && Touch->Touch_leave == 1)
            ||((* Control_Num) == MUSIC_NEXT))//下一曲
        {
            flag_AVI = 1;//改变播放状态
            (* Control_Num) = CONT_INIT;//清除标志位
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            LCD_BMPDisplay(LCD,SystemFile->FilePath[AVI_PLAY_NUM],DISPLAY_NODIR,NO_SPEED);//LCD显示视频播放界面
            AVI_pid_Num = 1;//改变线程执行方式
            printf("Next AVI Play\n");//打印控制命令
            WR_Fifo(FIFOPATH_AVI,"quit\n");//执行控制命令
        }
        else if ((Touch->x > 180 && Touch->x < 280 && Touch->y > 380 && Touch->Touch_leave == 1)
            ||((* Control_Num) == MUSIC_PREV))//上一曲
        {
            flag_AVI = 1;//改变播放状态
            (* Control_Num) = CONT_INIT;//清除标志位
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            LCD_BMPDisplay(LCD,SystemFile->FilePath[AVI_PLAY_NUM],DISPLAY_NODIR,NO_SPEED);//LCD显示视频播放界面
            AVI_pid_Num = 2;//改变线程执行方式
            printf("Next AVI Play\n");//打印控制命令
            WR_Fifo(FIFOPATH_AVI,"quit\n");//执行控制命令
        }
        else if (Touch->move_dir ==  TOUCH_LEFT && Touch->Touch_leave == 1)//快退10s
        {
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            LCD_BMPDisplay(LCD,SystemFile->FilePath[AVI_PLAY_NUM],DISPLAY_NODIR,NO_SPEED);//LCD显示视频播放界面
            printf("seek -10\n");//打印控制命令
            WR_Fifo(FIFOPATH_AVI,"seek -10\n");//执行控制命令
        }
        else if (Touch->move_dir ==  TOUCH_RIGHT && Touch->Touch_leave == 1)//快进10s
        {
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            LCD_BMPDisplay(LCD,SystemFile->FilePath[AVI_PLAY_NUM],DISPLAY_NODIR,NO_SPEED);//LCD显示视频播放界面
            printf("seek 10\n");//打印控制命令
            WR_Fifo(FIFOPATH_AVI,"seek 10\n");//执行控制命令
        }
        else if ((Touch->x > 600 && Touch->x < 800 && Touch->y < 400 && Touch->move_dir ==  TOUCH_UP && Touch->Touch_leave == 1)
               ||(Touch->x > 0 && Touch->x < 200 && Touch->y < 400 &&Touch->move_dir ==  TOUCH_UP && Touch->Touch_leave == 1)
               ||(Touch->x > 685 && Touch->x < 800 && Touch->y > 390 && Touch->Touch_leave == 1))//音量+10
        {
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            LCD_BMPDisplay(LCD,SystemFile->FilePath[AVI_PLAY_NUM],DISPLAY_NODIR,NO_SPEED);//LCD显示视频播放界面
            printf("volume 10\n");//打印控制命令
            WR_Fifo(FIFOPATH_AVI,"volume 10\n");//执行控制命令
        }
        else if ((Touch->x > 600 && Touch->x < 800 && Touch->y < 400 && Touch->move_dir ==  TOUCH_DOWN && Touch->Touch_leave == 1)
               ||(Touch->x > 0 && Touch->x < 200 && Touch->y < 400 && Touch->move_dir ==  TOUCH_DOWN && Touch->Touch_leave == 1)
               ||(Touch->x > 35 && Touch->x < 120 && Touch->y > 390 && Touch->Touch_leave == 1))//音量-10       
        {
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            LCD_BMPDisplay(LCD,SystemFile->FilePath[AVI_PLAY_NUM],DISPLAY_NODIR,NO_SPEED);//LCD显示视频播放界面
            printf("volume -10\n");//打印控制命令
            WR_Fifo(FIFOPATH_AVI,"volume -10\n");//执行控制命令
        }
        else if ((Touch->x > 200 && Touch->x < 600 && Touch->move_dir ==  TOUCH_UP && Touch->Touch_leave == 1)
                ||((* Control_Num) == BACK))//退出并返回主菜单
        {
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            (* Control_Num) = CONT_INIT;//清除标志位
            AVI_pid_Num = 5;//线程控制标志位为5代表退出线程
            printf("Quit AVI\n");//打印控制命令
            WR_Fifo(FIFOPATH_AVI,"quit\n");//执行控制命令
            break;//并跳出AVI控制的
        }
        else if(Touch->move_dir != TOUCH_INIT && Touch->Touch_leave == 1)//上面都判断了还是出现了移动操作肯定是误操作
        {
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
        }
    }
}

/*
    函数名  ：AVI_PLAY
    作  用  ：AVI播放的线程
    输入参数：AVIDir 是AVI目录的结构体指针强制转换而来
    返 回 值：空指针
*/
void* AVI_PLAY(void* AVIDir)
{
    struct Filedir * AVIDIR;
    AVIDIR = AVIDir;//传参完成类型转换
    char command[130];
    while (1)
    {
        //播放前置，管道路径，AVI文件路径，初始声音设置
        sprintf(command,"%s%s %s%s",MPLAYER_TIPS2,FIFOPATH_AVI,AVIDIR->FilePath[count_AVI],MPLAYER_INIT_V);
        printf("%s\n",command);//打印命令看是否有问题
        //让系统运行播放器
        system(command);
        //执行完命令后即使清除命令缓存区的内容
        sprintf(command,"");
        printf("------------------------Play end------------------------\n");//打印这个播放结束的信息
        if (AVI_pid_Num == 1)//收到播放下一曲的信息
        {
            count_AVI++;//播放计数+1
            if (count_AVI > AVIDIR->FileNum - 1)
            {count_AVI = 0;}
        }
        else if (AVI_pid_Num == 2)//收到播放上一曲的信息
        {
            AVI_pid_Num = 1;//恢复线程执行标志位
            count_AVI--;//播放计数-1
            if (count_AVI < 0)
            {count_AVI = AVIDIR->FileNum - 1;}
        }
        else if (AVI_pid_Num == 5)//收到退出线程的AVI线程的信息
        {
            AVI_pid_Num = 1;//恢复线程执行标志位
            printf("------------------------Quit AVIPLAYER------------------------\n");//打印退出信息
            pthread_exit(thr_exit);//退出该线程，并将该线程退出信息存储在空指针里
        }
    }
}
/*
    函数名  ：MP3_PlayStart
    作  用  ：开启MP3播放功能
    输入参数：
            LCD         LCD结构体指针，用于操作LCD显示
            Touch       触摸屏结构体指针，用于获取触摸屏坐标
            MP3File     MP3目录结构体指针，用于获取MP3文件路径
            SystemFile  系统图片结构体指针，用于获取界面图片路径
            MP3_pid     MP3线程
            Control_Num 用于获取来自网络通信的控制数字
    返 回 值：空指针
*/
void MP3_PlayStart(struct Lcd_Init * LCD,struct Touch_val * Touch,struct Filedir * MP3File,struct Filedir * SystemFile,
                    pthread_t MP3_pid,int * Control_Num)
{
    (* Control_Num) = CONT_INIT;//清除标志位
    LCD_bmp_X_Y(LCD,SystemFile->FilePath[MUSIC_STOP_NUM],Touch->x,Touch->y);//进入MP3播放界面
    Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
    pthread_create(&MP3_pid,NULL,MP3_PLAY,(void *)MP3File);//配置AVI相关线程
    sleep(1);//延时一小会
    WR_Fifo(FIFOPATH_MP3,"pause\n");//配置完后先输入暂停命令让它不要播放
    int flag_MP3 = 0;//定义一个播放AVI的标志位
    while (1)
    {
        usleep(2000);//延时一小会
        if ((Touch->x > 345 && Touch->x < 460 && Touch->y > 380 && Touch->y < 480 && Touch->Touch_leave == 1)
            ||((* Control_Num) == MUSIC_STOP_CONT))//暂停/播放
        {
            if (flag_MP3 == 0)//如果视频没有开始播放
            {
                flag_MP3 = 1;//改变播放状态
                LCD_BMPDisplay(LCD,SystemFile->FilePath[MUSIC_PLAY_NUM],DISPLAY_NODIR,NO_SPEED);//LCD显示视频播放界面
            }else
            {
                flag_MP3 = 0;//改变播放状态
                LCD_BMPDisplay(LCD,SystemFile->FilePath[MUSIC_STOP_NUM],DISPLAY_NODIR,NO_SPEED);//LCD显示视频暂停界面
            }
            (* Control_Num) = CONT_INIT;//清除标志位
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            printf("pause\n");//打印命令
            WR_Fifo(FIFOPATH_MP3,"pause\n");//执行控制命令
        }
        else if ((Touch->x > 540 && Touch->x < 630 && Touch->y > 390 && Touch->y < 480 && Touch->Touch_leave == 1)
                ||((* Control_Num) == MUSIC_NEXT))//下一曲
        {
            flag_MP3 = 1;//改变播放状态
            (* Control_Num) = CONT_INIT;//清除标志位
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            LCD_BMPDisplay(LCD,SystemFile->FilePath[MUSIC_PLAY_NUM],DISPLAY_NODIR,NO_SPEED);//LCD显示音乐播放界面
            MP3_pid_Num = 1;//改变线程执行方式
            printf("Next MP3 Play\n");//打印控制命令
            WR_Fifo(FIFOPATH_MP3,"quit\n");//执行控制命令
        }
        else if ((Touch->x > 195 && Touch->x < 285 && Touch->y > 390 && Touch->y < 480 && Touch->Touch_leave == 1)
                ||((* Control_Num) == MUSIC_PREV))//上一曲
        {
            flag_MP3 = 1;//改变播放状态
            (* Control_Num) = CONT_INIT;//清除标志位
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            LCD_BMPDisplay(LCD,SystemFile->FilePath[MUSIC_PLAY_NUM],DISPLAY_NODIR,NO_SPEED);//LCD显示音乐播放界面
            MP3_pid_Num = 2;//改变线程执行方式
            printf("Last MP3 Play\n");//打印控制命令
            WR_Fifo(FIFOPATH_MP3,"quit\n");//执行控制命令
        }
        else if (Touch->x > 690 && Touch->x < 800 && Touch->y > 390 && Touch->y < 480 && Touch->Touch_leave == 1)//快进10S
        {
            flag_MP3 = 1;//改变播放状态
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            LCD_BMPDisplay(LCD,SystemFile->FilePath[MUSIC_PLAY_NUM],DISPLAY_NODIR,NO_SPEED);//LCD显示音乐播放界面
            printf("seek 10\n");//打印控制命令
            WR_Fifo(FIFOPATH_MP3,"seek 10\n");//执行控制命令
        }
        else if (Touch->x > 40 && Touch->x < 125 && Touch->y > 390 && Touch->y < 480 && Touch->Touch_leave == 1)//快退10S
        {
            flag_MP3 = 1;//改变播放状态
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            LCD_BMPDisplay(LCD,SystemFile->FilePath[MUSIC_PLAY_NUM],DISPLAY_NODIR,NO_SPEED);//LCD显示音乐播放界面
            printf("seek -10\n");//打印控制命令
            WR_Fifo(FIFOPATH_MP3,"seek -10\n");//执行控制命令
        }
        else if (Touch->x > 695 && Touch->x < 775 && Touch->y > 60 && Touch->y < 145 && Touch->Touch_leave == 1)//音量+10
        {
            flag_MP3 = 1;//改变播放状态
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            LCD_BMPDisplay(LCD,SystemFile->FilePath[MUSIC_PLAY_NUM],DISPLAY_NODIR,NO_SPEED);//LCD显示音乐播放界面
            printf("volume 10\n");//打印控制命令
            WR_Fifo(FIFOPATH_MP3,"volume 10\n");//执行控制命令
        }
        else if (Touch->x > 695 && Touch->x < 775 && Touch->y > 230 && Touch->y < 305 && Touch->Touch_leave == 1)//音量-10
        {
            flag_MP3 = 1;//改变播放状态
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            LCD_BMPDisplay(LCD,SystemFile->FilePath[MUSIC_PLAY_NUM],DISPLAY_NODIR,NO_SPEED);//LCD显示音乐播放界面
            printf("volume -10\n");//打印控制命令
            WR_Fifo(FIFOPATH_MP3,"volume -10\n");//执行控制命令
        }
        else if ((Touch->move_dir ==  TOUCH_UP && Touch->Touch_leave == 1)
                ||((* Control_Num) == BACK))//返回主菜单
        {
            (* Control_Num) = CONT_INIT;//清除标志位
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            MP3_pid_Num = 5;//线程控制标志位为5代表退出线程
            printf("Quit MP3\n");//打印控制命令
            WR_Fifo(FIFOPATH_MP3,"quit\n");//执行控制命令
            break;//并跳出MP3控制的循环
        }
        else if(Touch->move_dir != TOUCH_INIT && Touch->Touch_leave == 1)//上面都判断了还是出现了移动操作肯定是误操作
        {
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
        }
    }
}
/*
    函数名  ：MP3_PLAY
    作  用  ：MP3播放的线程
    输入参数：MP3Dir 是AVI目录的结构体指针强制转换而来
    返 回 值：空指针
*/
void* MP3_PLAY(void* MP3Dir)
{
    struct Filedir * MP3DIR;
    MP3DIR = MP3Dir;//传参完成类型转换
    char command[130];
    while (1)
    {
        //播放前置，管道路径，AVI文件路径，初始声音设置
        sprintf(command,"%s%s %s%s",MPLAYER_TIPS1,FIFOPATH_MP3,MP3DIR->FilePath[count_MP3],MPLAYER_INIT_V);
        printf("%s\n",command);//打印命令看是否有问题
        //让系统运行播放器
        system(command);
        //执行完命令后即使清除命令缓存区的内容
        sprintf(command,"");
        printf("------------------------Play end------------------------\n");//打印这个播放结束的信息
        if (MP3_pid_Num == 1)//收到播放下一曲的信息
        {
            count_MP3++;//播放计数+1
            if (count_MP3 > MP3DIR->FileNum - 1)
            {count_MP3 = 0;}
        }
        else if (MP3_pid_Num == 2)//收到播放上一曲的信息
        {
            MP3_pid_Num = 1;//恢复线程执行标志位
            count_MP3--;//播放计数-1
            if (count_MP3 < 0)
            {count_MP3 = MP3DIR->FileNum - 1;}
        }
        else if (MP3_pid_Num == 5)//收到退出线程的AVI线程的信息
        {
            MP3_pid_Num = 1;//恢复线程执行标志位
            printf("------------------------Quit AVIPLAYER------------------------\n");//打印退出信息
            pthread_exit(thr_exit);//退出该线程，并将该线程退出信息存储在空指针里
        }
    }
    
}
