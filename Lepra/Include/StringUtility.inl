
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#define STR_UTIL_QUAL StringUtilityTemplate<_String>



namespace lepra {



STR_UTIL_TEMPLATE typename STR_UTIL_QUAL::strvec STR_UTIL_QUAL::Split(const _String& s, const _String& char_delimitors, int split_max_count) {
	strvec _token_vector;
	size_t last_position = 0;
	// Find next delimiter at end of token
	size_t current_position = s.find_first_of(char_delimitors, last_position);
	for (int split_count = 0; split_count < split_max_count &&
		(current_position != _String::npos || last_position != _String::npos); ++split_count) {
		// Found a token, add it to the vector.
		_token_vector.push_back(s.substr(last_position, current_position - last_position));
		// Skip delimitor characters after token.  Note the "not_of". this is beginning of token
		last_position = s.find_first_not_of(char_delimitors, current_position);
		// Find next delimiter at end of token.
		current_position = s.find_first_of(char_delimitors, last_position);
	}
	// If not all has been added.
	if (last_position != _String::npos) {
		_token_vector.push_back(s.substr(last_position));
	}
	// If the string ends with a delimitor.
	else if (s.length() > 0 && char_delimitors.find_first_of(s[s.length()-1]) != str::npos) {
		_token_vector.push_back(_String());
	}
	return (_token_vector);
}

STR_UTIL_TEMPLATE typename STR_UTIL_QUAL::strvec STR_UTIL_QUAL::BlockSplit(const _String& s, const _String& char_delimitors, bool keep_quotes, bool is_c_string, int split_max_count) {
	strvec _token_vector;
	FastBlockSplit(_token_vector, s, char_delimitors, keep_quotes, is_c_string, split_max_count);
	return _token_vector;
}

STR_UTIL_TEMPLATE void STR_UTIL_QUAL::FastBlockSplit(strvec& token_vector, const _String& s, const _String& char_delimitors, bool keep_quotes, bool is_c_string, int split_max_count) {
	typename _String::value_type current_token[32*1024];
	int y = 0;
	bool take_next_string = true;
	bool inside_string = false;
	size_t x = 0;
	for (int split_count = 0; x < s.length(); ++x) {
		if (is_c_string && s[x] == '\\' && x+1 < s.length()) {
			current_token[y++] = s[x];
			current_token[y++] = s[++x];
		} else if (s[x] == '"') {
			inside_string = !inside_string;
			if (keep_quotes) {
				current_token[y++] = s[x];
			} else if (y == 0 && !inside_string) {
				// Push empty token.
				token_vector.push_back(_String());
			}
		} else if (!inside_string && char_delimitors.find_first_of(s[x]) != str::npos) {
			if (take_next_string) {
				token_vector.push_back(_String(current_token, y));
				y = 0;
				if (++split_count >= split_max_count) {
					x = s.find_first_not_of(char_delimitors, x);
					break;
				}
			}
			take_next_string = false;
		} else {
			current_token[y++] = s[x];
			take_next_string = true;
		}
		deb_assert(y < sizeof(current_token));
	}
	// If we reached end of string while parsing.
	if (y != 0) {
		token_vector.push_back(_String(current_token, y));
	}
	if (x < s.length()) {	// If we stopped splitting due to split maximum count.
		token_vector.push_back(s.substr(x));
	}
}

STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::Join(const strvec& string_vector, const _String& join_string, size_t start_index, size_t end_index) {
	_String result_string;
	for (size_t x = start_index; x < string_vector.size() && x < end_index; ++x) {
		if (x > start_index) {
			result_string += join_string;
		}
		result_string += string_vector[x];
	}
	return (result_string);
}

STR_UTIL_TEMPLATE void STR_UTIL_QUAL::Append(strvec& destination, const strvec& append) {
		destination.insert(destination.end(), append.begin(), append.end());
}



STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::StringToCString(const _String& s) {
	_String _c_string;
	size_t __length = s.length();
	for (size_t x = 0; x < __length; ++x) {
		switch (s[x]) {
			case '\\':	_c_string += "\\\\";	break;
			case '\t':	_c_string += "\\t";	break;
			case '\v':	_c_string += "\\v";	break;
			case '\r':	_c_string += "\\r";	break;
			case '\n':	_c_string += "\\n";	break;
			case '"':	_c_string += "\\\"";	break;
			default:	_c_string += s[x];	break;
		}
	}
	return (_c_string);
}

STR_UTIL_TEMPLATE bool STR_UTIL_QUAL::CStringToString(const _String& c_string, _String& s) {
	s.clear();
	bool ok = true;
	size_t __length = c_string.length();
	for (size_t x = 0; x < __length; ++x) {
		if (c_string[x] == '\\') {
			if (x+1 >= __length) {
				ok = false;
				break;
			}
			switch (c_string[x+1]) {
				case 'n': {
					s += '\n';
					++x;
				} break;
				case 'r': {
					s += '\r';
					++x;
				} break;
				case 't': {
					s += '\t';
					++x;
				} break;
				case 'v': {
					s += '\v';
					++x;
				} break;
				case '\\': {
					s += '\\';
					++x;
				} break;
				case '"': {
					s += '"';
					++x;
				} break;
				case 'x': {
					const char* number = c_string.c_str()+x+2;
					char* __end = 0;
					int character = StrToL(number, &__end, 16);
					s += (char)character;
					x += 1+(unsigned)(__end-number);
				} break;
				case '0': {
					const char* number = c_string.c_str()+x+2;
					char* __end = 0;
					int character = StrToL(number, &__end, 8);
					s += (char)character;
					x += 1+(unsigned)(__end-number);
				} break;
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9': {
					const char* number = c_string.c_str()+x+1;
					char* __end = 0;
					int character = StrToL(number, &__end, 10);
					s += (char)character;
					x += 1+(unsigned)(__end-number);
				} break;
				default: {
					s += '\\';
					ok = false;
				} break;

			}
		} else {
			s += c_string[x];
		}
	}
	return (ok);
}



STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::StripLeft(const _String& s, const _String& char_delimitors) {
	// Skip delimitor characters at beginning, find start of first token.
	size_t token_start_position = s.find_first_not_of(char_delimitors, 0);
	if (token_start_position == 0) {
		return s;
	}
	if (token_start_position != _String::npos) {
		return (s.substr(token_start_position));
	}
	return (_String());
}

STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::StripRight(const _String& s, const _String& char_delimitors) {
	// Skip delimitor characters at end, find end of last token.
	size_t token_end_position = s.find_last_not_of(char_delimitors, s.size()-1);
	if (token_end_position != _String::npos) {
		return (s.substr(0, token_end_position+1));
	}
	return (_String());
}

STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::Strip(const _String& s, const _String& char_delimitors) {
	return StripRight(StripLeft(s, char_delimitors), char_delimitors);
}



STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::ReplaceAll(const _String& s, typename _String::value_type from_char, typename _String::value_type to_char) {
	_String _s = s;
	int length = (int)s.length();
	for (int x = 0; x < length; ++x) {
		if (_s[x] == from_char) {
			_s[x] = to_char;
		}
	}
	return (_s);
}

STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::ReplaceAll(const _String& s, const _String& from, const _String& to) {
	_String _s = s;
	typename _String::size_type position = 0;
	while ((position = _s.find(from, position)) != str::npos) {
		_s.replace(position, from.length(), to);
		position += to.length();
	}
	return (_s);
}



STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::Format(const typename _String::value_type* format, ...) {
	va_list	arguments;
	va_start(arguments, format);
	_String result = VFormat(format, arguments);
	va_end(arguments);
	return (result);
}



STR_UTIL_TEMPLATE size_t STR_UTIL_QUAL::FindPreviousWord(const _String& line, const _String& delimitors, size_t start_index) {
	_String reverse_line = Reverse(line);
	size_t reverse_index = line.length()-start_index;
	size_t index = reverse_line.find_first_not_of(delimitors, reverse_index);
	index = reverse_line.find_first_of(delimitors, index);
	if (index != str::npos) {
		reverse_index = index;
	} else {
		reverse_index = line.length();
	}
	start_index = line.length()-reverse_index;
	return (start_index);
}

STR_UTIL_TEMPLATE size_t STR_UTIL_QUAL::FindNextWord(const _String& line, const _String& delimitors, size_t start_index) {
	size_t index = line.find_first_of(delimitors, start_index);
	index = line.find_first_not_of(delimitors, index);
	if (index != str::npos) {
		start_index = index;
	} else {
		start_index = line.length();
	}
	return (start_index);
}

STR_UTIL_TEMPLATE int STR_UTIL_QUAL::FindFirstWhiteSpace(const _String& s, size_t offset, int search_direction) {
	int __length = (int)s.length();
	int x = (int)offset;
	for (; x >= 0 && x < __length; x += search_direction) {
		if (IsWhiteSpace(s[x])) {
			break;
		}
	}
	if (x >= __length) {
		x = -1;
	}
	return (x);
}

STR_UTIL_TEMPLATE void STR_UTIL_QUAL::StripAllWhiteSpaces(_String& s) {
	int dest_index = 0;
	for (size_t i = 0; i < s.length(); i++) {
		if (!IsWhiteSpace(s[i])) {
			s[dest_index++] = s[i];
		}
	}
	s.resize(dest_index);
}

STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::ReplaceCtrlChars(const _String& s, typename _String::value_type replacement) {
	_String _s(s);
	for (size_t x = 0; x < _s.length(); ++x) {
		if (_s[x] < ' ') {
			_s[x] = replacement;
		}
	}
	return (_s);
}

STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::DumpData(const uint8* data, size_t _length) {
	const char hex_table[] = "0123456789ABCDEF";
	str data_string(_length*2, '?');
	for (size_t x = 0; x < _length; ++x) {
		data_string[x*2+0] = hex_table[data[x]>>4];
		data_string[x*2+1] = hex_table[data[x]&0xF];
	}
	return data_string;
}



#ifdef LEPRA_POSIX

