#pragma once

class Lock
{
private:
    volatile bool mLocked;
public:
    bool tryAcquire();
    void release();
};

struct _Locker
{
    Lock &lock;
    bool acquired;
    bool _init = false;
    
    _Locker(Lock &lck);
    ~_Locker();
};

// #define LOCK(_lock, lockerVar) for (_Locker lockerVar = _Locker(_lock); !lockerVar._init; lockerVar._init = true)