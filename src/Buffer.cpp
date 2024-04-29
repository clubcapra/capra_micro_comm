#include "Buffer.h"

Buffer::Buffer(size_t size)
{
    mBuff = mHead = mTail = new uint8_t[size];
    mSize = size;
    mBytesStored = 0;
}

// Buffer::Buffer(const Buffer& b)
// {
        
// }

int Buffer::peek()
{
    if (!available()) return -1;
    return *mHead;
}

size_t Buffer::write(uint8_t b)
{
    if (!availableForWrite()) return 0;
    *mTail = b;
    incTail();
    return 1;
}

size_t Buffer::write(uint8_t *b, size_t length)
{
    size_t l = GMin(length, availableForWrite());
    for (size_t i = 0; i < l; ++i)
    {
        *mTail = *(b+i);
        incTail();
    }
    return l;
}

size_t Buffer::available()
{
    return mBytesStored;
}

size_t Buffer::availableForWrite()
{
    return mSize - available();
}

int Buffer::read()
{
    if (!available()) return -1;
    uint8_t r = *mHead;
    incHead();
    return r;
}

size_t Buffer::read(uint8_t *b, size_t length)
{
    size_t l = GMin(length, available());
    for (size_t i = 0; i < l; ++i)
    {
        *(b+i) = *mHead;
        incHead();
    }
    return l;
}

Buffer::~Buffer()
{
    delete mBuff;
    mBuff = mHead = mTail = nullptr;
}

void Buffer::incHead()
{
    ++mHead;
    --mBytesStored;
    if (mHead == mBuff + mSize) mHead = mBuff;
}

void Buffer::incTail()
{
    ++mTail;
    ++mBytesStored;
    if (mTail == mBuff + mSize) mTail = mBuff;
}

