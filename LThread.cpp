
#include "LThread.h"
#include <iostream>


//---------------<constructor>----------------------
ThreadBase::ThreadBase()
{
}



//----------------<stdat thread runing>-------------
void ThreadBase::start()
{
    int pthread;
    pthread = pthread_create(&thread_ID, NULL, threadOps, (void*)this);
    if(pthread != 0)
    {
        std::cout<<"failed to create thread";
    }
            return;
    return;
}

//---------------<this is where the derived processing gets to run>----

void* ThreadBase::threadOps(void* pArg)
{
    ((ThreadBase*)pArg)->run();
    return 0;
}

//---------------<wait for child thread to exit>-----------------------

void ThreadBase::join()
{
    int pThread_J;
    pThread_J=pthread_join(thread_ID, NULL);
    if(pThread_J != 0)
    {
        std::cout<<"\n failed to join\n";
    }
    return;
}

#include <cstdlib>


using namespace std;

/*
 * 
 */
#ifdef TEST1
int main(int argc, char** argv) {
    
   // ThreadBase* tb = new ThreadBase();
   // delete tb;

    return 0;
}
#endif


