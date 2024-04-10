#include "unity.h"
#include "capra_comm.h"

#define TEST_BUFFER_SIZE 64
uint8_t* inputBuffer;
uint8_t* outputBuffer;
_CommandManager* cmdMan = &CommandManager;

void setUp()
{
    inputBuffer = new uint8_t[TEST_BUFFER_SIZE];
    outputBuffer = new uint8_t[TEST_BUFFER_SIZE];
    // cmdMan = new _CommandManager(TEST_BUFFER_SIZE, TEST_BUFFER_SIZE);
}

void tearDown()
{
    delete[] inputBuffer;
    delete[] outputBuffer;
    // delete cmdMan;
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

bool manageTest = false;
void sendCB(uint8_t* buff, size_t length)
{
    DataHook hook = DataHook(buff, length);
    auto result = hook.decode<ExampleReturn>();
    TEST_ASSERT_TRUE(result.success);
    TEST_ASSERT_EQUAL_FLOAT(2.5f, result.result);
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

    bool res = cmdMan->handleCommand(encodedCmd, sizeof(encodedCmd));

    String msg = String("Failed to handle! Status code: ");
    msg.concat(cmdMan->status());
    TEST_MESSAGE(msg.c_str());
    TEST_ASSERT_TRUE_MESSAGE(res, msg.c_str());

    TEST_ASSERT_TRUE_MESSAGE(manageTest, "Send callback was not called");

}

int runUnityTests(void) 
{
    UNITY_BEGIN();
    RUN_TEST(test_commandEncode);
    RUN_TEST(test_functionSizes);
    RUN_TEST(test_functionRun);
    // RUN_TEST(test_commandManagerLock);
    RUN_TEST(test_simpleEncoding);
    // RUN_TEST(test_commandManagerManageFunction);
    return UNITY_END();
}

#ifdef ARDUINO
/**
  * For Arduino framework
  */
void setup() 
{
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


