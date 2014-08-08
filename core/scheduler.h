#ifndef __NB_SCHEDULER_H__
#define __NB_SCHEDULER_H__
#include "nbthread.h"
class Scheduler: public Runable{
public:
    
    void MainLoop();
    void Init();
    void Run();
};
#endif
