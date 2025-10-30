#define I2C_BUFFER_LENGTH 128
#include <Wire.h>
#include "telStruct.h"
#include "lora.h"

TelemetryPacket pkt;
Lora lora;

void receiveEvent(int numBytes)
{
    if (numBytes != sizeof(pkt))
    {
        Serial.print("Unexpected packet size: ");
        Serial.println(numBytes);
        while (Wire.available())
            Wire.read();
        return;
    }

    Wire.readBytes((char *)&pkt, sizeof(pkt));

    uint8_t crcCalc = computeCRC((uint8_t *)&pkt, sizeof(pkt) - 1);
    if (crcCalc != pkt.crc)
    {
        Serial.println("CRC mismatch!");
        return;
    }

    Serial.println("Packet received");
    Serial.println(pkt.toCSV());
}

void setup()
{
    Serial.begin(115200);
    while (!Serial && millis() < 3000)
        ;

    lora.init();

    Wire.begin(0x42);
    Wire.onReceive(receiveEvent);
    Serial.println("Teensy I2C telemetry listener @0x42 ready");
}

void loop()
{
    lora.send(pkt);
}