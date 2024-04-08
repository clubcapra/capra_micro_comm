#pragma once

#include <common.h>
#include <DataHook.h>
#include <Lock.h>
#include <Function.h>

class _CommandManager
{
public:
    typedef void (*sendCB_t)(uint8_t* buff, size_t length);
    
    enum StatusCode
    {
        NONE,
        LOCK_ACQUIRE_FAILED,
        CMD_ID_OUT_OF_RANGE,
        CALLBACK_NULL,
    };
private:
    uint8_t* mInputBuffer;
    const size_t mInputBufferLength;
    uint8_t* mOutputBuffer;
    const size_t mOutputBufferLength;
    Lock mLock;
    size_t mCommandsCount;
    BaseFunction_ptr* mCommands;
    sendCB_t mSendCB;
    StatusCode mStatusCode = StatusCode::NONE;

public:

    _CommandManager(size_t inBufferLength, size_t outBufferLength);

    Lock& getLock();

    StatusCode status();

    bool handleCommand(const uint8_t *buffer, size_t length);

    void setSendCB(sendCB_t cb);

    void setCommands(BaseFunction_ptr* commands, size_t length);

    // template <size_t SIZE>
    // void setCommands(BaseFunction_ptr (&commands)[SIZE]);

    ~_CommandManager();
};

// template <size_t SIZE>
// inline void _CommandManager::setCommands(BaseFunction_ptr (&commands)[SIZE])
// {
//     mCommandsCount = SIZE;
//     mCommands = commands;
// }
