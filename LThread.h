/* 
 * File:   LThread.h
 * Author: stanley
 *
 * Created on March 30, 2012, 3:55 PM
 */

#ifndef LTHREAD_H
#define	LTHREAD_H

#include <pthread.h>
#include <unistd.h>

class ThreadBase
{
public:
    ThreadBase();
    virtual ~ThreadBase(){}       ////don't forget!!!!!
    void start();
    void join();
protected:
    pthread_t thread_ID;
private:
    virtual void run()=0;
    static void* threadOps(void* pArg);   
    
};
#endif	/* LTHREAD_H */

