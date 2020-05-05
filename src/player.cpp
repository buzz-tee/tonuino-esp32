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
    _paused(false),
    _pauseTriggered(false),
    _bufferDirty(0),
    _stopTriggered(false),
    _playlistUrls(),
    _playlistIndex(-1)
{
    _file = new AudioFileSourceHTTPStream();
#ifdef PLAYER_SPIRAM
    _buffer = new AudioFileSourceSPIRAMBuffer(_file, 5, 131072);
#else
    _buffer = new AudioFileSourceBuffer(_file, 4096);
#endif
    _output = new AudioOutputI2S(1, AudioOutputI2S::EXTERNAL_I2S, 32, AudioOutputI2S::APLL_DISABLE);
    _mp3 = new AudioGeneratorMP3a();

#ifdef PLAYER_DEBUG
    _file->RegisterMetadataCB(MDCallback, (void *)"HTTP");
    _buffer->RegisterStatusCB(StatusCallback, (void *)"buffer");
    _mp3->RegisterStatusCB(StatusCallback, (void *)"mp3");
#endif

    _output->SetRate(44100);
    _output->SetOutputModeMono(true);
}

Player::~Player()
{
    stop();
    delete _mp3;
    delete _output;
    delete _buffer;
    delete _file;
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
    _pauseTriggered = 0;
    _paused = false;
    _file->open(url);
    _buffer->seek(0, 0);
    Serial.print("Starting audio stream from "); Serial.println(url);
    _mp3->begin(_buffer, _output);
    if (_bufferDirty == 0) {
        _setVolume();
    }
}

bool Player::isPlaying()
{
    return _mp3->isRunning();
}

void Player::stop()
{
    _output->SetGainF2P6(0);

    _stopTriggered = true;
#ifdef PLAYER_SPIRAM
    _bufferDirty = 16;
#endif
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
    if ((_pauseTriggered > 0 || !_paused || _stopTriggered)      // if pause() was triggered, run for one more loop even if _paused is true
        && _mp3->isRunning())
    {
        if (!_mp3->loop() || _stopTriggered)
        {
            _stopTriggered = false;
            Serial.println("Stopping");
            _mp3->stop();
        }
        if (_bufferDirty > 0 && _mp3->isRunning()) {
            _bufferDirty--;
            if (_bufferDirty == 0) {
                _setVolume();
            }
        }
        if (_pauseTriggered > 0) {
            _pauseTriggered--;
        }
    }
    else
    {
        delay(100);
    }
}

void Player::volumeUp() {
    _volume++;
    if (_volume > PLAYER_MAX_VOLUME) _volume = PLAYER_MAX_VOLUME;
    _setVolume();
}

void Player::volumeDown() {
    _volume--;
    if (_volume < 0) _volume = 0;
    _setVolume();
}

void Player::_setVolume() {
#ifdef PLAYER_DEBUG
    Serial.print("_setVolume() => "); Serial.println(_volume);
#endif
    uint8_t gain = _volume * PLAYER_VOLUME_FACTOR;
    _output->SetGainF2P6(gain);
}

void Player::next() {

}

void Player::previous() {

}

void Player::pause() {
    _paused = !_paused;
    if (_paused) {
        _output->SetGainF2P6(0);
        _pauseTriggered = 4;
    } else {
        _setVolume();
    }
}

void Player::playlist(const char* url) {
    stop();
    for (int i=0; i<_playlistUrls.size(); i++) {
        free(_playlistUrls[i]);
    }
    _playlistUrls.clear();
    
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        int len = http.getSize();
        Serial.printf("GET OK, size = %d\n", len);
        uint8_t buff[256] = { 0 };
        WiFiClient* stream = http.getStreamPtr();

        while(http.connected() && (len > 0 || len == -1)) {
            size_t size = stream->available();
            if (size) {
                int c = stream->readBytesUntil('\n', buff, std::min(sizeof(buff), size)) + 1;
                if (c > 0 && buff[0] != '#') {
                    char* playlistEntry = (char *)malloc(c);
                    strncpy(playlistEntry, (char *)buff, c-1);
                    playlistEntry[c-1] = '\0';
                    if (playlistEntry[c-2] == '\r') playlistEntry[c-2] = '\0';
                    _playlistUrls.push_back(playlistEntry);
                    Serial.printf("Got line entry: %s\n", playlistEntry);
                }
                if (len > 0) {
                    len -= c;
                }
            }
            Serial.printf("Waiting for %d more bytes\n", len);
            delay(1);
        }
    } else {
        Serial.printf("ERROR GET failed, response code was: %d\n", httpCode);
    }
    http.end();
}
