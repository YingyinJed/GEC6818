#ifndef _CONNECTONENET_
#define _CONNECTONENET_

//头文件
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "EdpKit.h"

//外部变量声明
extern int sockfd;

/* linux程序需要定义_LINUX */
#ifdef _LINUX
#define Socket(a,b,c)          socket(a,b,c)
#define Connect(a,b,c)         connect(a,b,c)
#define Close(a)               close(a)
#define Read(a,b,c)            read(a,b,c)
#define Recv(a,b,c,d)          recv(a, (void *)b, c, d)
#define Select(a,b,c,d,e)      select(a,b,c,d,e)
#define Send(a,b,c,d)          send(a, (const int8 *)b, c, d)
#define Write(a,b,c)           write(a,b,c)
#define GetSockopt(a,b,c,d,e)  getsockopt((int)a,(int)b,(int)c,(void *)d,(socklen_t *)e)
#define SetSockopt(a,b,c,d,e)  setsockopt((int)a,(int)b,(int)c,(const void *)d,(int)e)
#define GetHostByName(a)       gethostbyname((const char *)a)
#endif



//宏定义
#define SERVERPORT  	876							//这两个是中移物联网EDP协议登录地址(IP+PORT)
#define SERVERIP  "183.230.40.39"					//这两个不可更改

#define DEVICE_ID  "642427755"						//用户根据自己的实际情况决定
#define AUTH_KEY   "M7uXCc2iRoIAfhiPvK0HSMOqNlQ="	//用户根据自己的实际情况决定

//函数声明
//==========================================================
//  函数名称：   TcpClientInit
//  函数功能：   Tcp客户端初始化
//  入口参数：   
//          ServerIp    --->服务器IP
//          ServerPort  --->服务器端口号
//  返回参数：   无
//  说明：       无
//==========================================================
void TcpClientInit(const char *ServerIp ,int ServerPort);

//==========================================================
//  函数名称：   TcpClientClose
//  函数功能：   Tcp套接字关闭
//  入口参数：   无
//  返回参数：   无
//  说明：       无
//==========================================================
void TcpClientClose(void);

//==========================================================
//	函数名称：	OneNet_DevLink
//	函数功能：	与onenet创建连接
//	入口参数：	无
//	返回参数：	0-成功	-1-失败
//	说明：		与 OneNet 平台建立连接
//==========================================================
int OneNet_DevLink(const char* devid, const char* auth_key);



/******************************** 以下三个是用户自己使用的接口函数 ********************************/
//==========================================================
//	函数名称：	OneNet_Init
//	函数功能：	Tcp客户端初始化 和 连接OneNet平台
//	入口参数：	无
//	返回参数：	无
//	说明：		无
//==========================================================
void OneNet_Init(void);

//==========================================================
//	函数名称：	OneNet_SendData
//	函数功能：	上传数据到平台
//	入口参数：	
//			buf 	--->指向保存发送数据的指针
//			stream	--->云平台上接收当前发送数据的数据流名字，这个位置不要乱写，要和平台上的数据流名字一致
//			wdata	--->要发送的数据
//	返回参数：	无
//	说明：		
//==========================================================
void OneNet_SendData(char *buf,char *stream,int wdata);

//==========================================================
//	函数名称：	OneNet_RecvData
//	函数功能：	上传数据到平台
//	入口参数：	
//			arg    		--->socket描述符
//			CmdBuffer	--->指向保存云平台下发数据的指针
//	返回参数：	无
//	说明：	   	无
//  返回值:  	无	
//==========================================================
void OneNet_RecvData(void* arg , char CmdBuffer[]);

#endif
