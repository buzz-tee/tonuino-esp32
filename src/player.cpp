#include "player.h"

#include <AudioFileSourceBuffer.h>
#include <AudioFileSourceHTTPStream.h>
#ifdef PLAYER_SPIRAM
#include <AudioFileSourceSPIRAMBuffer.h>
#else
#include <AudioFileSourceBuffer.h>
#endif

#ifdef PLAYER_DEBUG
// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
    const char *ptr = reinterpret_cast<const char *>(cbData);
    (void)isUnicode; // Punt this ball for now
    // Note that the type and string may be in PROGMEM, so copy them to RAM for printf
    char s1[32], s2[64];
    strncpy_P(s1, type, sizeof(s1));
    s1[sizeof(s1) - 1] = 0;
    strncpy_P(s2, string, sizeof(s2));
    s2[sizeof(s2) - 1] = 0;
    Serial.printf("METADATA(%s) '%s' = '%s'\n", ptr, s1, s2);
    Serial.flush();
}

// Called when there's a warning or error (like a buffer underflow or decode hiccup)
void StatusCallback(void *cbData, int code, const char *string)
{
    const char *ptr = reinterpret_cast<const char *>(cbData);
    // Note that the string may be in PROGMEM, so copy it to RAM for printf
    char s1[64];
    strncpy_P(s1, string, sizeof(s1));
    s1[sizeof(s1) - 1] = 0;
    Serial.printf("STATUS(%s) '%d' = '%s'\n", ptr, code, s1);
    Serial.flush();
}
#endif

Player::Player() :
    _volume(PLAYER_INIT_VOLUME),
    _actions(nullptr),
    _previousAction(NONE),
    _playlistUrls(),
    _playlistIndex(-1),
    _started(0),
    _bufferDirty(0)
{
    _file = new AudioFileSourceHTTPStream();
#ifdef PLAYER_SPIRAM
    _buffer = new AudioFileSourceSPIRAMBuffer(_file, 5, 131072);
#else
    _buffer = new AudioFileSourceBuffer(_file, 4096);
#endif
    _output = new AudioOutputI2S(0, AudioOutputI2S::EXTERNAL_I2S, 32, AudioOutputI2S::APLL_ENABLE);
    _mp3 = new AudioGeneratorMP3a();

#ifdef PLAYER_DEBUG
    _file->RegisterMetadataCB(MDCallback, (void *)"HTTP");
    _buffer->RegisterStatusCB(StatusCallback, (void *)"buffer");
    _mp3->RegisterStatusCB(StatusCallback, (void *)"mp3");
#endif

    _output->SetRate(44100);
    _output->SetOutputModeMono(true);

    _beeper = new BeepGenerator(_output);
}

Player::~Player()
{
    stop(true);
    delete _mp3;
    delete _output;
    delete _buffer;
    delete _file;
    delete _beeper;
}

bool Player::_clearActions(ActionCode keep) {
    bool found = false;
    Action* action = _actions;
    _actions = nullptr;
    while (action) {
        Action* next = action->next;
        if (action->code == keep && _actions == nullptr) {
            action->next = nullptr;
            _actions = action;
            found = true;
        } else {
            free(action);
            action = next;
        }
    }
    return found;
}

void Player::_addAction(ActionCode code, ulong param, bool asNext) {
    Action* newAction = (Action*)malloc(sizeof(Action));
    newAction->code = code;
    newAction->param = param;
    newAction->next = nullptr;

    if (code == BEEP || code == SILENCE)
        newAction->param += millis();

    if (asNext) {
        newAction->next = _actions;
        _actions = newAction;
    } else {
        if (_actions == nullptr) {
            _actions = newAction;
        } else {
            Action* action = _actions;
            while (action) {
                if (action->next == nullptr) {
                    action->next = newAction;
                    break;
                }
                action = action->next;
            }
        }
    }
#ifdef PLAYER_DEBUG
    _dumpActions();
#endif
}

bool Player::_removeActions(ActionCode code) {
    bool found = false;
    Action *newActions = nullptr;
    Action *action = _actions;
    Action *prev = nullptr, *next = nullptr;

    while (action) {
        next = action->next;
        if (action->code == code) {
            if (prev) prev->next = nullptr;
            free(action);
            found = true;
        } else {
            if (prev) prev->next = action;
            prev = action;
            if (!newActions) newActions = action;
        }
        action = next;
    }
    _actions = newActions;

    return found;
}

Player::ActionCode Player::_nextAction() {
    if (_actions == nullptr) return NONE;

    Action* action = _actions;
    ActionCode code = action->code;
#ifdef PLAYER_DEBUG    
    if (code != _previousAction) _dumpActions();
#endif
    bool consume = true;
    switch (code) {
        case SILENCE:
        case BEEP: {
            consume = (action->param < millis());
        } break;
        case PAUSE: {
            consume = false;
        } break;
        default: { }
    }
    if (consume) {
#ifdef PLAYER_DEBUG
        Serial.printf("Consumed action %d from stack\n", (int)code);
#endif
        _actions = action->next;
        free(action);
    }
    return code;
}

bool Player::begin()
{
    disableCore0WDT();
    xTaskCreatePinnedToCore(
        Player::_playerWorker,
        "AudioPlayer",
        10000,
        this,
        10,
        &this->_playerTask,
        0);
    return true;
}

void Player::start(const char *url)
{
    _started = millis();
    _file->open(url);
    _buffer->seek(0, 0);
    Serial.print("Starting audio stream from "); Serial.println(url);
    _mp3->begin(_buffer, _output);
    if (_bufferDirty == 0) {
        _setVolume();
    } else {
        _output->SetGainF2P6(0);
    }
}

