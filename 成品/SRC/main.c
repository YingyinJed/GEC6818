#include "main.h"
/*---------------------------定义全局变量---------------------------*/
int count_BMP = 0;//定义一个用于记录现在图片播到哪的数字
int Control_Num = CONT_INIT;//定义一个用于分辨从手机接收到的控制命令的数字
/*---------------------------定义全局变量---------------------------*/

int main(int argc, char const *argv[])
{
    /*------------打开路径并读取路径下的相关类型文件---------*/
    struct Filedir * SystemPhotoDir = calloc(1,sizeof(struct Filedir));//定义界面图片目录结构体指针并为其配置内存空间
    struct Filedir * BMPPhotoDir = calloc(1,sizeof(struct Filedir));//定义相册目录结构体指针并为其配置内存空间
    struct Filedir * MP3Dir = calloc(1,sizeof(struct Filedir));  //定义MP3目录结构体指针并为其配置内存空间
    struct Filedir * AVIDir = calloc(1,sizeof(struct Filedir));  //定义AVI目录结构体指针并为其配置内存空间
    struct Filedir * AVICameraDir = calloc(1,sizeof(struct Filedir));//定义相机录制目录结构体指针并为其配置内存空间
    struct Filedir * PhoCameraDir = calloc(1,sizeof(struct Filedir));//定义相机相册目录结构体指针并为其配置内存空间
    Read_SystemDir(SystemPhotoDir,SYSTEM_PHOTO_DIR);//读取系统目录下的文件并进行一定的排序
    Read_Dir(BMPPhotoDir,BMP_PHOTO_DIR,TYPE_BMP);//读取图片路径下的BMP文件
    Read_Dir(MP3Dir,MP3_DIR,TYPE_MP3);//读取图片路径下的内容
    Read_Dir(AVIDir,AVI_DIR,TYPE_AVI);//读取图片路径下的内容
    Read_Dir(AVICameraDir,url_video,TYPE_AVI);//读取video目录下的AVI文件
    Read_Dir(PhoCameraDir,PHOTO_url,TYPE_JPG);//读取相册目录下的JPG文件
    /*------------打开路径并读取路径下的相关类型文件---------*/

    /*------------------------初始化打开特定的文件---------------------*/
    struct Touch_val *Touch = calloc(1,sizeof(struct Touch_val));//定义触摸屏结构体指针并为其配置内存空间
    struct Lcd_Init *LCD = calloc(1,sizeof(struct Lcd_Init));//定义LCD结构体指针并为其配置内存空间
    Touch_Init(Touch);//获取打开触摸屏文件的文件编号
    LCD_Init(LCD);//获取打开LCD文件的文件编号，并为LCD显示的像素点配置内存映射
    LED_CONT_OFF();//初始化的时候先关闭LED
    OneNet_Init();//初始化OneNet平台
    Fifo_Init(FIFOPATH_AVI);//查看是否存在AVI的管道文件不存在则建立存在则不操作
    Fifo_Init(FIFOPATH_MP3);//查看是否存在MP3的管道文件不存在则建立存在则不操作
    /*------------------------初始化打开特定的文件---------------------*/

    /*------------------------定义线程相关---------------------*/
    pthread_t Touch_pid;//定义一个用于扫描触摸屏的线程
    pthread_t AVI_pid;//定义一个用于AVI播放的线程
    pthread_t MP3_pid;//定义一个用于MP3播放的线程
    pthread_t Camera_pid;//定义一个相机将JPG转换为AVI的线程
    pthread_create(&Touch_pid,NULL,Touch_SCAN,(void *)Touch);//配置扫描触摸屏线程
    /*------------------------定义线程相关---------------------*/

    /*------------------------主函数应用---------------------*/
    OneNet_Start(&Control_Num);
    Phone_Network(LCD,Touch,&Control_Num);//启动手机服务器
    Start_System(LCD);//启动开机动画
    LCD_bmp_X_Y(LCD,SystemPhotoDir->FilePath[BACKGROUND_NUM],400,240);//显示主界面图
    while (1)//进入程序大循环
    {
        //摄像头
        if ((Touch->x > 35 && Touch->x < 155 && Touch->y > 360 && Touch->y < 470 && Touch->Touch_leave == 1)
            ||(Control_Num == GET_VIDEO))
        { 
           
            Camera_Start(LCD,Touch,SystemPhotoDir,Camera_pid,AVI_pid,AVICameraDir,PhoCameraDir,&Control_Num); //启动摄像头功能
        }
        //MP3播放
        else if ((Touch->x > 235 && Touch->x < 365 && Touch->y > 360 && Touch->y < 470 && Touch->Touch_leave == 1)
            ||(Control_Num == MUSIC_PLAY))
        {
            MP3_PlayStart(LCD,Touch,MP3Dir,SystemPhotoDir,MP3_pid,&Control_Num);//调用启动MP3播放功能
            LCD_BMPDisplay(LCD,SystemPhotoDir->FilePath[BACKGROUND_NUM],DISPLAY_DOWN,FAST);//显示主界面图
        }
        //AVI播放
        else if ((Touch->x > 455 && Touch->x < 570 && Touch->y > 360 && Touch->y < 470 && Touch->Touch_leave == 1)
                ||(Control_Num == ENTER_KUGOU))
        { 
            AVI_PlayStart(LCD,Touch,AVIDir,SystemPhotoDir,AVI_pid,&Control_Num); //调用启动AVI播放器
            LCD_BMPDisplay(LCD,SystemPhotoDir->FilePath[BACKGROUND_NUM],DISPLAY_DOWN,FAST);//显示主界面图
        }
        //BMP图像显示
        else if ((Touch->x > 650 && Touch->x < 770 && Touch->y > 360 && Touch->y < 470 && Touch->Touch_leave == 1)
                ||(Control_Num == BACK))
        { Album_Start(Touch,LCD,BMPPhotoDir,SystemPhotoDir,&count_BMP,&Control_Num); }//调用启动相册功能
        //刮图
        else if (Touch->x > 665 && Touch->x < 755 && Touch->y > 20 && Touch->y < 135 && Touch->Touch_leave == 1)
        { GuaPhotoStart(LCD,Touch,BMPPhotoDir,SystemPhotoDir); }//调用启动刮刮乐功能函数
        
        //开启LED
        else if (Control_Num == LED_ON)
        {
            Control_Num = CONT_INIT;//清除标志位
            LED_CONT_ON();
        }
        //关闭LED
        else if (Control_Num == LED_OFF)
        {
            Control_Num = CONT_INIT;//清除标志位
            LED_CONT_OFF();
        }
        //测试蜂鸣器
        else if (Control_Num == BEEP)
        {
            Control_Num = CONT_INIT;//清除标志位
            BEEP_Test();
        }
    }
    return 0;
}
