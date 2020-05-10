#include "cardreader.h"
#include "ndef.h"

#include <SPI.h>

CardReader::CardReader()
{
    _mfrc522 = new MFRC522(CARD_SS_PIN, CARD_RST_PIN, CARD_SPI_BUS);
}

CardReader::~CardReader()
{
    _mfrc522->PICC_HaltA();
}

void CardReader::begin()
{
    _mfrc522->PCD_Init();
    _mfrc522->PCD_DumpVersionToSerial();
}

void CardReader::handleCard()
{
    _mfrc522->PICC_IsNewCardPresent();

    _previousCard.size = _currentCard.size;
    _previousCard.sak = _currentCard.sak;
    memcpy(_previousCard.uidByte, _currentCard.uidByte, _currentCard.size);
}

bool CardReader::isCardPresent()
{
    bool readSerial = _mfrc522->PICC_ReadCardSerial();
    if (!readSerial)
    {
        _mfrc522->PICC_IsNewCardPresent();
        readSerial = _mfrc522->PICC_ReadCardSerial();
    }
    if (readSerial)
    {
        _currentCard.size = _mfrc522->uid.size;
        _currentCard.sak = _mfrc522->uid.sak;
        memcpy(_currentCard.uidByte, _mfrc522->uid.uidByte, _mfrc522->uid.size);
        return true;
    }
    else
    {
        _currentCard.size = 0;
        _currentCard.sak = 0;
        return false;
    }
}

bool CardReader::isNewCardPresent()
{
    return !(_currentCard.size == _previousCard.size &&
             memcmp(_currentCard.uidByte, _previousCard.uidByte, _currentCard.size) == 0);
}

bool CardReader::readCard(char* destination, size_t length)
{
    MFRC522::StatusCode status;
    MFRC522::MIFARE_Key key;
    MFRC522::PICC_Type piccType = _mfrc522->PICC_GetType(_currentCard.sak);
    Serial.print("PICC Type: ");
    Serial.println(_mfrc522->PICC_GetTypeName(piccType));

    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
    byte buffer[18];
    byte size = sizeof(buffer);

    if (piccType == MFRC522::PICC_TYPE_MIFARE_UL) {
        byte pACK[] = {0, 0}; //16 bit PassWord ACK returned by the tempCard

        // Authenticate using Ultralight
        Serial.println(F("Authenticating MIFARE UL..."));
        status = _mfrc522->PCD_NTAG216_AUTH(key.keyByte, pACK);
    }
    /*
    // Ignore Mini and Classic
    // Note CARD_TRAILER_BLOCK = 7
    else if ((piccType == MFRC522::PICC_TYPE_MIFARE_MINI) ||
        (piccType == MFRC522::PICC_TYPE_MIFARE_1K) ||
        (piccType == MFRC522::PICC_TYPE_MIFARE_4K))
    {
        Serial.println(F("Authenticating Classic using key A..."));
        status = _mfrc522->PCD_Authenticate(
            MFRC522::PICC_CMD_MF_AUTH_KEY_A, CARD_TRAILER_BLOCK, &key, &(_currentCard));
    } */
    else {
        return false;
    }

    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(_mfrc522->GetStatusCodeName(status));
        return false;
    }

    // Show the whole sector as it currently is
    // Serial.println(F("Current data in sector:"));
    // mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
    // Serial.println();

    // Read data from the block
    if (piccType == MFRC522::PICC_TYPE_MIFARE_UL)
    {
        NdefParser parser(_mfrc522);
        parser.getText(destination, length);
    }
    /*
    // Ignore Mini and Classic
    // Note CARD_BLOCK_ADDR = 4
    else if ((piccType == MFRC522::PICC_TYPE_MIFARE_MINI) ||
        (piccType == MFRC522::PICC_TYPE_MIFARE_1K) ||
        (piccType == MFRC522::PICC_TYPE_MIFARE_4K))
    {
        Serial.print(F("Reading data from block "));
        Serial.print(CARD_BLOCK_ADDR);
        Serial.println(F(" ..."));
        status = (MFRC522::StatusCode)_mfrc522->MIFARE_Read(CARD_BLOCK_ADDR, buffer, &size);
        if (status != MFRC522::STATUS_OK)
        {
            Serial.print(F("MIFARE_Read() failed: "));
            Serial.println(_mfrc522->GetStatusCodeName(status));
            return false;
        }
    }
    */
}
