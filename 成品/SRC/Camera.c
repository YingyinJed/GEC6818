/****************************************************************************************
 *文件名称:lcd.c
 *设    计:温子祺
 *日    期:2015-5-29
 *说	明:	1)支持显示任意大小的bmp位图
			2)自动识别位图格式	
  -------------------------------------------------------------------------------------
 *修改日期:2015-6-5
			1)添加了jpg文件、jpg流显示的功能
****************************************************************************************/
#include <stdio.h>   	//printf scanf
#include <fcntl.h>		//open write read lseek close  	 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>

#include "Camera.h"
#include "jpeglib.h"
#include "api_v4l2.h"

static char g_color_buf[FB_SIZE]={0};

static int  g_fb_fd;

/* video_chat.c 画中画显示的坐标 */
volatile int g_jpg_in_jpg_x=400;
volatile int g_jpg_in_jpg_y=240;

void * thr_exit;//定义一个空指针用于线程的退出
int AVI_NUM = 0;//定义一个AVI文件名的计数值让AVI文件的文件名不同
int countPhoto = 0;//定义一个摄像头相册使用的照片计数全局变量

FrameBuffer freambuf;//定义一个摄像头数据相关的结构体指针并为其分配内存
//以下属于网络通信用到的全局变量
//初始化UDP通信的服务器信息
int iphonex;
struct sockaddr_in phoneaddr ;
struct sockaddr_in bindaddr;
int addrsize ;
/****************************************************
 *函数名称:file_size_get
 *输入参数:pfile_path	-文件路径
 *返 回 值:-1		-失败
		   其他值	-文件大小
 *说	明:获取文件大小
 ****************************************************/
unsigned long file_size_get(const char *pfile_path)
{
	unsigned long filesize = -1;	
	struct stat statbuff;
	
	if(stat(pfile_path, &statbuff) < 0)
	{
		return filesize;
	}
	else
	{
		filesize = statbuff.st_size;
	}
	
	return filesize;
}
/*
    函数名  ：lcd_draw_jpg
    作  用  ：在LCD上绘制JPG图片
    输入参数：
				x			控制LCD图片绘制的起始坐标
				y			控制LCD图片绘制的起始坐标
				pjpg_path	jpg文件的路径指针，如果用摄像头则填NULL
				pjpg_buf	jpg摄像头结构体数据流，如果是绘制JPG文件则填NULL
				jpg_buf_sizejpg摄像头结构体数据流长度，如果是绘制JPG文件则填0
				jpg_half	一般为0
				LCD			LCD结构体指针用于操作LCD
    返 回 值：空指针
*/
int lcd_draw_jpg(unsigned int x,unsigned int y,const char *pjpg_path,char *pjpg_buf,unsigned int jpg_buf_size,unsigned int jpg_half,
				struct Lcd_Init * LCD)  
{
	/*定义解码对象，错误处理对象*/
	struct 	jpeg_decompress_struct 	cinfo;
	struct 	jpeg_error_mgr 			jerr;	
	
	char 	*pcolor_buf = g_color_buf;
	char 	*pjpg;
	
	unsigned int 	i=0;
	unsigned int	color =0;
	unsigned int	count =0;
	
	unsigned int 	x_s = x;
	unsigned int 	x_e ;	
	unsigned int 	y_e ;
	
			 int	jpg_fd;
	unsigned int 	jpg_size;
	
	unsigned int 	jpg_width;
	unsigned int 	jpg_height;
	
	if(pjpg_path!=NULL)
	{
		/* 申请jpg资源，权限可读可写 */	
		jpg_fd=open(pjpg_path,O_RDWR);
		
		if(jpg_fd == -1)
		{
		   printf("open %s error\n",pjpg_path);
		   
		   return -1;	
		}	
		
		/* 获取jpg文件的大小 */
		jpg_size=file_size_get(pjpg_path);	

		/* 为jpg文件申请内存空间 */	
		pjpg = malloc(jpg_size);

		/* 读取jpg文件所有内容到内存 */		
		read(jpg_fd,pjpg,jpg_size);
	}
	else
	{
		jpg_size = jpg_buf_size;
		
		pjpg = pjpg_buf;
	}
	/*注册出错处理*/
	cinfo.err = jpeg_std_error(&jerr);

