#include "include/client_thread.h"
#include "include/data.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <include/log_queue.h>
#include <iostream>
#include <cstring>
#include <include/data.h>
#include <cstdlib>
#include "include/server_exception.h"

//#define _DEBUG

using std::cout;
using std::endl;

ClientThread::ClientThread(int conn_fd) : conn_fd(conn_fd)
{

}

ClientThread::~ClientThread()
{
    // close this thread
    pthread_exit(NULL);
}

/**************************************************
*作者：Liu Chaoyang
*日期：2017.06.26
*函数名：run
*功能：receive message from clients
*输入参数：none
*输出参数：none
*返回值：none
**************************************************/
#define DATA_SENDER 1
#define DATA_GETTER 2

void ClientThread::run()
{
    int rlen;
    int flag;   
    rlen = recv(conn_fd, (int*)&flag, sizeof(int), 0);

    if (rlen < 0)
    {
#ifdef _DEBUG
            cout << "Receiving data failed!" << endl;
#endif
            throw ServerException("Receiving data failed!");
    }

    if (flag == DATA_SENDER)
    {
        getData();
    }
    else if (flag == DATA_GETTER)
    {
        pushData();
    }
}

void ClientThread::readData(list<MatchedLogRec> & matched_log)
{
    string matched_log_file ="matched_record.txt";
    cout<<"Starting to read unsended matched log..."<<endl;
#ifdef _TEST
    int num_read=0;
#endif
    try
    {
        //open the file storing unsended matched log.
        ifstream fin(unsended_file.c_str(),ifstream::in);
        if (fin.fail())
        {
            throw ReadException("Open file failed");
        }
        else
        {
#ifdef _DEBUG
            cout<<"OK:open \"unsended_matched_log\" file."<<endl;
#endif
        }
        //read the matched log and insert into the matched_log list.
        MatchedLogRec log;
        while (!fin.eof())
        {
            fin>>log;
            if (fin.fail())
            {
                if (!fin.eof())
                {
                    throw ReadException("Read unsended log failed");
                }
                else
                {
                    break;
                }

            }
            matched_log.push_back(log);
#ifdef _TEST
            ++num_read;
#endif
        }

        //close the file.
        fin.close();

        cout<<"OK:read unsended matched log finished."<<endl;

#ifdef _TEST
        cout<<"Number of matched log read:"<<num_read<<endl;
#endif
    } catch (ClientException & e)
    {
        cout<<e.what()<<endl;
        exit(-1);
    }
}
void ClientThread::pushData(list<MatchedLogRec> & matched_log)
{
    int num_send_log=0;
    try
    {
        cout<<"Starting to send data to server..."<<endl;
        int send_num;
        for (list<MatchedLogRec>::iterator it=matched_log.begin();it!=matched_log.end();)
        {
#ifdef _DEBUG
            sleep(1);
#endif
            send_num=send(socket_fd,(void *)&(*it),sizeof(MatchedLogRec),0);
            if (send_num<0)
            {
                throw SendException("Send data to server failed");
            }
            else
            {
#ifdef _DEBUG
                cout<<*it<<endl;
                cout<<"OK:client socket sended."<<endl;
#endif
                ++num_send_log;
                it=matched_log.erase(it);
            }
        }
        cout<<"OK:send data to server finished."<<endl;
        cout<<"Total number of sended matched log: "<<num_send_log<<endl;
    } catch (ClientException & e)
    {
        cout<<e.what()<<endl;
        cout<<"Total number of sended matched log: "<<num_send_log<<endl;
    }
}

void ClientThread::getData()
{
    int rlen;
    MatchedLogRec buf;
    int rcv_count = 0;      // count the number of received logs

#ifdef _DEBUG
    int error_num=0;
#endif

    while (true)
    {
        rlen = recv(conn_fd, (MatchedLogRec*)&buf, sizeof(MatchedLogRec), 0);
        if (rlen < 0)
        {
#ifdef _DEBUG
            cout << "Receiving data failed!" << endl;
#endif
            throw ServerException("Receiving data failed!");
        }
        else if (rlen == 0)
        {           
            //cout << "Disconnected to this client." << endl;
            cout << "Received: " << rcv_count << endl;
#ifdef _DEBUG
            cout<<"Error number: "<<error_num<<endl;
#endif
            delete this;
        }
        else if (rlen == 80)
        {
            rcv_count++;
#ifdef _DEBUG
            // print data received to console(just for test)
            //cout << buf << endl;
#endif
            // insert data received into log queue
            g_log_queue << buf;
        }
        else if (rlen < sizeof(MatchedLogRec))
        {
            int buf_cur_num = 0;
#ifdef _DEBUG
            //cout << "Oops!" << endl;
            error_num++;
#endif

            while (true)
            {
                buf_cur_num += rlen;
                if (buf_cur_num == 80)
                {
                    break;
                }
                //cout << "Before Receive: " << sizeof(MatchedLogRec) - buf_cur_num << endl;
                rlen = recv(conn_fd, (MatchedLogRec*)&buf + buf_cur_num,
                            sizeof(MatchedLogRec) - buf_cur_num, 0);
                //cout << "After Receive: " << rlen << endl;
                try
                {
                    if (rlen == 0)
                    {
                        break;
                    }
                    else if (rlen < 0)
                    {
                        throw ServerException("Receiving data failed!");
                    }
                }
                catch (ServerException s)
                {
                    cout << s.what() << endl;
                    delete this;
                }
            }

            rcv_count++;
#ifdef _DEBUG          
            // print data received to console(just for test)
            //cout << buf << endl;
#endif
            // insert data received into log queue
            g_log_queue << buf;
        }
    }
}
