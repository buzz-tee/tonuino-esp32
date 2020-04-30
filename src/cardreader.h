#ifndef __CARDREADER_H__
#define __CARDREADER_H__

#include <MFRC522.h>

class CardReader {
    public:
        CardReader();
        ~CardReader();
        void begin();
        void handleCard();
        bool isCardPresent();
        bool isNewCardPresent();
    private:
        MFRC522* _mfrc522;

        MFRC522::Uid _previousCard;
        MFRC522::Uid _currentCard;
};

#endif