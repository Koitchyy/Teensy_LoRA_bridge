#define I2C_BUFFER_LENGTH 128
#include <Wire.h>
#include <TeensyThreads.h>
#include <string.h>
#include "telStruct.h"
#include "lora.h"

TelemetryPacket *pkt;
Lora *lora;

Threads::Mutex packetMutex;

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

    Serial.println("Packet received");
}

int i = 0;

void loraThread()
{
    static char csvBuffer[256];

    while (true)
    {
        packetMutex.lock();
        pkt->toCSV(csvBuffer, sizeof(csvBuffer));
        packetMutex.unlock();

        Serial.println(i);
        Serial.println("Sending packet");
        csvBuffer[255] = 0;
        i++;

        lora->sendCSV(csvBuffer);

        threads.delay(1000);
    }
}

void setup()
{
    Serial.begin(115200);
    while (!Serial && millis() < 3000)
        ;

    pkt = new TelemetryPacket();
    lora = new Lora();
    lora->init();

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
