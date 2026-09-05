#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "ServoMotorImpl.h"
#include "config.h"
#include "Scheduler.h"
#include "src/tasks/ButtonTask.h"
#include "src/tasks/LcdTask.h"
#include "src/tasks/PotentiometerTask.h"
#include "src/tasks/SerialTask.h"
#include "src/tasks/ValveTask.h"
#include "src/tasks/WcsContext.h"

Scheduler scheduler;
WcsContext context;

ServoMotorImpl valveServo(SERVO_PIN);
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);

ButtonTask buttonTask(context);
SerialTask serialTask(context);
PotentiometerTask potentiometerTask(context);
ValveTask valveTask(context, valveServo);
LcdTask lcdTask(context, lcd);

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);

    scheduler.addTask(&serialTask);
    scheduler.addTask(&buttonTask);
    scheduler.addTask(&potentiometerTask);
    scheduler.addTask(&valveTask);
    scheduler.addTask(&lcdTask);
    scheduler.init();
}

void loop() {
    scheduler.schedule();
}
