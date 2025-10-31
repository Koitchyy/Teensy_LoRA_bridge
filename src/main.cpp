#define I2C_BUFFER_LENGTH 128
#include <Wire.h>
#include <TeensyThreads.h>
#include <string.h>
#include "telStruct.h"
#include "lora.h"

TelemetryPacket *pkt;
Lora *lora;

Threads::Mutex packetMutex;
Threads::Mutex spiMutex;

void receiveEvent(int numBytes)
{
    if (numBytes != sizeof(*pkt))
    {
        Serial.print("Unexpected packet size: ");
        Serial.println(numBytes);
        while (Wire.available())
            Wire.read();
        return;
    }

    TelemetryPacket tempPkt;
    Wire.readBytes((char *)&tempPkt, sizeof(tempPkt));

    uint8_t crcCalc = computeCRC((uint8_t *)&tempPkt, sizeof(tempPkt) - 1);
    if (crcCalc != tempPkt.crc)
    {
        Serial.println("CRC mismatch!");
        return;
    }

    packetMutex.lock();
    *pkt = tempPkt;
    packetMutex.unlock();
}

void loraThread()
{
    static char csvBuffer[256];

    while (true)
    {
        packetMutex.lock();
        pkt->toCSV(csvBuffer, sizeof(csvBuffer));
        packetMutex.unlock();

        spiMutex.lock();
        Serial.println("Sending packet");
        lora->sendCSV(csvBuffer);
        spiMutex.unlock();

        threads.delay(100);
    }
}

void setup()
{
    Serial.begin(115200);
    while (!Serial && millis() < 3000)
        ;

    spiMutex.lock();
    lora->init();
    spiMutex.unlock();

    Wire.begin(0x42);
    Wire.setClock(100000);
    Wire.onReceive(receiveEvent);
    Serial.println("Teensy I2C telemetry listener @0x42 ready");

    threads.addThread(loraThread, 0, 2048);
    Serial.println("LoRa thread started");
}

void loop()
{
    threads.yield();
}