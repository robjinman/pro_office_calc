#include <cassert>
#include "strings.hpp"


//===========================================
// ucs4ToUtf8
//===========================================
utf8string_t ucs4ToUtf8(const ucs4string_t& ucs) {
  utf8string_t utf;

  for (unsigned int i = 0; i < ucs.length(); ++i) {
    uint32_t c = ucs.data()[i];

    if (c < 0x80) {
      utf.push_back(c);
    }
    else if (c < 0x800) {
      utf.push_back(0b11000000 + ((c & 0b11111000000) >> 6));
      utf.push_back(0b10000000 + (c & 0b111111));
    }
    else if (c < 0x10000) {
      utf.push_back(0b11100000 + ((c & 0b1111000000000000) >> 12));
      utf.push_back(0b10000000 + ((c & 0b111111000000) >> 6));
      utf.push_back(0b10000000 + (c & 0b111111));
    }
    else if (c < 0x200000) {
      utf.push_back(0b11110000 + ((c & 0b111000000000000000000) >> 18));
      utf.push_back(0b10000000 + ((c & 0b111111000000000000) >> 12));
      utf.push_back(0b10000000 + ((c & 0b111111000000) >> 6));
      utf.push_back(0b10000000 + (c & 0b111111));
    }
    else if (c < 0x4000000) {
      utf.push_back(0b11111000 + ((c & 0b11000000000000000000000000) >> 24));
      utf.push_back(0b10000000 + ((c & 0b111111000000000000000000) >> 18));
      utf.push_back(0b10000000 + ((c & 0b111111000000000000) >> 12));
      utf.push_back(0b10000000 + ((c & 0b111111000000) >> 6));
      utf.push_back(0b10000000 + (c & 0b111111));
    }
  }

  return utf;
}

//===========================================
// utf8ToUcs4
//===========================================
ucs4string_t utf8ToUcs4(const utf8string_t& utf) {
  ucs4string_t ucs;

  uint32_t val = 0;
  for (unsigned int i = 0; i < utf.length(); ++i) {
    uint8_t c = utf.data()[i];

    if (c >> 7 == 0b0) {
      if (i > 0) {
        ucs.push_back(val);
      }
      val = c;
      if (i + 1 == utf.length()) {
        ucs.push_back(val);
      }
    }
    else if (c >> 6 == 0b10) {
      val = val << 6;
      val += c & 0b111111;
      if (i + 1 == utf.length()) {
        ucs.push_back(val);
      }
    }
    else if (c >> 5 == 0b110) {
      if (i > 0) {
        ucs.push_back(val);
      }
      val = c & 0b11111;
    }
    else if (c >> 4 == 0b1110) {
      if (i > 0) {
        ucs.push_back(val);
      }
      val = c & 0b1111;
    }
    else if (c >> 3 == 0b11110) {
      if (i > 0) {
        ucs.push_back(val);
      }
      val = c & 0b111;
    }
    else if (c >> 2 == 0b111110) {
      if (i > 0) {
        ucs.push_back(val);
      }
      val = c & 0b11;
    }
    else if (c >> 1 == 0b1111110) {
      if (i > 0) {
        ucs.push_back(val);
      }
      val = c & 0b1;
    }
    else {
      assert(false);
    }
  }

  return ucs;
}
