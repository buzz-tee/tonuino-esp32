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
        void next();
        void previous();
        void pause();

        void beep(uint16_t ms = 200);
    private:
        enum ActionCode {
            PAUSE           = 1,
            PLAYLIST        = 2,
            STOP            = 3,
            BEEP            = 5,
            PLAY_SILENCE    = 6,
            NONE            = 0
        };
        struct Action {
            Player::ActionCode code;
            ulong param;
            Action* next;
        };
        AudioOutputI2S *_output;
        AudioFileSource *_buffer;
        AudioGeneratorMP3a *_mp3;
        AudioFileSource *_file;

        TaskHandle_t _playerTask;

        uint8_t _volume;
        Action* _actions;
        ActionCode _previousAction;

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