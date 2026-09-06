#pragma once

#include <Arduino.h>

// WiFi and MQTT configuration.
// Replace these values before flashing the board.
constexpr char WIFI_SSID[] = "Gointernet_08E9B6";
constexpr char WIFI_PASSWORD[] = "gointernet123";

// Public MQTT broker used for the prototype.
constexpr char MQTT_HOST[] = "broker.mqtt-dashboard.com";
constexpr uint16_t MQTT_PORT = 1883;
constexpr char MQTT_USER[] = "";
constexpr char MQTT_PASSWORD[] = "";

// MQTT topics used by the CUS subsystem.
constexpr char MQTT_TOPIC_TELEMETRY[] = "assignment03/tms/telemetry";
constexpr char MQTT_TOPIC_STATUS[] = "assignment03/tms/status";
constexpr char MQTT_TOPIC_AVAILABILITY[] = "assignment03/tms/availability";

// Hardware pins. Adjust them to match the actual ESP32 wiring.
constexpr uint8_t SONAR_TRIG_PIN = 5;
constexpr uint8_t SONAR_ECHO_PIN = 18;
constexpr uint8_t GREEN_LED_PIN = 2;
constexpr uint8_t RED_LED_PIN = 4;

// Sampling frequency F from the assignment/diagram: 1 sample every 500 ms.
constexpr unsigned long MEASURING_PERIOD_MS = 500;
constexpr unsigned long LED_PERIOD_MS = 500;
constexpr unsigned long NETWORK_PERIOD_MS = 100;

constexpr unsigned long WIFI_RECONNECT_INTERVAL_MS = 5000;
constexpr unsigned long MQTT_RECONNECT_INTERVAL_MS = 3000;

// Sonar calibration.
// SONAR_FULL_CM is the distance from the sensor to the water when the tank is full.
// SONAR_EMPTY_CM is the distance from the sensor to the bottom/empty water level.
constexpr float SONAR_FULL_CM = 5.0f;
constexpr float SONAR_EMPTY_CM = 45.0f;
constexpr unsigned long SONAR_TIMEOUT_US = 30000;
