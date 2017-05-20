#ifndef __PROCALC_STRINGS_HPP__
#define __PROCALC_STRINGS_HPP__


#include <cstdint>
#include <string>


typedef std::basic_string<uint32_t> ucs4string_t;
typedef std::string utf8string_t;


utf8string_t ucs4ToUtf8(const ucs4string_t& ucs);
ucs4string_t utf8ToUcs4(const utf8string_t& utf);


#endif
