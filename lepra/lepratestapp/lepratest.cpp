
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



// Unreachable code warning below (MSVC8). For some reason just this file happens to temper with some shitty template.
#include "pch.h"
#include "../../lepra/include/lepratarget.h"
#ifdef LEPRA_MSVC
#pragma warning(push)
#pragma warning(disable: 4702)
#endif // LEPRA_MSVC
#include "../include/lepra.h"
#ifdef LEPRA_MSVC
#pragma warning(pop)
#endif // LEPRA_MSVC

//#define TEST_WEB_AND_MAIL

#include "../../lepra/include/lepraassert.h"
#include <math.h>
#include "../include/canvas.h"
#include "../include/des.h"
#include "../include/diskfile.h"
#include "../include/idmanager.h"
#include "../include/iobuffer.h"
#include "../include/jsonstring.h"
#include "../include/lepra.h"
#include "../include/logger.h"
#include "../include/loglistener.h"
#include "../include/math.h"
#include "../include/network.h"
#include "../include/number.h"
#include "../include/orderedmap.h"
#include "../include/path.h"
#include "../include/performance.h"
#include "../include/random.h"
#include "../include/rotationmatrix.h"
#include "../include/sha1.h"
#include "../include/socket.h"
#include "../include/spinlock.h"
#include "../include/string.h"
#include "../include/systemmanager.h"
#include "../include/tcpmuxsocket.h"
#include "../include/timer.h"
#include "../include/transformation.h"


using namespace lepra;

class LepraTest{};
static LogDecorator gLLog(LogType::GetLogger(LogType::kTest), typeid(LepraTest));



//bool TestSkipList(const LogDecorator& account);
bool TestBinTree(const LogDecorator& account);
bool TestLooseBintree(const LogDecorator& account);
bool TestLooseQuadtree(const LogDecorator& account);
bool TestLooseOctree(const LogDecorator& account);
bool TestThreading(const LogDecorator& account);
bool TestUDPSockets(const LogDecorator& account);
bool TestRotationMatrix(const LogDecorator& account);

void ReportTestResult(const LogDecorator& log, const str& test_name, const str& context, bool result) {
	if (result) {
		log.Headlinef("%20s: passed.", test_name.c_str());
	} else {
		log.Fatalf("%20s: failed (%s)!", test_name.c_str(), context.c_str());
	}
}

bool TestString(const LogDecorator& account) {
	str _context;
	bool _test_ok = true;

	// Verify str basics.
	if (_test_ok) {
		_context = "String basics";
		str s = "ABCdefghijklmnopqrstUvwxyz";
		s += "97531" + s + "02468";
		s.erase(31, 4);	// Remove second ABCd.
		s = strutil::ReplaceAll(s, 'x', ' ');
		str tmp1 = "oA";
		strutil::ToUpper(tmp1);
		str tmp2 = "Oa";
		strutil::ToLower(tmp2);
		s.insert(10, tmp1+tmp2);
		tmp1 = "*!\" #\t%&/\r()=\n\v";
		strutil::StripAllWhiteSpaces(tmp1);
		s.insert(30, tmp1);
		s.insert(0, strutil::Right("Johannes", 6) + str("Sune").substr(0, 1));
		_test_ok = (s == "hannesSABCdefghijOAoaklmnopqrstUvw yz*!\"#%&/()=97531efghijklmnopqrstUvw yz02468" &&
			s.find('S', 0) == 6 &&
			s.rfind('S', s.length()-1) == 6);
		deb_assert(_test_ok);
	}

	// Verify str basics.
	if (_test_ok) {
		_context = "String start/end";
		str data("This is the start of something new!");
		_test_ok = (strutil::StartsWith(data, "This is") &&
			!strutil::StartsWith(data, "That is") &&
			strutil::EndsWith(data, " new!") &&
			!strutil::EndsWith(data, " old!"));
		deb_assert(_test_ok);
	}

	// Verify strutil::Format.
	if (_test_ok) {
		_context = "strutil::Format()";
		int i = 123;
		float f = 123.321f;
		str s = "String " + strutil::Format("format test: %i, %.4f, %s", i, f, "Hello World!");
		_test_ok = (s == "String format test: 123, 123.3210, Hello World!");
		deb_assert(_test_ok);
	}

	// Verify str conversion to/from Ansi/Unicode.
	if (_test_ok) {
		_context = "Ansi/Unicode conversion";
		const char* test_data1    =  "Knastest !\"#%&/()=?'-_+\\}][{$@'*.:,;~^<>|";
		const wchar_t* test_data2 = L"Knastest !\"#%&/()=?'-_+\\}][{$@'*.:,;~^<>|";
		wstr unicode = wstrutil::Encode(str(test_data1));
		str ansi = strutil::Encode(wstr(test_data2));
		_test_ok = (ansi == test_data1 && unicode == test_data2);
		deb_assert(_test_ok);
	}

	// Test empty int conversion.
	if (_test_ok) {
		_context = "empty int conversion";
		int value = 0;
		_test_ok = !strutil::StringToInt("", value);
		deb_assert(_test_ok);
	}

	// Test floating point conversion.
	if (_test_ok) {
		_context = "float conversion";
		float64 value = 1.5;
		str s = strutil::Format("%.5f", value);
		_test_ok = strutil::StringToDouble(s, value);
		if (_test_ok) {
			_test_ok = (s == "1.50000" && value == 1.5f);
		}
		deb_assert(_test_ok);
	}

	// Test empty floating point conversion.
	if (_test_ok) {
		_context = "empty float conversion";
		double value = 0;
		_test_ok = !strutil::StringToDouble("", value);
		deb_assert(_test_ok);
	}

	// Test fast floating point conversion.
	if (_test_ok) {
		_context = "fast float conversion";
		_test_ok = (strutil::FastDoubleToString(0.0) == "0.0");
		deb_assert(_test_ok);
		if (_test_ok) {
			_test_ok = (strutil::FastDoubleToString(10.0) == "10.0");
			deb_assert(_test_ok);
		}
		if (_test_ok) {
			_test_ok = (strutil::FastDoubleToString(-287.75) == "-287.75");
			deb_assert(_test_ok);
		}
		if (_test_ok) {
			_test_ok = (strutil::FastDoubleToString(-2.625) == "-2.625");
			deb_assert(_test_ok);
		}
	}

	if (_test_ok) {
		_context = "string -> C string";
		_test_ok = (strutil::StringToCString("Hej\"\\\n'!#\r\t") == "Hej\\\"\\\\\\n'!#\\r\\t");
		deb_assert(_test_ok);
	}

	if (_test_ok) {
		_context = "C string -> string";
		str value;
		_test_ok = strutil::CStringToString("Hej\\\"\\\\\\n'!#\\r\\t", value);
		deb_assert(_test_ok);
		if (_test_ok) {
			_test_ok = (value == "Hej\"\\\n'!#\r\t");
			deb_assert(_test_ok);
		}
	}

	strutil::strvec test_words;
	if (_test_ok) {
		_context = "plain string splitting";
		test_words = strutil::Split("Den \"kyliga Trazan'\"\tapansson\r\n\v.\t\t", " \t\v\r\n");
		_test_ok = (test_words.size() == 6 && test_words[0] == "Den" && test_words[1] == "\"kyliga" &&
			test_words[2] == "Trazan'\"" && test_words[3] == "apansson" && test_words[4] == "." &&
			test_words[5].empty());
		deb_assert(_test_ok);
	}

	if (_test_ok) {
		_context = "string stripping";
		test_words[1] = strutil::StripLeft(test_words[1], "\"");
		_test_ok = (test_words[1] == "kyliga");
		if (_test_ok) {
			_test_ok = (strutil::StripRight("aabbcc", "c") == "aabb");
		}
		if (_test_ok) {
			_test_ok = (strutil::Strip("  Jonte  ", " ") == "Jonte");
		}
		deb_assert(_test_ok);
	}

	if (_test_ok) {
		_context = "string joining";
		str s = strutil::Join(test_words, " ");
		_test_ok = (s == "Den kyliga Trazan'\" apansson . ");
		deb_assert(_test_ok);
	}

	if (_test_ok) {
		_context = "block string splitting 1";
		strutil::strvec words = strutil::BlockSplit("\"Hej du glade\" sade jag  \ttill\n\n\r\vhonom igen.", " \t\v\r\n", false, false, 4);
		size_t phrase_count = words.size();
		const str& word0 = words[0];
		const str& word1 = words[1];
		const str& word2 = words[2];
		const str& word3 = words[3];
		const str& word4 = words[4];
		_test_ok = (phrase_count == 5 && word0 == "Hej du glade" && word1 == "sade" &&
			word2 == "jag" && word3 == "till" && word4 == "honom igen.");
		deb_assert(_test_ok);
	}

	if (_test_ok) {
		_context = "block string splitting 2";
		strutil::strvec words = strutil::BlockSplit("\"Hej du glade \" sade jag  \t\"till\n\"\n\r\vhan..\nhonom igen.", " \t\v\r\n", true, false, 4);
		_test_ok = (words.size() == 5 && words[0] == "\"Hej du glade \"" && words[1] == "sade" &&
			words[2] == "jag" && words[3] == "\"till\n\"" && words[4] == "han..\nhonom igen.");
		deb_assert(_test_ok);
	}

	if (_test_ok) {
		_context = "block string splitting 3";
		strutil::strvec words = strutil::BlockSplit("\"a\\\"b\"", " \t\v\r\n\"", false, true, 4);
		_test_ok = (words.size() == 1 && words[0] == "a\\\"b");
		deb_assert(_test_ok);
	}

	if (_test_ok) {
		_context = "block whitespace limited splitting";
		strutil::strvec words = strutil::BlockSplit("abc def", " \t\v\r\n\"", false, false, 1);
		_test_ok = (words.size() == 2 && words[0] == "abc" && words[1] == "def");
		deb_assert(_test_ok);
	}

	if (_test_ok) {
		_context = "block string splitting empty quote";
		strutil::strvec words = strutil::BlockSplit("\"\"", " \t\v\r\n", false, true);
		_test_ok = (words.size() == 1 && words[0].empty());
		deb_assert(_test_ok);
	}

	if (_test_ok) {
		_context = "block string splitting quote with spaces";
		strutil::strvec words = strutil::BlockSplit(" \" \" ", " \t\v\r\n", false, true);
		_test_ok = (words.size() == 2 && words[0].empty() && words[1] == " ");
		deb_assert(_test_ok);
	}

	if (_test_ok) {
		_context = "JSON coder";
		const wstr wc = _WIDE("åäöabcÅÄÖACQñï");
		const str __c = strutil::Encode(wc);
		const str json = "\"\\u00E5\\u00E4\\u00F6abc\\u00C5\\u00C4\\u00D6ACQ\\u00F1\\u00EF\"";
		const str json_string = JsonString::ToJson(__c);
		_test_ok = (json_string == json);
		deb_assert(_test_ok);
		if (_test_ok) {
			const str s = JsonString::FromJson(json);
			_test_ok = (s == __c);
			deb_assert(_test_ok);
		}
	}

	ReportTestResult(account, "String", _context, _test_ok);
	return (_test_ok);
}

