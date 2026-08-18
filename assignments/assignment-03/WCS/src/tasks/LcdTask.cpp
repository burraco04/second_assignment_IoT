#include "LcdTask.h"

#include <Arduino.h>
#include <stdio.h>
#include <string.h>

LcdTask::LcdTask(WcsContext& context, LiquidCrystal& lcd)
    : Task(LCD_PERIOD_MS), context(context), lcd(lcd) {}

void LcdTask::init() {
    lcd.begin(LCD_COLUMNS, LCD_ROWS);
    lcd.clear();
}

void LcdTask::tick() {
    switch (state) {
    case LcdState::Display: {
        char firstLine[LCD_COLUMNS + 1];
        char secondLine[LCD_COLUMNS + 1];

        snprintf(firstLine, sizeof(firstLine), "Mode:%s", modeToText(context.mode));
        snprintf(secondLine, sizeof(secondLine), "Valve:%3d%%", context.valveOpening);

        printPadded(0, firstLine);
        printPadded(1, secondLine);
        break;
    }
    }
}

void LcdTask::printPadded(const uint8_t row, const char* text) {
    char line[LCD_COLUMNS + 1];
    const size_t length = strlen(text);

    for (uint8_t i = 0; i < LCD_COLUMNS; i++) {
        line[i] = i < length ? text[i] : ' ';
    }
    line[LCD_COLUMNS] = '\0';

    lcd.setCursor(0, row);
    lcd.print(line);
}
