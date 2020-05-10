#include "ndef.h"

NdefParser::NdefParser(MFRC522* mfrc522) :
    _mfrc522(mfrc522),
    _blockAddr(MIFARE_START_BLOCK),
    _offset(0) {

}

NdefParser::~NdefParser() {

}

// cf. NFC Forum Type 2 Tag Operation Specification
// e.g. http://sweet.ua.pt/andre.zuquete/Aulas/IRFID/11-12/docs/NFC%20Type%202%20Tag%20Operation%20Specification.pdf

size_t NdefParser::getText(char* destination, size_t length) {
    _blockAddr = MIFARE_START_BLOCK;
    if (!_readBlock()) return false;

    size_t pos = 0;

    byte tagValue = 0x00;
    do {
        tagValue = _buffer[_offset];
        switch (tagValue) {
            case 0x00: {    // NULL TLV
                _advanceOffset(1);                       // No L or V
            } break;
            case 0x01:      // Lock Control TLV
            case 0x02: {    // Memory Control TLV
                _advanceOffset(5);                       // (L)ength = 0x03, skip all 5 bytes
            } break;
            case 0x03: {    // NDEF
                _advanceOffset(1);
                byte ndefLength = _buffer[_offset];
                _advanceOffset(1);
                size_t remaining = length - pos;
                size_t copied = _getTextFromNdef(destination + pos, remaining, ndefLength);
                pos += copied;
            } break;
            case 0xfd: {    // Proprietary TLV
                _advanceOffset(1);                       // advance to (L)ength off proprietary TLV
                _advanceOffset(1 + _buffer[_offset]);    // skip proprietary (V)alue
            } break;
            case 0xfe: {    // Terminator TLV
            } break;
            default: {
                Serial.printf("NDEF Parser ERROR - parsing NDEF/NFC data, unknown tag 0x%02x\n", tagValue);
                return 0;
            }
        }
    } while (tagValue != 0xfe);
    return pos;
}

// cf. NFC Forum NFC Data Exchange Format Technical Specification
// e.g. http://sweet.ua.pt/andre.zuquete/Aulas/IRFID/11-12/docs/NFC%20Data%20Exchange%20Format%20(NDEF).pdf

size_t NdefParser::_getTextFromNdef(char* destination, size_t length, byte recordLength) {
    byte targetOffset = _offset + recordLength;
    byte targetBlockAddr = _blockAddr + (targetOffset >> 2);
    targetOffset %= 4;

    byte header = _buffer[_offset];
    bool shortRecord = (header & 0x10) == 0x10;
    bool hasIdLength = (header & 0x08) == 0x08;
    byte idLength = 0;
    byte tnf = (header & 0x07);

#ifdef NDEF_DEBUG
    Serial.printf("Got NDEF header: 0x%02x\n", header);
#endif

    if (tnf != 1) return 0;

    _advanceOffset(1);
    byte typeLength = _buffer[_offset];

    uint32_t payloadLength = 0;
    for (uint8_t i=0; i<(shortRecord ? 1 : 4); i++) {
        _advanceOffset(1);
        payloadLength = (payloadLength << 8) + _buffer[_offset];
    }

    if (hasIdLength) {
        _advanceOffset(1);
        idLength = _buffer[_offset];
    }

    _advanceOffset(1);
    byte *type = new byte[typeLength];
    _copy(type, typeLength);
    bool isText = (typeLength == 1 && type[0] == 'T');
    delete type;

#ifdef NDEF_DEBUG
    Serial.printf("Got type length: 0x%02x, payloadLength: 0x%08x, idLength: 0x%02x, isText: %s\n", typeLength, payloadLength, idLength, isText ? "true" : "false");
#endif

    if (!isText) return 0;

    _advanceOffset(idLength);       // ignore ID

    byte rtdHeader = _buffer[_offset];
#ifdef NDEF_DEBUG
    Serial.printf("Got RTD header: 0x%02x\n", rtdHeader);
#endif

    bool isUtf16 = (rtdHeader & 0x80) == 0x80;
    byte lcLength = rtdHeader & 0x3f;

#ifdef NDEF_DEBUG
    Serial.printf("UTF16: %s, language code length: %d\n", isUtf16 ? "true" : "false", lcLength);
#endif

    if (isUtf16) return 0;
    _advanceOffset(1 + lcLength);

    size_t copy = payloadLength - lcLength - 1;
    if (copy > length - 1) copy = length - 1;
    _copy((byte*)destination, copy);
    destination[copy] = '\0';

    if (_blockAddr != targetBlockAddr || _offset != targetOffset) {
        Serial.printf("NDEF Parser WARNING - current block address and offset don't match expected ranges - block %d <-> %d - offset %d <-> %d\n", _blockAddr, targetBlockAddr, _offset, targetOffset);
        _blockAddr = targetBlockAddr;
        _offset = targetOffset;
    }

    return copy;
}

void NdefParser::_copy(byte* destination, size_t length) {
    size_t pos = 0;
    while (pos < length) {
        size_t copy = length - pos;
        if (copy + _offset > 4) copy = 4 - _offset;
        memcpy(destination + pos, _buffer + _offset, copy);
        _advanceOffset(copy);
        pos += copy;
    }
}

bool NdefParser::_advanceOffset(byte by) {
    _offset += by;
    if (_offset > 3) {
        _blockAddr += _offset >> 2;
        _offset %= 4;
        return _readBlock();
    } else {
        return true;
    }
}

bool NdefParser::_readBlock() {
    byte size = sizeof(_buffer);
    MFRC522::StatusCode status = (MFRC522::StatusCode)_mfrc522->MIFARE_Read(_blockAddr, _buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(_mfrc522->GetStatusCodeName(status));
        return false;
    } else {
        return true;
    }
}