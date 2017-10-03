#ifndef SHARED_STATE_H
#define SHARED_STATE_H
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>

template <typename T>
class shared_state
{
    public:
        shared_state() = default;
        T value;
        std::mutex mut;
        std::condition_variable cv;
        std::atomic_bool set_flag;
        std::atomic_bool ex_flag;
        std::exception_ptr ex;
};

template <typename T>
class shared_state<T&>{
    public:
        shared_state() = default;
        T a = T();
        T& value = a ;
        std::mutex mut;
        std::condition_variable cv;
        std::atomic_bool set_flag;
        std::atomic_bool ex_flag;
        std::exception_ptr ex;
};



template <>
class shared_state<void>{
    public:
        shared_state() = default;
        std::mutex mut;
        std::condition_variable cv;
        std::atomic_bool set_flag;
        std::atomic_bool ex_flag;
        std::exception_ptr ex;
};




#endif // SHARED_STATE_H
