#include "Touch.h"
/*
    函数名  ：dif_absolute
    作  用  ：获得a，b之间差值的绝对值
    输入参数：需要相减的两个参数
    返 回 值：两个数相减后的绝对值
*/
int dif_absolute(int a,int b)
{
    int result;
    if (a > b)
    { result = a - b; }
    if (a < b)
    { result = b - a; }
    return result;
}

/*
    函数名  ：Touch_Init
    作  用  ：初始化触摸屏
    输入参数：触摸屏结构体指针，因为是直接操作指针所以不需要返回值
    返 回 值：返回触摸屏文件的文件编号
*/
void Touch_Init(struct Touch_val *Touch)
{
    Touch->ts_fd = open(TOUCH_PATH,O_RDWR);
    //打开触摸屏
    if (Touch->ts_fd == -1)
    {
        perror("open ts failed");
        return;
    }
    return;
}
/*
    函数名  ：Touch_Scan
    作  用  ：扫描触摸屏的触摸情况
    输入参数：触摸屏结构体指针
    返 回 值：无
*/
void Touch_Scan(struct Touch_val *Touch)
{
    struct input_event buf;//定义一个关于输入的结构体
    int Touch_x_before,Touch_x_dif,Touch_y_before,Touch_y_dif;//给X，Y分别定义一个变量记录初值，一个变量记录差值
    Touch_x_before = Touch_x_dif = Touch_y_before = Touch_y_dif = 0;//初始化这个值
    while (1)//重复获取事件内容
    {
        bzero(&buf,sizeof(buf));//置字节字符串s的前n个字符为0包括‘\0’
        read(Touch->ts_fd,&buf,sizeof(buf));//读取触摸屏内容
        if (buf.type == EV_ABS && buf.code == ABS_X)//获取X轴坐标   
        { Touch->x = buf.value; }
        if (buf.type == EV_ABS && buf.code == ABS_Y)//获取X轴坐标   
        { Touch->y = buf.value; }
        if (buf.type == EV_KEY && buf.code == BTN_TOUCH && buf.value == 1)//第一次读取的坐标值标志
        {
            Touch_x_before = Touch->x;//因此记录下刚按下的坐标位置
            Touch_y_before = Touch->y;//因此记录下刚按下的坐标位置
        }
        if(Touch_x_before != 0 && Touch_y_before != 0)//获取过了第一次的值才判断是否有移动
        {
            //说明用户在X轴上进行了滑动操作
            Touch_x_dif = dif_absolute(Touch->x,Touch_x_before);//获得X坐标初值与终值的差值
            Touch_y_dif = dif_absolute(Touch->y,Touch_y_before);//获得Y坐标初值与终值的差值
            if (Touch_x_dif > 300 && Touch_y_dif < 200)
            {
                if (Touch_x_before > Touch->x)//如果初值大于终值说明是向左
                { Touch->move_dir = TOUCH_LEFT;}
                if (Touch_x_before < Touch->x)//如果初值小于终值说明是向右
                { Touch->move_dir = TOUCH_RIGHT; }
            }//说明用户在Y轴上进行了滑动操作
            else if (Touch_y_dif > 250 && Touch_x_dif < 200)
            {
                if (Touch_y_before > Touch->y)//如果初值大于终值说明是向上
                { Touch->move_dir = TOUCH_UP;}
                if (Touch_y_before < Touch->y)//如果初值小于终值说明是向下
                { Touch->move_dir = TOUCH_DOWN;}
            }//不符合上诉所有情况说明没有移动，是单点
            else
            { Touch->move_dir = TOUCH_NOMV; }   
        }
        if (buf.type == EV_KEY && buf.code == BTN_TOUCH &&buf.value == 0)//松手检测
        { 
            break; 
        }
    }
    Touch->Touch_leave = 1;
}
/*
    函数名  ：Touch_SCAN
    作  用  ：作为触摸屏线程的执行函数
    输入参数：结构体指针
    返 回 值：无
*/
void* Touch_SCAN(void* Touch)
{
    while (1)
    {
        Touch_Scan((struct Touch_val *)Touch);
    }//扫描并获取现在的触摸位置
}