bool TestRandom(const LogDecorator& account) {
	str _context;
	bool _test_ok = true;

	if (_test_ok) {
		_context = "uniform span";
		const double low = 0.5;
		const double high = 2.0;
		double average = 0;
		const int cnt = 10000;
		for (int x = 0; x < cnt; ++x) {
			const double _result = Random::Uniform(low, high);
			_test_ok = (_result >= low && _result <= high);
			deb_assert(_test_ok);
			average += _result;
		}
		if (_test_ok) {
			_context = "uniform mean";
			average /= cnt;
			_test_ok = (average >= 1.23 && average <= 1.27);
			deb_assert(_test_ok);
		}
	}

	if (_test_ok) {
		double average = 0;
		const double mean = -3;
		const double std_dev = 2;
		const int cnt = 10000;
		std::vector<double> values;
		for (int x = 0; x < cnt; ++x) {
			values.push_back(Random::Normal(mean, std_dev, -10.0, +10.0));
			deb_assert(values[x] >= -10 && values[x] <= +10);
			average += values[x];
		}
		_context = "normal mean";
		average /= cnt;
		_test_ok = (average >= mean-0.03*std_dev && average <= mean+0.03*std_dev);
		deb_assert(_test_ok);
		if (_test_ok) {
			_context = "normal distribuion";
			const double actual_std_dev = Math::CalculateDeviation<double>(values, mean);
			_test_ok = (actual_std_dev >= std_dev-0.04 && actual_std_dev <= std_dev+0.04);
			deb_assert(_test_ok);
		}
	}

	ReportTestResult(account, "Random", _context, _test_ok);
	return (_test_ok);
}

bool TestMath(const LogDecorator& account) {
	str _context;
	bool _test_ok = true;
	double _result;

	if (_test_ok) {
		_context = "smooth clamp narrow below max exp";
		_result = Math::SmoothClamp(0.9, 0.0, 1.0, 0.1);
		_test_ok = (_result == 0.9);
		deb_assert(_test_ok);
	}

	if (_test_ok) {
		_context = "smooth clamp wide above max exp";
		_result = Math::SmoothClamp(0.95, 0.0, 1.0, 0.5);
		_test_ok = (_result < 0.8);
		deb_assert(_test_ok);
	}

	if (_test_ok) {
		_context = "smooth clamp above min exp";
		_result = Math::SmoothClamp(0.3, 0.0, 1.0, 0.3);
		_test_ok = (_result == 0.3);
		deb_assert(_test_ok);
	}

	if (_test_ok) {
		_context = "smooth clamp below min exp";
		_result = Math::SmoothClamp(0.2, 0.0, 1.0, 0.4);
		_test_ok = (_result > 0.23);
		deb_assert(_test_ok);
	}

	ReportTestResult(account, "Math", _context, _test_ok);
	return (_test_ok);
}

bool TestNumber(const LogDecorator& account) {
	// Verify Number basics.
	str _context;
	bool _test_ok = true;
	str _result;
	str desired_result;

	if (_test_ok) {
		desired_result = "1.00 ";
		_context = "testing "+desired_result;
		_result = Number::ConvertToPostfixNumber(1, 2);
		_test_ok = (_result == desired_result);
		deb_assert(_test_ok);
	}

	if (_test_ok) {
		desired_result = "1.41 k";
		_context = "testing "+desired_result;
		_result = Number::ConvertToPostfixNumber(1.414444e3, 2);
		_test_ok = (_result == desired_result);
		deb_assert(_test_ok);
	}

	if (_test_ok) {
		desired_result = "7.6667 m";
		_context = "rounding "+desired_result;
		_result = Number::ConvertToPostfixNumber(7.66666666666666e-3, 4);
		_test_ok = (_result == desired_result);
		deb_assert(_test_ok);
	}

	if (_test_ok) {
		desired_result = "10 M";
		_context = "testing "+desired_result;
		_result = Number::ConvertToPostfixNumber(10e6, 0);
		_test_ok = (_result == desired_result);
		deb_assert(_test_ok);
	}

	ReportTestResult(account, "Number", _context, _test_ok);
	return (_test_ok);
}

bool TestVector3D(const LogDecorator& account) {
	// Verify Number basics.
	str _context;
	bool _test_ok = true;
	float _result;
	float desired_result;

	if (_test_ok) {
		desired_result = 0;
		_context = "testing polar angle Y " + strutil::Format("%.1f", desired_result);
		_result = vec3(1,0,0).GetPolarCoordAngleY();
		_test_ok = Math::IsEpsEqual(_result, desired_result);
		deb_assert(_test_ok);
		if (_test_ok) {
			desired_result = PIF/2;
			_context = "testing polar angle Y " + strutil::Format("%.1f", desired_result);
			_result = vec3(0,0,1).GetPolarCoordAngleY();
			_test_ok = Math::IsEpsEqual(_result, desired_result);
			deb_assert(_test_ok);
		}

		if (_test_ok) {
			desired_result = -5*PIF/6;
			_context = "testing polar angle Y " + strutil::Format("%.1f", desired_result);
			//_result = vec3(-sqrtf(3),0,-1).GetPolarCoordAngleY();
			_result = atan2(-1.0f, -sqrtf(3.0f));
			_test_ok = Math::IsEpsEqual(_result, desired_result);
			deb_assert(_test_ok);
		}

		if (_test_ok) {
			desired_result = +PIF;
			_context = "testing polar angle Y " + strutil::Format("%.1f", desired_result);
			//_result = vec3(-1,0,0).GetPolarCoordAngleY();
			_result = atan2(0.0f, -1.0f);
			_test_ok = Math::IsEpsEqual(_result, desired_result);
			deb_assert(_test_ok);
		}
	}

	ReportTestResult(account, "Vector3D", _context, _test_ok);
	return (_test_ok);
}

bool TestOrderedMap(const LogDecorator& account) {
	str _context;
	bool _test_ok = true;

	OrderedMap<str, int> map;
	if (_test_ok) {
		_context = "empty start";
		_test_ok = (map.GetCount() == 0 && map.Find("4") == map.End());
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "insert unique";
		map.PushBack("4",  5);
		_test_ok = (map.GetCount() == 1 && map.Find("4").GetObject() == 5);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "erase -> empty";
		map.Remove(map.Find("4"));
		_test_ok = (map.GetCount() == 0 && map.Find("4") == map.End());
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "insert two";
		map.PushBack("4",  5);
		map.PushBack("5",  4);
		_test_ok = (map.GetCount() == 2 && map.Find("4").GetObject() == 5 && map.Find("5").GetObject() == 4);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "pop front";
		str key;
		int value;
		map.PopFront(key, value);
		_test_ok = (key == "4" && value == 5 && map.GetCount() == 1);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "pop back";
		str key;
		int value;
		map.PopBack(key, value);
		_test_ok = (key == "5" && value == 4 && map.GetCount() == 0);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "insert three";
		map.PushFront("5",  4);
		map.PushFront("4",  5);
		map.PushFront("3",  3);
		_test_ok = (map.GetCount() == 3 && map.Find("3").GetObject() == 3 &&
			map.Find("4").GetObject() == 5 && map.Find("5").GetObject() == 4);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "erase mid";
		map.Remove("4");
		_test_ok = (map.GetCount() == 2 && map.Find("3").GetObject() == 3 &&
			map.Find("4") == map.End() && map.Find("5").GetObject() == 4);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "erase end";
		map.Remove("5");
		_test_ok = (map.GetCount() == 1 && map.Find("3").GetObject() == 3 &&
			map.Find("4") == map.End() && map.Find("5") == map.End());
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "erase last";
		map.Remove("3");
		_test_ok = (map.GetCount() == 0 && map.Find("3") == map.End() &&
			map.Find("4") == map.End() && map.Find("5") == map.End());
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "random add/remove";
		const int entries = 11;
		str set[entries];
		size_t set_count = 0;
		for (int x = 1; _test_ok && x < 10000; ++x) {
			deb_assert(map.GetCount() == set_count);
			int index = Random::GetRandomNumber()%entries;
			if (!(x%59)) {
				for (int y = 0; y < entries; ++y) {
					set[y] = EmptyAnsiString;
				}
				map.RemoveAll();
				set_count = 0;
				_test_ok = (map.GetCount() == set_count);
				deb_assert(_test_ok);
			} else if (set[index].empty()) {
				set[index] = ":"+strutil::Format("%i", x);
				_test_ok = map.Find(set[index]) == map.End();
				deb_assert(_test_ok);
				if (_test_ok) {
					map.PushBack(set[index], x);
					++set_count;
					_test_ok = (map.GetCount() == set_count &&
						map.Find(set[index]).GetObject() == x &&
						map.Find(set[index]) == map.Last());
					deb_assert(_test_ok);
				}
			} else {
				_test_ok = (map.Find(set[index]) != map.End());
				deb_assert(_test_ok);
				if (_test_ok) {
					int value = -1;
					strutil::StringToInt(set[index].c_str()+1, value, 10);
					_test_ok = (_test_ok && map.Find(set[index]).GetObject() == value);
					deb_assert(_test_ok);
				}
				if (_test_ok) {
					static bool iterator_remove = false;
					iterator_remove = !iterator_remove;
					if (iterator_remove) {
						map.Remove(map.Find(set[index]));
					} else {
						map.Remove(set[index]);
					}
					set[index].clear();
					--set_count;
					_test_ok = (map.GetCount() == set_count && map.Find(set[index]) == map.End());
					deb_assert(_test_ok);
				}
			}
		}
		_test_ok = (map.GetCount() == set_count);
		deb_assert(_test_ok);
	}

	ReportTestResult(account, "OrderedMap", _context, _test_ok);
	return (_test_ok);
}

