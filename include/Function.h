#pragma once

#include <BaseFunction.h>

template <typename R, typename T>
class Function : public BaseFunction
{
public:
    typedef R (*command_t)(T);
private:
    command_t mCmd;
    
public:
    Function(command_t command);
    virtual void operator() (DataHook& input, DataHook& output) override;
    virtual size_t paramSize() const override;
    virtual size_t returnSize() const override;
};

template <typename R, typename T>
inline void Function<R, T>::operator()(DataHook& input, DataHook& output)
{
    T param = input.decode<T>();
    R result = mCmd(param);
    output.encode<R>(result);
}

template<typename R, typename T> inline Function<R, T>::Function(command_t command)
{
    mCmd = command;
}

template <typename R, typename T>
inline size_t Function<R, T>::paramSize() const
{
    return sizeof(T);
}

template <typename R, typename T>
inline size_t Function<R, T>::returnSize() const
{
    return sizeof(R);
}