#include "telStruct.h"
#include "lora.h"

Lora lora;

void setup()
{
    Serial.begin(115200);
    while (!Serial && millis() < 3000)
        ;

    Serial.println("LoRa CSV receiver ready");
    lora.init();
}

void loop()
{
    static char csvBuffer[256];

    if (lora.receiveCSV(csvBuffer, sizeof(csvBuffer)))
    {
        TelemetryPacket pkt = TelemetryPacket::fromCSV(csvBuffer);
        Serial.println(pkt.toString());
    }

    delay(10);
}