	/*创建解码*/
	jpeg_create_decompress(&cinfo);

	/*直接解码内存数据*/		
	jpeg_mem_src(&cinfo,pjpg,jpg_size);
	
	/*读文件头*/
	jpeg_read_header(&cinfo, TRUE);

	/*开始解码*/
	jpeg_start_decompress(&cinfo);	
	
	
	if(jpg_half)
	{
		x_e	= x_s+(cinfo.output_width/2);
		y_e	= y  +(cinfo.output_height/2);		
		
		/*读解码数据*/
		while(cinfo.output_scanline < cinfo.output_height)
		{		
			pcolor_buf = g_color_buf;
			
			/* 读取jpg一行的rgb值 */
			jpeg_read_scanlines(&cinfo,(JSAMPARRAY)&pcolor_buf,1);			
			
			/* 再读取jpg一行的rgb值 */
			jpeg_read_scanlines(&cinfo,(JSAMPARRAY)&pcolor_buf,1);

			for(i=0; i<(cinfo.output_width/2); i++)
			{
				/* 获取rgb值 */
				color = 		*(pcolor_buf+2);
				color = color | *(pcolor_buf+1)<<8;
				color = color | *(pcolor_buf)<<16;
				
				/* 显示像素点 */
				// lcd_draw_point(x,y,color);
				*(LCD->p_lcd+y*800+x)=color;
				
				pcolor_buf +=6;
				
				x++;
			}
			
			/* 换行 */
			y++;					
			
			
			x = x_s;	

			
		}
	}
	else
	{
		x_e	= x_s+cinfo.output_width;
		y_e	= y  +cinfo.output_height;		
		/*读解码数据*/
		while(cinfo.output_scanline < cinfo.output_height )
		{		
			pcolor_buf = g_color_buf;
			/* 读取jpg一行的rgb值 */
			jpeg_read_scanlines(&cinfo,(JSAMPARRAY)&pcolor_buf,1);
			
			for(i=0; i<cinfo.output_width; i++)
			{
				/* 获取rgb值 */
				color = 		*(pcolor_buf+2);
				color = color | *(pcolor_buf+1)<<8;
				color = color | *(pcolor_buf)<<16;
				/* 显示像素点 */
				// lcd_draw_point(x,y,color);
				*(LCD->p_lcd+y*800+x)=color;
				
				pcolor_buf +=3;
				
				x++;
			}
			
			/* 换行 */
			y++;			
			
			x = x_s;
			
		}		
	}		
	/*解码完成*/
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	if(pjpg_path!=NULL)
	{
		/* 关闭jpg文件 */
		close(jpg_fd);	
		
		/* 释放jpg文件内存空间 */
		free(pjpg);		
	}


	
	return 0;
}

