#include "unity.h"
#include "capra_comm.h"
#include "Buffer.h"

#define TEST_BUFFER_SIZE 64
uint8_t* inputBuffer = new uint8_t[TEST_BUFFER_SIZE];
uint8_t* outputBuffer = new uint8_t[TEST_BUFFER_SIZE];
_CommandManager* cmdMan = &CommandManager;

#ifndef SAM
    EmptyStream debug = EmptyStream();
    HardwareSerial& comm = Serial;
#else
    Serial_& debug = SerialUSB;
    UARTClass& comm = Serial;
#endif

void setUp()
{
    Debugln("SetUp start");
    // cmdMan = new _CommandManager(TEST_BUFFER_SIZE, TEST_BUFFER_SIZE);
    Debugln("SetUp end");
}

void tearDown()
{
    Debugln("tearDown start");
    // delete cmdMan;
    Debugln("tearDown end");
}


struct TestData
{
    euint8_t test_euint8_t;
    eint8_t test_eint8_t;
    euint16_t test_euint16_t;
    eint16_t test_eint16_t;
    euint32_t test_euint32_t;
    eint32_t test_eint32_t;
    // euint64_t test_euint64_t;
    // eint64_t test_eint64_t;
    efloat_t test_efloat_t;
    eboolean_t test_eboolean_t;
};

#define ASSERT_TEST_DATA_VARIABLE(test, in, out, type) __CONCAT(TEST_ASSERT_EQUAL_, test)(in.__CONCAT(test_, type), out.__CONCAT(test_, type))

void test_commandEncode(void) 
{
    const TestData input{
        3,
        -4,
        10,
        -546,
        428098,
        -1111234,
        // 48093280492380,
        // -442842979792,
        0.578f,
        true,
    };
    TestData output;

    {
        auto cmd = DataHook(inputBuffer, TEST_BUFFER_SIZE);
        cmd.encode(input);
        output = cmd.decode<TestData>();
    }

    ASSERT_TEST_DATA_VARIABLE(UINT8, input, output, euint8_t);
    ASSERT_TEST_DATA_VARIABLE(INT8, input, output, eint8_t);
    ASSERT_TEST_DATA_VARIABLE(UINT16, input, output, euint16_t);
    ASSERT_TEST_DATA_VARIABLE(INT16, input, output, eint16_t);
    ASSERT_TEST_DATA_VARIABLE(UINT32, input, output, euint32_t);
    ASSERT_TEST_DATA_VARIABLE(INT32, input, output, eint32_t);
    // ASSERT_TEST_DATA_VARIABLE(UINT64, input, output, euint64_t);
    // ASSERT_TEST_DATA_VARIABLE(INT64, input, output, eint64_t);
    ASSERT_TEST_DATA_VARIABLE(FLOAT, input, output, efloat_t);
    TEST_ASSERT_EQUAL(input.test_eboolean_t, output.test_eboolean_t);
}


struct ExampleParameter
{
    eint32_t id;
};

struct ExampleReturn
{
    eboolean_t success;
    efloat_t result;
};

ExampleReturn myFunc(ExampleParameter param)
{
    if (param.id < 0) return ExampleReturn{false,0};
    return ExampleReturn{true, param.id/2.0f};
}

void test_functionSizes(void) 
{
    BaseFunction_ptr func = new Function<ExampleReturn, ExampleParameter>(&myFunc);

    TEST_ASSERT_EQUAL_size_t(sizeof(ExampleParameter), func->paramSize());
    TEST_ASSERT_EQUAL_size_t(sizeof(ExampleReturn), func->returnSize());
}

void test_functionRun(void)
{
    // Create function hook
    BaseFunction_ptr func_ptr = new Function<ExampleReturn, ExampleParameter>(&myFunc);
    BaseFunction &func = *func_ptr;

    // Create data buffer hooks
    DataHook input = DataHook(inputBuffer, func.paramSize());
    DataHook output = DataHook(outputBuffer, func.returnSize());
    
    // Encode data
    ExampleParameter param{5};
    input.encode(param);

    // Call func
    func(input, output);

    // Compare output
    ExampleReturn result = output.decode<ExampleReturn>();
    TEST_ASSERT_TRUE(result.success);
    TEST_ASSERT_EQUAL_FLOAT(2.5f, result.result);
}

// bool subLock()
// {
//     auto &lock = cmdMan->getLock();

//     LOCK(lock, locker)
//     {
//         return locker.acquired;
//     }
// }

// void test_commandManagerLock(void)
// {
//     auto &lock = cmdMan->getLock();

//     LOCK(lock, locker)
//     {
//         TEST_ASSERT_TRUE(locker.acquired);
//         TEST_ASSERT_FALSE(subLock());
//     }
// }

void test_simpleEncoding(void)
{
    uint8_t encodedData[] = {
        5, 0, 0, 0,
    };

    DataHook hook = DataHook(encodedData, sizeof(encodedData));
    auto res = hook.decode<ExampleParameter>();
    TEST_ASSERT_EQUAL_INT32(5, res.id);
}

volatile bool manageTest = false;
void sendCB(uint8_t* buff, size_t length)
{
    DataHook hook = DataHook(buff, length);
    auto result = hook.decode<ExampleReturn>();
    TEST_ASSERT_TRUE(result.success);
    TEST_ASSERT_EQUAL_FLOAT(2.5f, result.result);
    Debugln("CB called");
    manageTest = true;
}

