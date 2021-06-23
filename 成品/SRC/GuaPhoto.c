#include "GuaPhoto.h"
/*
    函数名  ：GuaPhotoStart
    作  用  ：启动刮刮乐相关功能
    输入参数：
                LCD       LCD结构体指针用于操作LCD显示
                Touch     触摸屏结构体指针用于获取触摸屏操作坐标
                FilePhoto 刮刮乐刮出来的图片的目录路径
                FileSystem系统界面的目录路径
    返 回 值：无
*/
void GuaPhotoStart(struct Lcd_Init * LCD,struct Touch_val * Touch,struct Filedir * FilePhoto ,struct Filedir * FileSystem)
{
    LCD_bmp_X_Y(LCD,FileSystem->FilePath[GUAPHOTO_TIP],Touch->x,Touch->y);//显示相册操作提示图
    Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
     while (1)
    {
        if(Touch->x != 0 && Touch->y != 0 && Touch->Touch_leave == 1)//有触碰触摸屏程序才继续
        {
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            break;
        }
    }
    LCD_BMPDisplay(LCD,FileSystem->FilePath[GUABACKGROUND],DISPLAY_NODIR,NO_SPEED);//刮刮乐纯色背景
    int rand_num;//定义一个随机数用于使刮刮乐刮开的图片每次不同
    srand((int)time(0));//设置随机数种子
    rand_num =  (int) (rand()%(FilePhoto->FileNum - 1));
    printf("Use PhotoNum = %d\n",rand_num);
    while (1)
    {
        if(Touch->x != 0 && Touch->y != 0)
        {
            Touch->Touch_leave = 0;
            LCD_X_Y(LCD,FilePhoto->FilePath[rand_num],Touch->x,Touch->y);
        }
        if(Touch->x > 650 && Touch->x < 800 && Touch->y > 0 && Touch->y < 40 && Touch->Touch_leave == 1 && Touch->move_dir == TOUCH_NOMV)
        {
            Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
            LCD_BMPDisplay(LCD,FileSystem->FilePath[BACKGROUND_NUM],DISPLAY_UP,VERY_FAST);//回到主菜单显示主菜单
            break;
        }
    }
}
