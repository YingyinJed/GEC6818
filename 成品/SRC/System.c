#include "System.h"
/*
    函数名  ：sleep_ms
    作  用  ：完成毫秒级别的延时
    输入参数：需要延时多少毫秒
    返 回 值：无
*/
void sleep_ms(unsigned int secs)
{
    struct timeval tval;
    tval.tv_sec=secs/1000;
    tval.tv_usec=(secs*1000)%1000000;
    select(0,NULL,NULL,NULL,&tval);
}