#include "Filedir.h"
/*
    函数名  ：Read_Dir
    作  用  ：根据需要的文件类型，读取目录下相应的文件名返回路径
    输入参数：
                File      在主函数中存储目录的结构体指针
                url       需要检索的路径
                type      需要检索的文件类型
    返 回 值：无
*/
void Read_Dir(struct Filedir * File,char * url,char * type)
{
    File->FileNum = 0;//初始化输入结构体的文件数量值
    DIR *pDir = NULL;//创建一个DIR*存储打开的路径
    struct dirent * pEnt = NULL;//创建一个存储路径中读到的信息
    pDir = opendir(url);//打开相应路径
    if (NULL == pDir)
    {
        perror("opendir is NULL!\n");//打印错误信息
    }
    //读取目录中的内容
    while (1)
    {
        pEnt = readdir(pDir);//利用结构体dirent读取本次读到的目录内容
        if (pEnt != NULL)//如果结构体读到了内容则继续对读到的内容进行处理
        {
            if (pEnt->d_type == DT_REG)//获得目录内的普通文件
            {
                sprintf(File->FilePath[File->FileNum],pEnt->d_name);//先暂时获取该文件的文件名，为的是暂时判断该文件是否是自己需要的类型文件
                if (strstr(File->FilePath[File->FileNum],type) != NULL)//判断是不是输入的文件类型
                {
                    sprintf(File->FilePath[File->FileNum],url);//先添加文件路径
                    strcat(File->FilePath[File->FileNum],pEnt->d_name);//追加文件名
                    printf("%s\n",File->FilePath[File->FileNum]);//打印看读取是否正常
                    if (File->FileNum >= max_FileNum)//看是否超过了最开始设定的能存储的最多文件数量
                    {
                        printf("The number of files in the directory exceeds the maximum number that can be read initially\n");
                        printf("There are now %d reads in the directory ",File->FileNum);
                        break;//由于超过了最开始设定能存储的目录文件量故直接退出
                    }
                    File->FileNum++;//读取完成后文件数量记录+1
                }
            }
        }
        else
        {
            closedir(pDir);//读取完成关闭目录
            break;
        }
    }
}

