#include <CommandManager.h>

_CommandManager::_CommandManager(size_t inBufferLength, size_t outBufferLength) : mInputBufferLength(inBufferLength), mOutputBufferLength(outBufferLength)
{
    mInputBuffer = new uint8_t[inBufferLength];
    mOutputBuffer = new uint8_t[outBufferLength];
    mLock = Lock();
}

Lock& _CommandManager::getLock()
{
    return mLock;
}

_CommandManager::StatusCode _CommandManager::status()
{
    return mStatusCode;
}

bool _CommandManager::handleCommand(const uint8_t *buffer, size_t length)
{
    // Lock to prevent writing from interupts
    // LOCK(mLock, locker)
    {
        // // Return if lock couldn't be acquired
        // if (!locker.acquired) 
        // {
        //     mStatusCode = StatusCode::LOCK_ACQUIRE_FAILED;
        //     return false;
        // }

        // Copy buffer
        const uint8_t* src = buffer;

        // Grab command ID
        euint8_t cmdID = *src;

        // Verify if ID is possible
        if (cmdID >= mCommandsCount) 
        {
            mStatusCode = StatusCode::CMD_ID_OUT_OF_RANGE;
            return false;
        }

        ++src;
        for (uint8_t* dst = mInputBuffer; dst < mInputBuffer+length-1; ++src, ++dst)
        {
            *dst = *src;
        }

        // Call command
        BaseFunction* func = mCommands[cmdID];
        DataHook input = DataHook(mInputBuffer, func->paramSize());
        DataHook output = DataHook(mOutputBuffer, func->returnSize());
        func->operator()(input, output);

        // Send result
        if (mSendCB == nullptr) 
        {
            mStatusCode = StatusCode::CALLBACK_NULL;
            return false;
        }
        mSendCB(output.buffer, output.length);
    }
    mStatusCode = StatusCode::NONE;
    return true;
}

void _CommandManager::setSendCB(sendCB_t cb)
{
    mSendCB = cb;
}

void _CommandManager::setCommands(BaseFunction_ptr *commands, size_t length)
{
    mCommands = commands;
    mCommandsCount = length;
}

_CommandManager::~_CommandManager()
{
    delete[] mInputBuffer;
    delete[] mOutputBuffer;
}