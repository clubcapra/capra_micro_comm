#ifndef BUFFER_H
#define BUFFER_H

#include "common.h"


class Buffer
{
private:
    size_t mSize;
    uint8_t *mBuff;
    uint8_t *mHead;
    uint8_t *mTail;
    size_t mBytesStored;

public:
    Buffer(size_t size);
    Buffer(const Buffer& b) = delete;
    Buffer(Buffer&& b) = default;
    int peek();
    size_t write(uint8_t b);
    size_t write(uint8_t* b, size_t length);
    size_t available();
    size_t availableForWrite();
    int read();
    size_t read(uint8_t* b, size_t length);

    ~Buffer();
private:
    void incHead();
    void incTail();
};

#endif // BUFFER_H