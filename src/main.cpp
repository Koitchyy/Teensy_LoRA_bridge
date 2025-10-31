#define I2C_BUFFER_LENGTH 128
#include <Wire.h>
#include <TeensyThreads.h>
#include <string.h>
#include "telStruct.h"
#include "lora.h"

TelemetryPacket pkt;
volatile bool newPacketAvailable = false;

Lora *lora;

void receiveEvent(int numBytes)
{
    if (numBytes != sizeof(pkt))
    {
        while (Wire.available())
            Wire.read();
        return;
    }

    TelemetryPacket tempPkt;
    Wire.readBytes((char *)&tempPkt, sizeof(tempPkt));

    uint8_t crcCalc = computeCRC((uint8_t *)&tempPkt, sizeof(tempPkt) - 1);
    if (crcCalc != tempPkt.crc)
    {
        return;
    }

    noInterrupts();
    pkt = tempPkt;
    newPacketAvailable = true;
    interrupts();
}

int i = 0;

void loraThread()
{
    static char csvBuffer[256];

    while (true)
    {
        if (newPacketAvailable)
        {
            noInterrupts();
            TelemetryPacket localPkt = pkt;
            newPacketAvailable = false;
            interrupts();

            localPkt.toCSV(csvBuffer, sizeof(csvBuffer));
            csvBuffer[255] = 0;

            Serial.print("Sending packet #");
            Serial.println(i);
            i++;

            lora->sendCSV(csvBuffer);
        }
        else
        {
            Serial.println("No new packet yet");
        }

        threads.delay(1000);
    }
}

void setup()
{
    delay(5000);

    Serial.begin(115200);
    while (!Serial && millis() < 3000)
        ;

    lora = new Lora();
    lora->init();

    Wire.begin(0x42);
    Wire.setClock(100000);
    Wire.onReceive(receiveEvent);

    Serial.println("Teensy I2C telemetry listener @0x42 ready");

    threads.addThread(loraThread, 0, 32768);
    Serial.println("LoRa thread started");
}

void loop()
{
    threads.yield();
}
