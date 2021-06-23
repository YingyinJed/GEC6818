#include "ConnectOneNet.h"

int sockfd;

//==========================================================
//  函数名称：   TcpClientInit
//  函数功能：   Tcp客户端初始化
//  入口参数：   
//          ServerIp    --->服务器IP
//          ServerPort  --->服务器端口号
//  返回参数：   无
//  说明：     无
//==========================================================
void TcpClientInit(const char *ServerIp ,int ServerPort)
{
    int res;
    struct sockaddr_in ser;

    sockfd = socket(AF_INET ,SOCK_STREAM ,0);
    if (sockfd < 0)
    {
        printf("socket error!\n");
    }else{
        printf("socket success!\n");
    }

    memset(&ser ,0 ,sizeof(ser));
    ser.sin_family = AF_INET;
    ser.sin_port = htons(ServerPort);
    ser.sin_addr.s_addr = inet_addr(ServerIp);

    res = connect(sockfd ,(struct sockaddr *)&ser ,sizeof(ser));
    if (res < 0)
    {
        printf("connect error!\n");
    }else{
        printf("connect success!\n");
    }
}

//==========================================================
//  函数名称：   TcpClientClose
//  函数功能：   Tcp套接字关闭
//  入口参数：   无
//  返回参数：   无
//  说明：     无
//==========================================================
void TcpClientClose(void)
{
    close(sockfd);
}

//==========================================================
//	函数名称：	OneNet_Init
//	函数功能：	Tcp客户端初始化 和 连接OneNet平台
//	入口参数：	无
//	返回参数：	无
//	说明：		无
//==========================================================
void OneNet_Init(void)
{
	TcpClientInit(SERVERIP ,SERVERPORT);
	//接入 OneNet 云平台
	int ret = OneNet_DevLink(DEVICE_ID, AUTH_KEY);
	if (ret < 0)
	{
		printf("OneNet_DevLink error!\n");
	}else{
		printf("OneNet_DevLink success!\n");
		
	}

}

//==========================================================
//	函数名称：	OneNet_DevLink
//	函数功能：	与onenet创建连接
//	入口参数：	无
//	返回参数：	0-成功	-1-失败
//	说明：		与onenCIPTMODEet平台建立连接
//==========================================================
int OneNet_DevLink(const char* devid, const char* auth_key)
{
	EdpPacket* pEdp = NULL;   //数据包

	pEdp = PacketConnect1(devid, auth_key);					   //根据devid 和 apikey封装协议包 
	if (pEdp == NULL)
	{
		printf("PacketConnect1 error!\n");
		return -1;
	}else{
		printf("PacketConnect1 success!\n");
	} 

	int ret = send(sockfd ,pEdp->_data, pEdp->_write_pos ,0);       //发送数据到平台
	if (ret < 0)
	{
		printf("send send_pkg->data error!\n");
	}else{
		printf("send send_pkg->data success!\n");
		return -1;
	}

	usleep(100*1000);	
	DeleteBuffer(&pEdp);										//删包
	printf("DeleteBuffer success!\n");

	return 0;
}

//把数据设置为平台可识别的格式
void OneNet_FillBuf(char *buf,char *buf1,int number)  
{
	char text[25] = {0};
	memset(buf, 0, sizeof(buf));
	strcat((char *)buf, ",;");	
	strcat((char *)buf, buf1);
	strcat((char *)buf, ",");
	sprintf(text,"%d",number);
	strcat((char *)buf, text);
	strcat((char *)buf, ";");
}

//==========================================================
//	函数名称：	OneNet_SendData
//	函数功能：	上传数据到平台
//	入口参数：	type：发送数据的格式
//	返回参数：	无
//	说明：		
//==========================================================
void OneNet_SendData(char *buf,char *buf1,int number)
{
	EdpPacket* pEdp = NULL;   //数据包

  	OneNet_FillBuf(buf,buf1,number);														//封装数据流	
	
	pEdp = PacketSavedataSimpleString(NULL,(const char *) buf ,0);							//封包-Type5
	
	int ret = send(sockfd ,pEdp->_data, pEdp->_write_pos ,0);       //发送数据到平台
	if (ret < 0)
	{
		
	}else{
		
		return ;
	}
	
	DeleteBuffer(&pEdp);		 		//删包
	
}