int lcd_draw_jpg_in_jpg(unsigned int x,unsigned int y,const char *pjpg_path,char *pjpg_buf,unsigned int jpg_buf_size,struct Lcd_Init * LCD)  
{
	/*定义解码对象，错误处理对象*/
	struct 	jpeg_decompress_struct 	cinfo;
	struct 	jpeg_error_mgr 			jerr;	
	
	char 	*pcolor_buf = g_color_buf;
	char 	*pjpg;
	
	unsigned int 	i=0,j=0;
	unsigned int	color =0;
	unsigned int	count =0;
	
	unsigned int 	x_s = x;
	unsigned int 	x_e ;	
	unsigned int 	y_e ;
	unsigned int	y_n	= y;
	unsigned int	x_n	= x;
	
			 int	jpg_fd;
	unsigned int 	jpg_size;

	if(pjpg_path!=NULL)
	{
		/* 申请jpg资源，权限可读可写 */	
		jpg_fd=open(pjpg_path,O_RDWR);
		
		if(jpg_fd == -1)
		{
		   printf("open %s error\n",pjpg_path);
		   
		   return -1;	
		}	
		
		/* 获取jpg文件的大小 */
		jpg_size=file_size_get(pjpg_path);	

		/* 为jpg文件申请内存空间 */	
		pjpg = malloc(jpg_size);

		/* 读取jpg文件所有内容到内存 */		
		read(jpg_fd,pjpg,jpg_size);
	}
	else
	{
		jpg_size = jpg_buf_size;
		
		pjpg = pjpg_buf;
	}

	/*注册出错处理*/
	cinfo.err = jpeg_std_error(&jerr);

	/*创建解码*/
	jpeg_create_decompress(&cinfo);

	/*直接解码内存数据*/		
	jpeg_mem_src(&cinfo,pjpg,jpg_size);
	
	/*读文件头*/
	jpeg_read_header(&cinfo, TRUE);

	/*开始解码*/
	jpeg_start_decompress(&cinfo);	
	
	
	x_e	= x_s+cinfo.output_width;
	y_e	= y  +cinfo.output_height;	

	/*读解码数据*/
	while(cinfo.output_scanline < cinfo.output_height )
	{		
		pcolor_buf = g_color_buf;
		
		/* 读取jpg一行的rgb值 */
		jpeg_read_scanlines(&cinfo,(JSAMPARRAY)&pcolor_buf,1);
		
		for(i=0; i<cinfo.output_width; i++)
		{
			/* 不显示的部分 */
			if(y_n>g_jpg_in_jpg_y && y_n<g_jpg_in_jpg_y+240)
				if(x_n>g_jpg_in_jpg_x && x_n<g_jpg_in_jpg_x+320)
				{
					pcolor_buf +=3;
				
					x_n++;
					
					continue;
				}
				
			/* 获取rgb值 */
			color = 		*(pcolor_buf+2);
			color = color | *(pcolor_buf+1)<<8;
			color = color | *(pcolor_buf)<<16;	
			
			/* 显示像素点 */
			// lcd_draw_point(x_n,y_n,color);
			*(LCD->p_lcd+y_n*800+x_n)=color;
			
			pcolor_buf +=3;
			
			x_n++;
		}
		
		/* 换行 */
		y_n++;			
		
		x_n = x_s;
		
	}		

				
	/*解码完成*/
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	if(pjpg_path!=NULL)
	{
		/* 关闭jpg文件 */
		close(jpg_fd);	
		
		/* 释放jpg文件内存空间 */
		free(pjpg);		
	}


	
	return 0;
}
/*
    函数名  ：Camera_Start
    作  用  ：开启摄像头功能
    输入参数：
				LCD			LCD结构体指针，用于操作LCD显示
				Touch		触摸屏结构体指针用于获取触摸屏坐标及操作形式
				SystemFile	系统图片结构体指针，用于获取系统文件目录路径
				Camera_pid	摄像头录像JPG转AVI线程
				AVI_pid		摄像头回看摄像内容的AVI线程
				AVICameraDir需要AVI播放的目录结构体指针
				PhoCameraDir相册目录结构体
				Control_Num	获取从网络通信获得的控制数字
    返 回 值：空
*/
void Camera_Start(struct Lcd_Init *LCD,struct Touch_val * Touch,struct Filedir * SystemFile,pthread_t Camera_pid,
					pthread_t AVI_pid,struct Filedir * AVICameraDir,struct Filedir * PhoCameraDir,int * Control_Num)
{
	LCD_bmp_X_Y(LCD,SystemFile->FilePath[CAMERA_STOP],Touch->x,Touch->y);//显示相册操作提示图
	Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
	(* Control_Num) = CONT_INIT;//清除标志位
	/* 摄像头设备初始化 */
	linux_v4l2_device_init("/dev/video7");
	/* 启动摄像头设备 */
	linux_v4l2_start_capturing();
	while (1)
	{
		usleep(2000);//延时一小会
		/* 从内核队列中读取一帧摄像头数据 */
		linux_v4l2_get_fream(&freambuf);
		/* 显示jpg图片 */
		lcd_draw_jpg(80,0,NULL,freambuf.buf,freambuf.length,0,LCD);
		//发送实时监控到的数据
		sendto(iphonex,freambuf.buf,freambuf.length,0,(struct sockaddr *)&phoneaddr,sizeof(phoneaddr));
		if ((Touch->move_dir ==  TOUCH_UP && Touch->Touch_leave == 1)
			||((* Control_Num) == BACK))//回到主菜单显示主菜单
		{
			(* Control_Num) = CONT_INIT;//清除标志位
			Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
			LCD_BMPDisplay(LCD,SystemFile->FilePath[BACKGROUND_NUM],DISPLAY_DOWN,FAST);//回到主菜单显示主菜单
			break;
		}
		else if (Touch->x < 80 && Touch->y < 240 && Touch->Touch_leave == 1)//录像功能
		{
			LCD_BMPDisplay(LCD,SystemFile->FilePath[CAMERA_PLAY],DISPLAY_NODIR,NO_SPEED);//LCD显示启动录制界面
			Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
			char command[200];
			sprintf(command,"rm ");//格式化原命令并放入现在的命令
			strcat(command,url_pic);//添加要删除的路径
			strcat(command,"*.jpg");//添加需要删除的文件类型
			system(command); // 执行删除缓存区中缓存的jpg文件

			int fd = -1 ;//记录是否成功创建文件s
			int count = 0 ;//进度显示的一部分
			char pic_name[64] = {'0'};//打印录像的进度
			while(1)
			{
				/* 从内核队列中读取一帧摄像头数据 */
				linux_v4l2_get_fream(&freambuf);
				/* 显示jpg图片 */
				lcd_draw_jpg(80,0,NULL,freambuf.buf,freambuf.length,0,LCD);
				bzero(pic_name, 20);//及时清空打印内容并重新打印
				sprintf(pic_name, url_pic);//添加缓存区域的缓存路径
				sprintf(command,"%d.jpg",count++);
				strcat(pic_name,command);//添加打开进度
				printf("pic_name = %s\n", pic_name);//打印提示内容
				//创建一个新的文件存储这一帧的视频图像
				fd = open(pic_name, O_RDWR | O_CREAT, 0777);
				if(-1 == fd)
				{
					perror("create jpg failed");
					continue;
				}
				write(fd, freambuf.buf,freambuf.length);//在刚刚创建的文件中写入相关数据
				close(fd);
				if (Touch->x < 80 && Touch->y < 240 && Touch->Touch_leave == 1)//点击停止录制则跳出循环
				{
					Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
					break;//退出该循环
				}	
			}
			LCD_BMPDisplay(LCD,SystemFile->FilePath[CAMERA_TIP_AVI],DISPLAY_NODIR,NO_SPEED);//LCD显示是否进行录制的提示界面
			while (1)
			{
				if (Touch->x > 245 && Touch->x < 355 && Touch->y > 240 && Touch->y < 310 && Touch->Touch_leave == 1)//点击是
				{
					printf("it is saving the video!!!\n");
					Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
					pthread_create(&Camera_pid,NULL,Camera_AVI,(void *)command);//配置JPG转AVI的线程
					LCD_BMPDisplay(LCD,SystemFile->FilePath[CAMERA_STOP],DISPLAY_NODIR,NO_SPEED);//LCD显示摄像头不录制片的界面
					break;//退出该循环
				}
				else if(Touch->x > 435 && Touch->x < 500 && Touch->y > 240 && Touch->y < 310 && Touch->Touch_leave == 1)//点击否
				{
					Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
					LCD_BMPDisplay(LCD,SystemFile->FilePath[CAMERA_STOP],DISPLAY_NODIR,NO_SPEED);//LCD显示摄像头不录制片的界面
					break;//点击否则直接退出这个循环
				}
			}
		}
		else if (Touch->x < 80 && Touch->y > 240 && Touch->Touch_leave == 1)//观看录制
		{
			Read_Dir(AVICameraDir,url_video,TYPE_AVI);//重新读取录像文件防止刚刚录制的没有添加
			AVI_PlayStart(LCD,Touch,AVICameraDir,SystemFile,AVI_pid,Control_Num); //调用启动AVI播放器
			LCD_BMPDisplay(LCD,SystemFile->FilePath[CAMERA_STOP],DISPLAY_DOWN,FAST);//重新显示摄像头界面
		}
		else if (Touch->x > 720 && Touch->y < 240 && Touch->Touch_leave == 1)//拍照功能
		{
			Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
			char PhotoName[20];
			int count_Photo = 0;
			int fd = -1 ;//记录是否成功创建文件
			char pic_name[64] = {'0'};//打印录像的进度，并作为JPG的名字
			while (1)
			{
				sprintf(pic_name,PHOTO_url);//添加存储路径
				sprintf(PhotoName,"%d.jpg",count_Photo++);//为路径添加文件名
				strcat(pic_name,PhotoName);//为路径添加文件名
				if (access(pic_name,F_OK))//若不存在则开始存储图片
				{
					/* 从内核队列中读取一帧摄像头数据 */
					linux_v4l2_get_fream(&freambuf);
					/* 显示jpg图片 */
					lcd_draw_jpg(80,0,NULL,freambuf.buf,freambuf.length,0,LCD);
					printf("Photo_name = %s\n", pic_name);//打印提示内容
					//创建一个新的文件存储这一帧的视频图像
					fd = open(pic_name, O_RDWR | O_CREAT, 0777);
					if(-1 == fd)
					{
						perror("create jpg failed");
						continue;
					}
					write(fd, freambuf.buf,freambuf.length);//在刚刚创建的文件中写入相关数据
					close(fd);	
					break;//写完就退出循环
				}else//若文件存在则在存储路径的字符串中删除刚刚的jpg文件名在下次循环中重新赋值
				{
					continue;
				}
			}
			printf("Just take a photo %s\n",pic_name);
		}
		else if (Touch->x > 720 && Touch->y > 240 && Touch->Touch_leave == 1)//相册会看功能
		{
			Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
			LCD_BMPDisplay(LCD,SystemFile->FilePath[CAMERA_PHO],DISPLAY_NODIR,NO_SPEED);//显示回看相册的界面
			Read_Dir(PhoCameraDir,PHOTO_url,TYPE_JPG);//进入前再读取相册目录下的JPG文件
			lcd_draw_jpg(80,0,PhoCameraDir->FilePath[countPhoto],NULL,0,0,LCD);//打印好上次浏览到的照片
			while(1)
			{
				sleep_ms(200);//延时200毫秒防止BUG
				if (Touch->x < 80 && Touch->Touch_leave == 1)//判断是不是点击了上一张
				{
					printf("show last photo\n");
					printf("now its num is %d\n",countPhoto);
					Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
					countPhoto--;
					usleep(1000);
					if (countPhoto < 0)
					{countPhoto = (PhoCameraDir->FileNum - 1);}
					lcd_draw_jpg(80,0,PhoCameraDir->FilePath[countPhoto],NULL,0,0,LCD);//这个函数执行就会将所有照片打印一遍
				}
				else if (Touch->x > 720 && Touch->Touch_leave == 1)//判断是不是点击了下一张
				{
					printf("show next photo\n");
					printf("now its num is %d\n",countPhoto);
					Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
					countPhoto++;
					usleep(1000);
					if (countPhoto > (PhoCameraDir->FileNum - 1))
					{countPhoto = 0;}
					lcd_draw_jpg(80,0,PhoCameraDir->FilePath[countPhoto],NULL,0,0,LCD);//这个函数执行就会将所有照片打印一遍
				}
				else if(Touch->x > 80 && Touch->x < 720 &&Touch->move_dir == TOUCH_UP && Touch->Touch_leave == 1)//判断是否要退出相册
				{
					Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
					LCD_BMPDisplay(LCD,SystemFile->FilePath[CAMERA_STOP],DISPLAY_DOWN,FAST);//重新显示摄像头界面
					break;//退出该相册循环
				}
			}
		}
	}
	/* 停止运行摄像头 */
	linux_v4l2_stop_capturing();
	/* 解除涉嫌头设备 */
	linux_v4l2_device_uinit();
}

