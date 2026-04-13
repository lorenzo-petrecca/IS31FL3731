#pragma once

#include "Arduino.h"
#include "FontType.hpp"

class Font {
    public:
        explicit Font(const FontType *font);
        const uint8_t *getCharacter (unsigned char c) const;
        #ifdef ESP32
            FontType getCurrentFont () const;
        #endif
        void setCurrentFont (FontType *font);
        ~Font();
    private:
        const FontType *current_font;
        uint8_t mapCharToIndex (unsigned char c) const;
};
