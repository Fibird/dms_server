#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/tcp.h>

#include "include/socket_exception.h"
#include "include/client_thread.h"
#include "include/server_socket.h"
#include "include/thread_exception.h"

using std::cout;
using std::endl;

//#define _DEBUG
#define IP_ADDR "172.29.23.167"
#define PORT 4096

/**************************************************
*作者：Liu Chaoyang
*日期：2017.06.12
*函数名：ServerSocket
*功能：default constructor of ServerSocket
*输入参数：none
*输出参数：none
*返回值：none
**************************************************/
ServerSocket::ServerSocket()
{
    int ret = socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (ret < 0)
    {
#ifdef _DEBUG
        cout << "Socket creation failed!" << endl;
#endif
        throw SocketException("Socket creation failed!");
    }
    else
    {
        cout << "OK:socket creation succeeded." << endl;
    }
    strcpy(server_ip, IP_ADDR);
    server_port = PORT;
}

ServerSocket::ServerSocket(short server_port, char *server_ip)
{
    int ret = socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (ret < 0)
    {
#ifdef _DEBUG
        cout << "Socket creation failed!" << endl;
#endif
        throw SocketException("Socket creation failed!");
    }
    else
    {
#ifdef _DEBUG
        cout << "OK:Socket creation succeeded." << endl;
#endif
    }
    strcpy(this->server_ip, server_ip);
    this->server_port = server_port;
}

/**************************************************
*作者：Liu Chaoyang
*日期：2017.06.12
*函数名：ServerSocket
*功能：Destructor of ServerSocket
*输入参数：none
*输出参数：none
*返回值：none
**************************************************/
ServerSocket::~ServerSocket()
{
    close(socket_fd);
    close(client_sockfd);
}

/**************************************************
*作者：Liu Chaoyang
*日期：2017.06.12
*函数名：acceptClient
*功能：listen requests from client, and return socket
* file descriptor and create a thread to communicate
* with the client if there is request
*输入参数：none
*输出参数：none
*返回值：none
**************************************************/
void ServerSocket::acceptClient()
{
    // name socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(server_ip);
    server_address.sin_port = htons(server_port);
    int server_len = sizeof(server_address);

    // bind socket and address 
    int ret = bind(socket_fd, (struct sockaddr *)&server_address, server_len);

    if (ret < 0)
    {
#ifdef _DEBUG
        cout << "Socket bind failed!" << endl;
#endif
        throw SocketException("Socket bind failed!");
    }
    else
    {
#ifdef _DEBUG
        cout << "OK:socket bind succeeded." << endl;
#endif
    }


    // listen socket
    listen(socket_fd, 100);

    // waiting for request of connection
    while (true)
    {
        struct sockaddr_in client_address;
        int client_len = sizeof(client_address);
        cout << "Waiting for connections..." << endl;
        client_sockfd = accept(socket_fd, (struct sockaddr *)&client_address, (socklen_t *)&client_len);
        if (client_sockfd < 0)
        {
#ifdef _DEBUG
            cout << "Connection error!" << endl;
#endif
            throw SocketException("Connection error!");
        }
        else
        {            
          int keepAlive = 1;
          setsockopt(socket_fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive));
          int keepIdle = 5;
          int keepInterval = 5;
          int keepCount = 5;
          setsockopt(socket_fd, SOL_TCP, TCP_KEEPIDLE, (void *)&keepIdle, sizeof(keepIdle));
          setsockopt(socket_fd, SOL_TCP,TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
          setsockopt(socket_fd,SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));
            //cout << "Connected to a new client." << endl;
            // start a new client thread to deal with the client connecting
            try
            {
                cout << "Starting a new client thread..." << endl;
                ClientThread *client_thread = new ClientThread(client_sockfd);
                client_thread->start();
                cout << "OK:a new client thread has started." << endl;
            }
            catch(ThreadException te)
            {
                cout << te.what() << endl;
            }
            catch(ServerException se)
            {
                cout << se.what() << endl;
            }
        }
    }
}