bool TestIdManager(const LogDecorator& account) {
	str _context;
	bool _test_ok = true;
	str debug_state;

	IdManager<int> id_manager(3, 100, -2);
	if (_test_ok) {
		_context = "min ID";
		_test_ok = (id_manager.GetMinId() == 3);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "max ID";
		_test_ok = (id_manager.GetMaxId() == 100);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "invalid ID";
		_test_ok = (id_manager.GetInvalidId() == -2);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "reserve ID (4)";
		_test_ok = id_manager.ReserveId(4);
		deb_assert(_test_ok);
		debug_state = id_manager.GetDebugState();
		_test_ok = (debug_state == "3-2, 3-3,\n4-4, 5-100,\n");
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "alloc ID (3)";
		_test_ok = (id_manager.GetFreeId() == 3);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "alloc ID (5)";
		_test_ok = (id_manager.GetFreeId() == 5);
		debug_state = id_manager.GetDebugState();
		_test_ok = (debug_state == "3-5, 6-100,\n");
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "recycle ID (4)";
		_test_ok = id_manager.RecycleId(4);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "invalid recycle ID (4)";
		_test_ok = !id_manager.RecycleId(4);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "invalid recycle ID (4)";
		_test_ok = !id_manager.RecycleId(4);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "invalid recycle ID (7)";
		_test_ok = !id_manager.RecycleId(7);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "invalid recycle ID (1000)";
		_test_ok = !id_manager.RecycleId(1000);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "invalid recycle ID (-2)";
		_test_ok = !id_manager.RecycleId(-2);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "reserve ID (4)";
		_test_ok = id_manager.ReserveId(4);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "invalid reserve ID (3)";
		_test_ok = !id_manager.ReserveId(3);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "invalid reserve ID (4)";
		_test_ok = !id_manager.ReserveId(4);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "invalid reserve ID (2)";
		_test_ok = !id_manager.ReserveId(2);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "invalid reserve ID (-2)";
		_test_ok = !id_manager.ReserveId(-2);
		deb_assert(_test_ok);
	}
	for (int x = 6; _test_ok && x <= 100; ++x) {
		_context = "alloc many IDs";
		_test_ok = (id_manager.GetFreeId() == x);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "no free IDs";
		_test_ok = (id_manager.GetFreeId() == -2);
		deb_assert(_test_ok);
		debug_state = id_manager.GetDebugState();
		_test_ok = (debug_state == "3-100, 101-100,\n");
		deb_assert(_test_ok);
	}
	for (int y = 0; y < 1000; ++y) {
		for (int z = 100; _test_ok && z >= 3; z -= 2) {
			_context = "free dec-loop";
			_test_ok = id_manager.RecycleId(z);
			deb_assert(_test_ok);
		}
		for (int u = 100; _test_ok && u >= 3; u -= 2) {
			_context = "reserve dec-loop";
			_test_ok = id_manager.ReserveId(u);
			deb_assert(_test_ok);
		}
		for (int v = 3; _test_ok && v <= 100; ++v) {
			_context = "free inc-loop";
			_test_ok = id_manager.RecycleId(v);
			deb_assert(_test_ok);
		}
		for (int w = 3; _test_ok && w <= 100; ++w) {
			_context = "alloc inc-loop";
			_test_ok = (id_manager.GetFreeId() == w);
			deb_assert(_test_ok);
		}
	}
	if (_test_ok) {
		// Just simple an anti-crash test.
		IdManager<int> id_manager2(1, 0x7FFFFFFF-1, 0xFFFFFFFF);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-0, 1-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = id_manager2.ReserveId(1);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-1, 2-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = id_manager2.ReserveId(5);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-1, 2-4,\n5-5, 6-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = id_manager2.ReserveId(3);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-1, 2-2,\n3-3, 4-4,\n5-5, 6-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = id_manager2.RecycleId(1);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-0, 1-2,\n3-3, 4-4,\n5-5, 6-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = id_manager2.ReserveId(2147483646);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-0, 1-2,\n3-3, 4-4,\n5-5, 6-2147483645,\n2147483646-2147483646, 2147483647-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = !id_manager2.RecycleId(4);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-0, 1-2,\n3-3, 4-4,\n5-5, 6-2147483645,\n2147483646-2147483646, 2147483647-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = !id_manager2.RecycleId(6);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-0, 1-2,\n3-3, 4-4,\n5-5, 6-2147483645,\n2147483646-2147483646, 2147483647-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = !id_manager2.RecycleId(1);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-0, 1-2,\n3-3, 4-4,\n5-5, 6-2147483645,\n2147483646-2147483646, 2147483647-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = !id_manager2.RecycleId(2147483645);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-0, 1-2,\n3-3, 4-4,\n5-5, 6-2147483645,\n2147483646-2147483646, 2147483647-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = id_manager2.RecycleId(5);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-0, 1-2,\n3-3, 4-2147483645,\n2147483646-2147483646, 2147483647-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = id_manager2.RecycleId(2147483646);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-0, 1-2,\n3-3, 4-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = id_manager2.RecycleId(3);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-0, 1-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = id_manager2.ReserveId(1);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-1, 2-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = id_manager2.ReserveId(7);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-1, 2-6,\n7-7, 8-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = id_manager2.ReserveId(8);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-1, 2-6,\n7-8, 9-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = id_manager2.ReserveId(4);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-1, 2-3,\n4-4, 5-6,\n7-8, 9-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = id_manager2.RecycleId(1);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-0, 1-3,\n4-4, 5-6,\n7-8, 9-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = id_manager2.RecycleId(4);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-0, 1-6,\n7-8, 9-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = id_manager2.RecycleId(8);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-0, 1-6,\n7-7, 8-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = id_manager2.RecycleId(7);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-0, 1-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = id_manager2.ReserveId(8);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-0, 1-7,\n8-8, 9-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = id_manager2.ReserveId(7);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-0, 1-6,\n7-8, 9-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = id_manager2.RecycleId(8);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-0, 1-6,\n7-7, 8-2147483646,\n");
		deb_assert(_test_ok);

		_test_ok = id_manager2.RecycleId(7);
		deb_assert(_test_ok);
		debug_state = id_manager2.GetDebugState();
		_test_ok = (debug_state == "1-0, 1-2147483646,\n");
		deb_assert(_test_ok);
	}

	ReportTestResult(account, "IdManager", _context, _test_ok);
	return (_test_ok);
}

bool TestTransformation(const LogDecorator& account) {
	str _context;
	bool _test_ok = true;
	str debug_state;

	if (_test_ok) {
		_context = "anchor rotate 1";
		TransformationD transformation;
		transformation.SetPosition(Vector3DD(4, 1, 1));
		transformation.RotateAroundAnchor(Vector3DD(3, 0, -0.41), Vector3DD(0, 0, 1), PIF/2);
		_test_ok = (transformation.GetPosition().GetDistance(Vector3DD(2, 1, 1)) < 0.0001);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "matrix rotate";
		const Vector3DD axis(1, -1, 0);
		RotationMatrixD matrix1;
		matrix1.RotateAroundVector(axis, PI/4);
		RotationMatrixD matrix2;
		matrix2.RotateAroundVector(axis, -PI*3/4);
		Vector3DD new_position1 = matrix1*Vector3DD(2, 2, 2);
		Vector3DD new_position2 = matrix2*Vector3DD(-2, -2, -2);
		_test_ok = (new_position1.GetDistance(new_position2) < 0.0001 &&
			new_position1.GetDistance(Vector3DD(0.4142, 0.4142, 3.4142)) < 0.0001);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "quaternion rotate";
		const Vector3DD axis(1, -1, 0);
		QuaternionD quaternion1(PI/4, axis);
		QuaternionD quaternion2(-PI*3/4, axis);
		Vector3DD new_position1 = quaternion1*Vector3DD(2, 2, 2);
		Vector3DD new_position2 = quaternion2*Vector3DD(-2, -2, -2);
		_test_ok = (new_position1.GetDistance(new_position2) < 0.0001 &&
			new_position1.GetDistance(Vector3DD(0.4142, 0.4142, 3.4142)) < 0.0001);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "quaternion multiply";
		Vector3DD x(1, 0, 0);
		QuaternionD q1(PI/2, Vector3DD(0, -1, 0));
		x = q1*x;
		_test_ok = (x.GetDistance(Vector3DD(0, 0, 1)) < 0.0001);
		deb_assert(_test_ok);
		if (_test_ok) {
			QuaternionD q2(-PI*1.5, Vector3DD(1, 0, 0));
			x = (q1*q2)*x;
			_test_ok = (x.GetDistance(Vector3DD(0, -1, 0)) < 0.0001);
			deb_assert(_test_ok);
		}
	}
	if (_test_ok) {
		_context = "anchor rotate 2";
		const Vector3DD anchor(0, -1, -1);
		TransformationD transformation1;
		transformation1.SetPosition(Vector3DD(2, 1, -1));
		const Vector3DD diff1(transformation1.GetPosition()-anchor);
		TransformationD transformation2;
		double l2Side = diff1.GetLength()/::sqrt(2.0);
		transformation2.SetPosition(Vector3DD(-l2Side, l2Side-1, -1));
		const Vector3DD diff2(transformation2.GetPosition()-anchor);
		deb_assert(::fabs(diff1.GetLength()-diff2.GetLength()) < 0.0001);

		const Vector3DD axis1(1, -1, 0);
		transformation1.RotateAroundAnchor(anchor, axis1, PI/2);
		const Vector3DD axis2(1, 1, 0);
		transformation2.RotateAroundAnchor(anchor, axis2, PI/2);
		_test_ok = (transformation1.GetPosition().GetDistance(transformation2.GetPosition()) < 0.0001 &&
			transformation1.GetPosition().GetDistance(Vector3DD(0, -1, diff1.GetLength()-1)) < 0.0001);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "quaternion rotate 2";
		Vector3DD __vector(0, 1, 0);
		QuaternionD q(PI/4, Vector3DD(0, 0, 1));
		q.RotateAroundOwnX(PI/4);
		__vector = q*__vector;
		q.SetIdentity();
		q.RotateAroundVector(Vector3DD(0, 0, 1), -PI/4);
		__vector = q*__vector;
		q.Set(PI/4, Vector3DD(1, 0, 0));
		q.MakeInverse();
		__vector = q*__vector;
		_test_ok = (__vector.GetDistance(Vector3DD(0, 1, 0)) < 0.0001);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "quaternion back&forth 1";
		const QuaternionD parent_q(0.95, Vector3DD(1, 1, 0));
		const QuaternionD child_relative_q(PI/4, Vector3DD(1, 0, 0));
		const QuaternionD child_absolute_q = child_relative_q*parent_q;
		Vector3DD __vector = child_absolute_q*Vector3DD(-1, 1, 1);
		_test_ok = (__vector.GetDistance(Vector3DD(0, -1.2, 1.2)) < 0.06);
		deb_assert(_test_ok);
		if (_test_ok) {
			__vector = (child_absolute_q/parent_q)*Vector3DD(0, 1, 0);
			_test_ok = (__vector.GetDistance(Vector3DD(0, 0.707, 0.707)) < 0.001);
			deb_assert(_test_ok);
		}
		Vector3DD euler_angles1;
		if (_test_ok) {
			(child_absolute_q/parent_q).GetEulerAngles(euler_angles1);
			_test_ok = (euler_angles1.GetDistance(Vector3DD(0, PI/4, 0)) < 2e-11);
			deb_assert(_test_ok);
		}
		if (_test_ok) {
			Vector3DD euler_angles2;
			child_relative_q.GetEulerAngles(euler_angles2);
			_test_ok = (euler_angles1.GetDistance(euler_angles1) < 2e-11);
			deb_assert(_test_ok);
		}
	}
	if (_test_ok) {
		_context = "quaternion/matrix angle 1";
		QuaternionD q;
		q.RotateAroundOwnZ(PI/3);
		q.RotateAroundOwnX(-PI/7);
		q.RotateAroundOwnY(PI/5);
		const Vector3DD ax = q.GetAxisX();
		const Vector3DD ay = q.GetAxisY();
		const Vector3DD az = q.GetAxisZ();
		RotationMatrixD matrix(ax, ay, az);
		QuaternionD target(matrix);
		_test_ok = (Math::IsEpsEqual(q.a, target.a) &&
			Math::IsEpsEqual(q.b, target.b) &&
			Math::IsEpsEqual(q.c, target.c) &&
			Math::IsEpsEqual(q.d, target.d));
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "vector spherical angles 1";
		double theta = -1;
		double phi = -1;
		const double l45 = ::sin(PI/4);
		Vector3DD(l45, l45, 1).GetSphericalAngles(theta, phi);
		_test_ok = (Math::IsEpsEqual(theta, PI/4) &&
			Math::IsEpsEqual(phi, PI/4));
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "vector spherical angles 2";
		double theta = -1;
		double phi = -1;
		const double l45 = ::sin(PI/4);
		Vector3DD(-l45, -l45, -1).GetSphericalAngles(theta, phi);
		_test_ok = (Math::IsEpsEqual(theta, PI*3/4) &&
			Math::IsEpsEqual(phi, PI*5/4));
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "vector spherical angles 3";
		double theta = -1;
		double phi = -1;
		Vector3DD(1, 0, 0).GetSphericalAngles(theta, phi);
		_test_ok = (Math::IsEpsEqual(theta, PI/2) &&
			Math::IsEpsEqual(phi, 0.0));
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "vector spherical angles 3";
		double theta = -1;
		double phi = -1;
		Vector3DD(0, 0, 1).GetSphericalAngles(theta, phi);
		_test_ok = (Math::IsEpsEqual(theta, 0.0) &&
			Math::IsEpsEqual(phi, 0.0));
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "vector spherical angles 4";
		double theta = -1;
		double phi = -1;
		Vector3DD(0, -1, -1).GetSphericalAngles(theta, phi);
		_test_ok = (Math::IsEpsEqual(theta, PI*3/4) &&
			Math::IsEpsEqual(phi, PI*6/4));
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "quaternion euler angles 1";

		double yaw = 0;
		double pitch = PI/4;
		double roll = 0;
		for (double i = 0; _test_ok && i <= PI*4; i += 0.001) {
			QuaternionD q;
			q.RotateAroundOwnZ(yaw);
			q.RotateAroundOwnX(pitch);
			q.RotateAroundOwnY(roll);
			yaw   += 0.001;
			pitch += 0.001*3;
			roll  += 0.001*5;

			Vector3DD euler;
			q.GetEulerAngles(euler);
			QuaternionD __q2;
			__q2.SetEulerAngles(euler);
			const Vector3DD test(1, 1, 1);
			Vector3DD test1 = test*q;
			Vector3DD test2 = test*__q2;
			double distance = test1.GetDistance(test2);
			_test_ok = (Math::IsEpsEqual(distance, 0.0, 2e-11));
			deb_assert(_test_ok);
		}
	}

	ReportTestResult(account, "Transformation", _context, _test_ok);
	return (_test_ok);
}

