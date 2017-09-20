#include <iostream>
#include <ThreadPool.h>

using namespace std;

mutex mut;

void func( ){
    unique_lock<mutex> locker(mut);
    cout << "thread " << "started" << endl;
    locker.unlock();
    this_thread::sleep_for(chrono::seconds(2));
    locker.lock();
    cout << "thread "  <<  " is finished\n";
    locker.unlock();

}

int main()
{
    ThreadPool pool(5);
    for (int i = 0; i < 7; ++i){
        pool.execute(func);
    }




    return 0;
}
