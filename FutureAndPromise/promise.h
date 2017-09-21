#ifndef PROMISE_H
#define PROMISE_H
#include "shared_state.h"
#include "future.h"

template<typename T>
class promise
{
    public:
        promise() :
        _state {new shared_state<T>()},
        _get_flag {false}
        {};
        virtual ~promise();

        promise(const promise &) = delete;
        promise& operator=(promise &) = delete;

        promise(promise&&) = default;
        promise& operator=(promise &&) = default;

        void set(const T &);
        void set(const T  &&);
        void set_exception(const std::exception_ptr &);
        future<T> get_future();



    private:
        std::shared_ptr<shared_state<T>> _state;
        std::atomic_bool _get_flag;
};


template<typename T>
void promise<T>::set(const T  & arg){
    if (_state->set_flag){
        throw std::runtime_error("Tries to set value again");
    }
    if (_state->ex_flag){
        throw std::runtime_error("Exception value already set");
    }
    {
        std::unique_lock<std::mutex> locker(_state->mut);
        _state->value = arg;
    }
    _state->set_flag = true;
    _state->cv.notify_one();
}

template<typename T>
void promise<T>::set(const T && arg){
    if (_state->set_flag){
        throw std::runtime_error("Tries to set value again");
    }
    if (_state->ex_flag){
        throw std::runtime_error("Exception value already set");
    }
    {
        std::unique_lock<std::mutex> locker(_state->mut);
        _state->value = std::move(arg);
    }
    _state->set_flag = true;
    _state->cv.notify_one();
}

template<typename T>
void promise<T>::set_exception(const std::exception_ptr & e){
    if (_state->ex_flag){
        throw std::runtime_error("Tries to set exception again");
    }
    if (_state->set_flag){
        throw std::runtime_error("Value already set");
    }
    {
        std::unique_lock<std::mutex> locker(_state->mut);
        _state->ex = e;
    }
    _state->ex_flag = true;
    _state->cv.notify_one();

}

template<typename T>
future<T> promise<T>::get_future(){
    if (_get_flag){
        throw std::runtime_error("Tries to get future again");
    }
    if (!_state){
        throw std::runtime_error{"Promise not valid"};
    }
    _get_flag = true;
    return future<T>(_state);

}


template<typename T>
promise<T>::~promise()
{

}

template<>
class promise<void>
{
    public:
        promise()
            : _state {new shared_state<void>()}
            , _get_flag {false}
        {};

        ~promise();

        promise(const promise &) = delete;
        promise& operator=(promise const &) = delete;

        promise(promise&&) = default;
        promise& operator=(promise &&) = default;

        void set();
        void set_exception(const std::exception_ptr &);
        future<void> get_future();



    private:
        std::shared_ptr<shared_state<void>> _state;
        std::atomic_bool _get_flag;
};



void promise<void>::set(){
    if (_state->set_flag){
        throw std::runtime_error("Tries to set value again");
    }
     if (_state->ex_flag){
        throw std::runtime_error("Exception value already set");
    }
    _state->set_flag = true;
    _state->cv.notify_one();
}


void promise<void>::set_exception(const std::exception_ptr & e){
    if (_state->ex_flag){
        throw std::runtime_error("Tries to set exception again");
    }
    if (_state->set_flag){
        throw std::runtime_error("Value already set");
    }

    {
        std::unique_lock<std::mutex> locker(_state->mut);
        _state->ex = e;
    }
    _state->ex_flag = true;
    _state->cv.notify_one();

}

future<void> promise<void>::get_future(){
    if (_get_flag){
        throw std::runtime_error("Tries to get future again");
    }
    _get_flag = true;
    return future<void>(_state);
}



promise<void>::~promise()
{

}


#endif // PROMISE_H
