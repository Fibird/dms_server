#ifndef PUSH_THREAD_H
#define PUSH_THREAD_H
#include "include/log_dao.h"
#include "include/base_thread.h"

class PushThread : public BaseThread
{
private:
    bool data_updated;
public:
    PushThread(bool _data_updated = false);
    ~PushThread();
    void setDataUpdated(bool du = true);
    bool getDataUpdated();
    void run();
};

#endif // PUSH_THREAD_H
