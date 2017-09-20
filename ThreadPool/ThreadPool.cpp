#include "ThreadPool.h"

#include <iostream>
#include <thread>

using namespace std;

ThreadPool::ThreadPool()
{
    //ctor
}

ThreadPool::ThreadPool(size_t numOfThreads){
    enabled = true;
    workers.resize(numOfThreads);
    for (int i = 0; i < numOfThreads; ++i){
        workers[i] = thread(&ThreadPool::run, this);
    }
}

void ThreadPool::execute(const function<void()>  &fn){
    unique_lock<mutex> locker(mut);
    tasksList.push(cref(fn));
    cv.notify_one();

}

void ThreadPool::run(){
    while(true){
        if (!enabled)
            return;
        unique_lock<mutex> locker(mut);
        if (tasksList.empty()){
            if (enabled)
                cv.wait(locker);
            else
                return;
        }
        if (!tasksList.empty()) {
            const function<void()> &fn = tasksList.front();
            tasksList.pop();
            locker.unlock();
            try{
                fn();
            }
            catch(...){
                cout << "Something gone wrong\n";
            }
            locker.lock();
        }
    }

}

ThreadPool::~ThreadPool()
{
    enabled = false;
    cv.notify_all();
    for (auto &t : workers)
        t.join();
}


