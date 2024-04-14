#pragma once

#include <common.h>

struct DataHook
{
    const size_t length;
    uint8_t *buffer;

    DataHook(uint8_t *buffer, size_t length) : length(length), buffer(buffer) {}

    template <typename T>
    T decode() const;

    template <typename T>
    void encode(T value);
};

template <typename T>
inline T DataHook::decode() const
{
    T *res = reinterpret_cast<T *>(buffer);
    return *res;
}

template <typename T>
inline void DataHook::encode(T value)
{
    const T *ref = &value;
    const uint8_t *val = reinterpret_cast<const uint8_t *>(ref);
    const uint8_t *src = val;
    for (uint8_t *dst=buffer; dst < buffer + sizeof(T); ++src, ++dst)
    {
        *dst = *src;
    }
}