bool TestTimers(const LogDecorator& account) {
	str _context;
	bool _test_ok = true;

	// Basic timer test.
	if (_test_ok) {
		_context = "timer";
		Thread::Sleep(0.001);	// Bail early on cpu time slice.
		HiResTimer hi_timer(false);
		Timer lo_timer;
		Thread::Sleep(0.100);
		const double hi_time = hi_timer.QueryTimeDiff();
		const double lo_time = lo_timer.QueryTimeDiff();
		_test_ok = (hi_time > 0.090 && hi_time < 0.150 &&
			lo_time > 0.090 && lo_time < 0.150);
		deb_assert(_test_ok);
	}

	ReportTestResult(account, "Timers", _context, _test_ok);
	return (_test_ok);
}

bool TestSystemManager(const LogDecorator& account) {
	str _context;
	bool _test_ok = true;

	// Directory test.
	if (_test_ok) {
		_context = "root directory";
		str root_dir = SystemManager::GetRootDirectory();

#if defined(LEPRA_WINDOWS)
		// Hugge: This test will fail if this app runs from another partition.
		// _test_ok = (root_dir =="C:/");
		str drive_letter = root_dir.substr(0, 1);
		str rest = root_dir.substr(1);
		_test_ok = drive_letter >= "A" &&
				   drive_letter <= "Z" &&
				   rest == ":/";
#elif defined(LEPRA_POSIX)
		_test_ok = (root_dir == "/");
#else // <Unknown target>
#error "Unknown target!"
#endif // LEPRA_WINDOWS/LEPRA_POSIX/<Unknown target>
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "current/documents directory";
		_test_ok = (SystemManager::GetCurrentDirectory().length() >= 4 &&
			SystemManager::GetUserDirectory().length() >= 4);
		deb_assert(_test_ok);
	}

	// Platform/user test.
	if (_test_ok) {
		_context = "Os name";
		str os = SystemManager::GetOsName();
#if defined(LEPRA_WINDOWS)
		_test_ok = (os == "Windows NT");
#elif defined(LEPRA_POSIX)
		_test_ok = (os == "Darwin" || os == "Linux");
#else // <Unknown target>
#error "Not implemented for this platform!"
#endif // LEPRA_WINDOWS/LEPRA_POSIX/<Unknown target>
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "login name";
		_test_ok = (SystemManager::GetLoginName().length() >= 1);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "user name";
		_test_ok = (SystemManager::QueryFullUserName().length() >= 1);
		deb_assert(_test_ok);
	}

	// Cpu test.
	if (_test_ok) {
		_context = "physical CPU count";
		_test_ok = (SystemManager::GetPhysicalCpuCount() >= 1 && SystemManager::GetPhysicalCpuCount() <= 4);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "logical CPU count";
		_test_ok = (SystemManager::GetLogicalCpuCount() >= 1 && SystemManager::GetLogicalCpuCount() <= 16);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "CPU core count";
		_test_ok = (SystemManager::GetCoreCount() >= 1 && SystemManager::GetCoreCount() <= 4);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "Cpu type";
		str cpu_name = SystemManager::GetCpuName();
		_test_ok = (cpu_name == "GenuineIntel" || cpu_name == "AuthenticAMD" ||
			cpu_name == "x64" || cpu_name == "x86" || cpu_name == "PowerPC");
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "Cpu frequency";
		uint64 cpu_frequency = SystemManager::QueryCpuFrequency();
		_test_ok = (cpu_frequency >= 700*1000*1000 &&
			cpu_frequency <= (uint64)6*1000*1000*1000);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "Cpu MIPS";
		unsigned cpu_mips = SystemManager::QueryCpuMips();
		_test_ok = (cpu_mips >= 50 && cpu_mips <= 3500);
		deb_assert(_test_ok);
	}

	// Memory test.
	if (_test_ok) {
		_context = "Ram size";
		_test_ok = (SystemManager::GetAmountRam() >= 64*1024*1024 &&
			SystemManager::GetAmountRam() <= (uint64)4*1024*1024*1024);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "available Ram >= total Ram";
		_test_ok = (SystemManager::GetAvailRam() <= SystemManager::GetAmountRam());
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "available Ram size";
		_test_ok = (SystemManager::GetAvailRam() >= 30*1024*1024 &&
			SystemManager::GetAvailRam() <= (uint64)4*1024*1024*1024);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "virtual memory size";
		_test_ok = (SystemManager::GetAmountVirtualMemory() >= 150*1024*1024 &&
			SystemManager::GetAmountVirtualMemory() <= (uint64)20*1024*1024*1024);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "available virtual memory > total virtual memory";
		_test_ok = (SystemManager::GetAvailVirtualMemory() <= SystemManager::GetAmountVirtualMemory());
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "available virtual memory size";
		_test_ok = (SystemManager::GetAvailVirtualMemory() >= 100*1024*1024 &&
			SystemManager::GetAvailVirtualMemory() <= (uint64)19*1024*1024*1024);
		deb_assert(_test_ok);
	}
#ifdef TEST_WEB_AND_MAIL
	if (_test_ok) {
		// Just make sure we don't crash. Need manual verification that it works anyhoo.
		SystemManager::WebBrowseTo("http://trialepicfail.blogspot.com/");
	}
	if (_test_ok) {
		// Just make sure we don't crash. Need manual verification that it works anyhoo.
		SystemManager::EmailTo("info@pixeldoctrine.com", "Test subject?", "Hi,\n\nHow are you? Hope you're good!\n\nLater,\nJonas");
	}
#endif // !TEST_WEB_AND_MAIL

	ReportTestResult(account, "System", _context, _test_ok);
	return (_test_ok);
}

