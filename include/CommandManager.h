#pragma once

#include <common.h>
#include <DataHook.h>
#include <Lock.h>
#include <Function.h>
#include <Buffer.h>

class _CommandManager
{
public:
    typedef void (*sendCB_t)(uint8_t* buff, size_t length);
    
    enum StatusCode
    {
        NONE,
        CMD_ID_OUT_OF_RANGE,
        PEEK_ID_ERROR,
        READ_ID_ERROR,
        READ_PARAM_ERROR,
        PARAM_SIZE_MISMATCH,
        CALLBACK_NULL,
    };
private:
    uint8_t* mInputBuffer;
    const size_t mInputBufferLength;
    uint8_t* mOutputBuffer;
    const size_t mOutputBufferLength;
    size_t mCommandsCount;
    BaseFunction_ptr* mCommands;
    sendCB_t mSendCB;
    StatusCode mStatusCode = StatusCode::NONE;

public:

    _CommandManager(size_t inBufferLength, size_t outBufferLength);

    StatusCode status();

    bool handleCommand(const uint8_t *buffer, size_t length);

    bool handleCommand(Buffer& buff);

    void setSendCB(sendCB_t cb);

    void setCommands(BaseFunction_ptr* commands, size_t length);

    ~_CommandManager();
};