void Player::_loopSilence() {
    ulong end = millis() + 50;
    int16_t sample[2] = { 0, 0 };
    while (millis() < end) {
        _output->ConsumeSample(sample);
    }
}

bool Player::isPlaying()
{
    return _mp3->isRunning();
}

void Player::stop(bool clearPlaylist)
{
    _output->SetGainF2P6(0);

    _clearActions();

#ifdef PLAYER_SPIRAM
    _bufferDirty = 16;
#endif
    _addAction(STOP, 0, true);
    _removeActions(PAUSE);
    if (clearPlaylist)
        _clearPlaylist();
}

void Player::_playerWorker(void *args)
{
    for (;;)
    {
        ((Player*)args)->_playerLoop();
    }
}

void Player::_playerLoop()
{
    ActionCode action = _nextAction();
    switch (action) {
        case PLAYLIST: {
            if (_playlistIndex < _playlistUrls.size()) {
                start(_playlistUrls[_playlistIndex]);
            } else {
                _addAction(SILENCE, 100);
                _playlistIndex = -1;
            }
        } break;
        case SILENCE: {
            if (_previousAction != SILENCE) _output->SetGainF2P6(0);
            _loopSilence();
        } break;
        case BEEP: {
            _setVolume();
            _beeper->loop();
        } break;
        case PAUSE: {
            if (_previousAction != PAUSE) {
                _output->SetGainF2P6(0);
                if (_mp3->isRunning()) _mp3->loop();
            }
            delay(100);
        } break;
        case STOP: {
            _mp3->stop();
        } break;
        case NONE: {
            if (_previousAction == PAUSE || _previousAction == SILENCE) {
                _setVolume();
            }
            if (_mp3->isRunning()) {
                if (_bufferDirty > 0) {
                    _bufferDirty--;
                    if (_bufferDirty == 0) _setVolume();
                }
                if (!_mp3->loop()) {
                    _mp3->stop();
                    next(false);
                }
            } else {
                delay(100);
            }
        } break;
    }
    _previousAction = action;
}

void Player::volumeUp() {
    if (_volume < PLAYER_MAX_VOLUME) _volume++;
    _setVolume();
    _addAction(SILENCE, 50, true);
    _addAction(BEEP, 20, true);

}

void Player::volumeDown() {
    if (_volume > 0) _volume--;
    _setVolume();
    _addAction(SILENCE, 50, true);
    _addAction(BEEP, 20, true);
}

void Player::_setVolume() {
#ifdef PLAYER_DEBUG
    Serial.print("_setVolume() => "); Serial.println(_volume);
#endif
    uint8_t gain = _volume * PLAYER_VOLUME_FACTOR;
    _output->SetGainF2P6(gain);
}

void Player::next(bool beep) {
    if (isPlaying()) stop(false);
    _playlistIndex++;
    if (beep) {
        _addAction(BEEP, 20);
        _addAction(SILENCE, 50);
    }
    _addAction(PLAYLIST, 0);
    _removeActions(PAUSE);
}

void Player::previous(bool beep) {
    if (isPlaying()) stop(false);
    
    if ((millis() - _started) < PLAYER_PREV_RESTART)
        _playlistIndex--;
    if (beep) {
        _addAction(BEEP, 20);
        _addAction(SILENCE, 50);
    }
    _addAction(PLAYLIST, 0);
    _removeActions(PAUSE);
}

void Player::pause() {
    bool wasPaused = _removeActions(PAUSE);

    if (wasPaused) {
        _setVolume();
        _addAction(BEEP, 20);
    } else {
        _addAction(BEEP, 20);
        _addAction(SILENCE, 50);
        _addAction(PAUSE, 0);
    }
}

void Player::_clearPlaylist(void) {
    stop(false);
    for (int i=0; i<_playlistUrls.size(); i++) {
        free(_playlistUrls[i]);
    }
    _playlistUrls.clear();
    _playlistIndex = -1;
}

void Player::playlist(const char* url) {
    _clearPlaylist();

    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        int len = http.getSize();

        uint8_t buff[256] = { 0 };
        WiFiClient* stream = http.getStreamPtr();

        ulong start = millis();

        while(http.connected() && (len > 0 || len == -1)) {
            size_t size = stream->available();
            if (size) {
                int c = stream->readBytesUntil('\n', buff, std::min((size_t)sizeof(buff), size)) + 1;
                if (c > 0 && buff[0] != '#') {
                    char* playlistEntry = (char *)malloc(c);
                    strncpy(playlistEntry, (char *)buff, c-1);
                    playlistEntry[c-1] = '\0';
                    if (playlistEntry[c-2] == '\r') playlistEntry[c-2] = '\0';
                    _playlistUrls.push_back(playlistEntry);
                    Serial.printf("Loaded playlist entry: %s\n", playlistEntry);
                }
                if (len > 0) {
                    len -= c;
                }
            }

            if (millis() - start > PLAYER_HTTP_TIMEOUT) {
                Serial.println("ERROR Timeout in GET request");
                _clearPlaylist();
                break;
            }
            delay(1);
        }
    } else {
        Serial.printf("ERROR GET failed, response code was: %d\n", httpCode);
    }
    http.end();

    next(false);
}

void Player::_dumpActions() {
    Serial.printf("Action Stack:");
    Action* action = _actions;

    const char names[][16] = {
        "NONE",
        "PAUSE",
        "PLAYLIST",
        "STOP",
        "BEEP",
        "PLAY_SILENCE",
    };

    while (action) {
        Serial.printf("   %s ( %lu )", names[(int)action->code], action->param);
        action = action->next;
    }
    Serial.printf("\n");
    Serial.flush();
}
