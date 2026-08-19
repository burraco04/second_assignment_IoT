#pragma once

#include <Arduino.h>

constexpr unsigned long SERIAL_BAUD_RATE = 9600;

constexpr uint8_t BUTTON_PIN = 2;
constexpr bool BUTTON_ACTIVE_LOW = true;
constexpr uint8_t POT_PIN = A0;
constexpr uint8_t SERVO_PIN = 9;

constexpr uint8_t LCD_RS_PIN = 12;
constexpr uint8_t LCD_ENABLE_PIN = 11;
constexpr uint8_t LCD_D4_PIN = 5;
constexpr uint8_t LCD_D5_PIN = 4;
constexpr uint8_t LCD_D6_PIN = 3;
constexpr uint8_t LCD_D7_PIN = 6;
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
constexpr int MIN_SERVO_ANGLE = 0;
constexpr int MAX_SERVO_ANGLE = 90;
