#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <AudioFileSource.h>
#include <AudioGeneratorMP3a.h>
#include <AudioOutputI2S.h>

class Player {
    public:
        Player();
        ~Player();
        bool begin();
        void start(const char* url);
        void stop();
        bool isPlaying();

    private:
        AudioOutputI2S *_output;
        AudioFileSource *_buffer;
        AudioGeneratorMP3a *_mp3;
        AudioFileSource *_file;

        TaskHandle_t _playerTask;
  
    private:
        static void _playerWorker(void* arg);
        void _playerLoop();
};

#endif