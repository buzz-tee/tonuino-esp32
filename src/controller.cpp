#include "controller.h"

Controller::Controller(void) :
    dataVolumeUp(),
    dataVolumeDown(),
    dataPause(),
    dataNext(),
    dataPrevious() {
}

Controller::~Controller(void) {
    detachInterrupt(CTRL_PIN_VOLUME_UP);
    detachInterrupt(CTRL_PIN_VOLUME_DOWN);
    detachInterrupt(CTRL_PIN_PAUSE);
    detachInterrupt(CTRL_PIN_NEXT);
    detachInterrupt(CTRL_PIN_PREVIOUS);
}

bool Controller::begin(void) {
    pinMode(CTRL_PIN_VOLUME_UP, INPUT_PULLUP);
    attachInterruptArg(CTRL_PIN_VOLUME_UP, Controller::trigger, &dataVolumeUp, FALLING);
    pinMode(CTRL_PIN_VOLUME_DOWN, INPUT_PULLUP);
    attachInterruptArg(CTRL_PIN_VOLUME_DOWN, Controller::trigger, &dataVolumeDown, FALLING);
    pinMode(CTRL_PIN_PAUSE, INPUT_PULLUP);
    attachInterruptArg(CTRL_PIN_PAUSE, Controller::trigger, &dataPause, FALLING);
    pinMode(CTRL_PIN_NEXT, INPUT_PULLUP);
    attachInterruptArg(CTRL_PIN_NEXT, Controller::trigger, &dataNext, FALLING);
    pinMode(CTRL_PIN_PREVIOUS, INPUT_PULLUP);
    attachInterruptArg(CTRL_PIN_PREVIOUS, Controller::trigger, &dataPrevious, FALLING);

    return true;
}

void IRAM_ATTR Controller::trigger(void* arg) {
    CallbackData* data = reinterpret_cast<CallbackData*>(arg);
    if (*(data->callback) == nullptr)
        return;

    u_long previousTrigger = data->lastTrigger;
    data->lastTrigger = millis();
    if (data->lastTrigger - previousTrigger < COMMAND_DEBOUNCE)
        return;
    
    data->lastCommand = data->lastTrigger;
    (*(data->callback))();
}

void Controller::_commandRepeat(CallbackData* data, u_long timeout) {
    data->lastTrigger = millis();
    if (data->lastTrigger - data->lastCommand > timeout) {
        data->lastCommand = data->lastTrigger;
        (*(data->callback))();
    }
}

void Controller::loop() {
    if (digitalRead(CTRL_PIN_VOLUME_UP) == LOW) {
        _commandRepeat(&dataVolumeUp, COMMAND_REPEAT);
    }
    if (digitalRead(CTRL_PIN_VOLUME_DOWN) == LOW) {
        _commandRepeat(&dataVolumeDown, COMMAND_REPEAT);
    }
    if (digitalRead(CTRL_PIN_NEXT) == LOW) {
        _commandRepeat(&dataNext, COMMAND_REPEAT);
    }
    if (digitalRead(CTRL_PIN_PREVIOUS) == LOW) {
        _commandRepeat(&dataPrevious, COMMAND_REPEAT);
    }
    if (digitalRead(CTRL_PIN_PAUSE) == LOW) {
        dataPause.lastTrigger = millis(); // we don't repeat pause
    }
}
