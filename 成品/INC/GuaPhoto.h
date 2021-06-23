#ifndef _GUAPHOTO_H_
#define _GUAPHOTO_H_

#include "main.h"//由于是附属功能这边还是不独立的

/*-------------------定义结构体---------------*/
//声明已经定义过的结构体
struct Lcd_Init;
struct Touch_val;
struct Filedir;

void GuaPhotoStart(struct Lcd_Init * LCD,struct Touch_val * Touch,struct Filedir * FilePhoto ,struct Filedir * FileSystem);//启动刮刮乐相关功能
#endif