void * Camera_AVI(void * arg)
{
	char AVIurl[40];//用于存储现在准备生成的AVI的路径
	char * command = arg;//完成指针的强制类型转换
	while (1)
	{
		sprintf(command,url_video);//添加AVI路径
		sprintf(AVIurl,"%d.avi",AVI_NUM++);
		strcat(command,AVIurl);//添加AVI文件名
		// 检查管道文件是否存在
		if (access(command,F_OK))//若不存在则开始转换
		{
			sprintf(command,JPGToVideo);//添加JPG转AVI的执行命令
			strcat(command,url_pic);//添加jpg的缓存路径
			strcat(command,"%d.jpg ");//添加jpg缓存文件名
			strcat(command,url_video);//添加即将生成AVI文件的文件路径
			strcat(command,AVIurl);//添加要生成的AVI文件名
			system(command);// 转换成视频
			pthread_exit(thr_exit);//转换完成就退出线程
		}else
		{
			AVI_NUM++;//存在文件名就+1直到不存在
			continue;
		}
	}	
}


/*
    函数名  ：udp_init
    作  用  ：初始化UDP网络服务器，错误时返回错误信息
    输入参数：无
    返 回 值：错误信息
*/
int udp_init()
{
	int ret;
	//初始化该结构体
	bindaddr.sin_family=AF_INET;
	bindaddr.sin_addr.s_addr=inet_addr("192.168.1.71");////开发板无线网卡的ip地址
	bindaddr.sin_port=htons(2234);//不需要修改端口号
	//买手机---》创建一个udp套接字(数据报套接字)用于通信
	iphonex=socket(AF_INET,SOCK_DGRAM,0);
	if(iphonex==-1)
	{
		printf("create socket failed!\n");
		return -1;
	}
	//绑定 ---》自己ip和端口号   
	ret=bind(iphonex,(struct sockaddr *)&bindaddr,sizeof(bindaddr));
	if(ret==-1)
	{
		printf("bind failed!\n");
		return -1;
	}
	return 0;
}

