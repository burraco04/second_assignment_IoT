#pragma once

#include <Arduino.h>

class Task {
public:
    explicit Task(unsigned long periodMs) : period(periodMs) {}
    virtual ~Task() = default;

    virtual void init() {}
    virtual void tick() = 0;

    bool shouldRun(unsigned long now) const {
        return now - lastActivation >= period;
    }

    void run(unsigned long now) {
        lastActivation = now;
        tick();
    }

private:
    unsigned long period;
    unsigned long lastActivation = 0;
};

class Scheduler {
public:
    static constexpr uint8_t MAX_TASKS = 8;

    bool addTask(Task* task) {
        if (task == nullptr || taskCount >= MAX_TASKS) {
            return false;
        }
        tasks[taskCount++] = task;
        return true;
    }

    void init() {
        for (uint8_t i = 0; i < taskCount; i++) {
            tasks[i]->init();
        }
    }

    void schedule() {
        const unsigned long now = millis();
        for (uint8_t i = 0; i < taskCount; i++) {
            if (tasks[i]->shouldRun(now)) {
                tasks[i]->run(now);
            }
        }
    }

private:
    Task* tasks[MAX_TASKS] = {};
    uint8_t taskCount = 0;
};

