#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <Arduino.h>
#include <esp_adc_cal.h>


#define CTRL_PIN_VOLUME_UP      2
#define CTRL_PIN_VOLUME_DOWN    4
#define CTRL_PIN_PAUSE          27
#define CTRL_PIN_NEXT           17
#define CTRL_PIN_PREVIOUS       21

#define COMMAND_DEBOUNCE        200
#define COMMAND_REPEAT          500

#define CTRL_PIN_VBAT           33
#define CTRL_ADC_VBAT_CHANNEL   ADC1_GPIO33_CHANNEL
#define DEFAULT_VREF            3300
#define CTRL_VBAT_FACTOR        2.347       // Voltage divider from measurement R1 = 134.7kOhm, R2 = 100kOhm

#define CTRL_VBAT_FULL          3900
#define CTRL_VBAT_LOW           3300

#define CTRL_VBAT_READINGS      10

class Controller {
public:
    typedef void (*CallbackFunc)(void);
    struct CallbackData {
        CallbackData() :
            lastTrigger(0),
            lastCommand(0),
            callback(nullptr) {
        };
        u_long lastTrigger;
        u_long lastCommand;
        Controller::CallbackFunc callback;
    };

    Controller();
    ~Controller();
    bool begin();

    void loop();

    void setVolumeUpCallback(Controller::CallbackFunc callback) { dataVolumeUp.callback = callback; }
    void setVolumeDownCallback(Controller::CallbackFunc callback) { dataVolumeDown.callback = callback; }
    void setPauseCallback(Controller::CallbackFunc callback) { dataPause.callback = callback; }
    void setNextCallback(Controller::CallbackFunc callback) { dataNext.callback = callback; }
    void setPreviousCallback(Controller::CallbackFunc callback) { dataPrevious.callback = callback; }
    
    void setResetCallback(Controller::CallbackFunc callback) { resetCallback = callback; }
    void setPartyCallback(Controller::CallbackFunc callback) { partyCallback = callback; }

    bool isOnBattery() { return _onBattery; }
    bool isUnderVoltage() { return _underVoltage; }
protected:
    static void IRAM_ATTR trigger(void* arg);
private:
    void _commandRepeat(Controller::CallbackData* data, ulong timeout);
    void _readVoltage();

    Controller::CallbackData dataVolumeUp;
    Controller::CallbackData dataVolumeDown;
    Controller::CallbackData dataPause;
    Controller::CallbackData dataNext;
    Controller::CallbackData dataPrevious;

    Controller::CallbackFunc resetCallback;
    Controller::CallbackFunc partyCallback;

    esp_adc_cal_characteristics_t _adcChars;
    bool _onBattery;
    bool _underVoltage;

    uint32_t _batteryReadings[CTRL_VBAT_READINGS];
    uint8_t _batteryReading;
};

#endif
