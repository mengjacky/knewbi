#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "nbthread.h"
Thread::Thread():
    tid_(0),
    pid_(0),
    running_(false)

{

}
Thread::Thread(Runable *r):
    tid_(0),
    pid_(0),
    running_(false),
    r_(r)

{
    
}

Thread::~Thread()
{

}
void Thread::Start()
{
    if(r_ == NULL){
        printf("Runable is null\n");
        return ;
    }
    int ret = pthread_create(&tid_, NULL, ThreadEntity, this);
    if(ret != 0)
    {
        printf("create thread fail :%d\n", ret);
    }
}
void* Thread::ThreadEntity(void *arg)
{

    Thread *thd = (Thread*)arg;
    thd->SetPid(getpid());
    Runable *r = thd->GetRunable();
    if(r == NULL){
        printf("runable is NULL\n");
        return 0;
    }
    r->Run();
    return 0;
}
