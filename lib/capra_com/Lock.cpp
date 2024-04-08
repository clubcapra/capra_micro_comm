#include "Lock.h"

bool Lock::tryAcquire()
{
    if (mLocked) return false;
    mLocked = true;
    return true;
}

void Lock::release()
{
    mLocked = false;
}

_Locker::_Locker(Lock &lck) : lock(lck)
{
    acquired = lck.tryAcquire();
}

_Locker::~_Locker()
{
    if (acquired) lock.release();
}