/*
    函数名  ：Read_SystemDir
    作  用  ：读取系统目录下的文件主要是为了对特定的系统显示文件进行排序方便后面的调用
    输入参数：
                File      在主函数中存储目录的结构体指针
                url       需要检索的路径
                type      需要检索的文件类型
    返 回 值：无
*/
void Read_SystemDir(struct Filedir * File,char * url)
{
    //主界面背景
    char background[] = "background";
    //音乐播放图片
    char music_play[] = "music_play";
    char music_stop[] = "music_stop";
    //图片操作提示
    char photo_tip[] = "Photo_tip";
    //刮刮乐图片
    char gua_tip[] = "Gua_tip";
    char guaground[] = "Guaground";
    //AVI播放图片
    char avi_play[] = "AVI_play";
    char avi_stop[] = "AVI_stop";
    //相机界面图片
    char Camera_Pho[] = "Camera_Pho";
    char Camera_Play[] = "Camera_Play";
    char Camera_Stop[] = "Camera_Stop";
    char Camera_Tip_AVI[] = "Camera_Tip_AVI";

    char Check_FileName[100];

    File->FileNum = 0;//初始化输入结构体的文件数量值
    DIR *pDir = NULL;//创建一个DIR*存储打开的路径
    struct dirent * pEnt = NULL;//创建一个存储路径中读到的信息
    pDir = opendir(url);//打开相应路径
    if (NULL == pDir)
    {
        perror("opendir is NULL!\n");//打印错误信息
    }
    //读取目录中的内容
    while (1)
    {
        pEnt = readdir(pDir);//利用结构体dirent读取本次读到的目录内容
        if (pEnt != NULL)//如果结构体读到了内容则继续对读到的内容进行处理
        {
            if (pEnt->d_type == DT_REG)//获得目录内的普通文件
            {
                sprintf(Check_FileName,pEnt->d_name);//先暂时获取该文件的文件名判断它是哪一个文件
                
                if (strstr(Check_FileName,background) != NULL)//如果这幅图是背景图，放在第一张
                {
                    sprintf(File->FilePath[BACKGROUND_NUM],url);//先获得路径,并存在其对应的数字下
                    strcat(File->FilePath[BACKGROUND_NUM],pEnt->d_name);//追加路径文件名,并存在其对应的数字下
                    File->FileNum++;//文件记录数+1
                }//如果这幅图是音乐播放图，放在第二张
                else if (strstr(Check_FileName,music_play) != NULL)//如果是音乐播放界面，放第二张
                {
                    sprintf(File->FilePath[MUSIC_PLAY_NUM],url);//先获得路径,并存在其对应的数字下
                    strcat(File->FilePath[MUSIC_PLAY_NUM],pEnt->d_name);//追加路径文件名,并存在其对应的数字下
                    File->FileNum++;//文件记录数+1
                }//如果这幅图是音乐暂停图，放在第三张
                else if (strstr(Check_FileName,music_stop) != NULL)//如果是音乐暂停界面，放第三张
                {
                    sprintf(File->FilePath[MUSIC_STOP_NUM],url);//先获得路径,并存在其对应的数字下
                    strcat(File->FilePath[MUSIC_STOP_NUM],pEnt->d_name);//追加路径文件名,并存在其对应的数字下
                    File->FileNum++;//文件记录数+1
                }//如果这幅图是图片操作提示图，放在第四张
                else if (strstr(Check_FileName,photo_tip) != NULL)//如果是图片操作提示图，放第四张
                {
                    sprintf(File->FilePath[BMPPHOTO_TIP],url);//先获得路径,并存在其对应的数字下
                    strcat(File->FilePath[BMPPHOTO_TIP],pEnt->d_name);//追加路径文件名,并存在其对应的数字下
                    File->FileNum++;//文件记录数+1
                }//如果这幅图是刮图操作提示图，放在第五张
                else if (strstr(Check_FileName,gua_tip) != NULL)//如果是刮刮乐提示图，放在第五张
                {
                    sprintf(File->FilePath[GUAPHOTO_TIP],url);//先获得路径,并存在其对应的数字下
                    strcat(File->FilePath[GUAPHOTO_TIP],pEnt->d_name);//追加路径文件名,并存在其对应的数字下
                    File->FileNum++;//文件记录数+1
                }//如果这幅图是刮图纯色背景图，放在第六张
                else if (strstr(Check_FileName,guaground) != NULL)//如果是刮刮乐纯色背景，放在第六张
                {
                    sprintf(File->FilePath[GUABACKGROUND],url);//先获得路径,并存在其对应的数字下
                    strcat(File->FilePath[GUABACKGROUND],pEnt->d_name);//追加路径文件名,并存在其对应的数字下
                    File->FileNum++;//文件记录数+1
                }//如果这幅图是AVI播放背景图，放在第七张
                else if (strstr(Check_FileName,avi_play) != NULL)//如果是AVI播放界面，放在第七张
                {
                    sprintf(File->FilePath[AVI_PLAY_NUM],url);//先获得路径,并存在其对应的数字下
                    strcat(File->FilePath[AVI_PLAY_NUM],pEnt->d_name);//追加路径文件名,并存在其对应的数字下
                    File->FileNum++;//文件记录数+1
                }//如果这幅图是AVI暂停背景图，放在第八张
                else if (strstr(Check_FileName,avi_stop) != NULL)//如果是AVI暂停界面，放在第八张
                {
                    sprintf(File->FilePath[AVI_STOP_NUM],url);//先获得路径,并存在其对应的数字下
                    strcat(File->FilePath[AVI_STOP_NUM],pEnt->d_name);//追加路径文件名,并存在其对应的数字下
                    File->FileNum++;//文件记录数+1
                } 
                else if (strstr(Check_FileName,Camera_Pho) != NULL)//如果是摄像机相册界面，放在第九张
                {
                    sprintf(File->FilePath[CAMERA_PHO],url);//先获得路径,并存在其对应的数字下
                    strcat(File->FilePath[CAMERA_PHO],pEnt->d_name);//追加路径文件名,并存在其对应的数字下
                    File->FileNum++;//文件记录数+1
                }
                else if (strstr(Check_FileName,Camera_Play) != NULL)//如果是摄像机开始摄像界面，放在第十张
                {
                    sprintf(File->FilePath[CAMERA_PLAY],url);//先获得路径,并存在其对应的数字下
                    strcat(File->FilePath[CAMERA_PLAY],pEnt->d_name);//追加路径文件名,并存在其对应的数字下
                    File->FileNum++;//文件记录数+1
                }
                else if (strstr(Check_FileName,Camera_Stop) != NULL)//如果是摄像机暂停摄像界面，放在第十一张
                {
                    sprintf(File->FilePath[CAMERA_STOP],url);//先获得路径,并存在其对应的数字下
                    strcat(File->FilePath[CAMERA_STOP],pEnt->d_name);//追加路径文件名,并存在其对应的数字下
                    File->FileNum++;//文件记录数+1
                }
                else if (strstr(Check_FileName,Camera_Tip_AVI) != NULL)//如果是摄像机存储提示图，放在第十二张
                {
                    sprintf(File->FilePath[CAMERA_TIP_AVI],url);//先获得路径,并存在其对应的数字下
                    strcat(File->FilePath[CAMERA_TIP_AVI],pEnt->d_name);//追加路径文件名,并存在其对应的数字下
                    File->FileNum++;//文件记录数+1
                }
            }
        }
        else
        {
            closedir(pDir);//读取完成关闭目录
            break;
        }
    }
    printf("System File:\n");
    for (int i = 0; i < File->FileNum; i++)
    {
        printf("%s\n",File->FilePath[i]);
    }
    printf("Other File:\n");
}