STR_UTIL_TEMPLATE int STR_UTIL_QUAL::StrCmpI(const typename _String::value_type* string1, const typename _String::value_type* string2) {
	typename _String::value_type c1;
	typename _String::value_type c2;

	while (true) {
		c1 = *string1++;
		c2 = *string2++;

		if (!c1 || !c2) {
			break;
		}

		if (c1 == c2) {
			continue;
		}

		c1 = std::tolower(c1, locale_);
		c2 = std::tolower(c2, locale_);

		if (c1 != c2) {
			break;
		}
	}
	return (int)c1 - (int)c2;
}

#endif // !LEPRA_POSIX



STR_UTIL_TEMPLATE bool STR_UTIL_QUAL::StringToInt(const _String& s, int& value, int radix) {
	if (s.empty()) {
		return (false);
	}
	typename _String::value_type* end_ptr;
	value = StrToL(s.c_str(), &end_ptr, radix);
	return (s.c_str()+s.length() == end_ptr);
}

STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::IntToString(int64 value, int radix) {
	bool sign = (value < 0);
	if (sign) {
		value = -value;
	}

	static typename _String::value_type _s[64] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int i = 62;	// Leave a zero at the end.
	if (radix == 10) {
		while (value > 0) {
			const int digit = (int)(value % 10);
			_s[i--] = (typename _String::value_type)('0' + digit);
			value /= 10;
		}
	} else {
		while (value > 0) {
			int digit = (int)(value % radix);
			value /= radix;

			if (digit >= 0 && digit <= 9) {
				_s[i--] = (typename _String::value_type)('0' + digit);
			} else if(digit > 9) {
				_s[i--] = (typename _String::value_type)('a' + (digit - 10));
			}
		}
	}
	if (sign) {
		_s[i--] = '-';
	}
	if (i == 62) {	// Empty string means zero.
		_s[i--] = '0';
	}
	return &_s[i+1];
}

STR_UTIL_TEMPLATE bool STR_UTIL_QUAL::StringToBool(const _String& s, bool& value) {
	bool ok = false;
	if (s == "false") {
		value = false;
		ok = true;
	} else if (s == "true") {
		value = true;
		ok = true;
	}
	return (ok);
}

STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::DoubleToString(double value, int num_decimals) {
	_String _value;
	DoubleToString(value, num_decimals, _value);
	return (_value);
}

STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::FastDoubleToString(double value) {
	if (value > -1000 && value < 1000) {
		int _value = (int)(value*1000000);
		bool positive;
		if (_value >= 0) {
			positive = true;
		} else {
			positive = false;
			_value = -_value;
		}
		const size_t buffer_size = 11;
		typename _String::value_type _s[buffer_size];
		size_t index = buffer_size-1;
		int x;
		for (x = 0; x < 5; ++x, _value /= 10) {
			int digit;
			if ((digit = _value%10) != 0) {
				_s[index--] = '0' + typename _String::value_type(digit);
			}
		}
		for (; x < 6; ++x, _value /= 10) {
			_s[index--] = '0' + typename _String::value_type(_value%10);
		}
		_s[index--] = '.';
		_s[index--] = '0' + typename _String::value_type(_value%10);
		_value /= 10;
		for (x = 0; _value && x < 2; ++x, _value /= 10) {
			_s[index--] = '0' + typename _String::value_type(_value%10);
		}
		if (positive) {
			++index;
		} else {
			_s[index] = '-';
		}
		const size_t count = buffer_size-index;
		return _String(&_s[index], count);
	} else {
		return DoubleToString(value, 6);
	}
}



STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::Reverse(const _String& s) {
	_String reverse(s);
	for (size_t x = 0; x < s.length(); ++x) {
		reverse[s.length()-x-1] = s[x];
	}
	return (reverse);
}

STR_UTIL_TEMPLATE const _String STR_UTIL_QUAL::Right(const _String& s, size_t char_count) {
	if (char_count <= s.length()) {
		return (s.substr(s.length()-char_count, char_count));
	}
	return (s);
}

STR_UTIL_TEMPLATE bool STR_UTIL_QUAL::StartsWith(const _String& s, const _String& start) {
	if (s.length() >= start.length()) {
		return (s.substr(0, start.length()) == start);
	}
	return (false);
}

STR_UTIL_TEMPLATE bool STR_UTIL_QUAL::EndsWith(const _String& s, const _String& _end) {
	return (Right(s, _end.length()) == _end);
}



STR_UTIL_TEMPLATE void STR_UTIL_QUAL::ToLower(_String& s) {
	for (int i = 0; i < (int)s.length(); i++) {
		s[i] = std::tolower(s[i], locale_);
	}
}

STR_UTIL_TEMPLATE void STR_UTIL_QUAL::ToUpper(_String& s) {
	for (int i = 0; i < (int)s.length(); i++) {
		s[i] = std::toupper(s[i], locale_);
	}
}



STR_UTIL_TEMPLATE int STR_UTIL_QUAL::CompareIgnoreCase(const _String& string1, const _String& string2) {
	return (StrCmpI(string1.c_str(), string2.c_str()));
}



}



#undef STR_UTIL_QUAL
