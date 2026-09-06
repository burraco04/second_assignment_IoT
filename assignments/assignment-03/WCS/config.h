#pragma once

#include <Arduino.h>

constexpr unsigned long SERIAL_BAUD_RATE = 9600;

constexpr uint8_t BUTTON_PIN = 2;
constexpr bool BUTTON_ACTIVE_LOW = true;
constexpr uint8_t POT_PIN = A0;
constexpr uint8_t SERVO_PIN = 9;

// I2C LCD wiring on Arduino UNO:
// GND -> GND, VCC -> 5V, SDA -> A4, SCL -> A5.
constexpr uint8_t LCD_SDA_PIN = A4;
constexpr uint8_t LCD_SCL_PIN = A5;
constexpr uint8_t LCD_I2C_ADDRESS = 0x27;
constexpr uint8_t LCD_COLUMNS = 16;
constexpr uint8_t LCD_ROWS = 2;

constexpr unsigned long BUTTON_PERIOD_MS = 50;
constexpr unsigned long SERIAL_PERIOD_MS = 100;
constexpr unsigned long POTENTIOMETER_PERIOD_MS = 500;
constexpr unsigned long VALVE_PERIOD_MS = 500;
constexpr unsigned long LCD_PERIOD_MS = 500;

constexpr unsigned long BUTTON_DEBOUNCE_MS = 40;
constexpr unsigned long CUS_TIMEOUT_MS = 3000;
constexpr unsigned long MODE_REQUEST_RESEND_MS = 500;
constexpr int MIN_OPENING_PERCENT = 0;
constexpr int MAX_OPENING_PERCENT = 100;
constexpr int POT_REMOTE_OVERRIDE_THRESHOLD = 12;
constexpr int MIN_SERVO_ANGLE = 0;
constexpr int MAX_SERVO_ANGLE = 90;
