
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/lepraassert.h"
#include <utf8.h>
#include "../include/jsonstring.h"
#include "../include/stringutility.h"



namespace lepra {



str JsonString::ToJson(const str& s) {
	const str utf8_string = s;
	str _json_string = "\"";
	for (str::const_iterator x = utf8_string.begin(); x != utf8_string.end(); ++x) {
		const char ch = *x;
		switch (ch) {
			case '\"':	_json_string += "\\\"";	break;
			case '\\':	_json_string += "\\\\";	break;
			case '\b':	_json_string += "\\b";	break;
			case '\f':	_json_string += "\\f";	break;
			case '\n':	_json_string += "\\n";	break;
			case '\r':	_json_string += "\\r";	break;
			case '\t':	_json_string += "\\t";	break;
			default: {
				if (ch >= 31) {
					_json_string += (char)ch;
				} else {
					// Convert UTF-8 to UTF-16 and write out each short as "\uXXXX".
					try {
						str::const_iterator y = x;
						utf8::next(y, utf8_string.end());	// Advance y.
						utf8::uint16_t utf16_string[8];
						const utf8::uint16_t* utf16_end_index = utf8::utf8to16(x, y, &utf16_string[0]);
						for (utf8::uint16_t* z = &utf16_string[0]; z < utf16_end_index; ++z) {
							_json_string += strutil::Format("\\u%.4X", (unsigned)*z);
						}
						x = y-1;
					} catch (std::exception e) {
						_json_string += '?';
					}
				}
			} break;
		}
	}
	_json_string += "\"";
	return _json_string;
}

str JsonString::FromJson(const str& json_string) {
	if (json_string.size() < 2 || json_string[0] != '"' || json_string[json_string.size()-1] != '"') {
		deb_assert(false);
		return str();
	}
	str::const_iterator __end = json_string.end()-1;
	str _s;
	for (str::const_iterator x = json_string.begin()+1; x != __end; ++x) {
		const char ch = *x;
		if (ch == '\\') {
			if (x+1 == __end) {	// Fluke ending?
				_s += '?';
				break;
			}
			++x;
			const char ch2 = *x;
			switch (ch2) {
				case 'b':	_s += '\b';	break;
				case 'f':	_s += '\f';	break;
				case 'n':	_s += '\n';	break;
				case 'r':	_s += '\r';	break;
				case 't':	_s += '\t';	break;
				case 'u': {
					// Convert JSON's UTF-16 encoded characters to UTF-8, then to str.
					utf8::uint16_t utf16_string[128];
					size_t utf16_index = 0;
					for (;;) {
						if (x+5 > __end) {
							_s += '?';
							return _s;
						}
						str number = str(x+1, x+5);
						int value = 0;
						if (strutil::StringToInt(number, value, 16)) {
							utf16_string[utf16_index++] = (utf8::uint16_t)value;
						}
						x += 5;
						if (x > __end-6 || *x != '\\' || *(x+1) != 'u') {
							--x;
							break;
						}
						++x;
					}
					char utf8_string[sizeof(utf16_string)*2];
					char* utf8_end = utf8::utf16to8(&utf16_string[0], &utf16_string[utf16_index], &utf8_string[0]);
					_s += str(utf8_string, utf8_end-&utf8_string[0]);
				} break;
				default: {
					_s += ch2;
				} break;
			}
		} else {
			_s += ch;
		}
	}
	return _s;
}



str JsonString::UrlEncode(const str& url) {
	str escaped;
	const size_t l = url.size();
	for (size_t x = 0; x < l; ++x) {
		const char c = url[x];
		if ((c >= '0' && c <= '9') ||
			(c >= 'A' && c <= 'Z') ||
			(c >= 'a' && c <= 'z') ||
			c == '~' || c == '!' || c == '*' || c == '(' || c == ')' || c == '\'') {
			escaped += c;
		} else {
			escaped += strutil::Format("%%%.2X", (unsigned)c);
		}
	}
	return escaped;
}



}
