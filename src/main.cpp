#include <Arduino.h>
#include <capra_comm.h>
#include "./../examples/Arduino/Blink/api.h"
#include <base64.hpp>

uint8_t inputEncoded[MAX_ENCODED_SIZE+1];
uint8_t outputEncoded[MAX_ENCODED_SIZE+1];

uint8_t inputDecoded[MAX_DECODED_SIZE];
uint8_t outputDecoded[MAX_DECODED_SIZE];

Serial_& debug = SerialUSB;
UARTClass& comm = Serial;

Status ledOn(Void _)
{
    digitalWrite(LED_BUILTIN, HIGH);
    return {0};
}

Status ledOff(Void _)
{
    digitalWrite(LED_BUILTIN, LOW);
    return {1};
}

Status setLedState(State s)
{
    if (s.state) return ledOn(Void{});
    else return ledOff(Void{});
}

State loopback(State s)
{
    digitalWrite(LED_BUILTIN, 0);
    delay(100);
    digitalWrite(LED_BUILTIN, 1);
    DebugVarln(s.state);
    return s;
}

void sendCB(uint8_t* buff, size_t length)
{
    size_t outputCount = encode_base64(buff, length, outputEncoded);
    comm.write(outputEncoded, outputCount);
    comm.write('\n');
    debug.println("Sending:");
    debug.write(outputEncoded, outputCount);
    debug.println();
    debugBytes(buff, length);
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    for (int i = 0; i < 10 && !comm; ++i) delay(100);
    
    comm.begin(9600);
    debug.begin(9600);
    for (int i = 0; i < 10 && !debug; ++i) delay(100);
    delay(1000);
    debug.println("Started!");


    CommandManager.setSendCB(&sendCB);
    CommandManager.setCommands(commands, COMMANDS_COUNT);
}


uint8_t* head = inputEncoded;
void loop()
{
    int r = comm.read();
    if (r != -1)
    {
        *head = (uint8_t)r;
        debug.print((char)(*head));

        ++head;
        if (head >= inputEncoded + MAX_ENCODED_SIZE) head = inputEncoded;
    }


    if (r == '\n')
    {
        // Read line
        size_t inputCount = (head-1) - inputEncoded;
        head = inputEncoded;
        // debugBytes(inputEncoded, inputCount);

        // Decode input
        size_t decodedCount = decode_base64(inputEncoded, inputCount, inputDecoded);
        debugBytes(inputDecoded, decodedCount);

        if (decodedCount == 0)
        {
            debugBytes(inputDecoded, MAX_DECODED_SIZE);
            debug.println("No decoded data!");
            return;
        }

        // Grab command ID
        uint8_t id = inputDecoded[0];

        // Command out of range
        if (id >= COMMANDS_COUNT) 
        {
            debug.println("Out of range!");
            DebugVarln(id);
            return;
        }

        if (decodedCount-1 != commands[id]->paramSize())
        {
            debug.println("Size mismatch");
            DebugVarln(inputCount);
            DebugVarln(decodedCount);
            DebugVarln(commands[id]->paramSize());
            return;
        }
        debugBytes(inputDecoded, decodedCount);
        CommandManager.handleCommand(inputDecoded, commands[id]->paramSize());
        // while (comm.available()) comm.read();
    }
}
