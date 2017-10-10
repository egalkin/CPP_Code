#ifndef PROMISE_H
#define PROMISE_H
#include "shared_state.h"
#include "future.h"

template<typename T>
class Promise
{
    public:
    Promise() :
        _state {new shared_state<T>()},
        _get_flag {false}
        {};
        ~Promise() = default;

        Promise(const Promise &) = delete;
        Promise& operator=(const Promise &) = delete;

        Promise(Promise && that) noexcept
            : _state(that._state)
        { }

        Promise & operator= (Promise && that) noexcept {
            *this = std::move(that);
            return *this;
        }

        void Set(const T &);
        void Set(const T  &&);
        void set_exception(const std::exception_ptr &);
        Future<T> GetFuture();



    private:
        std::shared_ptr<shared_state<T>> _state;
        std::atomic_bool _get_flag;
};


template<typename T>
void Promise<T>::Set(const T  & arg){
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
void Promise<T>::Set(const T && arg){
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
void Promise<T>::set_exception(const std::exception_ptr & e){
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
Future<T> Promise<T>::GetFuture(){
    if (_get_flag){
        throw std::runtime_error("Tries to get future again");
    }
    if (!_state){
        throw std::runtime_error{"Promise not valid"};
    }
    _get_flag = true;
    return Future<T>(_state);

}





template<typename T>
class Promise<T&>
{
    public:
    Promise() :
        _state {new shared_state<T&>()},
        _get_flag {false}
        {};
        ~Promise() = default;

        Promise(const Promise &) = delete;
        Promise& operator=(Promise &) = delete;

        Promise(Promise && that) noexcept
            : _state(that._state)
        { }

        Promise & operator= (Promise && that) noexcept {
            *this = std::move(that);
            return *this;
        }

        void Set(const T &);
        void Set(const T  &&);
        void set_exception(const std::exception_ptr &);
        Future<T&> GetFuture();



    private:
        std::shared_ptr<shared_state<T&>> _state;
        std::atomic_bool _get_flag;
};


template<typename T>
void Promise<T&>::Set(const T  & arg){
    if (_state->set_flag){
        throw std::runtime_error("Tries to set value again");
    }
    if (_state->ex_flag){
        throw std::runtime_error("Exception value already set");
    }
    {
        std::unique_lock<std::mutex> locker(_state->mut);
        _state->value =  arg;
    }
    _state->set_flag = true;
    _state->cv.notify_one();
}

template<typename T>
void Promise<T&>::Set(const T && arg){
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
void Promise<T&>::set_exception(const std::exception_ptr & e){
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
Future<T&> Promise<T&>::GetFuture(){
    if (_get_flag){
        throw std::runtime_error("Tries to get future again");
    }
    if (!_state){
        throw std::runtime_error{"Promise not valid"};
    }
    _get_flag = true;
    return Future<T&>(_state);

}





template<>
class Promise<void>
{
    public:
    Promise()
            : _state {new shared_state<void>()}
            , _get_flag {false}
        {};

        ~Promise() = default;

        Promise(const Promise &) = delete;
        Promise& operator=(Promise const &) = delete;

        Promise(Promise && that) noexcept
            : _state(that._state)
        { }

        Promise & operator= (Promise && that) noexcept {
            *this = std::move(that);
            return *this;
        }

        void Set();
        void set_exception(const std::exception_ptr &);
        Future<void> GetFuture();



    private:
        std::shared_ptr<shared_state<void>> _state;
        std::atomic_bool _get_flag;
};



void Promise<void>::Set(){
    if (_state->set_flag){
        throw std::runtime_error("Tries to set value again");
    }
     if (_state->ex_flag){
        throw std::runtime_error("Exception value already set");
    }
    _state->set_flag = true;
    _state->cv.notify_one();
}


void Promise<void>::set_exception(const std::exception_ptr & e){
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

Future<void> Promise<void>::GetFuture(){
    if (_get_flag){
        throw std::runtime_error("Tries to get future again");
    }
    _get_flag = true;
    return Future<void>(_state);
}






#endif // PROMISE_H