void test_commandManagerManageFunction(void)
{
    cmdMan->setSendCB(&sendCB);

    BaseFunction_ptr funcs[] = 
    {
        new Function<ExampleReturn, ExampleParameter>(&myFunc),
    };
    cmdMan->setCommands(funcs, 1);

    uint8_t encodedCmd[] = {
        0,
        5, 0, 0, 0,
    };

    manageTest = false;
    bool res = cmdMan->handleCommand(encodedCmd, sizeof(encodedCmd));

    String msg = String("Failed to handle! Status code: ");
    msg.concat(cmdMan->status());
    TEST_MESSAGE(msg.c_str());
    TEST_ASSERT_TRUE_MESSAGE(res, msg.c_str());
    TEST_ASSERT_TRUE_MESSAGE(manageTest, "Send callback was not called");

}

void test_commandManagerManageFunctionB(void)
{
    // _CommandManager* cmd = new _CommandManager(TEST_BUFFER_SIZE, TEST_BUFFER_SIZE);
    _CommandManager* cmd = cmdMan;
    cmd->setSendCB(&sendCB);

    BaseFunction_ptr funcs[] = 
    {
        new Function<ExampleReturn, ExampleParameter>(&myFunc),
    };
    cmd->setCommands(funcs, 1);

    Buffer b = Buffer(10);
    
    b.write(0);

    uint8_t encoded[] = {
        5, 0, 0, 0,
    };
    b.write(encoded, 4);

    String msg = String("Cmd: ");
    msg.concat(b.peek());

    manageTest = false;
    bool res = cmd->handleCommand(b);
    msg.concat(" | Failed to handle! Status code: ");
    msg.concat(cmd->status());
    TEST_ASSERT_TRUE_MESSAGE(res, msg.c_str());
    TEST_ASSERT_TRUE_MESSAGE(manageTest, "Send callback was not called");

    Debugln("Done");

}

#define TEST_ASSERT_BUFF_SIZE(buff, written) TEST_ASSERT_EQUAL(TEST_BUFFER_SIZE - written, buff.availableForWrite()); \
    TEST_ASSERT_EQUAL(written, b.available())

void test_bufferOperations(void)
{
    Buffer b(TEST_BUFFER_SIZE);
    TEST_ASSERT_EQUAL(-1, b.peek());
    TEST_ASSERT_BUFF_SIZE(b, 0);

    size_t w = b.write(50);
    TEST_ASSERT_EQUAL(50, b.peek());
    TEST_ASSERT_EQUAL(1, w);
    TEST_ASSERT_BUFF_SIZE(b, 1);

    uint8_t v = b.read();
    TEST_ASSERT_EQUAL(-1, b.peek());
    TEST_ASSERT_EQUAL(50, v);
    TEST_ASSERT_BUFF_SIZE(b, 0);

    int r = b.read();
    TEST_ASSERT_EQUAL(-1, b.peek());
    TEST_ASSERT_EQUAL(-1, r);
    TEST_ASSERT_BUFF_SIZE(b, 0);

    uint8_t batch[8] = {1,2,3,4,5,6,7};
    
    for (size_t i = 0; i < 8; ++i)
    {
        w+=b.write(batch, 8);
    }
    TEST_ASSERT_EQUAL(1, b.peek());
    TEST_ASSERT_BUFF_SIZE(b, TEST_BUFFER_SIZE);
    size_t ww = b.write(batch, 8);
    TEST_ASSERT_EQUAL(0, ww);
    TEST_ASSERT_BUFF_SIZE(b, TEST_BUFFER_SIZE);
    
    for (size_t i = TEST_BUFFER_SIZE; i > 0 ; i -= 8)
    {
        uint8_t rd[8] = {0};
        TEST_ASSERT_BUFF_SIZE(b, i);
        size_t rr =b.read(rd, 8);
        TEST_ASSERT_EQUAL(8, rr);
    }
    TEST_ASSERT_EQUAL(-1, b.peek());
    TEST_ASSERT_BUFF_SIZE(b, 0);
    uint8_t rf[8] = {0};
    size_t rff = b.read(rf, 8);
    TEST_ASSERT_EQUAL(-1, b.peek());
    TEST_ASSERT_EQUAL(0, rff);

    b.write(6);
    b.write(batch, 8);
    TEST_ASSERT_BUFF_SIZE(b, 9);
    TEST_ASSERT_EQUAL(6, b.peek());
}

int runUnityTests(void) 
{
    UNITY_BEGIN();
    RUN_TEST(test_commandEncode);
    RUN_TEST(test_functionSizes);
    RUN_TEST(test_functionRun);
    // RUN_TEST(test_commandManagerLock);
    RUN_TEST(test_simpleEncoding);
    RUN_TEST(test_bufferOperations);
    RUN_TEST(test_commandManagerManageFunction);
    RUN_TEST(test_commandManagerManageFunctionB);
    Debugln("Tests end");
    return UNITY_END();
}

#ifdef ARDUINO
/**
  * For Arduino framework
  */
void setup() 
{
    #ifdef SAM
    debug.begin(9600);
    #endif
    // Wait ~2 seconds before the Unity test runner
    // establishes connection with a board Serial interface
    delay(2000);

    runUnityTests();
}
void loop() {}

#else

/**
  * For native dev-platform or for some embedded frameworks
  */
int main(void) 
{
    return runUnityTests();
}

#endif // ARDUINO


