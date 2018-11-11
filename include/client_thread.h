#ifndef CLIENT_THREAD_H
#define CLIENT_THREAD_H
#include "base_thread.h"
#include <list>
#include "include/data.h"

using std::list;

class ClientThread : public BaseThread
{
private:
    int conn_fd;
public:
    ClientThread(int conn_fd);
    ~ClientThread();
    void run();
    void pushData(list<MatchedLogRec> & matched_log);
    void readData(list<MatchedLogRec> & matched_log);
    void getData();
};

#endif // CLIENT_THREAD_H
