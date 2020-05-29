#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <AudioFileSourceHTTPStream.h>
#include <AudioGeneratorMP3a.h>
#include <AudioOutputI2S.h>

#define PLAYER_MAX_VOLUME       10
#define PLAYER_INIT_VOLUME      3
#define PLAYER_VOLUME_FACTOR    2

#define PLAYER_FREQ_BEEP        4000
#define PLAYER_FREQ_ERROR       2000

#define PLAYER_HTTP_TIMEOUT     1000
#define PLAYER_PREV_RESTART     5000    // restart current track when more than 5 seconds in
#define PLAYER_PAUSE_EXPIRY     30000   // expire pause after 30 seconds -> state to stop, unpause restarts current track

#ifdef PLAYER_SPIRAM
#define PLAYER_SPIRAM_CS        5
#define PLAYER_BUFFER_SIZE      131072
#else
#define PLAYER_BUFFER_SIZE      4096
#endif

class Player {
    public:
        Player();
        ~Player();
        bool begin();
        void start(const char* url);
        void stop(bool clearPlaylist, bool stopPause = false);
        bool isPlaying();

        void playlist(const char* url);

        void volumeUp();
        void volumeDown();
        void next(bool beep = true);
        void previous(bool beep = true);
        void pause();

        void beep(ulong ms = 200, uint16_t freq = PLAYER_FREQ_BEEP);
        ulong idleSince();
    private:
        enum ActionCode {
            PAUSE       = 1,
            PLAYLIST    = 2,
            STOP        = 3,
            BEEP        = 4,
            SILENCE     = 5,
            PAUSE_STOP  = 6,    // PAUSE has expired (e.g. after 60 seconds close HTTP stream)
            NONE        = 0
        };
        struct Action {
            Player::ActionCode code;
            ulong timestamp;
            void* param;
            Player::Action* next;
        };
        class BeepGenerator {
            public:
                BeepGenerator(AudioOutput* output):
                    _output(output),
                    _sample{ 4000, 4000 },
                    _count(0) {
                    }
                void loop(uint16_t freq) {
                    const uint16_t halfWavelength = (44000 / freq);
                    ulong end = millis() + 10;
                    while (millis() < end) {
                        if (_count % halfWavelength == 0) {
                            _sample[0] = -1 * _sample[0];
                            _sample[1] = -1 * _sample[1];
                        }

                        if (_output->ConsumeSample(_sample)) {
                            _count++;
                        }
                    }
                }
            private:
                AudioOutput* _output;
                int16_t _sample[2];
                uint16_t _count;
        };
        AudioOutputI2S *_output;
        AudioFileSource *_buffer;
        AudioGeneratorMP3a *_mp3;
        AudioFileSourceHTTPStream *_file;

        Player::BeepGenerator *_beeper;
        TaskHandle_t _playerTask;

        uint8_t _volume;
        Player::Action* _actions;
        Player::Action _currentAction;
        Player::ActionCode _previousAction;

        std::vector<char*> _playlistUrls;
        uint8_t _playlistIndex;
        ulong _started;
        uint8_t _bufferDirty;
        ulong _idleSince;

        uint32_t _trackOffset;
    private:
        static void _worker(void* arg);
        void _loop();
        void _setVolume();
        void _clearPlaylist();
        bool _clearActions(ActionCode keep = NONE);
        void _addAction(ActionCode code, ulong timestamp, void* param, bool asNext = false);
        void _nextAction();
        bool _removeActions(ActionCode code);
        void _loopSilence();
        
        void _dumpActions();
};

#endif