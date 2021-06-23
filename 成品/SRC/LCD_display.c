#include "LCD_display.h"
/*
    函 数 名：LCDInit
    作    用：初始化打开LCD相关文件
    输入参数：LCD结构体指针
    返    回：无
*/
void LCD_Init(struct Lcd_Init * LCD)
{
    LCD->fd_lcd = open( LCD_PATH , O_RDWR);//打开LCD设备文件
    if (-1 == LCD->fd_lcd)//无法打开就报错
    { perror("open LCD error"); }
    //2、 内存映射
    LCD->p_lcd = (int *)mmap(NULL,	//需要内存映射的起始地址(通常设为 NULL, 代表让系统自动选定地址)
					LCD_SIZE,  //需要多大的一片内存(一字节为单位) LCD显示屏800*480个像素点 每个像素点占4字节
					PROT_READ | PROT_WRITE,  //映射区域可被读取 映射区域可被写入
					MAP_SHARED,					//对应射区域的写入数据会复制回文件内
					LCD->fd_lcd, 		//需要内存映射的文件的文件描述符
					0);			//参数offset为文件映射的偏移量, 通常设置为0
    if ( MAP_FAILED == LCD->p_lcd )
    { perror("mmap error"); }//无法配置就报错
}
/*
    函 数 名：LCD_BMPDisplay
    作    用：在LCD上根据输入的方向和速度进行显示
    输入参数：
                LCD             结构体指针
                PhotoPath       图片路径
                Display_Type    显示模式
                Display_Speed   显示速度
    返    回：无
*/
void LCD_BMPDisplay(struct Lcd_Init * LCD,char * PhotoPath,int Display_Type,int Display_Speed)
{
    //1、打开图片
    int fd_bmp = open( PhotoPath , O_RDONLY);
    lseek(fd_bmp, 54, SEEK_SET);    //跳过54个字节即跳过头信息
    if (-1 == fd_bmp)
    {perror("open BMP error");}//打印错误提示
    // 2.  读取图像中的BGR颜色数据
    char  buf_bmp [ BMP_SIZE ] ;
    int ret_val = read(fd_bmp, buf_bmp , BMP_SIZE );
    // 3.  把图像中的BGR 数据转换为 ARGB  24位转为 32位
    int  buf_lcd [ H ][ W ] ; 
    int x, y ;
    for ( y = 0; y < H ; y++)
    {
        for ( x = 0; x < W ; x++)
        {
            buf_lcd[y][x] = buf_bmp[(x+y*W)*3 + 0] << 0 | 
                            buf_bmp[(x+y*W)*3 + 1] << 8 | 
                            buf_bmp[(x+y*W)*3 + 2] << 16 ;
        }
    }
    // 4.  把转换后的数据显示到LCD文件中//H=480 W=800
    if (Display_Type == DISPLAY_LEFT)
    {
        //由于这个是从左向右，只考虑x轴因此只声明x轴
        int change_x = 0;
        while(1)
        {
            for ( y = 0; y < H; y++)
            {
                for ( x = 0; x < W; x++)
                { // 解引用
                    if((x <= (Display_Speed*(change_x+1)) && x >= (Display_Speed * change_x) ))
                    {
                        *(LCD->p_lcd+x+y*W) = buf_lcd[H-y][x] ;
                    }
                }
            }
            change_x++;
            if (change_x == (W/Display_Speed))
            {
                break;
            }
        }
    }
    else if (Display_Type == DISPLAY_RIGHT)
    {
        //由于这个是从右向左，只考虑x轴因此只声明x轴
        int change_x = (W/Display_Speed);
        while(1)
        {
            for ( y = 0; y < H; y++)
            {
                for ( x = 0; x < W; x++)
                { // 解引用
                    if((x <= (Display_Speed*(change_x+1)) && x >= (Display_Speed * change_x) ))
                    {
                        *(LCD->p_lcd+x+y*W) = buf_lcd[H-y][x] ;
                    }
                }
            }
            change_x--;
            if (change_x < 0)
            {
                break;
            }
        }
    }else if (Display_Type == DISPLAY_UP)
    {
        //由于这个是从上至下，只考虑y轴因此只声明y轴
        int change_y = 0;
        while(1)
        {
            for ( y = 0; y < H; y++)
            {
                for ( x = 0; x < W; x++)
                { // 解引用
                    if((y <= (Display_Speed*(change_y+1)) && y >= (Display_Speed * change_y) ))
                    {
                        *(LCD->p_lcd+x+y*W) = buf_lcd[H-y][x] ;
                    }
                }
            }
            change_y++;
            if (change_y > (H/Display_Speed))
            {
                break;
            }
        }
    }else if (Display_Type == DISPLAY_DOWN)
    {
        //由于这个是从下至上，只考虑y轴因此只声明y轴
        int change_y = (H/Display_Speed);
        while(1)
        {
            for ( y = 0; y < H; y++)
            {
                for ( x = 0; x < W; x++)
                { // 解引用
                    if((y <= (Display_Speed*(change_y+1)) && y >= (Display_Speed * change_y) ))
                    {
                        *(LCD->p_lcd+x+y*W) = buf_lcd[H-y][x] ;
                    }
                }
            }
            change_y--;
            if (change_y < 0)
            {
                break;
            }
        }
    }else if (Display_Type == DISPLAY_NODIR)//没有定义显示方向直接显示
    {
        for ( y = 0; y < H; y++)
        {
            for ( x = 0; x < W; x++)
            { // 解引用
            *(LCD->p_lcd+x+y*W) = buf_lcd[H-y][x] ;
            }
        }
    }
    // 5.  关闭图片文件释放资源
    close(fd_bmp);
}

