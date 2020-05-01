#ifndef __CARDREADER_H__
#define __CARDREADER_H__

#include <MFRC522.h>

#define CARD_SS_PIN     15
#define CARD_RST_PIN    16
#define CARD_SPI_BUS    HSPI


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