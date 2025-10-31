#define I2C_BUFFER_LENGTH 128
#include <Wire.h>
#include "telStruct.h"
#include <RH_RF95.h>

TelemetryPacket pkt;
RH_RF95 *rf95;

void receiveEvent(int numBytes) {
    if (numBytes != sizeof(pkt)) {
        Serial.print("Unexpected packet size: ");
        Serial.println(numBytes);
        while (Wire.available()) Wire.read();
        return;
    }

    Wire.readBytes((char*)&pkt, sizeof(pkt));

    uint8_t crcCalc = computeCRC((uint8_t*)&pkt, sizeof(pkt) - 1);
    if (crcCalc != pkt.crc) {
        Serial.println("CRC mismatch!");
        return;
    }

    Serial.print("t=");
    Serial.print(pkt.time_ms);
    Serial.print(" ms  Alt=");
    Serial.print(pkt.altitude, 2);
    Serial.print(" m  State=");
    Serial.println(pkt.state);
    Serial.print("bno_x=");
    Serial.println(pkt.bno_x);
    Serial.print("g bno_y=");
    Serial.println(pkt.bno_y);
    Serial.print("g bno_z=");
    Serial.println(pkt.bno_z);
    Serial.print("g bno_i=");
    Serial.println(pkt.bno_i);
    Serial.print("bno_j=");
    Serial.println(pkt.bno_j);
    Serial.print("bno_k=");
    Serial.println(pkt.bno_k);
    Serial.print("bno_real=");
    Serial.println(pkt.bno_z);
}

void setup() {
    Serial.begin(115200);
    Wire.begin(0x42);
    Wire.onReceive(receiveEvent);
    Serial.println("Teensy I2C telemetry listener @0x42 ready");

    rf95 = new RH_RF95(10, 2);

    Serial.println("LoRa radio initializing");
    pinMode(9, OUTPUT);
    digitalWrite(9, LOW);
    delay(50);
    digitalWrite(9, HIGH);
    delay(100);

    Serial.println(rf95->init());
    if (!rf95->init())
    {
        Serial.println("LoRa radio init failed");
        while (1)
            ;
    }

    if (!rf95->setFrequency(915.0))
    {
        Serial.println("setFrequency failed");
        while (1)
            ;
    }

    rf95->setTxPower(13, false);
    Serial.println("LoRa radio initialized");
}

void loop() {
    //Serial.println("ho");
}
