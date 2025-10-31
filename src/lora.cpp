#include "lora.h"
#include <SPI.h>
#include <RH_RF95.h>

Lora::Lora(int RFM95_CS, int RFM95_RST, int RFM95_INT, float RF95_FREQ)
    : RFM95_CS(RFM95_CS), RFM95_RST(RFM95_RST), RFM95_INT(RFM95_INT), RF95_FREQ(RF95_FREQ), rf95(RFM95_CS, RFM95_INT)
{
}

void Lora::init()
{
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);

    if (!rf95.init())
    {
        Serial.println("LoRa radio init failed");
        while (1)
            ;
    }

    if (!rf95.setFrequency(RF95_FREQ))
    {
        Serial.println("LoRa setFrequency failed!");
        while (1)
            ;
    }

    rf95.setTxPower(23, false);
    Serial.println("LoRa radio init succeeded.");
}

bool Lora::receiveCSV(char *buffer, size_t bufferSize)
{
    if (!rf95.available())
        return false;

    uint8_t len = bufferSize - 1;
    if (!rf95.recv((uint8_t *)buffer, &len))
        return false;

    buffer[len] = '\0';
    return true;
}