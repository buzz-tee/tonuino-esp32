#include "cardreader.h"

#include <SPI.h>

CardReader::CardReader() {
    _mfrc522 = new MFRC522(CARD_SS_PIN, CARD_RST_PIN, CARD_SPI_BUS);
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