/*
    函数名  ：myrecvmsg
    作  用  ：创建线程用于接收网络通信获得的信息
    输入参数：将命令转化为控制数字的整型指针
    返 回 值：空指针
*/
void *myrecvmsg(void *arg)
{
	int * Control_Num = arg;
	char buf[200];
	addrsize=sizeof(phoneaddr);
	printf("\nServer is ready!!!\n");
	while(1)
	{
		usleep(1000);
		bzero(buf,200);//清零
		//聊天，收发数据，接收手机发送过来的控制命令
		recvfrom(iphonex,buf,200,0,(struct sockaddr *)&phoneaddr,&addrsize);
		printf("iphoneX send msg is:%s ip:%s  port:%hu\n",buf,inet_ntoa(phoneaddr.sin_addr),ntohs(phoneaddr.sin_port));
		//判断收到的命令
		if(strcmp(buf,"MUSIC_PLAY")==0)//字符串的比较 
		{
			(*Control_Num) = MUSIC_PLAY;
		}
		else if (strcmp(buf,"GET_VIDEO")==0)
		{
			(*Control_Num) = GET_VIDEO;
		}
		else if (strcmp(buf,"BACK")==0)
		{
			(*Control_Num) = BACK;
		}
		else if (strcmp(buf,"ENTER_KUGOU")==0)
		{
			(*Control_Num) = ENTER_KUGOU;
		}
		else if (strcmp(buf,"MUSIC_STOP_CONT")==0)
		{
			(*Control_Num) = MUSIC_STOP_CONT;
		}
		else if (strcmp(buf,"MUSIC_PREV")==0)
		{
			(*Control_Num) = MUSIC_PREV;
		}
		else if (strcmp(buf,"MUSIC_NEXT")==0)
		{
			(*Control_Num) = MUSIC_NEXT;
		}
		else if (strcmp(buf,"LED_ON")==0)
		{
			(*Control_Num) = LED_ON;
		}
		else if (strcmp(buf,"LED_OFF")==0)
		{
			(*Control_Num) = LED_OFF;
		}
		else if (strcmp(buf,"BEEP")==0)
		{
			(*Control_Num) = BEEP;
		}
	}
}

