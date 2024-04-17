#include <CommandManager.h>

_CommandManager::_CommandManager(size_t inBufferLength, size_t outBufferLength) : mInputBufferLength(inBufferLength), mOutputBufferLength(outBufferLength)
{
    mInputBuffer = new uint8_t[inBufferLength];
    mOutputBuffer = new uint8_t[outBufferLength];
}


_CommandManager::StatusCode _CommandManager::status()
{
    return mStatusCode;
}

bool _CommandManager::handleCommand(const uint8_t *buffer, size_t length)
{
    const uint8_t* src = buffer;

    // Grab command ID
    euint8_t cmdID = *src;

    // Verify if ID is possible
    if (cmdID >= mCommandsCount) 
    {
        mStatusCode = StatusCode::CMD_ID_OUT_OF_RANGE;
        return false;
    }

    // Verify length
    if (mCommands[cmdID]->paramSize() > length-1)
    {
        mStatusCode = StatusCode::PARAM_SIZE_MISMATCH;
        return false;
    }

    // Copy buffer
    ++src;
    for (uint8_t* dst = mInputBuffer; dst < mInputBuffer+length; ++src, ++dst)
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

bool _CommandManager::handleCommand(Buffer& buff)
{
    // Grab command ID
    int cmdID = buff.peek();
    DebugVarln(cmdID);

    if (cmdID == -1)
    {
        mStatusCode = StatusCode::PEEK_ID_ERROR;
        return false;
    }

    // Verify if ID is possible
    if (cmdID >= (int)mCommandsCount) 
    {
        mStatusCode = StatusCode::CMD_ID_OUT_OF_RANGE;
        return false;
    }
    BaseFunction* func = mCommands[cmdID];
    DebugVarln(func->paramSize());
    DebugVarln(func->returnSize());

    // Verify length
    if (func->paramSize() + 1 > buff.available())
    {
        mStatusCode = StatusCode::PARAM_SIZE_MISMATCH;
        return false;
    }

    int r = buff.read();

    if (r == -1)
    {
        mStatusCode = StatusCode::READ_ID_ERROR;
        return false;
    }
    DebugVarln(r);

    // Copy buffer
    size_t rr = buff.read(mInputBuffer, func->paramSize());
    DebugVarln(rr);
    if (rr != func->paramSize())
    {
        mStatusCode = StatusCode::READ_PARAM_ERROR;
        return false;
    }

    debugBytes(mInputBuffer, mInputBufferLength);

    // Call command
    DataHook input = DataHook(mInputBuffer, func->paramSize());
    DataHook output = DataHook(mOutputBuffer, func->returnSize());
    func->operator()(input, output);

    Debugln();
    debugBytes(mOutputBuffer, mOutputBufferLength);

    // Send result
    if (mSendCB == nullptr) 
    {
        mStatusCode = StatusCode::CALLBACK_NULL;
        return false;
    }

    Debugln("Sending");
    mSendCB(output.buffer, output.length);
    Debugln("Sent");

    mStatusCode = StatusCode::NONE;
    return true;
}
