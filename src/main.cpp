#include <Arduino.h>
#include <simpleRPC.h>

void ledOn()
{
    digitalWrite(LED_BUILTIN, HIGH);
}

void ledOff()
{
    digitalWrite(LED_BUILTIN, LOW);
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    for (int i = 0; i < 10 && !Serial; ++i) delay(100);
    Serial.begin(115200);


}

void loop()
{
    interface(Serial, 
        ledOn, "ledOn",
        ledOff, "ledOff");
}