/*
    函数名  ：Phone_Network
    作  用  ：在主函数中调用用于初始化网络通信配置
    输入参数：
			LCD			这个多余了但是懒得删了
			Touch		这个也有点多余但是懒得删了
			Control_Num	将命令转换为数字的整型指针
    返 回 值：无
*/
void Phone_Network(struct Lcd_Init *LCD,struct Touch_val * Touch,int * Control_Num)
{
	Touch->move_dir = Touch->x = Touch->y = Touch->Touch_leave = TOUCH_INIT;//清除所有标志位
	udp_init() ;
	pthread_t tid;
	pthread_create(&tid,NULL,myrecvmsg,(void * )Control_Num);
}

//接收线程
void *OneNetReacv(void *arg)
{
	int * Control_Num = arg;
	while(1)
	{
		char RecvBuffer[20] = {0};  //保存接收的字符串
		// 把内存按字节为单位设置为0 
		memset(RecvBuffer ,0 ,sizeof(RecvBuffer));
		usleep(1000);
		OneNet_RecvData((void*)&sockfd , RecvBuffer);
		//判断收到的命令
		if(strcmp(RecvBuffer,"MUSIC_PLAY")==0)//字符串的比较 
		{
			(*Control_Num) = MUSIC_PLAY;
		}
		else if (strcmp(RecvBuffer,"VIDEO_PLAY")==0)
		{
			(*Control_Num) = GET_VIDEO;
		}
		else if (strcmp(RecvBuffer,"BACK______")==0)
		{
			(*Control_Num) = BACK;
		}
		else if (strcmp(RecvBuffer,"ENTERKUGOU")==0)
		{
			(*Control_Num) = ENTER_KUGOU;
		}
		else if (strcmp(RecvBuffer,"_STOP_CONT")==0)
		{
			(*Control_Num) = MUSIC_STOP_CONT;
		}
		else if (strcmp(RecvBuffer,"MUSIC_PREV")==0)
		{
			(*Control_Num) = MUSIC_PREV;
		}
		else if (strcmp(RecvBuffer,"MUSIC_NEXT")==0)
		{
			(*Control_Num) = MUSIC_NEXT;
		}
		else if (strcmp(RecvBuffer,"LED_____ON")==0)
		{
			(*Control_Num) = LED_ON;
		}
		else if (strcmp(RecvBuffer,"LED____OFF")==0)
		{
			(*Control_Num) = LED_OFF;
		}
		else if (strcmp(RecvBuffer,"BEEP______")==0)
		{
			(*Control_Num) = BEEP;
		}
	}
}

void* Send_OneNet(void * arg)
{
	int rand_num;//定义一个随机数用于使刮刮乐刮开的图片每次不同
    srand((int)time(0));//设置随机数种子
	while (1)
	{
		sleep(1);
		rand_num =  (int) (rand()%(100 - 1));
		char buf[20] = {0};
		/************************ 硬件初始化 **************************/
		//发送数据给 OneNet 平台
		memset(buf ,0 ,sizeof(buf));
		OneNet_SendData(buf,"temp",rand_num);
	}
}

void OneNet_Start(int * Control_Num)
{
	pthread_t OneNet_Read;
    pthread_create(&OneNet_Read, NULL,OneNetReacv, (void *)Control_Num);
    pthread_t OneNet_Send;
    pthread_create(&OneNet_Send, NULL,Send_OneNet, NULL);
}