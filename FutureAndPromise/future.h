#ifndef FUTURE_H
#define FUTURE_H
#include "shared_state.h"

template <typename T>
class Promise;


template <typename T>
class Future
{
    public:
        Future(const Future &) = delete;
        Future& operator=(const Future &) = delete;

        Future(Future && that) noexcept
            : _state(that._state)
        { }

        Future & operator=(Future && that) noexcept {
            *this = std::move(that);
            return *this;
        }

        ~Future() = default;
        T Get() const;
        void wait() const;
        bool IsReady() const;

    private:
        explicit Future(std::shared_ptr<shared_state<T>> state) : _state{state} {};

    private:
        friend class  Promise<T>;
        std::shared_ptr<shared_state<T>> _state;
};

template<typename T>
T Future<T>::Get() const{
    wait();
    if (_state->ex_flag){
        throw _state->ex;
    }
    return _state->value;
}

template<>
void Future<void>::Get() const {
    wait();
    if (_state->ex_flag){
        throw _state->ex;
    }
}

template<typename T>
void Future<T>::wait() const{
    if (!_state)
        throw std::runtime_error("future not valid");
    if(_state->set_flag || _state->ex_flag)
        return;

    std::unique_lock<std::mutex> locker(_state->mut);
    _state->cv.wait(locker);
}


template<typename T>
bool Future<T>::IsReady() const{
    return _state->set_flag;
}

template <typename T>
class Future<T&>
{
    public:
        Future(const Future &) = delete;
        Future& operator=(Future &) = delete;


        Future(Future && that) noexcept
            : _state(that._state)
        { }

        Future & operator=(Future && that) noexcept {
            *this = std::move(that);
            return *this;
        }

        ~Future() = default;
        T& Get() const;
        void wait() const;
        bool IsReady() const;


    private:
        explicit Future(std::shared_ptr<shared_state<T&>> state) : _state{state} {};

    private:
        friend class  Promise<T&>;
        std::shared_ptr<shared_state<T&>> _state;
};

template<typename T>
T& Future<T&>::Get() const{
    wait();
    if (_state->ex_flag){
        throw _state->ex;
    }
    return _state->value;
}


template<typename T>
void Future<T&>::wait() const{
    if (!_state)
        throw std::runtime_error("future not valid");
    if(_state->set_flag || _state->ex_flag)
        return;

    std::unique_lock<std::mutex> locker(_state->mut);
    _state->cv.wait(locker);
}


template<typename T>
bool Future<T&>::IsReady() const{
    return _state->set_flag;
}


#endif // FUTURE_H
