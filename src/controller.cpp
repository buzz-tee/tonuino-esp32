#include "controller.h"

#include <driver/adc.h>

Controller::Controller(void) :
    dataVolumeUp(),
    dataVolumeDown(),
    dataPause(),
    dataNext(),
    dataPrevious(),
    resetCallback(nullptr),
    partyCallback(nullptr),
    _adcChars(),
    _onBattery(false),
    _underVoltage(false) {
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

    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(CTRL_ADC_VBAT_CHANNEL, ADC_ATTEN_11db);
#ifndef DEBUG
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_12Bit, DEFAULT_VREF, &_adcChars);
#else
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_12Bit, DEFAULT_VREF, &_adcChars);
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        Serial.println("[ADC-cal] eFuse Vref");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        Serial.println("[ADC-cal] Two Point");
    } else {
        Serial.println("[ADC-cal] Default");
    }
#endif

    return true;
}

void IRAM_ATTR Controller::trigger(void* arg) {
    CallbackData* data = reinterpret_cast<CallbackData*>(arg);
    if (*(data->callback) == nullptr) return;

    u_long previousTrigger = data->lastTrigger;
    data->lastTrigger = millis();
    if (data->lastTrigger - previousTrigger < COMMAND_DEBOUNCE) return;
    
    data->lastCommand = data->lastTrigger;
    (*(data->callback))();
}

void Controller::_commandRepeat(Controller::CallbackData* data, ulong timeout) {
    if (*(data->callback) == nullptr) return;

    data->lastTrigger = millis();
    if (data->lastTrigger - data->lastCommand > timeout) {
        data->lastCommand = data->lastTrigger;
        (*(data->callback))();
    }
}

void Controller::_readVoltage() {
    uint32_t adc_reading = 0;
    for (int i = 0; i < 4; i++) {
        adc_reading += adc1_get_raw(CTRL_ADC_VBAT_CHANNEL);
    }
    uint32_t value = esp_adc_cal_raw_to_voltage(adc_reading / 4, &_adcChars) * CTRL_VBAT_FACTOR;
    _onBattery = (value < CTRL_VBAT_FULL);
    _underVoltage = (value < CTRL_VBAT_LOW);
}

void Controller::loop() {
    _readVoltage();
    
    bool volumeUpPressed = (digitalRead(CTRL_PIN_VOLUME_UP) == LOW);
    bool volumeDownPressed = (digitalRead(CTRL_PIN_VOLUME_DOWN) == LOW);
    if (volumeUpPressed && volumeDownPressed) {
        dataVolumeUp.lastTrigger = millis();
        dataVolumeDown.lastTrigger = dataVolumeUp.lastTrigger;

        if (dataVolumeUp.lastTrigger - std::max(dataVolumeUp.lastCommand, dataVolumeDown.lastCommand) > 10 * COMMAND_REPEAT) {
            dataVolumeUp.lastCommand = dataVolumeUp.lastTrigger;
            if (resetCallback) resetCallback();
        }
    } else if (volumeUpPressed) {
        _commandRepeat(&dataVolumeUp, COMMAND_REPEAT);
    } else if (volumeDownPressed) {
        _commandRepeat(&dataVolumeDown, COMMAND_REPEAT);
    }

    bool nextPressed = (digitalRead(CTRL_PIN_NEXT) == LOW);
    bool previousPressed = (digitalRead(CTRL_PIN_PREVIOUS) == LOW);
    if (nextPressed && previousPressed) {
        // Hold for shuffle / party mode
        dataNext.lastTrigger = millis();
        dataPrevious.lastTrigger = dataNext.lastTrigger;

        if (dataNext.lastTrigger - std::max(dataNext.lastCommand, dataPrevious.lastCommand) > 3 * COMMAND_REPEAT) {
            dataNext.lastCommand = dataNext.lastTrigger;
            if (partyCallback) partyCallback();
        }
    } else if (nextPressed) {
        _commandRepeat(&dataNext, COMMAND_REPEAT);
    } if (previousPressed) {
        _commandRepeat(&dataPrevious, COMMAND_REPEAT);
    }
    if (digitalRead(CTRL_PIN_PAUSE) == LOW) {
        dataPause.lastTrigger = millis(); // we don't repeat pause
    }
}