bool TestNetwork(const LogDecorator& account) {
	str _context;
	bool _test_ok = true;

	if (_test_ok) {
		_context = "network start";
		_test_ok = Network::Start();
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "local resolve";
		SocketAddress address;
		_test_ok = address.Resolve(":1024");
		deb_assert(_test_ok);
		if (_test_ok) {
			const str local_address = address.GetAsString();
			_test_ok = ((strutil::StartsWith(local_address, "127.0.") &&
				strutil::EndsWith(local_address, ".1:1024")) ||
				(strutil::StartsWith(local_address, "192.168.") &&
				strutil::EndsWith(local_address, ":1024")));
			deb_assert(_test_ok);
		}
	}
	if (_test_ok) {
		_context = "DNS resolve";
		IPAddress address;
		_test_ok = Network::ResolveHostname("ftp.sunet.se", address);
		deb_assert(_test_ok);
	}

	if (_test_ok) {
		_context = "UDP exclusive bind";
		SocketAddress address;
		address.Resolve(":1025");
		UdpSocket socket1(address, true);
		UdpSocket socket2(address, true);
		_test_ok = (socket1.IsOpen() && !socket2.IsOpen());
		deb_assert(_test_ok);
	}

	if (_test_ok) {
		_context = "UDP send";
		SocketAddress receive_address;
		receive_address.Resolve(":47346");
		SocketAddress send_address;
		send_address.Resolve(":47347");
		UdpSocket receiver(receive_address, true);
		deb_assert(receiver.IsOpen());
		UdpSocket sender(send_address, true);
		deb_assert(sender.IsOpen());
		_test_ok = (sender.SendTo((const uint8*)"Hello World", 12, receive_address) == 12);
		deb_assert(_test_ok);
		if (_test_ok) {
			_context = "UDP receive";
			uint8 message[12];
			SocketAddress source_address;
			int recv_count = receiver.ReceiveFrom(message, sizeof(message), source_address);
			_test_ok = (recv_count == 12 && ::strncmp("Hello World", (const char*)message, 12) == 0 &&
				source_address.GetIP() == send_address.GetIP() && source_address.GetPort() == send_address.GetPort());
			deb_assert(_test_ok);
		}
	}

	if (_test_ok) {
		_context = "TCP server init";
		SocketAddress receive_address;
		receive_address.Resolve(":47346");
		SocketAddress send_address;
		send_address.Resolve(":47347");
		TcpListenerSocket _server(receive_address, true);
		_test_ok = _server.IsOpen();
		deb_assert(_test_ok);

		class DummyAcceptor: public Thread {
		public:
			DummyAcceptor(TcpListenerSocket* server_socket):
				Thread("DummyAcceptor"),
				server_socket_(server_socket),
				connect_socket_(0) {
			}
			void Run() {
				connect_socket_ = server_socket_->Accept();
				deb_assert(connect_socket_);
			}
			TcpListenerSocket* server_socket_;
			TcpSocket* connect_socket_;
		};
		DummyAcceptor* acceptor = 0;
		if (_test_ok) {
			_context = "TCP acceptor start";
			acceptor = new DummyAcceptor(&_server);
			_test_ok = acceptor->Start();
			deb_assert(_test_ok);
		}
		TcpSocket sender(send_address);
		if (_test_ok) {
			_context = "TCP connect";
			_test_ok = false;
			for (int x = 0; x < 3 && !_test_ok; ++x) {
				Thread::Sleep(0.01);
				_test_ok	= sender.Connect(receive_address);
			}
			deb_assert(_test_ok);
		}
		TcpSocket* receiver = 0;
		if (_test_ok) {
			_context = "TCP accept";
			for (int x = 0; !receiver && x < 300; ++x) {
				receiver = acceptor->connect_socket_;
				if (!receiver) {
					Thread::Sleep(0.001);
				}
			}
			_test_ok = (receiver != 0);
			deb_assert(_test_ok);
		}
		const int packet_count = 300;
		const int packet_byte_count = 100;
		if (_test_ok) {
			_context = "TCP overflow sends";
			for (int x = 0; _test_ok && x < packet_count; ++x) {
				uint8 value[packet_byte_count];
				for (int y = 0; y < packet_byte_count; ++y) {
					value[y] = (uint8)x;
				}
				_test_ok = (sender.Send(value, packet_byte_count) == packet_byte_count);
				deb_assert(_test_ok);
			}
		}
		if (_test_ok) {
			_context = "TCP underflow receive";
			Thread::Sleep(0.2f);	// On POSIX, it actually takes some time to pass through firewall and stuff.
			for (int x = 0; _test_ok && x < packet_count/10; ++x) {
				const int read_size = packet_byte_count*10;
				uint8 value[read_size];
				int actual_read_size = receiver->Receive(value, read_size);
				_test_ok = (actual_read_size == read_size);
				deb_assert(_test_ok);
				if (_test_ok) {
					for (int y = 0; _test_ok && y < read_size; ++y) {
						int z = x*10+y/packet_byte_count;
						_test_ok = (value[y] == (uint8)z);
						deb_assert(_test_ok);
					}
				}
			}
		}
		delete (acceptor);

		if (_test_ok) {
			_context = "TCP algo receiver";
			sender.Send("A1B2C3", 6);
			class TestReceiver: public DatagramReceiver {
				int Receive(TcpSocket* _socket, void* buffer, int) {
					static int x = 6;
					x -= 2;
					return (_socket->Receive(buffer, x));
				}
			};
			TestReceiver algo;
			receiver->SetDatagramReceiver(&algo);
			char _buffer[6] = {0, 1, 2, 3, 4, 5};
			int receive_count = receiver->ReceiveDatagram(_buffer, 6);
			_test_ok = (receive_count == 4 && _buffer[0] == 'A' && _buffer[3] == '2');
			deb_assert(_test_ok);
			if (_test_ok) {
				receive_count = receiver->ReceiveDatagram(_buffer, 6);
				_test_ok = (receive_count == 2 && _buffer[0] == 'C' && _buffer[1] == '3');
				deb_assert(_test_ok);
			}
		}
	}

	if (_test_ok) {
		_context = "network stop";
		_test_ok = Network::Stop();
		deb_assert(_test_ok);
	}

	ReportTestResult(account, "Network", _context, _test_ok);
	return (_test_ok);
}

bool TestTcpMuxSocket(const LogDecorator& account) {
	str _context;
	bool _test_ok = Network::Start();

	//uint16 lAcceptPort = (uint16)Random::Uniform(40000, 50000);
	SocketAddress accept_address;
	TcpMuxSocket* accept_socket = 0;
	if (_test_ok) {
		_context = "resolving acceptor";
		_test_ok = accept_address.Resolve(":46767");
		//accept_address.SetPort(lAcceptPort);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "opening TCP MUX";
		accept_socket = new TcpMuxSocket("T1", accept_address, true);
		accept_socket->SetConnectIdTimeout(5.0);
		_test_ok = accept_socket->IsOpen();
		deb_assert(_test_ok);
	}
	//uint16 lConnectPort = (uint16)Random::Uniform(40000, 50000);
	SocketAddress connector_address;
	TcpSocket* connect_socket = 0;
	if (_test_ok) {
		_context = "resolving connector";
		_test_ok = connector_address.Resolve(":47676");
		//connector_address.SetPort(lConnectPort);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "creating TCP";
		connect_socket = new TcpSocket(connector_address);
		_test_ok = connect_socket->IsOpen();
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "connecting TCP";
		_test_ok = false;
		for (int x = 0; !_test_ok && x < 1000; ++x) {
			_test_ok = connect_socket->Connect(accept_address);
			Thread::Sleep(0.001);
		}
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "verifying TCP MUX connection count";
		unsigned connection_count = 0;
		for (int x = 0; connection_count == 0 && x < 500; ++x) {
			connection_count = accept_socket->GetConnectionCount();
			Thread::Sleep(0.001f);
		}
		_test_ok = (connection_count == 1);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "dropping non-V TCP connect";
		accept_socket->SetConnectIdTimeout(0.01);
		_test_ok = false;
		for (int x = 0; x < 3 && !_test_ok; ++x) {
			Thread::Sleep(0.02);
			accept_socket->PollAccept();
			unsigned connection_count = accept_socket->GetConnectionCount();
			_test_ok = (connection_count == 0);
		}
		deb_assert(_test_ok);
	}

	if (connect_socket) {
		delete (connect_socket);
		connect_socket = 0;
	}

	if (accept_socket) {
		delete (accept_socket);
		accept_socket = 0;
	}

	Network::Stop();

	ReportTestResult(account, "TcpMuxSocket", _context, _test_ok);
	return (_test_ok);
}

class DualSocketClientTest {
public:
	bool Test();

private:
	template<class _Server> bool TestClientServerTransmit(str& context, _Server& server,
		DualMuxSocket& client_mux_socket, DualSocket* client_socket, bool safe);

	logclass();
};

template<class _MuxSocket, class _VSocket> class ServerSocketHandler: public Thread {
public:
	typedef _VSocket VSocket;
	typedef _VSocket* (_MuxSocket::*_AcceptMethod)();
	ServerSocketHandler(const str& name, _MuxSocket& server_socket, _AcceptMethod accept_method, bool loop):
		Thread(name),
		server_socket_(0),
		server_mux_socket_(server_socket),
		accept_method_(accept_method),
		loop_(loop) {
		bool started = Start();
		if (!started) {
			deb_assert(false);
		}
	}
	~ServerSocketHandler() {
		CloseSocket();
	}
	void CloseSocket() {
		if (server_socket_) {
			server_mux_socket_.CloseSocket(server_socket_);
			server_socket_ = 0;
		}
	}
	_VSocket* server_socket_;
	_MuxSocket& server_mux_socket_;
private:
	void Run() {
		do {
			do {
				server_socket_ = (server_mux_socket_.*accept_method_)();
			} while (loop_ && !server_socket_ && !GetStopRequest());
			while (loop_ && server_socket_ && !GetStopRequest()) {
				Thread::Sleep(0.01);
			}
		} while (loop_ && !GetStopRequest());
	}
	_AcceptMethod accept_method_;
	bool loop_;
	void operator=(const ServerSocketHandler&) {};
};

bool DualSocketClientTest::Test() {
	str _context;
	bool _test_ok = true;

	if (_test_ok) {
		_context = "network startup error";
		_test_ok = Network::Start();
		deb_assert(_test_ok);
	}

	SocketAddress local_address;
	if (_test_ok) {
		_context = "address resolve";
		_test_ok = local_address.Resolve(":1025");
		deb_assert(_test_ok);
	}

	// Create client.
	SocketAddress client_address(local_address);
	client_address.SetPort(55113);
	DualMuxSocket _client_mux_socket("Client ", client_address, false);
	if (_test_ok) {
		_context = "client socket open";
		_test_ok = _client_mux_socket.IsOpen();
		deb_assert(_test_ok);
	}

	// Make sure server connect fails (server not up yet).
	SocketAddress server_address(local_address);
	server_address.SetPort(55112);
	DualSocket* _client_socket = 0;
	if (_test_ok) {
		log_.Headline("Connect without TCP+UDP.");
		_context = "client forced invalid connect";
		const std::string id = SystemManager::GetRandomId();
		_client_socket = _client_mux_socket.Connect(server_address, id, 0.5);
		_test_ok = (_client_socket == 0);
		deb_assert(_test_ok);
	}

	// Create and start TCP server (connect should fail if not UDP is up).
	{
		log_.Headline("Connect without UDP.");
		TcpMuxSocket* server_tcp_mux_socket = new TcpMuxSocket("Srv ", server_address, true);
		if (_test_ok) {
			_context = "server TCP socket open";
			_test_ok = server_tcp_mux_socket->IsOpen();
			deb_assert(_test_ok);
		}
		ServerSocketHandler<TcpMuxSocket, TcpVSocket>* _server =
			new ServerSocketHandler<TcpMuxSocket, TcpVSocket>(
				"TcpDummyServerSocket", *server_tcp_mux_socket, &TcpMuxSocket::Accept, false);
		if (_test_ok) {
			_context = "client connected without UDP";
			const std::string id = SystemManager::GetRandomId();
			_client_socket = _client_mux_socket.Connect(server_address, id, 1.0);
			_test_ok = (_client_socket == 0);
			deb_assert(_test_ok);
		}
		if (_test_ok) {
			_context = "server did not drop client TCP connection in time";
			Thread::Sleep(0.01);
			TcpVSocket* connector_socket = server_tcp_mux_socket->PopReceiverSocket();
			if (connector_socket != 0) {	// Already dropped?
				char a[1];
				_test_ok = (connector_socket->Receive(a, 1, false) < 0);
				deb_assert(_test_ok);
			}
		}
		delete (_server);	// Must delete manually, due to dependency on scope MUX socket.
		delete (server_tcp_mux_socket);
	}

	// Create and start UDP server (connect should fail if not TCP is up).
	log_.Headline("Connect without TCP.");
	UdpMuxSocket server_udp_mux_socket("Srv ", server_address, true);
	if (_test_ok) {
		_context = "server UDP socket open";
		_test_ok = server_udp_mux_socket.IsOpen();
		deb_assert(_test_ok);
	}
	ServerSocketHandler<UdpMuxSocket, UdpVSocket>* udp_server =
		new ServerSocketHandler<UdpMuxSocket, UdpVSocket>(
			"UDP Server", server_udp_mux_socket, &UdpMuxSocket::Accept, true);
	if (_test_ok) {
		_context = "client connected without TCP";
		const std::string id = SystemManager::GetRandomId();
		_client_socket = _client_mux_socket.Connect(server_address, id, 0.5);
		_test_ok = (_client_socket == 0);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "UDP server did not accept MUX connection";
		_test_ok = (server_udp_mux_socket.GetConnectionCount() == 1);
		deb_assert(_test_ok);
		udp_server->CloseSocket();
	}
	if (_test_ok) {
		_context = "server did not drop client UDP connection";
		_test_ok = (server_udp_mux_socket.GetConnectionCount() == 0);
		deb_assert(_test_ok);
	}

	// With both TCP and UDP sockets setup, connect should pull through.
	log_.Headline("Connect for real.");
	TcpMuxSocket server_tcp_mux_socket("Srv ", server_address, true);
	if (_test_ok) {
		_context = "server TCP socket open";
		_test_ok = server_tcp_mux_socket.IsOpen();
		deb_assert(_test_ok);
	}
	ServerSocketHandler<TcpMuxSocket, TcpVSocket>* tcp_server =
		new ServerSocketHandler<TcpMuxSocket, TcpVSocket>(
			"TCP Server", server_tcp_mux_socket, &TcpMuxSocket::Accept, true);
	if (_test_ok) {
		_context = "client TCP+UDP connect";
		Thread::Sleep(0.2);
		const std::string id = SystemManager::GetRandomId();
		_client_socket = _client_mux_socket.Connect(server_address, id, 2.0);
		_test_ok = (_client_socket != 0);
		deb_assert(_test_ok);
	}

	if (_test_ok) {
		_test_ok = TestClientServerTransmit(_context, *udp_server, _client_mux_socket, _client_socket, false);
	}

	if (_test_ok) {
		_test_ok = TestClientServerTransmit(_context, *tcp_server, _client_mux_socket, _client_socket, true);
	}

	delete (udp_server);	// Must delete manually, due to dependency on scope MUX socket.
	delete (tcp_server);	// Must delete manually, due to dependency on scope MUX socket.

	if (_test_ok) {
		_context = "network shutdown error";
		_test_ok = Network::Stop();
		deb_assert(_test_ok);
	}

	ReportTestResult(log_, "GameClientSocket", _context, _test_ok);
	return (_test_ok);
}

