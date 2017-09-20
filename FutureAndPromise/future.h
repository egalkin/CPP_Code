#ifndef FUTURE_H
#define FUTURE_H
#include "shared_state.h"

template <typename T>
class promise;



template <typename T>
class future
{
    public:
        future(const future &) = delete;
        future& operator=(future &) = delete;

        future(future &&) = default;
        future& operator=(future &&) = default;

        virtual ~future();
        T get() const;
        void wait() const;
        bool is_ready() const;

    protected:

    private:
        friend class promise<T>;
        future(std::shared_ptr<shared_state<T>> state) : _state{state} {};
        std::shared_ptr<shared_state<T>> _state;
};




template<typename T>
T future<T>::get() const{
    wait();
    return _state->value;
}

template<>
void future<void>::get() const {
    wait();
}

template<typename T>
void future<T>::wait() const{
    if (!_state)
        throw std::runtime_error("future invalidated");
    if(_state->set_flag)
        return;
    std::unique_lock<std::mutex> locker(_state->mut);
    _state->cv.wait(locker);
}


template<typename T>
bool future<T>::is_ready() const{
    return _state->set_flag;
}

template<typename T>
future<T>::~future()
{
    //dtor
}

#endif // FUTURE_H
