#pragma once

#include <common.h>
#include <DataHook.h>
#include <Lock.h>
#include <Function.h>
#include <Buffer.h>

#if defined(__EXCEPTIONS)

#include <string>

class CommandIDOutOfRangeException : public std::exception 
{
public:
    const int mid;
    CommandIDOutOfRangeException(int id) : mid(id)
    {
    }
    const char* what() const throw()
    {
        auto res = std::string("Id out of range ");
        int id = mid;
        res += std::to_string(id);
        return res.c_str();
    }
};

class ParamSizeMismatchException : public std::exception
{
public:
    int expectedSize;
    int actualSize;
    ParamSizeMismatchException(int expectedSize, int actualSize)
    {
        this->expectedSize = expectedSize;
        this->actualSize = actualSize;
    }

    const char* what() const throw()
    {
        const auto res = std::string("Parameter size mismatch, expected ") +
            std::to_string(this->expectedSize) +
            " but got " +
            std::to_string(this->actualSize);
        return res.c_str();
    }
};

class CallbackNullException : public std::exception
{
public:
    CallbackNullException() {}
    const char* what() const throw()
    {
        return "Callback is not assigned. You forgot to set the callback: CommandManager.setSendCB(&sendCallback)";
    }
};

class PeekIDException : public std::exception
{
public:
    PeekIDException(){}
    const char* what() const throw()
    {
        return "Failed to peek ID";
    }
};

class ReadIDException : public std::exception
{
public:
    ReadIDException(){}
    const char* what() const throw()
    {
        return "Failed to read ID";
    }
};

class ReadParamException : public std::exception
{
public:
    ReadParamException(){}
    const char* what() const throw()
    {
        return "Failed to read param";
    }
};

#endif // __EXCEPTIONS


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

    /**
     * @brief The status of the command manager
     * 
     * @return A StatusCode
     */
    StatusCode status();


    /**
     * @brief Handle a command
     * 
     * @param buffer A raw byte array
     * @param length The number of bytes to use from the buffer
     * @return true If successful
     * @return false If failed
     * @throws CommandIDOutOfRangeException If the command ID is out of range
     * @throws ParamSizeMismatchException If the parameter size is different than expected
     * @throws CallbackNullException If the command manager send callback hasn't been set
     */
    bool handleCommand(const uint8_t *buffer, size_t length);

    /**
     * @brief Handle a command
     * 
     * @param buffer A raw byte array
     * @param length The number of bytes to use from the buffer
     * @return true If successful
     * @return false If failed
     * @throws CommandIDOutOfRangeException If the command ID is out of range
     * @throws ParamSizeMismatchException If the parameter size is different than expected
     * @throws CallbackNullException If the command manager send callback hasn't been set
     * @throws PeekIDException If peeking the id from the buffer fails
     * @throws ReadIDException If reading the id from the buffer fails
     * @throws ReadParamException If reading the parameters from the buffer fails
     */
    bool handleCommand(Buffer& buff);

    /**
     * @brief Set the Send callback function
     * 
     * @param cb A function pointer with the signature void (*)(uint8_t*, size_t)
     */
    void setSendCB(sendCB_t cb);

    /**
     * @brief Set the command array
     * 
     * @param commands Commands array as given in api.h, generated by the capra_micro_comm_py package
     * @param length The number of commands (COMMANDS_COUNT)
     */
    void setCommands(BaseFunction_ptr* commands, size_t length);

    ~_CommandManager();
};

