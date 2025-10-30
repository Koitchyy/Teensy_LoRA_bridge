#pragma once
#include <Arduino.h>

struct TelemetryPacket
{
    uint32_t time_ms;
    float accel_x, accel_y, accel_z;                      // ADXL345
    float accel_x_high_g, accel_y_high_g, accel_z_high_g; // ADXL375
    float pressure;
    float temperature;
    float altitude;
    float bno_x, bno_y, bno_z;
    float bno_i, bno_j, bno_k, bno_real;
    uint8_t state;
    uint8_t crc;

    String toCSV() const
    {
        char buf[256];
        snprintf(
            buf, sizeof(buf), "%lu,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%u",
            time_ms, accel_x, accel_y, accel_z,
            accel_x_high_g, accel_y_high_g, accel_z_high_g,
            pressure, temperature, altitude,
            bno_x, bno_y, bno_z,
            bno_i, bno_j, bno_k, bno_real, state);
        return String(buf);
    }

    String toString() const
    {
        char buf[384];
        snprintf(
            buf, sizeof(buf),
            "Time: %lums\n"
            "ADXL345: %.2f,%.2f,%.2f\n"
            "ADXL375: %.2f,%.2f,%.2f\n"
            "Pressure: %.2f Pa, Temp: %.2f C, Alt: %.2f m\n"
            "BNO055: %.2f,%.2f,%.2f\n"
            "Quat: %.2f,%.2f,%.2f,%.2f\n"
            "State: %u",
            time_ms,
            accel_x, accel_y, accel_z,
            accel_x_high_g, accel_y_high_g, accel_z_high_g,
            pressure, temperature, altitude,
            bno_x, bno_y, bno_z,
            bno_i, bno_j, bno_k, bno_real,
            state);
        return String(buf);
    }

    bool verifyCRC() const
    {
        uint8_t calculatedCRC = 0;
        const uint8_t *data = (const uint8_t *)this;
        for (size_t i = 0; i < sizeof(TelemetryPacket) - 1; i++)
        {
            calculatedCRC ^= data[i];
        }
        return calculatedCRC == crc;
    }
} __attribute__((packed));

inline uint8_t computeCRC(const uint8_t *data, size_t len)
{
    uint8_t crc = 0;
    for (size_t i = 0; i < len; i++)
        crc ^= data[i];
    return crc;
}
