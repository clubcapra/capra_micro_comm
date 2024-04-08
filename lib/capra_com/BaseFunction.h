#pragma once

#include <DataHook.h>

class BaseFunction
{
public:
    virtual size_t paramSize() const = 0;
    virtual size_t returnSize() const = 0;
    virtual void operator() (DataHook& input, DataHook& output) = 0;
};

using BaseFunction_ptr = BaseFunction*;