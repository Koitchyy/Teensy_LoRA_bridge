#ifndef LORA_H
#define LORA_H

#include <SPI.h>
#include <RH_RF95.h>
#include "telStruct.h"

class Lora
{
private:
    int RFM95_CS;
    int RFM95_RST;
    int RFM95_INT;
    float RF95_FREQ;

    RH_RF95 rf95;

public:
    Lora(
        int RFM95_CS = 10,
        int RFM95_RST = 1,
        int RFM95_INT = 0,
        float RF95_FREQ = 915.0);
    void init();
    bool receiveCSV(char *buffer, size_t bufferSize);
};

#endif