template<class _Server> bool DualSocketClientTest::TestClientServerTransmit(str& context, _Server& server,
		DualMuxSocket& client_mux_socket, DualSocket* client_socket, bool safe) {
	bool _test_ok = true;
	if (_test_ok) {
		context = "server append send";
		IOError error = server.server_socket_->AppendSendBuffer("Hi", 3);
		_test_ok = (error == kIoOk);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		context = "server pop sender";
		typename _Server::VSocket* __socket = server.server_mux_socket_.PopSenderSocket();
		_test_ok = (__socket == server.server_socket_);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		context = "server send";
		int __length = server.server_socket_->SendBuffer();
		_test_ok = (__length == 3);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		context = "client pop receiver";
		DualSocket* __socket = 0;
		for (int x = 0; __socket == 0 && x < 500; ++x) {
			__socket = client_mux_socket.PopReceiverSocket(safe);
			Thread::Sleep(0.001f);
		}
		_test_ok = (__socket == client_socket);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		context = "client receive";
		uint8 _buffer[16];
		int __length = client_socket->Receive(safe, _buffer, 16);
		_test_ok = (__length == 3 && memcmp("Hi", _buffer, 3) == 0);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		context = "client append send";
		IOError error = client_socket->AppendSendBuffer(safe, "Hi", 3);
		_test_ok = (error == kIoOk);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		context = "client pop sender";
		DualSocket* __socket = client_mux_socket.PopSenderSocket();
		_test_ok = (__socket == client_socket);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		context = "client send";
		int __length = client_socket->SendBuffer();
		_test_ok = (__length == 3);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		context = "server pop receiver";
		typename _Server::VSocket* __socket = 0;
		for (int x = 0; __socket == 0 && x < 500; ++x) {
			__socket = server.server_mux_socket_.PopReceiverSocket();
			Thread::Sleep(0.001f);
		}
		_test_ok = (__socket != 0 && __socket == server.server_socket_);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		context = "server receive";
		uint8 _buffer[16];
		int __length = server.server_socket_->Receive(_buffer, 16);
		_test_ok = (__length == 3 && memcmp("Hi", _buffer, 3) == 0);
		deb_assert(_test_ok);
	}
	return (_test_ok);
}



class DualSocketServerTest {
public:
	bool Test();

	logclass();
};

bool DualSocketServerTest::Test() {
	str _context;
	bool _test_ok = true;

	if (_test_ok) {
		_context = "network startup error";
		_test_ok = Network::Start();
		deb_assert(_test_ok);
	}

	SocketAddress local_address;
	if (_test_ok) {
		_context = "address resolve";
		_test_ok = local_address.Resolve(":1025");
		deb_assert(_test_ok);
	}

	// Create server.
	SocketAddress server_address(local_address);
	server_address.SetPort(55113);
	DualMuxSocket server_mux_socket("Server ", server_address, true);
	server_mux_socket.SetConnectDualTimeout(0.5f);
	if (_test_ok) {
		_context = "server socket open";
		_test_ok = server_mux_socket.IsOpen();
		deb_assert(_test_ok);
	}
	class ServerSocketHandler: public Thread {
	public:
		ServerSocketHandler(DualMuxSocket& server_socket):
			Thread("GameServerSocket"),
			server_mux_socket_(server_socket) {
			bool started = Start();
			if (!started) {
				deb_assert(false);
			}
		}
	private:
		void Run() {
			while (!GetStopRequest() && server_mux_socket_.PollAccept() == 0) {
				Thread::Sleep(0.001);
			}
		}
		void operator=(const ServerSocketHandler&) {};
		DualMuxSocket& server_mux_socket_;
	};

	// Check one-sided client connect.
	SocketAddress client_address(local_address);
	client_address.SetPort(55112);
	TcpMuxSocket client_tcp_socket("Client", client_address, false);
	if (_test_ok) {
		_context = "client TCP connect failed";
		ServerSocketHandler handler(server_mux_socket);
		_test_ok = (client_tcp_socket.Connect(server_address, "Hejsan", 0.5) != 0);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "server didn't let TCP connect through";
		for (int x = 0; server_mux_socket.GetConnectionCount() == 0 && x < 500; ++x) {
			Thread::Sleep(0.001);
		}
		_test_ok = (server_mux_socket.GetConnectionCount() == 1);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "server let TCP-only connect stay";
		ServerSocketHandler handler(server_mux_socket);
		for (int x = 0; server_mux_socket.GetConnectionCount() == 1 && x < 1000; ++x) {
			Thread::Sleep(0.001);
		}
		_test_ok = (server_mux_socket.GetConnectionCount() == 0);
		deb_assert(_test_ok);
	}


	if (_test_ok) {
		_context = "network shutdown error";
		_test_ok = Network::Stop();
		deb_assert(_test_ok);
	}

	ReportTestResult(log_, "GameServerSocket", _context, _test_ok);
	return (_test_ok);
}

loginstance(kTest, DualSocketClientTest);
loginstance(kTest, DualSocketServerTest);



bool TestArchive(const LogDecorator& account) {
	str _context;
	bool _test_ok = true;

	if (_test_ok) {
		// TODO: implement!
		_context = "?";
		_test_ok = true;
		deb_assert(_test_ok);
	}

	ReportTestResult(account, "Archive", _context, _test_ok);
	return (_test_ok);
}
/*
bool TestFFT(const LogDecorator& account) {
	str _context;
	bool _test_ok = true;

	enum {
		NUM_POINTS = 256
	};

	FFT lFFT(NUM_POINTS, NUM_POINTS);
	float32 lSignal[NUM_POINTS];

	if (_test_ok) {
		_context = "FFT Direct Current failed.";

		// Generate a direct current signal.
		int i;
		for (i = 0; i < NUM_POINTS; i++) {
			lSignal[i] = 1.234f;
		}

		lFFT.Transform(lSignal, NUM_POINTS);

		// Check if it's close to what we expected...
		float32 lAmp = (float32)lFFT.GetAmp(0);
		_test_ok = abs(lAmp - 1.234f) < 1e-5f;

		if (_test_ok) {
			// The rest should be zero.
			for (i = 1; _test_ok && i < NUM_POINTS; i++) {
				_test_ok = abs(lFFT.GetAmp(i)) < 1e-5f;
			}
		}

		deb_assert(_test_ok);
	}

	if (_test_ok) {
		_context = "FFT single sine failed.";

		// Generate a cosine wave signal.
		int i;
		for (i = 0; i < NUM_POINTS; i++) {
			lSignal[i] = (float32)(cos(2.0 * PI * (float64)i / (float64)NUM_POINTS + 0.4f) * 1.234);
		}

		lFFT.Transform(lSignal, NUM_POINTS);

		// Check if near...
		float32 lAmp = (float32)(lFFT.GetAmp(1) + lFFT.GetAmp(-1));
		_test_ok = abs(lAmp - 1.234f) < 1e-5f;

		if (_test_ok) {
			float32 lPhase1 = (float32)lFFT.GetPhase(1);
			float32 lPhase2 = (float32)lFFT.GetPhase(-1);
			_test_ok = (abs(lPhase1 - 0.4f) < 1e-5f) &&
					(abs(lPhase2 + 0.4f) < 1e-5f);
		}

		if (_test_ok) {
			// The rest should be zero.
			for (i = -NUM_POINTS / 2; i < NUM_POINTS / 2; i++) {
				if (i != 1 && i != -1) {
					lAmp = (float32)lFFT.GetAmp(i);
					_test_ok = abs(lAmp) < 1e-5f;
				}
			}
		}

		deb_assert(_test_ok);
	}

	ReportTestResult(account, "FFT", _context, _test_ok);
	return (_test_ok);
}
*/

bool TestCrypto(const LogDecorator& account) {
	str _context;
	bool _test_ok = true;

	if (_test_ok) {
		_context = "DES encrypt failed.";
		DES des;
		des.SetKey(LEPRA_ULONGLONG(0x1234567890FEDABC));
		const char s[16] = "Hello World!!!!";
		uint8 data[16];
		::memcpy(data, s, sizeof(data));
		des.Encrypt(data, 16);
		_test_ok = (::memcmp(data, s, sizeof(data)) != 0);
		deb_assert(_test_ok);
		if (_test_ok) {
			_context = "DES decrypt failed.";
			des.Decrypt(data, 16);
			_test_ok = (::memcmp(data, s, sizeof(data)) == 0);
			deb_assert(_test_ok);
		}
	}

	if (_test_ok) {
		_context = "RSA encrypt failed.";
		// TODO: implement!
//		RSA lRSA;
//		des.SetKey(0x1234567890FEDABC);
//		const char* s = "Hello World 123!";
//		uint8 data[17];
//		::memcpy(data, s, sizeof(data));
//		des.Encrypt(data, 17);
//		_test_ok = (::memcmp(data, s, sizeof(data)) != 0);
//		deb_assert(_test_ok);
//		if (_test_ok)
//		{
//			_context = "RSA decrypt failed.";
//			des.Decrypt(data, 17);
//			_test_ok = (::memcmp(data, s, sizeof(data)) == 0);
//			deb_assert(_test_ok);
//		}
	}

	if (_test_ok) {
		_context = "SHA-1 hashing failed.";
		SHA1 sh_a1;
		uint8 hash[20];
		sh_a1.Hash((const uint8*)"Hello World!", 12, hash);
		uint8 wanted[20] = { 0x2e,0xf7,0xbd,0xe6,0x08,0xce,0x54,0x04,0xe9,0x7d,0x5f,0x04,0x2f,0x95,0xf8,0x9f,0x1c,0x23,0x28,0x71 };
		_test_ok = (::memcmp(hash, wanted, sizeof(hash)) == 0);
		deb_assert(_test_ok);
	}

	ReportTestResult(account, "Crypto", _context, _test_ok);
	return (_test_ok);
}

