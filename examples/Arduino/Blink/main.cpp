#include <Arduino.h>
#include <capra_comm.h>
#include "./../examples/Arduino/Blink/api.h"

class EmptyStream : public Stream
{
public:
    int available() override { return 0; }
    int read() override { return 0; }
    int peek() override { return 0; }
    void flush() override { }
    size_t write(uint8_t) override { return 0; }
};

#ifndef SAM
    EmptyStream debug = EmptyStream();
    Serial_& comm = Serial
#else
    Serial_& debug = SerialUSB;
    UARTClass& comm = Serial;
#endif




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
    return s;
}

void sendCB(uint8_t* buff, size_t length)
{
    comm.write(buff, length);
    comm.flush();
}


void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    for (int i = 0; i < 10 && !comm; ++i) delay(100);
    comm.begin(115200);

    CommandManager.setSendCB(&sendCB);
    CommandManager.setCommands(commands, COMMANDS_COUNT);
}

void loop()
{
    if (comm.available())
    {
        int id = comm.peek();
        // if (id < COMMANDS_COUNT)
        // {
        //     while (comm.read() != -1);
        //     return;
        // }
        // if (commands[id]->paramSize() >= comm.available()) return;
        uint8_t* buff = new uint8_t[commands[id]->paramSize()];
        comm.readBytes(buff, commands[id]->paramSize());
        CommandManager.handleCommand(buff, commands[id]->paramSize());
        delete[] buff;
        while (comm.available()) comm.read();
    }
}