//==========================================================
//	函数名称：	recv_thread_func
//	函数功能：	上传数据到平台
//	入口参数：	arg     socket描述符
//	返回参数：	无
//	说明：	   	这里只是给出了一个从socket接收数据的例子, 其他方式请查询相关socket api
//          	一般来说, 接收都需要循环接收, 是因为需要接收的字节数 > socket的读缓存区时, 一次recv是接收不完的
//  返回值:  无	
//==========================================================
void OneNet_RecvData(void* arg , char CmdBuffer[])
{
    int sockfd = *(int*)arg;
    int error = 0;
    int n, rtn;
    uint8 mtype, jsonorbin;
    char buffer[4096];
    RecvBuffer* recv_buf = NewBuffer();
    EdpPacket* pkg;
    
    char* src_devid;
    char* push_data;
    uint32 push_datalen;

    cJSON* save_json;
    char* save_json_str;

    cJSON* desc_json;
    char* desc_json_str;
    char* save_bin; 
    uint32 save_binlen;
    unsigned short msg_id;
    unsigned char save_date_ret;

    char* cmdid;
    uint16 cmdid_len;
    char*  cmd_req;
    uint32 cmd_req_len;
    EdpPacket* send_pkg;
    char* ds_id;
    double dValue = 0;
    int iValue = 0;
    char* cValue = NULL;

    char* simple_str = NULL;
    char cmd_resp[] = "ok";
    unsigned cmd_resp_len = 0;

	DataTime stTime = {0};

    FloatDPS* float_data = NULL;
    int count = 0;
    int i = 0;

    struct UpdateInfoList* up_info = NULL;

#ifdef _DEBUG
    printf("[%s(%d)] recv thread start ...\n", __func__, __LINE__);
#endif

    while (error == 0)
    {
        /* 试着接收1024个字节的数据 */
        n = Recv(sockfd, buffer, sizeof(buffer), MSG_NOSIGNAL);
        if (n <= 0)
            break;
        printf("[%d]recv from server, bytes: %d\n", __LINE__, n);
        /* wululu test print send bytes */
       // hexdump((const unsigned char *)buffer, n);
        /* 成功接收了n个字节的数据 */
        WriteBytes(recv_buf, buffer, n);
        while (1)
        {
            /* 获取一个完成的EDP包 */
            if ((pkg = GetEdpPacket(recv_buf)) == 0)
            {
                printf("[%d]need more bytes...\n", __LINE__);
                break;
            }
            /* 获取这个EDP包的消息类型 */
            mtype = EdpPacketType(pkg);
			printf("EdpPacketType is %d \n", mtype);

            /* 根据这个EDP包的消息类型, 分别做EDP包解析 */
            switch(mtype)
            {
                case CONNRESP:																//连接请求响应 (client to server)
                    /* 解析EDP包 - 连接响应 */
                    rtn = UnpackConnectResp(pkg);
                    printf("[%d]recv connect resp, rtn: %d\n", __LINE__, rtn);
                    break;
                case PUSHDATA:																//转发（透传）数据 (双向)
                    /* 解析EDP包 - 数据转发 */
                    UnpackPushdata(pkg, &src_devid, &push_data, &push_datalen);
                    printf("recv push data, src_devid: %s, push_data: %s, len: %d\n",
                           src_devid, push_data, push_datalen);
                    free(src_devid);
                    free(push_data);
                    break;
                case UPDATERESP:															//平台下发当前最新的软件信息 (client to server)
                    UnpackUpdateResp(pkg, &up_info);
                    while (up_info){
                        printf("name = %s\n", up_info->name);
                        printf("version = %s\n", up_info->version);
                        printf("url = %s\nmd5 = ", up_info->url);
                        for (i=0; i<32; ++i){
                            printf("%c", (char)up_info->md5[i]);
                        }
                        printf("\n");
                        up_info = up_info->next;
                    }
                    FreeUpdateInfolist(up_info);
                    break;

                case SAVEDATA:																//存储（转发）数据 (双向)
                    /* 解析EDP包 - 数据存储 */
                    if (UnpackSavedata(pkg, &src_devid, &jsonorbin) == 0)
                    {
                        if (jsonorbin == kTypeFullJson
                            || jsonorbin == kTypeSimpleJsonWithoutTime
                            || jsonorbin == kTypeSimpleJsonWithTime)
                        {
                            printf("[%d]json type is %d\n", __LINE__, jsonorbin);
                            /* 解析EDP包 - json数据存储 */
                            /* UnpackSavedataJson(pkg, &save_json); */
                            /* save_json_str=cJSON_Print(save_json); */
                            /* printf("recv save data json, src_devid: %s, json: %s\n", */
                            /*     src_devid, save_json_str); */
                            /* free(save_json_str); */
                            /* cJSON_Delete(save_json); */

                            /* UnpackSavedataInt(jsonorbin, pkg, &ds_id, &iValue); */
                            /* printf("ds_id = %s\nvalue= %d\n", ds_id, iValue); */

                            UnpackSavedataDouble(jsonorbin, pkg, &ds_id, &dValue);
                            printf("[%d]ds_id = %s\nvalue = %f\n", __LINE__, ds_id, dValue);

                            /* UnpackSavedataString(jsonorbin, pkg, &ds_id, &cValue); */
                            /* printf("ds_id = %s\nvalue = %s\n", ds_id, cValue); */
                            /* free(cValue); */

                            free(ds_id);
    				
                        }
                        else if (jsonorbin == kTypeBin)
                        {/* 解析EDP包 - bin数据存储 */
                            UnpackSavedataBin(pkg, &desc_json, (uint8**)&save_bin, &save_binlen);
                            desc_json_str=cJSON_Print(desc_json);
                            printf("recv save data bin, src_devid: %s, desc json: %s, bin: %s, binlen: %d\n",
                                   src_devid, desc_json_str, save_bin, save_binlen);
                            free(desc_json_str);
                            cJSON_Delete(desc_json);
                            free(save_bin);
                        }
                        else if (jsonorbin == kTypeString ){
                            UnpackSavedataSimpleString(pkg, &simple_str);
    			    
                            printf("%s\n", simple_str);
                            free(simple_str);
                        }else if (jsonorbin == kTypeStringWithTime){
    						UnpackSavedataSimpleStringWithTime(pkg, &simple_str, &stTime);
    			    
                            printf("time:%u-%02d-%02d %02d-%02d-%02d\nstr val:%s\n", 
    							stTime.year, stTime.month, stTime.day, stTime.hour, stTime.minute, stTime.second, simple_str);
                            free(simple_str);
    					}else if (jsonorbin == kTypeFloatWithTime){
                            if(UnpackSavedataFloatWithTime(pkg, &float_data, &count, &stTime)){
                                printf("UnpackSavedataFloatWithTime failed!\n");
                            }

                            printf("read time:%u-%02d-%02d %02d-%02d-%02d\n", 
                                stTime.year, stTime.month, stTime.day, stTime.hour, stTime.minute, stTime.second);
                            printf("read float data count:%d, ptr:[%p]\n", count, (FloatDPS*)float_data);
                            
                            for(i = 0; i < count; ++i){
                                printf("ds_id=%u,value=%f\n", float_data[i].ds_id, float_data[i].f_data);
                            }

                            free(float_data);
                            float_data = NULL;
                        }
                        free(src_devid);
                    }else{
                        printf("error\n");
                    }
                    break;
            case SAVEACK:																			//存储确认（server to client）
                UnpackSavedataAck(pkg, &msg_id, &save_date_ret);
                printf("[%d]save ack, msg_id = %d, ret = %d\n", __LINE__, msg_id, save_date_ret);
                break;
            case CMDREQ:																			//云端发送命令响应（server to client）
                if (UnpackCmdReq(pkg, &cmdid, &cmdid_len, &cmd_req, &cmd_req_len) == 0)
				{
					printf("[%d]cmdid: %s, req:%s, req_len: %d\n", __LINE__, cmdid, cmd_req, cmd_req_len);
					strcpy(CmdBuffer ,cmd_req );  //将接收到的指令拷贝到 CmdBuffer 中					 
		    
                    free(cmdid);
                    free(cmd_req);
                }
                break;
            case PINGRESP:																			//心跳响应 (client to server)
                /* 解析EDP包 - 心跳响应 */
                UnpackPingResp(pkg);
                printf("recv ping resp\n");
                break;

            default:
                /* 未知消息类型 */
                error = 1;
                printf("recv failed...\n");
                break;
            }
            DeleteBuffer(&pkg);
            break;
        }
        break;
    }
    DeleteBuffer(&recv_buf);
} 