void DummyThread(void*) {
}

Lock* g_mutex;
Condition* g_condition;

void ConditionThread(void*) {
	g_mutex->Acquire();
	g_condition->Wait();
	g_mutex->Release();
	Thread::Sleep(0.1);
}

bool TestPerformance(const LogDecorator& account) {
	str _context;
	bool _test_ok = true;

	Network::Start();

	// Loop twice: first time to register nodes in
	// the performance tree, second time to do the
	// actual measuring with high accuracy.
	for (unsigned z = 0; z < 2; ++z) {
		if (z == 1) {	// Reset time from first loop (only measure time second loop).
			ScopePerformanceData::ResetAll();
			Thread::Sleep(0.3);	// Try to make the scheduler rank us high.
		}

		{
			LEPRA_MEASURE_SCOPE(Cpucategory);

			{
				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(NOP1);
				{
					LEPRA_MEASURE_SCOPE(NOP2);
				}
			}

			{
				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(forx0to1000);
				int y = 7;
				for (int x = 0; x < 1000; ++x) {
					y += x;
					--y;
				}
			}

			{
				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(String);
				str s("Apa");
				s += "Esau";
				s.rfind("e", s.length()-1);
			}

//			{
//				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
//				LEPRA_MEASURE_SCOPE(BigInt);
//				BigInt lBigInt("84879234798733231872345687123101");
//				lBigInt >>= 8;
//				lBigInt.Sqrt();
//			}
		}

		{
			LEPRA_MEASURE_SCOPE(Threadcategory);

			{
				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(Lock);
				Lock mutex;
				{
					LEPRA_MEASURE_SCOPE(Acquire);
					mutex.Acquire();
				}
				{
					LEPRA_MEASURE_SCOPE(Release);
					mutex.Release();
				}
			}

#ifdef LEPRA_WINDOWS
			{
				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(CRITICAL_SECTION);
				CRITICAL_SECTION section;
				::InitializeCriticalSection(&section);
				{
					LEPRA_MEASURE_SCOPE(Enter);
					::EnterCriticalSection(&section);
				}
				{
					LEPRA_MEASURE_SCOPE(Leave);
					::LeaveCriticalSection(&section);
				}
				::DeleteCriticalSection(&section);
			}
#endif // LEPRA_WINDOWS

#ifdef LEPRA_WINDOWS
			{
				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(SpinLock);
				SpinLock lock;
				{
					LEPRA_MEASURE_SCOPE(Acquire);
					lock.Acquire();
				}
				{
					LEPRA_MEASURE_SCOPE(Release);
					lock.Release();
				}
			}
#endif // LEPRA_WINDOWS

			{
				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(Semaphore);
				Semaphore _semaphore;
				{
					LEPRA_MEASURE_SCOPE(Signal);
					_semaphore.Signal();
				}
				{
					LEPRA_MEASURE_SCOPE(Wait);
					_semaphore.Wait();
				}
			}

			{
				StaticThread thread("Condition");
				Lock mutex;
				Condition condition(&mutex);
				g_mutex = &mutex;
				g_condition = &condition;
				thread.Start(ConditionThread, 0);
				Thread::Sleep(0.1);	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(Condition);
				{
					LEPRA_MEASURE_SCOPE(Signal);
					condition.Signal();
				}
				{
					LEPRA_MEASURE_SCOPE(SignalAll);
					condition.SignalAll();
				}
				{
					LEPRA_MEASURE_SCOPE(Wait(0));
					mutex.Acquire();
					condition.Wait(0);
					mutex.Release();
				}
				thread.Join();	// Ensure thread terminates before condition is destroyed.
			}

			{
				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(StaticThread);
				StaticThread thread("PerformanceTest");
				{
					LEPRA_MEASURE_SCOPE(Start);
					thread.Start(DummyThread, 0);
				}
				{
					LEPRA_MEASURE_SCOPE(Join);
					thread.Join();
				}
				deb_assert(!thread.IsRunning());
			}
		}

		{
			LEPRA_MEASURE_SCOPE(Networkcategory);

			{
				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(sys_socket(UDP));
				sys_socket fd = ::socket(PF_INET, SOCK_DGRAM, 0);
				deb_assert(fd > 0);
				sockaddr_in sa;
				::memset(&sa, 0, sizeof(sa));
				sa.sin_family = AF_INET;
				char hostname[256] = "";
				::gethostname(hostname, sizeof(hostname));
				sa.sin_addr.s_addr = *(unsigned int*)(gethostbyname(hostname)->h_addr_list[0]);
				sa.sin_port = Endian::HostToBig((uint16)46666);
				_test_ok = (::bind(fd, (sockaddr*)&sa, sizeof(sa)) >= 0);
				deb_assert(_test_ok);
				{
					LEPRA_MEASURE_SCOPE(sendto);
					::sendto(fd, "Hello World", 12, 0, (sockaddr*)&sa, sizeof(sa));
				}
				{
					LEPRA_MEASURE_SCOPE(recvfrom);
					char buf[12] = "";
					socklen_t fromlen = sizeof(sa);
					::recvfrom(fd, buf, 12, 0, (sockaddr*)&sa, &fromlen);
					deb_assert(::strcmp(buf, "Hello World") == 0);
				}
#ifdef LEPRA_WINDOWS
				::closesocket(fd);
#else // !LEPRA_WINDOWS
				::close(fd);
#endif // LEPRA_WINDOWS/!LEPRA_WINDOWS
			}

			{
				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(UdpSocket);
				SocketAddress address;
				address.Resolve(":46666");
				UdpSocket __socket(address, true);
				{
					LEPRA_MEASURE_SCOPE(SendTo);
					__socket.SendTo((const uint8*)"Hello World", 12, address);
				}
				{
					LEPRA_MEASURE_SCOPE(ReceiveFrom);
					char _buffer[12] = "";
					__socket.ReceiveFrom((uint8*)_buffer, 12, address);
					deb_assert(::strcmp(_buffer, "Hello World") == 0);
				}
			}

			{
				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(UdpMuxSocket);
				SocketAddress address1;
				address1.Resolve(":46666");
				SocketAddress address2;
				address2.Resolve(":46667");
				UdpVSocket* __socket = 0;
				UdpMuxSocket mux_socket1("#1 ", address1, false);
				UdpMuxSocket mux_socket2("#2 ", address2, true);
				class DummyAcceptor: public Thread {
				public:
					DummyAcceptor(UdpMuxSocket* _socket):
						Thread("DummyAcceptor"),
						socket_(_socket) {
					}
				private:
					void Run() {
						UdpVSocket* __socket = socket_->Accept();
						deb_assert(__socket);
						IOError io = __socket->AppendSendBuffer("Hello Client", 13);
						deb_assert(io == kIoOk);
						if (io == kIoOk) {}	// TRICKY.
						int r = __socket->SendBuffer();
						deb_assert(r == 13);
						if (r == 13) {}	// TRICKY.
						__socket->WaitAvailable(0.5);
						char _buffer[13] = "";
						__socket->ReadRaw(_buffer, 13);
						bool _test_ok = (::strcmp(_buffer, "Hello Server") == 0);
						deb_assert(_test_ok);
						if (_test_ok) {}	// TRICKY.
						socket_->CloseSocket(__socket);
					}
					UdpMuxSocket* socket_;
				};
				DummyAcceptor acceptor(&mux_socket2);
				acceptor.Start();
				{
					LEPRA_MEASURE_SCOPE(Connect);
					__socket = mux_socket1.Connect(address2, "", 0.5);
					deb_assert(__socket);
				}
				{
					LEPRA_MEASURE_SCOPE(WriteRaw+Flush);
					IOError io = __socket->AppendSendBuffer("Hello Server", 13);
					deb_assert(io == kIoOk);
					if (io == kIoOk) {}	// TRICKY.
					int r = __socket->SendBuffer();
					deb_assert(r == 13);
					if (r == 13) {}	// TRICKY.
				}
				{
					LEPRA_MEASURE_SCOPE(Refill+ReadRaw);
					__socket->WaitAvailable(0.5);
					char _buffer[13] = "";
					__socket->ReadRaw(_buffer, 13);
					_test_ok = (::strcmp(_buffer, "Hello Client") == 0);
					deb_assert(_test_ok);
				}
				{
					LEPRA_MEASURE_SCOPE(Close);
					for (int x = 0; acceptor.IsRunning() && x < 10; ++x) {
						Thread::Sleep(0.06);
					}
					mux_socket1.CloseSocket(__socket);
				}
			}
		}
	}

	Network::Stop();

	if (_test_ok) {
		_context = "?";
		_test_ok = true;
		deb_assert(_test_ok);
	}

	ReportTestResult(account, "Performance", _context, _test_ok);
	return (_test_ok);
}

