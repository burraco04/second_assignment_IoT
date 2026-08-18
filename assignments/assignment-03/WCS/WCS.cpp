#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Servo.h>

#include "Scheduler.h"
#include "config.h"
#include "src/tasks/ButtonTask.h"
#include "src/tasks/CusTask.h"
#include "src/tasks/LcdTask.h"
#include "src/tasks/PotentiometerTask.h"
#include "src/tasks/ValveTask.h"
#include "src/tasks/WcsContext.h"

Scheduler scheduler;
WcsContext context;

Servo valveServo;
LiquidCrystal lcd(
    LCD_RS_PIN,
    LCD_ENABLE_PIN,
    LCD_D4_PIN,
    LCD_D5_PIN,
    LCD_D6_PIN,
    LCD_D7_PIN
);

ButtonTask buttonTask(context);
CusTask cusTask(context);
PotentiometerTask potentiometerTask(context);
ValveTask valveTask(context, valveServo);
LcdTask lcdTask(context, lcd);

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);

    scheduler.addTask(&cusTask);
    scheduler.addTask(&buttonTask);
    scheduler.addTask(&potentiometerTask);
    scheduler.addTask(&valveTask);
    scheduler.addTask(&lcdTask);
    scheduler.init();
}

void loop() {
    scheduler.schedule();
}

