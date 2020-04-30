#include "cardreader.h"

#include <SPI.h>

#define SS_PIN      15
#define RST_PIN     16
/*
#define SCK_PIN     14
#define MISO_PIN    12
#define MOSI_PIN    13
*/

CardReader::CardReader() {
    _mfrc522 = new MFRC522(SS_PIN, RST_PIN, HSPI);
}

CardReader::~CardReader() {
    _mfrc522->PICC_HaltA();
}

void CardReader::begin() {
    _mfrc522->PCD_Init();
    _mfrc522->PCD_DumpVersionToSerial();
}

void CardReader::handleCard() {
    _mfrc522->PICC_IsNewCardPresent();

    _previousCard.size = _currentCard.size;
    _previousCard.sak = _currentCard.sak;
    memcpy(_previousCard.uidByte, _currentCard.uidByte, _currentCard.size);
}

bool CardReader::isCardPresent() {
    bool readSerial = _mfrc522->PICC_ReadCardSerial();
    if (!readSerial) {
        _mfrc522->PICC_IsNewCardPresent();
        readSerial = _mfrc522->PICC_ReadCardSerial();
    }
    if (readSerial) {
        _currentCard.size = _mfrc522->uid.size;
        _currentCard.sak = _mfrc522->uid.sak;
        memcpy(_currentCard.uidByte, _mfrc522->uid.uidByte, _mfrc522->uid.size);
        return true;
    } else {
        _currentCard.size = 0;
        _currentCard.sak = 0;
        return false;
    }
}

bool CardReader::isNewCardPresent() {
    return ! (_currentCard.size == _previousCard.size && \
        memcmp(_currentCard.uidByte, _previousCard.uidByte, _currentCard.size) == 0);
}
