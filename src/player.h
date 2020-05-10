#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <AudioFileSource.h>
#include <AudioGeneratorMP3a.h>
#include <AudioOutputI2S.h>

#define PLAYER_MAX_VOLUME       10
#define PLAYER_INIT_VOLUME      2
#define PLAYER_VOLUME_FACTOR    1

#define PLAYER_HTTP_TIMEOUT     1000
#define PLAYER_PREV_RESTART     5000    // restart current track when more than 5 seconds in

class Player {
    public:
        Player();
        ~Player();
        bool begin();
        void start(const char* url);
        void stop(bool clearPlaylist);
        bool isPlaying();

        void playlist(const char* url);

        void volumeUp();
        void volumeDown();
        void next(bool beep = true);
        void previous(bool beep = true);
        void pause();
    private:
        enum ActionCode {
            PAUSE       = 1,
            PLAYLIST    = 2,
            STOP        = 3,
            BEEP_ERROR  = 4,
            BEEP        = 5,
            SILENCE     = 6,
            NONE        = 0
        };
        struct Action {
            Player::ActionCode code;
            ulong param;
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
        AudioFileSource *_file;

        Player::BeepGenerator *_beeper;
        TaskHandle_t _playerTask;

        uint8_t _volume;
        Player::Action* _actions;
        Player::ActionCode _previousAction;

        std::vector<char*> _playlistUrls;
        uint8_t _playlistIndex;
        ulong _started;
        uint8_t _bufferDirty;
    private:
        static void _playerWorker(void* arg);
        void _playerLoop();
        void _setVolume();
        void _clearPlaylist();
        bool _clearActions(ActionCode keep = NONE);
        void _addAction(ActionCode code, ulong param, bool asNext = false);
        ActionCode _nextAction();
        bool _removeActions(ActionCode code);
        void _loopSilence();
        
        void _dumpActions();
};

#endif