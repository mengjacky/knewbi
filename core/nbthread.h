#ifndef __NB_TREAD_H__ 
#define __NB_TREAD_H__ 
#include <pthread.h>
class Runable{
public:
    virtual void Run() {}
};

class Thread 
{
public:
    explicit Thread (Runable *r);
    Thread();
    virtual ~Thread ();
    void Start();
    void SetPid(pid_t pid){ pid_ = pid;}
    pid_t GetPid(){ return pid_; }
    pthread_t GetThreadId(){ return tid_;}
    void Stop(){ running_ = false;}
    Runable* GetRunable(){ return r_;}
    static void* ThreadEntity(void *arg);
    void SetRunning(bool status){ running_ = status;}
private:
    pthread_t tid_;
    pid_t     pid_;
    bool      running_;
    Runable *r_;
}; //class Thread

#endif
