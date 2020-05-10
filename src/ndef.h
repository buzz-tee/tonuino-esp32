#ifndef __NDEF_H__
#define __NDEF_H__

#include <Arduino.h>
#include <MFRC522.h>

#define NDEF_BUFFER_SIZE    18
#define MIFARE_START_BLOCK  4

class NdefParser {
    public:
        NdefParser(MFRC522* mfrc522);
        ~NdefParser();
        size_t getText(char* destination, size_t length);
    private:
        bool _advanceOffset(byte by = 1);
        bool _readBlock();
        size_t _getTextFromNdef(char* destination, size_t length, byte recordLength);
        void _copy(byte* destination, size_t length);

        MFRC522* _mfrc522;
        byte _blockAddr;
        byte _offset;
        byte _buffer[NDEF_BUFFER_SIZE];
};

#endif