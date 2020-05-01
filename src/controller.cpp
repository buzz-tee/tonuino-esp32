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

    u_long previousCommand = data->lastCommand;
    data->lastCommand = millis();
    if (data->lastCommand - previousCommand < COMMAND_DEBOUNCE)
        return;
    
    (*(data->callback))();
}
