#include "lora.h"
#include <SPI.h>
#include <RH_RF95.h>

Lora::Lora(
    int RFM95_CS,
    int RFM95_RST,
    int RFM95_INT,
    float RF95_FREQ)
    : RFM95_CS(RFM95_CS),
      RFM95_RST(RFM95_RST),
      RFM95_INT(RFM95_INT),
      RF95_FREQ(RF95_FREQ)
{
    rf95 = new RH_RF95(RFM95_CS, RFM95_INT);
}

Lora::~Lora()
{
    if (rf95 != nullptr)
    {
        delete rf95;
    }
}

void Lora::init()
{
    Serial.println("LoRa radio initializing");
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, LOW);
    delay(50);
    digitalWrite(RFM95_RST, HIGH);
    delay(100);

    if (!rf95->init())
    {
        Serial.println("LoRa radio init failed");
        while (1)
            ;
    }

    if (!rf95->setFrequency(RF95_FREQ))
    {
        Serial.println("setFrequency failed");
        while (1)
            ;
    }

    rf95->setTxPower(13, false);
    Serial.println("LoRa radio initialized");
}

void Lora::sendCSV(const char *csvBuffer)
{
    uint8_t len = strlen(csvBuffer);
    if (len > RH_RF95_MAX_MESSAGE_LEN)
    {
        len = RH_RF95_MAX_MESSAGE_LEN;
    }

    rf95->send((uint8_t *)csvBuffer, len);
    rf95->waitPacketSent(1000);
}