bool TestPath(const LogDecorator& account) {
	str _context;
	bool _test_ok = true;
	const char* test_path1 = "/usr/bin/.hid";
	const char* test_path2 = "C:\\Documents and settings\\Sverker\\Mina dokument\\.skit...apansson";
	const char* test_path3 = "apa_bepa0.cepa";
	const char* test_path4 = "d1/f3";

	if (_test_ok) {
		_context = "extension 1";
		str extension = Path::GetExtension(test_path1);
		_test_ok = (extension == "");
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "file 1";
		str _file = Path::GetFileBase(test_path1);
		_test_ok = (_file == ".hid");
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "file 2";
		str dir = Path::GetDirectory(test_path3);
		str _file = Path::GetFileBase(test_path3);
		str ext = Path::GetExtension(test_path3);
		_test_ok = (dir == "" && _file == "apa_bepa0" && ext == "cepa");
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "file 3";
		str dir = Path::GetDirectory(test_path4);
		str _file = Path::GetFileBase(test_path4);
		str ext = Path::GetExtension(test_path4);
		_test_ok = (dir == "d1/" && _file == "f3" && ext == "");
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "directory 1";
		str directory = Path::GetDirectory(test_path1);
		_test_ok = (directory == "/usr/bin/");
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "extension 2";
		str extension = Path::GetExtension(test_path2);
		_test_ok = (extension == "apansson");
		deb_assert(_test_ok);
	}
#ifdef LEPRA_WINDOWS
	if (_test_ok) {
		_context = "file 2";
		str _file = Path::GetFileBase(test_path2);
		_test_ok = (_file == ".skit..");
		deb_assert(_test_ok);
	}
	str directory;
	if (_test_ok) {
		_context = "directory 2";
		directory = Path::GetDirectory(test_path2);
		_test_ok = (directory == "C:\\Documents and settings\\Sverker\\Mina dokument\\");
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "directory 3";
		strutil::strvec directory_array = Path::SplitNodes(directory);
		_test_ok = (directory_array.size() == 4 && directory_array[0] == "C:" &&
			directory_array[1] == "Documents and settings" && directory_array[2] == "Sverker" &&
			directory_array[3] == "Mina dokument");
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "directory 4";
		strutil::strvec directory_array = Path::SplitNodes("\\WINDOWS.0\\");
		_test_ok = (directory_array.size() == 1 && directory_array[0] == "WINDOWS.0");
		deb_assert(_test_ok);
	}
#endif // Windows
	if (_test_ok) {
		_context = "directory 5";
		strutil::strvec directory_array = Path::SplitNodes(test_path1);
		_test_ok = (directory_array.size() == 3 && directory_array[0] == "usr" &&
			directory_array[1] == "bin" && directory_array[2] == ".hid");
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "split";
		strutil::strvec path_parts = Path::SplitPath("a/b.c/d.e");
		_test_ok = (path_parts.size() == 3 && path_parts[0] == "a/b.c/" &&
			path_parts[1] == "d" && path_parts[2] == "e");
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "normalize 1";
		str path;
		_test_ok = Path::NormalizePath("/./apa/../", path);
		if (_test_ok) {
			_test_ok = (path == "/");
		}
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "normalize 2";
		str path;
		_test_ok = Path::NormalizePath("/sune./apa/../", path);
		if (_test_ok) {
			_test_ok = (path == "/sune./");
		}
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "normalize 3";
		str path;
		_test_ok = Path::NormalizePath("./apa/../", path);
		if (_test_ok) {
			_test_ok = (path == "./");
		}
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "normalize 4";
		str path;
		_test_ok = Path::NormalizePath("./apa/..", path);
		if (_test_ok) {
			_test_ok = (path == ".");
		}
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "normalize 5";
		str path;
		_test_ok = Path::NormalizePath("./apa/../sunk/..", path);
		if (_test_ok) {
			_test_ok = (path == ".");
		}
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "normalize 6";
		str path;
		_test_ok = Path::NormalizePath("/apa/../sunk/..", path);
		if (_test_ok) {
			_test_ok = (path == "/");
		}
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "normalize 7";
		str path;
		_test_ok = Path::NormalizePath("/apa/../fagott/sunk/..", path);
		if (_test_ok) {
			_test_ok = (path == "/fagott");
		}
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "normalize 8";
		str path;
		_test_ok = Path::NormalizePath("./..", path);
		if (_test_ok) {
			_test_ok = (path == "..");
		}
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "normalize 9";
		str path;
		_test_ok = Path::NormalizePath(".//", path);
		if (_test_ok) {
			_test_ok = (path == "./");
		}
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "normalize 10";
		str path;
		_test_ok = Path::NormalizePath("//a", path);
		if (_test_ok) {
			_test_ok = (path == "/a");
		}
		deb_assert(_test_ok);
	}
#ifdef LEPRA_WINDOWS
	if (_test_ok) {
		_context = "normalize 11";
		str path;
		_test_ok = Path::NormalizePath("C:\\a..\\b\\..\\c", path);
		if (_test_ok) {
			_test_ok = (path == "C:/a../c");
		}
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "normalize 12";
		str path;
		_test_ok = Path::NormalizePath("\\\\.\\C:\\a\\\\b\\.\\.\\c\\", path);	// UNC name.
		if (_test_ok) {
			_test_ok = (path == "\\\\.\\C:/a/b/c/");
		}
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "normalize 13";
		str path;
		_test_ok = Path::NormalizePath("\\\\MyServer\\$Share_1$\\.\\.\\Porn\\..\\Bible\\NT.txt", path);
		if (_test_ok) {
			_test_ok = (path == "\\\\MyServer/$Share_1$/Bible/NT.txt");
		}
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "normalize 14";
		str path;
		_test_ok = Path::NormalizePath("..\\..\\", path);
		if (_test_ok) {
			_test_ok = (path == "../../");
		}
		deb_assert(_test_ok);
	}
#endif // Windows
	if (_test_ok) {
		_context = "normalize error 1";
		str path;
		_test_ok = !Path::NormalizePath("/../", path);
		deb_assert(_test_ok);
	}
	if (_test_ok) {
		_context = "wildcard error";
		_test_ok &=  Path::IsWildcardMatch("/a/b/c.txt",	"/a/b/c.txt");	deb_assert(_test_ok);
		_test_ok &=  Path::IsWildcardMatch("/a/*/c.txt",	"/a/b/c.txt");	deb_assert(_test_ok);
		_test_ok &=  Path::IsWildcardMatch("/a/b/*.txt",	"/a/b/c.txt");	deb_assert(_test_ok);
		_test_ok &=  Path::IsWildcardMatch("/*/*/c.txt",	"/a/b/c.txt");	deb_assert(_test_ok);
		_test_ok &=  Path::IsWildcardMatch("/a/*/c.txt",	"/a/bug/c.txt");	deb_assert(_test_ok);
		_test_ok &=  Path::IsWildcardMatch("*/c.txt",	"banana/c.txt");	deb_assert(_test_ok);
		_test_ok &=  Path::IsWildcardMatch("c.*",		"c.txt");		deb_assert(_test_ok);
		_test_ok &=  Path::IsWildcardMatch("*.*",		"c.txt");		deb_assert(_test_ok);
		_test_ok &=  Path::IsWildcardMatch("*.txt",		"c.txt");		deb_assert(_test_ok);
		_test_ok &=  Path::IsWildcardMatch("*.t*t",		"c.txt");		deb_assert(_test_ok);
		_test_ok &= !Path::IsWildcardMatch("/a/c/c.txt",	"/a/b/c.txt");	deb_assert(_test_ok);
		_test_ok &= !Path::IsWildcardMatch("/a*/c.txt",	"/a/b/c.txt");	deb_assert(_test_ok);
		_test_ok &= !Path::IsWildcardMatch("/*/c.txt",	"/a/b/c.txt");	deb_assert(_test_ok);
		_test_ok &= !Path::IsWildcardMatch("*/*.txt",	"/a/b/c.txt");	deb_assert(_test_ok);
		_test_ok &= !Path::IsWildcardMatch("*.txt",		"/a/b/c.txt");	deb_assert(_test_ok);
		_test_ok &= !Path::IsWildcardMatch("/a/b*",		"/a/bug/c.txt");	deb_assert(_test_ok);
		_test_ok &= !Path::IsWildcardMatch("/a/b/c.t*x",	"banana/c.txt");	deb_assert(_test_ok);
		_test_ok &= !Path::IsWildcardMatch("t*",		"c.txt");		deb_assert(_test_ok);
		_test_ok &= !Path::IsWildcardMatch("*c",		"c.txt");		deb_assert(_test_ok);
	}

	ReportTestResult(account, "Path", _context, _test_ok);
	return (_test_ok);
}

bool TestMemFileConcurrency(const LogDecorator& account) {
	str _context;
	bool _test_ok = true;

	if (_test_ok) {
		_context = "thread trashing";
		class FileTrasher: public Thread {
		public:
			FileTrasher(LogListener& file, Semaphore& semaphore):
				Thread("Trasher"),
				file_(file),
				semaphore_(semaphore) {
				Start();
			}
			void Run() {
				semaphore_.Wait();
				Logger* _log = LogType::GetLogger(LogType::kGeneralResources);
				for (int x = 0; x < 1000; ++x) {
					file_.OnLog(_log, "?", kLevelTrace);
				}
			}
			LogListener& file_;
			Semaphore& semaphore_;
			void operator=(const FileTrasher&) {};
		};
		const int count = 20;
		FileTrasher* trashers[count];
		::memset(trashers, 0, sizeof(trashers));
		MemFileLogListener _file(5*1024);
		_file.Clear();
		Semaphore _semaphore;
		for (int x = 0; x < count; ++x) {
			if (!trashers[x]) {
				trashers[x] = new FileTrasher(_file, _semaphore);
			}
		}
		for (int u = 0; u < count; ++u) {
			_semaphore.Signal();
		}
		Thread::Sleep(0.1);
		for (int z = 0; z < count; ++z) {
			if (trashers[z]->IsRunning()) {
				Thread::Sleep(0.001);
				z = -1;
			}
		}
		for (int y = 0; y < count; ++y) {
			delete (trashers[y]);
		}
	}

	ReportTestResult(account, "MemFileConcurrency", _context, _test_ok);
	return (_test_ok);
}

void ShowTestResult(const LogDecorator& account, bool test_ok) {
	::printf("\n");
	ReportTestResult(account, "Total test result", "?", test_ok);

	// Print termination "progress bar".
	const char* end_message = "Termination progress: [";
	const int progress_bar_steps = 45;
	const double termination_time = test_ok? 2.0 : 35.0;
	::printf("\n\n%s", end_message);
	for (int x = 0; x < progress_bar_steps; ++x) {
		::printf(" ");
	}
	::printf("]\r%s", end_message);
	for (int y = 0; y < progress_bar_steps; ++y) {
		::printf("*");
		::fflush(stdout);
		Thread::Sleep(termination_time/progress_bar_steps);
	}
	printf("]\n");
}

bool TestLepra() {
	bool _test_ok = true;

	if (_test_ok) {
		_test_ok = TestString(gLLog);
	}
	if (_test_ok) {
		_test_ok = TestRandom(gLLog);
	}
	if (_test_ok) {
		_test_ok = TestMath(gLLog);
	}
	if (_test_ok) {
		_test_ok = TestNumber(gLLog);
	}
	if (_test_ok) {
		_test_ok = TestVector3D(gLLog);
	}
	if (_test_ok) {
		_test_ok = TestOrderedMap(gLLog);
	}
	if (_test_ok) {
		_test_ok = TestBinTree(gLLog);
	}
	if (_test_ok) {
		_test_ok = TestLooseBintree(gLLog);
	}
	if (_test_ok) {
		_test_ok = TestLooseQuadtree(gLLog);
	}
	if (_test_ok) {
		_test_ok = TestLooseOctree(gLLog);
	}
	if (_test_ok) {
		_test_ok = TestIdManager(gLLog);
	}
	if (_test_ok) {
		_test_ok = TestTransformation(gLLog);
	}
	if (_test_ok) {
		_test_ok = TestTimers(gLLog);
	}
	if (_test_ok) {
		_test_ok = TestThreading(gLLog);
	}
	if (_test_ok) {
		_test_ok = TestSystemManager(gLLog);
	}
	if (_test_ok) {
		_test_ok = TestNetwork(gLLog);
	}
	if (_test_ok) {
		_test_ok = TestTcpMuxSocket(gLLog);
	}
	if (_test_ok) {
		// A special test for the mux- and virtual sockets.
		_test_ok = TestUDPSockets(gLLog);
	}
	if (_test_ok) {
		DualSocketClientTest dual_socket_client_test;
		_test_ok = dual_socket_client_test.Test();
	}
	if (_test_ok) {
		DualSocketServerTest dual_socket_server_test;
		_test_ok = dual_socket_server_test.Test();
	}
	if (_test_ok) {
		_test_ok = TestArchive(gLLog);
	}
	if (_test_ok) {
		_test_ok = TestCrypto(gLLog);
	}
	if (_test_ok) {
		_test_ok = TestPerformance(gLLog);
	}
	if (_test_ok) {
		_test_ok = TestPath(gLLog);
	}
	if (_test_ok) {
		_test_ok = TestMemFileConcurrency(gLLog);
	}

	return (_test_ok);
}
