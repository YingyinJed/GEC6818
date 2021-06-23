#include "Album.h"
int AUTO_CON = 0;//自动播放的控制标志位
void * auto_exit;
/*
    函数名  ：Album_Start
    作  用  ：开启相册功能
    输入参数：
                Touch       触摸屏结构体指针，获取坐标值和操作状态
                LCD         LCD结构体指针，用于操作LCD显示
                File_Photo  显示图片的文件目录指针
                File_System 系统文件目录结构体指针打开界面相关图片
                count       获取网络通信控制数字指针
    返 回 值：无
*/
void Album_Start(struct Touch_val * Touch,struct Lcd_Init * LCD,struct Filedir * File_Photo,struct Filedir * File_System,int * count,int * Control_Num)
{
    LCD_bmp_X_Y(LCD,File_System->FilePath[BMPPHOTO_TIP],Touch->x,Touch->y);//显示相册操作提示图
    Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
    (* Control_Num) = CONT_INIT;//清除标志位
    while (1)
    {
        if ((Touch->x != 0 && Touch->y != 0 && Touch->Touch_leave == 1)
            ||(* Control_Num) != CONT_INIT)//只有触摸后并松手才能退出提示界面
        {
            Touch->x = Touch->y = Touch->Touch_leave = 0;//清除所有标志位
            (* Control_Num) = CONT_INIT;//清除标志位
            break;
        }
    }
    LCD_BMPDisplay(LCD,File_Photo->FilePath[*count],DISPLAY_NODIR,NO_SPEED);//显示当前图片
    while (1)
    {
        usleep(2000);//延时一小会
        if ((Touch->move_dir == TOUCH_RIGHT && Touch->Touch_leave == 1)
            ||(* Control_Num) == MUSIC_PREV)   //上一张  
        {
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            (* Control_Num) = CONT_INIT;//清除标志位
            (*count)--; 
            if ((*count) < 0)//如果已经到第一张了则去到最后一张
            { (*count) = File_Photo->FileNum - 1; }
            printf("Last Photo\n");//打印显示信息
            printf("now the PhotoNum is %d\n",*count);
            printf("now the Photo's url is :%s\n",File_Photo->FilePath[*count]);
            LCD_BMPDisplay(LCD,File_Photo->FilePath[*count],DISPLAY_LEFT,VERY_FAST);//显示当前图片
        }
        else if ((Touch->move_dir ==  TOUCH_LEFT && Touch->Touch_leave == 1)
                ||(* Control_Num) == MUSIC_NEXT)   //下一张
        {
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            (* Control_Num) = CONT_INIT;//清除标志位
            (*count)++;
            if ((*count) > File_Photo->FileNum - 1)//如果已经时最后一张了则回到第一张
            {(*count) = 0;}
            printf("Next Photo\n");//打印显示信息
            printf("now the PhotoNum is %d\n",*count);
            printf("now the Photo's url is :%s\n",File_Photo->FilePath[*count]);
            LCD_BMPDisplay(LCD,File_Photo->FilePath[*count],DISPLAY_RIGHT,VERY_FAST);//显示当前图片
        }
        else if ((Touch->move_dir ==  TOUCH_DOWN && Touch->Touch_leave == 1)
            ||(* Control_Num) == MUSIC_STOP_CONT)//进入自动播放模式
        {
            (* Control_Num) = CONT_INIT;//清除标志位
            struct AUTO_ON_OFF * AUTO = calloc(1,sizeof(struct AUTO_ON_OFF));//创建一个记录文件数量和现在播放到那张图片的结构体指针
            AUTO->FileNum = File_Photo->FileNum;//为刚刚建立的结构体赋值
            AUTO->count = count;
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            printf("Auto Play mode\n");
            AUTO_CON = 1;
            if (AUTO_CON == 1)
            {
                pthread_t AUTO_pid;//定义一个线程用来看是否收到停止自动播放的信号，输入刚刚创建的结构体指针
                pthread_create(&AUTO_pid,NULL,Auto_Num,(void *)AUTO);
            }
            while (1)
            {
                LCD_BMPDisplay(LCD,File_Photo->FilePath[*count],DISPLAY_RIGHT,VERY_FAST);//显示当前图片
                if ((Touch->move_dir ==  TOUCH_DOWN && Touch->Touch_leave == 1)
                    ||(* Control_Num) == MUSIC_STOP_CONT)
                {
                    Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
                    (* Control_Num) = CONT_INIT;//清除标志位
                    printf("Out of Auto Play mode\n");
                    AUTO_CON = 0;
                    break;
                }
                else if (Touch->move_dir !=  TOUCH_INIT && Touch->Touch_leave == 1)
                {
                    Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
                }
            }
        }
        else if ((Touch->move_dir ==  TOUCH_UP && Touch->Touch_leave == 1)
            ||(* Control_Num) == BACK)//返回主菜单
        {
            (* Control_Num) = CONT_INIT;//清除标志位
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            printf("Quit\n");//打印退出信息
            LCD_BMPDisplay(LCD,File_System->FilePath[BACKGROUND_NUM],DISPLAY_DOWN,FAST);//回到主菜单显示主菜单
            break;
        }
        else if(Touch->move_dir != TOUCH_INIT && Touch->Touch_leave == 1)//上面都判断了还是出现了移动操作肯定是误操作
        {
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
        }
    }
}
/*
    函数名  ： Auto_Num
    作  用  ：自动模式下的数字自加线程
    输入参数：
                arg     对应结构体指针AUTO_ON_OFF里面存储了图片的数量和现在播放到哪一张
    返 回 值：无
*/
void * Auto_Num(void * arg)
{
    struct AUTO_ON_OFF * AUTO = arg;
    while (1)
    {
        if (AUTO_CON == 0)//是否接收到退出该循环的信号
        {
            break;
        }
        sleep(3);
        (*(AUTO->count))++;
        if ((*(AUTO->count)) > ((AUTO->FileNum) - 1))//如果已经时最后一张了则回到第一张
        {(*(AUTO->count)) = 0;}  
    }
    pthread_exit(auto_exit);//退出该线程，并将该线程退出信息存储在空指针里
}
/*
    函数名  ： Start_System
    作  用  ：作为开机显示动画
    输入参数：
                LCD     输入LCD结构体指针用于操作LCD的显示
    返 回 值：无
*/
void Start_System(struct Lcd_Init * LCD)
{
    int Display_Num = 1;
    char StartPath[45];
    while (Display_Num <= 34)
    {
        usleep(13000);
        sprintf(StartPath,"%sb%d.bmp",START_URL,Display_Num++);
        LCD_BMPDisplay(LCD,StartPath,DISPLAY_NODIR,NO_SPEED);
    }
}
