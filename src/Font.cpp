#include "Font.hpp"

Font::Font(const FontType *font) : current_font(font) {}

const uint8_t* Font::getCharacter (unsigned char c) const {
    uint8_t index = mapCharToIndex(c);
    if (index == 255) {
        return nullptr; // Unsupported character
    }
    #ifdef ESP32
        // Accesso diretto su ESP32
        return (const uint8_t *)((const uint8_t *)current_font + (index * 8)) ;
    #else
        // Accesso tramite PROGMEM su AVR
        static uint8_t character[8];
        memcpy_P(character, (const uint8_t *)((const uint8_t *)current_font + (index * 8 * sizeof(uint8_t))), sizeof(character));
        return character;
    #endif  
}

#ifdef ESP32
    FontType Font::getCurrentFont () const {
        return *this->current_font;
    }
#endif


uint8_t Font::mapCharToIndex (unsigned char c) const {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'A' && c <= 'Z') {
        return 10 + c - 'A';
    } else if (c >= 'a' && c <= 'z') {
        return 36 + c - 'a';
    } else if (c >= ' ' && c <= '/') {
        return 62 + c - ' ';
    } else if (c >= ':' && c <= '?') {
        return 78 + c - ':';
    } else if (c >= 0xB3 && c <= 0xCA) {        // from 179 to 202
        return c - 0xB3 + 84;                   // special characters
    }
    /*else {
        switch (c) {
            case '\\h':
                return 85;
            break;
            /*case ' ':
                return 62;
            break;

            case '!':
                return 63;
            break;

            case '?':
                return 64;
            break;

            case '.':
                return 65;
            break;

            case ':':
                return 66;
            break;

            case '"':
                return 66;
            break;

            case '\'':
                return 66;
            break;

            case '(':
                return 66;
            break;

            case ')':
                return 66;
            break;

            case '[':
                return 66;
            break;

            case ']':
                return 66;
            break;

            case '{':
                return 66;
            break;
            
        }
    }*/

    return 255; // Unsupported character
} 

void Font::setCurrentFont (FontType *font) {
    this->current_font = font;
}

Font::~Font(){
    
}