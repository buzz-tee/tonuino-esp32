#include "player.h"

#include <AudioFileSourceBuffer.h>
#include <AudioFileSourceHTTPStream.h>
#include <AudioFileSourceSPIRAMBuffer.h>

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

Player::Player()
{
    _file = new AudioFileSourceHTTPStream();
    //_file->RegisterMetadataCB(MDCallback, (void *)"HTTP");
    _buffer = new AudioFileSourceSPIRAMBuffer(_file, 5, 131072);
    //_buffer = new AudioFileSourceBuffer(_file, 4096);
    //_buffer->RegisterStatusCB(StatusCallback, (void *)"buffer");
    _output = new AudioOutputI2S(0, AudioOutputI2S::EXTERNAL_I2S, 32, AudioOutputI2S::APLL_ENABLE);
    _mp3 = new AudioGeneratorMP3a();
    //_mp3->RegisterStatusCB(StatusCallback, (void *)"mp3");

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
    _file->open(url);
    Serial.println("Play mp3");
    _mp3->begin(_buffer, _output);
    _output->SetGain(0.05f);
}

bool Player::isPlaying()
{
    return _mp3->isRunning();
}

void Player::stop()
{
    _output->SetGain(0);
    _mp3->stop();
    _buffer->seek(0, 0);

    return;
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
    static int lastms = 0;

    if (_mp3->isRunning())
    {
        if (millis() - lastms > 1000)
        {
            lastms = millis();
            Serial.printf("Running for %d ms...\n", lastms);
            Serial.flush();
        }
        if (!_mp3->loop())
        {
            Serial.println("Stopping");
            stop();
        }
    }
    else
    {
        delay(100);
    }
}