/*
    函数名  ：LCD_bmp_X_Y
    作  用  ：让图片在触摸部分扩散显示主要应用于点开功能时切换显示界面使用
    输入参数：LCD 输入LCD参数相关结构体指针
             PhotoPath输入需要显示的图片的图片路径
             Touch_x，Touch_y输入触摸屏的坐标
    返 回 值：成功操作返回0失败返回-1
*/
void LCD_bmp_X_Y(struct Lcd_Init* LCD,char * PhotoPath,int Touch_x,int Touch_y)
{
    // 1.  打开图片
    int fd_bmp = open( PhotoPath , O_RDONLY);
    lseek(fd_bmp, 54, SEEK_SET);    //便宜54个字节跳过头信息
    if (-1 == fd_bmp)
    {
        perror("open BMP error");
    }
    // 2.  读取图像中的BGR颜色数据
    char  buf_bmp [ BMP_SIZE ] ;
    int ret_val = read(fd_bmp, buf_bmp , BMP_SIZE );

    // 3.  把图像中的BGR 数据转换为 ARGB  24位转为 32位
    int  buf_lcd [ H ][ W ] ; 
    int x, y ;
    for ( y = 0; y < H ; y++)
    {
        for ( x = 0; x < W ; x++)
        {
            buf_lcd[y][x] = buf_bmp[(x+y*W)*3 + 0] << 0 | 
                            buf_bmp[(x+y*W)*3 + 1] << 8 | 
                            buf_bmp[(x+y*W)*3 + 2] << 16 ;
        }
    }
    // 4.  把转换后的数据显示到LCD文件中//H=480 W=800
    int change_x,change_y;
    int change_x_,change_y_;
    int count_1 = 1;
    int count_2 = 1;
    int count_3 = 1;
    int count_4 = 1;
    change_x_ = Touch_x - (VERY_FAST * count_1);
    change_x = Touch_x + (VERY_FAST * count_2);
    change_y_ = Touch_y - (FAST * count_3);
    change_y = Touch_y + (FAST * count_4);
    while (1)
    {
        if (change_x_ > 0)
        {
            count_1++;
            change_x_ = Touch_x - (VERY_FAST * count_1);
            if (change_x_ < 0)
            {
                change_x_ = 0;
            }
        }
        if (change_x < W)
        {
            count_2++;
            change_x = Touch_x + (VERY_FAST * count_2);
            if (change_x > W)
            {
                change_x = W;
            }
        }
        if (change_y_ > 0)
        {
            count_3++;
            change_y_ = Touch_y - (FAST * count_3);
            if (change_y_ < 0)
            {
                change_y_ = 0;
            }
        }
        if (change_y < H)
        {
            count_4++;
            change_y = Touch_y + (FAST * count_4);
            if (change_y > H)
            {
                change_y = H;
            }
        }
        for ( y = 0; y < H; y++)
        {
            for ( x = 0; x < W; x++)
            { // 解引用
                if(y <= change_y && y >= change_y_ && x <= change_x  && x >= change_x_)
                {
                    *(LCD->p_lcd+x+y*W) = buf_lcd[H-y][x] ;
                }
            }
        }
        if (change_x_ <= 0 && change_x >= W && change_y_ <= 0 && change_y >= H)
        {
            break;
        }
    }
    // 5.  关闭文件释放资源
    close(fd_bmp);
}

/*
    函数名  ：LCD_bmp_X_Y
    作  用  ：让图片在触摸部分扩散显示主要应用于点开功能时切换显示界面使用
    输入参数：LCD 输入LCD参数相关结构体指针
             PhotoPath输入需要显示的图片的图片路径
             Touch_x，Touch_y输入触摸屏的坐标
    返 回 值：成功操作返回0失败返回-1
*/
void LCD_X_Y(struct Lcd_Init* LCD,char * PhotoPath,int Touch_x,int Touch_y)
{
    // 1.  打开图片
    int fd_bmp = open( PhotoPath , O_RDONLY);
    lseek(fd_bmp, 54, SEEK_SET);    //便宜54个字节跳过头信息
    if (-1 == fd_bmp)
    {perror("open BMP error");}
    // 2.  读取图像中的BGR颜色数据
    char  buf_bmp [ BMP_SIZE ] ;
    int ret_val = read(fd_bmp, buf_bmp , BMP_SIZE );

    // 3.  把图像中的BGR 数据转换为 ARGB  24位转为 32位
    int  buf_lcd [ H ][ W ] ; 
    int x, y ;
    for ( y = 0; y < H ; y++)
    {
        for ( x = 0; x < W ; x++)
        {
            buf_lcd[y][x] = buf_bmp[(x+y*W)*3 + 0] << 0 | 
                            buf_bmp[(x+y*W)*3 + 1] << 8 | 
                            buf_bmp[(x+y*W)*3 + 2] << 16 ;
        }
    }
    // 4.  把转换后的数据显示到LCD文件中//H=480 W=800
    int rand_num;//定义一个随机数用于使刮刮乐刮开的图片每次不同
    srand((int)time(0));//设置随机数种子
    rand_num =  (int) ((rand()%25+20));
    for ( y = 0; y < H; y++)
    {
        for ( x = 0; x < W; x++)
        { // 解引用
            if(((y <= (Touch_y + rand_num) && y >= (Touch_y - rand_num))) && ((x <= (Touch_x + rand_num) && x >= (Touch_x - rand_num))))
            {
                *(LCD->p_lcd+x+y*W) = buf_lcd[H-y][x];
            }
        }
    }
    // 5.  关闭文件释放资源
    close(fd_bmp);
}
