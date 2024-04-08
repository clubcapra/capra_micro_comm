#include <Arduino.h>
#include <capra_comm.h>


struct Void
{
    euint8_t _;
};

static_assert(sizeof(Void) == 1);

Void ledOn(Void _)
{
    digitalWrite(LED_BUILTIN, HIGH);
}

Void ledOff(Void _)
{
    digitalWrite(LED_BUILTIN, LOW);
}

BaseFunction_ptr commands[] = {
    new Function<Void, Void>(&ledOn),
    new Function<Void, Void>(&ledOff),
};
#define COMMANDS_COUNT 2

void sendCB(uint8_t* buff, size_t length)
{
    Serial.write(buff, length);
    Serial.flush();
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    for (int i = 0; i < 10 && !Serial; ++i) delay(100);
    Serial.begin(115200);

    CommandManager.setSendCB(&sendCB);
    CommandManager.setCommands(commands, COMMANDS_COUNT);
}

void loop()
{
    if (Serial.available())
    {
        int id = Serial.peek();
        // if (id < COMMANDS_COUNT)
        // {
        //     while (Serial.read() != -1);
        //     return;
        // }
        // if (commands[id]->paramSize() >= Serial.available()) return;
        uint8_t* buff = new uint8_t[commands[id]->paramSize()];
        Serial.readBytes(buff, commands[id]->paramSize());
        CommandManager.handleCommand(buff, commands[id]->paramSize());
        delete[] buff;
        while (Serial.available()) Serial.read();
    }
}
