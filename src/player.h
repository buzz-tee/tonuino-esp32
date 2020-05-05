#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <AudioFileSource.h>
#include <AudioGeneratorMP3a.h>
#include <AudioOutputI2S.h>

#define PLAYER_MAX_VOLUME       10
#define PLAYER_INIT_VOLUME      2
#define PLAYER_VOLUME_FACTOR    1

class Player {
    public:
        Player();
        ~Player();
        bool begin();
        void start(const char* url);
        void stop();
        bool isPlaying();

        void playlist(const char* url);

        void volumeUp();
        void volumeDown();
        void next();
        void previous();
        void pause();

    private:
        AudioOutputI2S *_output;
        AudioFileSource *_buffer;
        AudioGeneratorMP3a *_mp3;
        AudioFileSource *_file;

        TaskHandle_t _playerTask;

        uint8_t _volume;
        bool _paused;
        uint8_t _pauseTriggered;
        uint8_t _bufferDirty;
        bool _stopTriggered;
        std::vector<char*> _playlistUrls;
        uint8_t _playlistIndex;
    private:
        static void _playerWorker(void* arg);
        void _playerLoop();
        void _setVolume();
};

#endif