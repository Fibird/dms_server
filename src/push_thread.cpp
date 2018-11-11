#include "include/push_thread.h"

using std::cout;
using std::endl;

//#define _DEBUG
/**************************************************
*作者：Liu Chaoyang
*日期：2017.06.12
*函数名：PushThread
*功能：Constructor of PushThread
*输入参数：log_dao
*输出参数：none
*返回值：none
**************************************************/

PushThread::PushThread(bool _data_updated)
{
    data_updated = _data_updated;
}

/**************************************************
*作者：Liu Chaoyang
*日期：2017.06.12
*函数名：~PushThread
*功能：Destructor of PushThread
*输入参数：none
*输出参数：none
*返回值：none
**************************************************/
PushThread::~PushThread()
{

}

/**************************************************
*作者：Liu Chaoyang
*日期：2017.06.26
*函数名：run
*功能：get data from log queue and insert data
*     into database
*输入参数：none
*输出参数：none
*返回值：none
**************************************************/
void PushThread::run()
{
        int socket_fd=socket(AF_INET,SOCK_STREAM,0);
        if (socket_fd<0)
        {
            throw SocketException("Create socket failed");
        }
        else
        {
#ifdef _DEBUG
            cout<<"OK:client create socket."<<endl;
#endif
        }

        //server sockaddr info.
        sockaddr_in server_sockaddr;
        memset(&server_sockaddr,0,sizeof(sockaddr_in));
        server_sockaddr.sin_family=AF_INET;
        server_sockaddr.sin_addr.s_addr=inet_addr(server_ip.c_str());
        server_sockaddr.sin_port=htons(server_port);

        //create a connection request to server.
        int connet_fd=connect(socket_fd,(struct sockaddr *)&server_sockaddr,sizeof(struct sockaddr));
        if (connet_fd<0)
        {
            throw SocketException("Connect to server failed");
        }
        else
        {
#ifdef _DEBUG
            cout<<"OK:connect to server succeeded."<<endl;
#endif
        }
#ifndef _DEBUG
        cout<<"OK:connect to server succeeded."<<endl;
#endif
    } catch (ClientException  & e)
    {
        cout<<e.what()<<endl;
        exit(1);
    }
}

void PushThread::setDataUpdated(bool du)
{
    data_updated = du;
}

bool PushThread::getDataUpdated()
{
    return data_updated;
}
