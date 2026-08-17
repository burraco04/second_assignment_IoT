#pragma once

#include <Arduino.h>

class SonarSensor {
public:
    SonarSensor(const uint8_t trigPin, const uint8_t echoPin, const unsigned long timeoutUs)
        : trigPin(trigPin), echoPin(echoPin), timeoutUs(timeoutUs) {}

    void init() const {
        pinMode(trigPin, OUTPUT);
        pinMode(echoPin, INPUT);
        digitalWrite(trigPin, LOW);
    }

    float readDistanceCm() const {
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);

        const unsigned long duration = pulseIn(echoPin, HIGH, timeoutUs);
        if (duration == 0) {
            return NAN;
        }
        return static_cast<float>(duration) * 0.0343f / 2.0f;
    }

private:
    uint8_t trigPin;
    uint8_t echoPin;
    unsigned long timeoutUs;
};

