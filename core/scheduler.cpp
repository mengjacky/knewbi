#include <unistd.h>
#include "scheduler.h"
#include "nbcore.h"
#include "nbactor.h"
#include "msgqueue.h"
void Scheduler::MainLoop(){
    printf("scheduler thread start\n");
    while(true){
        if(g_act_queue == NULL){
            printf("g_act_queue is NULL\n");
            return ;
        }
        Actor *act = g_act_queue->PopActor();
        if(act == NULL){
            //wait
            printf("wait ..\n");
            usleep(1000000);
            continue;
        }
        printf("act handle=%d\n", act->ctx->handle);
        Message *msg = act->PopMsg();
        if(msg == NULL){
            printf("error: the actor no message should not put into global queue\n");
            continue;
        }

        act->HandleMsg(msg);
        if(act->MsgSize() > 0){
            g_act_queue->PushActor(act);
        }

    }
}

void Scheduler::Run(){
    MainLoop();
}
void Scheduler::Init(){

}
