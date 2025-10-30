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

void Lora::send(const TelemetryPacket &packet)
{
    String csv = packet.toCSV();
    char buf[256];
    snprintf(buf, sizeof(buf), "%s", csv.c_str());
    uint8_t len = min(strlen(buf), (size_t)(RH_RF95_MAX_MESSAGE_LEN));
    Serial.println("Sending packet");
    Serial.println(buf);
    rf95->send((uint8_t *)buf, len);
    rf95->waitPacketSent();
}