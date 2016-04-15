
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



// Unreachable code warning below (MSVC8). For some reason just this file happens to temper with some shitty template.
#include "pch.h"
#include "../../Lepra/Include/LepraTarget.h"
#ifdef LEPRA_MSVC
#pragma warning(push)
#pragma warning(disable: 4702)
#endif // LEPRA_MSVC
#include "../Include/Lepra.h"
#ifdef LEPRA_MSVC
#pragma warning(pop)
#endif // LEPRA_MSVC

//#define TEST_WEB_AND_MAIL

#include "../../Lepra/Include/LepraAssert.h"
#include <math.h>
#include "../Include/Canvas.h"
#include "../Include/DES.h"
#include "../Include/DiskFile.h"
#include "../Include/IdManager.h"
#include "../Include/IOBuffer.h"
#include "../Include/JsonString.h"
#include "../Include/Lepra.h"
#include "../Include/Logger.h"
#include "../Include/LogListener.h"
#include "../Include/Math.h"
#include "../Include/Network.h"
#include "../Include/Number.h"
#include "../Include/OrderedMap.h"
#include "../Include/Path.h"
#include "../Include/Performance.h"
#include "../Include/Random.h"
#include "../Include/RotationMatrix.h"
#include "../Include/SHA1.h"
#include "../Include/Socket.h"
#include "../Include/SpinLock.h"
#include "../Include/String.h"
#include "../Include/SystemManager.h"
#include "../Include/TcpMuxSocket.h"
#include "../Include/Timer.h"
#include "../Include/Transformation.h"


using namespace Lepra;

class LepraTest{};
static LogDecorator gLLog(LogType::GetLogger(LogType::TEST), typeid(LepraTest));



//bool TestSkipList(const LogDecorator& pAccount);
bool TestBinTree(const LogDecorator& pAccount);
bool TestLooseBintree(const LogDecorator& pAccount);
bool TestLooseQuadtree(const LogDecorator& pAccount);
bool TestLooseOctree(const LogDecorator& pAccount);
bool TestThreading(const LogDecorator& pAccount);
bool TestUDPSockets(const LogDecorator& pAccount);
bool TestRotationMatrix(const LogDecorator& pAccount);

void ReportTestResult(const LogDecorator& pLog, const str& pTestName, const str& pContext, bool pResult)
{
	if (pResult)
	{
		pLog.Headlinef("%20s: passed.", pTestName.c_str());
	}
	else
	{
		pLog.Fatalf("%20s: failed (%s)!", pTestName.c_str(), pContext.c_str());
	}
}

bool TestString(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	// Verify str basics.
	if (lTestOk)
	{
		lContext = "String basics";
		str lString = "ABCdefghijklmnopqrstUvwxyz";
		lString += "97531" + lString + "02468";
		lString.erase(31, 4);	// Remove second ABCd.
		lString = strutil::ReplaceAll(lString, 'x', ' ');
		str lTmp1 = "oA";
		strutil::ToUpper(lTmp1);
		str lTmp2 = "Oa";
		strutil::ToLower(lTmp2);
		lString.insert(10, lTmp1+lTmp2);
		lTmp1 = "*!\" #\t%&/\r()=\n\v";
		strutil::StripAllWhiteSpaces(lTmp1);
		lString.insert(30, lTmp1);
		lString.insert(0, strutil::Right("Johannes", 6) + str("Sune").substr(0, 1));
		lTestOk = (lString == "hannesSABCdefghijOAoaklmnopqrstUvw yz*!\"#%&/()=97531efghijklmnopqrstUvw yz02468" &&
			lString.find('S', 0) == 6 &&
			lString.rfind('S', lString.length()-1) == 6);
		deb_assert(lTestOk);
	}

	// Verify str basics.
	if (lTestOk)
	{
		lContext = "String start/end";
		str lData("This is the start of something new!");
		lTestOk = (strutil::StartsWith(lData, "This is") &&
			!strutil::StartsWith(lData, "That is") &&
			strutil::EndsWith(lData, " new!") &&
			!strutil::EndsWith(lData, " old!"));
		deb_assert(lTestOk);
	}

	// Verify strutil::Format.
	if (lTestOk)
	{
		lContext = "strutil::Format()";
		int i = 123;
		float f = 123.321f;
		str lString = "String " + strutil::Format("format test: %i, %.4f, %s", i, f, "Hello World!");
		lTestOk = (lString == "String format test: 123, 123.3210, Hello World!");
		deb_assert(lTestOk);
	}

	// Verify str conversion to/from Ansi/Unicode.
	if (lTestOk)
	{
		lContext = "Ansi/Unicode conversion";
		const char* lTestData1    =  "Knastest !\"#%&/()=?'-_+\\}][{$@'*.:,;~^<>|";
		const wchar_t* lTestData2 = L"Knastest !\"#%&/()=?'-_+\\}][{$@'*.:,;~^<>|";
		wstr lUnicode = wstrutil::Encode(str(lTestData1));
		str lAnsi = strutil::Encode(wstr(lTestData2));
		lTestOk = (lAnsi == lTestData1 && lUnicode == lTestData2);
		deb_assert(lTestOk);
	}

	// Test empty int conversion.
	if (lTestOk)
	{
		lContext = "empty int conversion";
		int lValue = 0;
		lTestOk = !strutil::StringToInt("", lValue);
		deb_assert(lTestOk);
	}

	// Test floating point conversion.
	if (lTestOk)
	{
		lContext = "float conversion";
		float64 lValue = 1.5;
		str lString = strutil::Format("%.5f", lValue);
		lTestOk = strutil::StringToDouble(lString, lValue);
		if (lTestOk)
		{
			lTestOk = (lString == "1.50000" && lValue == 1.5f);
		}
		deb_assert(lTestOk);
	}

	// Test empty floating point conversion.
	if (lTestOk)
	{
		lContext = "empty float conversion";
		double lValue = 0;
		lTestOk = !strutil::StringToDouble("", lValue);
		deb_assert(lTestOk);
	}

	// Test fast floating point conversion.
	if (lTestOk)
	{
		lContext = "fast float conversion";
		lTestOk = (strutil::FastDoubleToString(0.0) == "0.0");
		deb_assert(lTestOk);
		if (lTestOk)
		{
			lTestOk = (strutil::FastDoubleToString(10.0) == "10.0");
			deb_assert(lTestOk);
		}
		if (lTestOk)
		{
			lTestOk = (strutil::FastDoubleToString(-287.75) == "-287.75");
			deb_assert(lTestOk);
		}
		if (lTestOk)
		{
			lTestOk = (strutil::FastDoubleToString(-2.625) == "-2.625");
			deb_assert(lTestOk);
		}
	}

	if (lTestOk)
	{
		lContext = "string -> C string";
		lTestOk = (strutil::StringToCString("Hej\"\\\n'!#\r\t") == "Hej\\\"\\\\\\n'!#\\r\\t");
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "C string -> string";
		str lValue;
		lTestOk = strutil::CStringToString("Hej\\\"\\\\\\n'!#\\r\\t", lValue);
		deb_assert(lTestOk);
		if (lTestOk)
		{
			lTestOk = (lValue == "Hej\"\\\n'!#\r\t");
			deb_assert(lTestOk);
		}
	}

	strutil::strvec lTestWords;
	if (lTestOk)
	{
		lContext = "plain string splitting";
		lTestWords = strutil::Split("Den \"kyliga Trazan'\"\tapansson\r\n\v.\t\t", " \t\v\r\n");
		lTestOk = (lTestWords.size() == 6 && lTestWords[0] == "Den" && lTestWords[1] == "\"kyliga" &&
			lTestWords[2] == "Trazan'\"" && lTestWords[3] == "apansson" && lTestWords[4] == "." &&
			lTestWords[5].empty());
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "string stripping";
		lTestWords[1] = strutil::StripLeft(lTestWords[1], "\"");
		lTestOk = (lTestWords[1] == "kyliga");
		if (lTestOk)
		{
			lTestOk = (strutil::StripRight("aabbcc", "c") == "aabb");
		}
		if (lTestOk)
		{
			lTestOk = (strutil::Strip("  Jonte  ", " ") == "Jonte");
		}
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "string joining";
		str lString = strutil::Join(lTestWords, " ");
		lTestOk = (lString == "Den kyliga Trazan'\" apansson . ");
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "block string splitting 1";
		strutil::strvec lWords = strutil::BlockSplit("\"Hej du glade\" sade jag  \ttill\n\n\r\vhonom igen.", " \t\v\r\n", false, false, 4);
		size_t lPhraseCount = lWords.size();
		const str& lWord0 = lWords[0];
		const str& lWord1 = lWords[1];
		const str& lWord2 = lWords[2];
		const str& lWord3 = lWords[3];
		const str& lWord4 = lWords[4];
		lTestOk = (lPhraseCount == 5 && lWord0 == "Hej du glade" && lWord1 == "sade" &&
			lWord2 == "jag" && lWord3 == "till" && lWord4 == "honom igen.");
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "block string splitting 2";
		strutil::strvec lWords = strutil::BlockSplit("\"Hej du glade \" sade jag  \t\"till\n\"\n\r\vhan..\nhonom igen.", " \t\v\r\n", true, false, 4);
		lTestOk = (lWords.size() == 5 && lWords[0] == "\"Hej du glade \"" && lWords[1] == "sade" &&
			lWords[2] == "jag" && lWords[3] == "\"till\n\"" && lWords[4] == "han..\nhonom igen.");
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "block string splitting 3";
		strutil::strvec lWords = strutil::BlockSplit("\"a\\\"b\"", " \t\v\r\n\"", false, true, 4);
		lTestOk = (lWords.size() == 1 && lWords[0] == "a\\\"b");
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "block whitespace limited splitting";
		strutil::strvec lWords = strutil::BlockSplit("abc def", " \t\v\r\n\"", false, false, 1);
		lTestOk = (lWords.size() == 2 && lWords[0] == "abc" && lWords[1] == "def");
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "block string splitting empty quote";
		strutil::strvec lWords = strutil::BlockSplit("\"\"", " \t\v\r\n", false, true);
		lTestOk = (lWords.size() == 1 && lWords[0].empty());
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "block string splitting quote with spaces";
		strutil::strvec lWords = strutil::BlockSplit(" \" \" ", " \t\v\r\n", false, true);
		lTestOk = (lWords.size() == 2 && lWords[0].empty() && lWords[1] == " ");
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "JSON coder";
		const wstr lWC = _WIDE("åäöabcÅÄÖACQñï");
		const str lC = strutil::Encode(lWC);
		const str lJson = "\"\\u00E5\\u00E4\\u00F6abc\\u00C5\\u00C4\\u00D6ACQ\\u00F1\\u00EF\"";
		const str lJsonString = JsonString::ToJson(lC);
		lTestOk = (lJsonString == lJson);
		deb_assert(lTestOk);
		if (lTestOk)
		{
			const str lString = JsonString::FromJson(lJson);
			lTestOk = (lString == lC);
			deb_assert(lTestOk);
		}
	}

	ReportTestResult(pAccount, "String", lContext, lTestOk);
	return (lTestOk);
}

bool TestRandom(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	if (lTestOk)
	{
		lContext = "uniform span";
		const double lLow = 0.5;
		const double lHigh = 2.0;
		double lAverage = 0;
		const int cnt = 10000;
		for (int x = 0; x < cnt; ++x)
		{
			const double lResult = Random::Uniform(lLow, lHigh);
			lTestOk = (lResult >= lLow && lResult <= lHigh);
			deb_assert(lTestOk);
			lAverage += lResult;
		}
		if (lTestOk)
		{
			lContext = "uniform mean";
			lAverage /= cnt;
			lTestOk = (lAverage >= 1.23 && lAverage <= 1.27);
			deb_assert(lTestOk);
		}
	}

	if (lTestOk)
	{
		double lAverage = 0;
		const double lMean = -3;
		const double lStdDev = 2;
		const int cnt = 10000;
		std::vector<double> lValues;
		for (int x = 0; x < cnt; ++x)
		{
			lValues.push_back(Random::Normal(lMean, lStdDev, -10.0, +10.0));
			deb_assert(lValues[x] >= -10 && lValues[x] <= +10);
			lAverage += lValues[x];
		}
		lContext = "normal mean";
		lAverage /= cnt;
		lTestOk = (lAverage >= lMean-0.03*lStdDev && lAverage <= lMean+0.03*lStdDev);
		deb_assert(lTestOk);
		if (lTestOk)
		{
			lContext = "normal distribuion";
			const double lActualStdDev = Math::CalculateDeviation<double>(lValues, lMean);
			lTestOk = (lActualStdDev >= lStdDev-0.04 && lActualStdDev <= lStdDev+0.04);
			deb_assert(lTestOk);
		}
	}

	ReportTestResult(pAccount, "Random", lContext, lTestOk);
	return (lTestOk);
}

bool TestMath(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;
	double lResult;

	if (lTestOk)
	{
		lContext = "smooth clamp narrow below max exp";
		lResult = Math::SmoothClamp(0.9, 0.0, 1.0, 0.1);
		lTestOk = (lResult == 0.9);
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "smooth clamp wide above max exp";
		lResult = Math::SmoothClamp(0.95, 0.0, 1.0, 0.5);
		lTestOk = (lResult < 0.8);
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "smooth clamp above min exp";
		lResult = Math::SmoothClamp(0.3, 0.0, 1.0, 0.3);
		lTestOk = (lResult == 0.3);
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "smooth clamp below min exp";
		lResult = Math::SmoothClamp(0.2, 0.0, 1.0, 0.4);
		lTestOk = (lResult > 0.23);
		deb_assert(lTestOk);
	}

	ReportTestResult(pAccount, "Math", lContext, lTestOk);
	return (lTestOk);
}

bool TestNumber(const LogDecorator& pAccount)
{
	// Verify Number basics.
	str lContext;
	bool lTestOk = true;
	str lResult;
	str lDesiredResult;

	if (lTestOk)
	{
		lDesiredResult = "1.00 ";
		lContext = "testing "+lDesiredResult;
		lResult = Number::ConvertToPostfixNumber(1, 2);
		lTestOk = (lResult == lDesiredResult);
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lDesiredResult = "1.41 k";
		lContext = "testing "+lDesiredResult;
		lResult = Number::ConvertToPostfixNumber(1.414444e3, 2);
		lTestOk = (lResult == lDesiredResult);
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lDesiredResult = "7.6667 m";
		lContext = "rounding "+lDesiredResult;
		lResult = Number::ConvertToPostfixNumber(7.66666666666666e-3, 4);
		lTestOk = (lResult == lDesiredResult);
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lDesiredResult = "10 M";
		lContext = "testing "+lDesiredResult;
		lResult = Number::ConvertToPostfixNumber(10e6, 0);
		lTestOk = (lResult == lDesiredResult);
		deb_assert(lTestOk);
	}

	ReportTestResult(pAccount, "Number", lContext, lTestOk);
	return (lTestOk);
}

bool TestVector3D(const LogDecorator& pAccount)
{
	// Verify Number basics.
	str lContext;
	bool lTestOk = true;
	float lResult;
	float lDesiredResult;

	if (lTestOk)
	{
		lDesiredResult = 0;
		lContext = "testing polar angle Y " + strutil::Format("%.1f", lDesiredResult);
		lResult = vec3(1,0,0).GetPolarCoordAngleY();
		lTestOk = Math::IsEpsEqual(lResult, lDesiredResult);
		deb_assert(lTestOk);
		if (lTestOk)
		{
			lDesiredResult = PIF/2;
			lContext = "testing polar angle Y " + strutil::Format("%.1f", lDesiredResult);
			lResult = vec3(0,0,1).GetPolarCoordAngleY();
			lTestOk = Math::IsEpsEqual(lResult, lDesiredResult);
			deb_assert(lTestOk);
		}

		if (lTestOk)
		{
			lDesiredResult = -5*PIF/6;
			lContext = "testing polar angle Y " + strutil::Format("%.1f", lDesiredResult);
			//lResult = vec3(-sqrtf(3),0,-1).GetPolarCoordAngleY();
			lResult = atan2(-1, -sqrtf(3));
			lTestOk = Math::IsEpsEqual(lResult, lDesiredResult);
			deb_assert(lTestOk);
		}

		if (lTestOk)
		{
			lDesiredResult = +PIF;
			lContext = "testing polar angle Y " + strutil::Format("%.1f", lDesiredResult);
			//lResult = vec3(-1,0,0).GetPolarCoordAngleY();
			lResult = atan2(0.0f, -1.0f);
			lTestOk = Math::IsEpsEqual(lResult, lDesiredResult);
			deb_assert(lTestOk);
		}
	}

	ReportTestResult(pAccount, "Vector3D", lContext, lTestOk);
	return (lTestOk);
}

bool TestOrderedMap(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	OrderedMap<str, int> lMap;
	if (lTestOk)
	{
		lContext = "empty start";
		lTestOk = (lMap.GetCount() == 0 && lMap.Find("4") == lMap.End());
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "insert unique";
		lMap.PushBack("4",  5);
		lTestOk = (lMap.GetCount() == 1 && lMap.Find("4").GetObject() == 5);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "erase -> empty";
		lMap.Remove(lMap.Find("4"));
		lTestOk = (lMap.GetCount() == 0 && lMap.Find("4") == lMap.End());
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "insert two";
		lMap.PushBack("4",  5);
		lMap.PushBack("5",  4);
		lTestOk = (lMap.GetCount() == 2 && lMap.Find("4").GetObject() == 5 && lMap.Find("5").GetObject() == 4);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "pop front";
		str lKey;
		int lValue;
		lMap.PopFront(lKey, lValue);
		lTestOk = (lKey == "4" && lValue == 5 && lMap.GetCount() == 1);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "pop back";
		str lKey;
		int lValue;
		lMap.PopBack(lKey, lValue);
		lTestOk = (lKey == "5" && lValue == 4 && lMap.GetCount() == 0);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "insert three";
		lMap.PushFront("5",  4);
		lMap.PushFront("4",  5);
		lMap.PushFront("3",  3);
		lTestOk = (lMap.GetCount() == 3 && lMap.Find("3").GetObject() == 3 &&
			lMap.Find("4").GetObject() == 5 && lMap.Find("5").GetObject() == 4);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "erase mid";
		lMap.Remove("4");
		lTestOk = (lMap.GetCount() == 2 && lMap.Find("3").GetObject() == 3 &&
			lMap.Find("4") == lMap.End() && lMap.Find("5").GetObject() == 4);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "erase end";
		lMap.Remove("5");
		lTestOk = (lMap.GetCount() == 1 && lMap.Find("3").GetObject() == 3 &&
			lMap.Find("4") == lMap.End() && lMap.Find("5") == lMap.End());
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "erase last";
		lMap.Remove("3");
		lTestOk = (lMap.GetCount() == 0 && lMap.Find("3") == lMap.End() &&
			lMap.Find("4") == lMap.End() && lMap.Find("5") == lMap.End());
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "random add/remove";
		const int lEntries = 11;
		str lSet[lEntries];
		size_t lSetCount = 0;
		for (int x = 1; lTestOk && x < 10000; ++x)
		{
			deb_assert(lMap.GetCount() == lSetCount);
			int lIndex = Random::GetRandomNumber()%lEntries;
			if (!(x%59))
			{
				for (int x = 0; x < lEntries; ++x)
				{
					lSet[x] = EmptyAnsiString;
				}
				lMap.RemoveAll();
				lSetCount = 0;
				lTestOk = (lMap.GetCount() == lSetCount);
				deb_assert(lTestOk);
			}
			else if (lSet[lIndex].empty())
			{
				lSet[lIndex] = ":"+strutil::Format("%i", x);
				lTestOk = lMap.Find(lSet[lIndex]) == lMap.End();
				deb_assert(lTestOk);
				if (lTestOk)
				{
					lMap.PushBack(lSet[lIndex], x);
					++lSetCount;
					lTestOk = (lMap.GetCount() == lSetCount &&
						lMap.Find(lSet[lIndex]).GetObject() == x &&
						lMap.Find(lSet[lIndex]) == lMap.Last());
					deb_assert(lTestOk);
				}
			}
			else
			{
				lTestOk = (lMap.Find(lSet[lIndex]) != lMap.End());
				deb_assert(lTestOk);
				if (lTestOk)
				{
					int lValue = -1;
					strutil::StringToInt(lSet[lIndex].c_str()+1, lValue, 10);
					lTestOk = (lTestOk && lMap.Find(lSet[lIndex]).GetObject() == lValue);
					deb_assert(lTestOk);
				}
				if (lTestOk)
				{
					static bool lIteratorRemove = false;
					lIteratorRemove = !lIteratorRemove;
					if (lIteratorRemove)
					{
						lMap.Remove(lMap.Find(lSet[lIndex]));
					}
					else
					{
						lMap.Remove(lSet[lIndex]);
					}
					lSet[lIndex].clear();
					--lSetCount;
					lTestOk = (lMap.GetCount() == lSetCount && lMap.Find(lSet[lIndex]) == lMap.End());
					deb_assert(lTestOk);
				}
			}
		}
		lTestOk = (lMap.GetCount() == lSetCount);
		deb_assert(lTestOk);
	}

	ReportTestResult(pAccount, "OrderedMap", lContext, lTestOk);
	return (lTestOk);
}

bool TestIdManager(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;
	str lDebugState;

	IdManager<int> lIdManager(3, 100, -2);
	if (lTestOk)
	{
		lContext = "min ID";
		lTestOk = (lIdManager.GetMinId() == 3);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "max ID";
		lTestOk = (lIdManager.GetMaxId() == 100);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "invalid ID";
		lTestOk = (lIdManager.GetInvalidId() == -2);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "reserve ID (4)";
		lTestOk = lIdManager.ReserveId(4);
		deb_assert(lTestOk);
		lDebugState = lIdManager.GetDebugState();
		lTestOk = (lDebugState == "3-2, 3-3,\n4-4, 5-100,\n");
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "alloc ID (3)";
		lTestOk = (lIdManager.GetFreeId() == 3);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "alloc ID (5)";
		lTestOk = (lIdManager.GetFreeId() == 5);
		lDebugState = lIdManager.GetDebugState();
		lTestOk = (lDebugState == "3-5, 6-100,\n");
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "recycle ID (4)";
		lTestOk = lIdManager.RecycleId(4);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "invalid recycle ID (4)";
		lTestOk = !lIdManager.RecycleId(4);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "invalid recycle ID (4)";
		lTestOk = !lIdManager.RecycleId(4);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "invalid recycle ID (7)";
		lTestOk = !lIdManager.RecycleId(7);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "invalid recycle ID (1000)";
		lTestOk = !lIdManager.RecycleId(1000);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "invalid recycle ID (-2)";
		lTestOk = !lIdManager.RecycleId(-2);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "reserve ID (4)";
		lTestOk = lIdManager.ReserveId(4);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "invalid reserve ID (3)";
		lTestOk = !lIdManager.ReserveId(3);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "invalid reserve ID (4)";
		lTestOk = !lIdManager.ReserveId(4);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "invalid reserve ID (2)";
		lTestOk = !lIdManager.ReserveId(2);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "invalid reserve ID (-2)";
		lTestOk = !lIdManager.ReserveId(-2);
		deb_assert(lTestOk);
	}
	for (int x = 6; lTestOk && x <= 100; ++x)
	{
		lContext = "alloc many IDs";
		lTestOk = (lIdManager.GetFreeId() == x);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "no free IDs";
		lTestOk = (lIdManager.GetFreeId() == -2);
		deb_assert(lTestOk);
		lDebugState = lIdManager.GetDebugState();
		lTestOk = (lDebugState == "3-100, 101-100,\n");
		deb_assert(lTestOk);
	}
	for (int y = 0; y < 1000; ++y)
	{
		for (int z = 100; lTestOk && z >= 3; z -= 2)
		{
			lContext = "free dec-loop";
			lTestOk = lIdManager.RecycleId(z);
			deb_assert(lTestOk);
		}
		for (int u = 100; lTestOk && u >= 3; u -= 2)
		{
			lContext = "reserve dec-loop";
			lTestOk = lIdManager.ReserveId(u);
			deb_assert(lTestOk);
		}
		for (int v = 3; lTestOk && v <= 100; ++v)
		{
			lContext = "free inc-loop";
			lTestOk = lIdManager.RecycleId(v);
			deb_assert(lTestOk);
		}
		for (int w = 3; lTestOk && w <= 100; ++w)
		{
			lContext = "alloc inc-loop";
			lTestOk = (lIdManager.GetFreeId() == w);
			deb_assert(lTestOk);
		}
	}
	if (lTestOk)
	{
		// Just simple an anti-crash test.
		IdManager<int> lIdManager2(1, 0x7FFFFFFF-1, 0xFFFFFFFF);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-0, 1-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = lIdManager2.ReserveId(1);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-1, 2-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = lIdManager2.ReserveId(5);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-1, 2-4,\n5-5, 6-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = lIdManager2.ReserveId(3);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-1, 2-2,\n3-3, 4-4,\n5-5, 6-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = lIdManager2.RecycleId(1);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-0, 1-2,\n3-3, 4-4,\n5-5, 6-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = lIdManager2.ReserveId(2147483646);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-0, 1-2,\n3-3, 4-4,\n5-5, 6-2147483645,\n2147483646-2147483646, 2147483647-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = !lIdManager2.RecycleId(4);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-0, 1-2,\n3-3, 4-4,\n5-5, 6-2147483645,\n2147483646-2147483646, 2147483647-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = !lIdManager2.RecycleId(6);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-0, 1-2,\n3-3, 4-4,\n5-5, 6-2147483645,\n2147483646-2147483646, 2147483647-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = !lIdManager2.RecycleId(1);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-0, 1-2,\n3-3, 4-4,\n5-5, 6-2147483645,\n2147483646-2147483646, 2147483647-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = !lIdManager2.RecycleId(2147483645);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-0, 1-2,\n3-3, 4-4,\n5-5, 6-2147483645,\n2147483646-2147483646, 2147483647-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = lIdManager2.RecycleId(5);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-0, 1-2,\n3-3, 4-2147483645,\n2147483646-2147483646, 2147483647-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = lIdManager2.RecycleId(2147483646);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-0, 1-2,\n3-3, 4-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = lIdManager2.RecycleId(3);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-0, 1-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = lIdManager2.ReserveId(1);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-1, 2-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = lIdManager2.ReserveId(7);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-1, 2-6,\n7-7, 8-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = lIdManager2.ReserveId(8);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-1, 2-6,\n7-8, 9-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = lIdManager2.ReserveId(4);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-1, 2-3,\n4-4, 5-6,\n7-8, 9-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = lIdManager2.RecycleId(1);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-0, 1-3,\n4-4, 5-6,\n7-8, 9-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = lIdManager2.RecycleId(4);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-0, 1-6,\n7-8, 9-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = lIdManager2.RecycleId(8);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-0, 1-6,\n7-7, 8-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = lIdManager2.RecycleId(7);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-0, 1-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = lIdManager2.ReserveId(8);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-0, 1-7,\n8-8, 9-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = lIdManager2.ReserveId(7);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-0, 1-6,\n7-8, 9-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = lIdManager2.RecycleId(8);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-0, 1-6,\n7-7, 8-2147483646,\n");
		deb_assert(lTestOk);

		lTestOk = lIdManager2.RecycleId(7);
		deb_assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == "1-0, 1-2147483646,\n");
		deb_assert(lTestOk);
	}

	ReportTestResult(pAccount, "IdManager", lContext, lTestOk);
	return (lTestOk);
}

bool TestTransformation(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;
	str lDebugState;

	if (lTestOk)
	{
		lContext = "anchor rotate 1";
		TransformationD lTransformation;
		lTransformation.SetPosition(Vector3DD(4, 1, 1));
		lTransformation.RotateAroundAnchor(Vector3DD(3, 0, -0.41), Vector3DD(0, 0, 1), PIF/2);
		lTestOk = (lTransformation.GetPosition().GetDistance(Vector3DD(2, 1, 1)) < 0.0001);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "matrix rotate";
		const Vector3DD lAxis(1, -1, 0);
		RotationMatrixD lMatrix1;
		lMatrix1.RotateAroundVector(lAxis, PI/4);
		RotationMatrixD lMatrix2;
		lMatrix2.RotateAroundVector(lAxis, -PI*3/4);
		Vector3DD lNewPosition1 = lMatrix1*Vector3DD(2, 2, 2);
		Vector3DD lNewPosition2 = lMatrix2*Vector3DD(-2, -2, -2);
		lTestOk = (lNewPosition1.GetDistance(lNewPosition2) < 0.0001 &&
			lNewPosition1.GetDistance(Vector3DD(0.4142, 0.4142, 3.4142)) < 0.0001);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "quaternion rotate";
		const Vector3DD lAxis(1, -1, 0);
		QuaternionD lQuaternion1(PI/4, lAxis);
		QuaternionD lQuaternion2(-PI*3/4, lAxis);
		Vector3DD lNewPosition1 = lQuaternion1*Vector3DD(2, 2, 2);
		Vector3DD lNewPosition2 = lQuaternion2*Vector3DD(-2, -2, -2);
		lTestOk = (lNewPosition1.GetDistance(lNewPosition2) < 0.0001 &&
			lNewPosition1.GetDistance(Vector3DD(0.4142, 0.4142, 3.4142)) < 0.0001);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "quaternion multiply";
		Vector3DD x(1, 0, 0);
		QuaternionD q1(PI/2, Vector3DD(0, -1, 0));
		x = q1*x;
		lTestOk = (x.GetDistance(Vector3DD(0, 0, 1)) < 0.0001);
		deb_assert(lTestOk);
		if (lTestOk)
		{
			QuaternionD q2(-PI*1.5, Vector3DD(1, 0, 0));
			x = (q1*q2)*x;
			lTestOk = (x.GetDistance(Vector3DD(0, -1, 0)) < 0.0001);
			deb_assert(lTestOk);
		}
	}
	if (lTestOk)
	{
		lContext = "anchor rotate 2";
		const Vector3DD lAnchor(0, -1, -1);
		TransformationD lTransformation1;
		lTransformation1.SetPosition(Vector3DD(2, 1, -1));
		const Vector3DD lDiff1(lTransformation1.GetPosition()-lAnchor);
		TransformationD lTransformation2;
		double l2Side = lDiff1.GetLength()/::sqrt(2.0);
		lTransformation2.SetPosition(Vector3DD(-l2Side, l2Side-1, -1));
		const Vector3DD lDiff2(lTransformation2.GetPosition()-lAnchor);
		deb_assert(::fabs(lDiff1.GetLength()-lDiff2.GetLength()) < 0.0001);

		const Vector3DD lAxis1(1, -1, 0);
		lTransformation1.RotateAroundAnchor(lAnchor, lAxis1, PI/2);
		const Vector3DD lAxis2(1, 1, 0);
		lTransformation2.RotateAroundAnchor(lAnchor, lAxis2, PI/2);
		lTestOk = (lTransformation1.GetPosition().GetDistance(lTransformation2.GetPosition()) < 0.0001 &&
			lTransformation1.GetPosition().GetDistance(Vector3DD(0, -1, lDiff1.GetLength()-1)) < 0.0001);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "quaternion rotate 2";
		Vector3DD lVector(0, 1, 0);
		QuaternionD lQ(PI/4, Vector3DD(0, 0, 1));
		lQ.RotateAroundOwnX(PI/4);
		lVector = lQ*lVector;
		lQ.SetIdentity();
		lQ.RotateAroundVector(Vector3DD(0, 0, 1), -PI/4);
		lVector = lQ*lVector;
		lQ.Set(PI/4, Vector3DD(1, 0, 0));
		lQ.MakeInverse();
		lVector = lQ*lVector;
		lTestOk = (lVector.GetDistance(Vector3DD(0, 1, 0)) < 0.0001);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "quaternion back&forth 1";
		const QuaternionD lParentQ(0.95, Vector3DD(1, 1, 0));
		const QuaternionD lChildRelativeQ(PI/4, Vector3DD(1, 0, 0));
		const QuaternionD lChildAbsoluteQ = lChildRelativeQ*lParentQ;
		Vector3DD lVector = lChildAbsoluteQ*Vector3DD(-1, 1, 1);
		lTestOk = (lVector.GetDistance(Vector3DD(0, -1.2, 1.2)) < 0.06);
		deb_assert(lTestOk);
		if (lTestOk)
		{
			lVector = (lChildAbsoluteQ/lParentQ)*Vector3DD(0, 1, 0);
			lTestOk = (lVector.GetDistance(Vector3DD(0, 0.707, 0.707)) < 0.001);
			deb_assert(lTestOk);
		}
		Vector3DD lEulerAngles1;
		if (lTestOk)
		{
			(lChildAbsoluteQ/lParentQ).GetEulerAngles(lEulerAngles1);
			lTestOk = (lEulerAngles1.GetDistance(Vector3DD(0, PI/4, 0)) < 2e-11);
			deb_assert(lTestOk);
		}
		if (lTestOk)
		{
			Vector3DD lEulerAngles2;
			lChildRelativeQ.GetEulerAngles(lEulerAngles2);
			lTestOk = (lEulerAngles1.GetDistance(lEulerAngles1) < 2e-11);
			deb_assert(lTestOk);
		}
	}
	if (lTestOk)
	{
		lContext = "quaternion/matrix angle 1";
		QuaternionD lQ;
		lQ.RotateAroundOwnZ(PI/3);
		lQ.RotateAroundOwnX(-PI/7);
		lQ.RotateAroundOwnY(PI/5);
		const Vector3DD lAx = lQ.GetAxisX();
		const Vector3DD lAy = lQ.GetAxisY();
		const Vector3DD lAz = lQ.GetAxisZ();
		RotationMatrixD lMatrix(lAx, lAy, lAz);
		QuaternionD lTarget(lMatrix);
		lTestOk = (Math::IsEpsEqual(lQ.a, lTarget.a) &&
			Math::IsEpsEqual(lQ.b, lTarget.b) &&
			Math::IsEpsEqual(lQ.c, lTarget.c) &&
			Math::IsEpsEqual(lQ.d, lTarget.d));
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "vector spherical angles 1";
		double lTheta = -1;
		double lPhi = -1;
		const double l45 = ::sin(PI/4);
		Vector3DD(l45, l45, 1).GetSphericalAngles(lTheta, lPhi);
		lTestOk = (Math::IsEpsEqual(lTheta, PI/4) &&
			Math::IsEpsEqual(lPhi, PI/4));
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "vector spherical angles 2";
		double lTheta = -1;
		double lPhi = -1;
		const double l45 = ::sin(PI/4);
		Vector3DD(-l45, -l45, -1).GetSphericalAngles(lTheta, lPhi);
		lTestOk = (Math::IsEpsEqual(lTheta, PI*3/4) &&
			Math::IsEpsEqual(lPhi, PI*5/4));
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "vector spherical angles 3";
		double lTheta = -1;
		double lPhi = -1;
		Vector3DD(1, 0, 0).GetSphericalAngles(lTheta, lPhi);
		lTestOk = (Math::IsEpsEqual(lTheta, PI/2) &&
			Math::IsEpsEqual(lPhi, 0.0));
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "vector spherical angles 3";
		double lTheta = -1;
		double lPhi = -1;
		Vector3DD(0, 0, 1).GetSphericalAngles(lTheta, lPhi);
		lTestOk = (Math::IsEpsEqual(lTheta, 0.0) &&
			Math::IsEpsEqual(lPhi, 0.0));
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "vector spherical angles 4";
		double lTheta = -1;
		double lPhi = -1;
		Vector3DD(0, -1, -1).GetSphericalAngles(lTheta, lPhi);
		lTestOk = (Math::IsEpsEqual(lTheta, PI*3/4) &&
			Math::IsEpsEqual(lPhi, PI*6/4));
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "quaternion euler angles 1";

		double lYaw = 0;
		double lPitch = PI/4;
		double lRoll = 0;
		for (double i = 0; lTestOk && i <= PI*4; i += 0.001)
		{
			QuaternionD lQ;
			lQ.RotateAroundOwnZ(lYaw);
			lQ.RotateAroundOwnX(lPitch);
			lQ.RotateAroundOwnY(lRoll);
			lYaw   += 0.001;
			lPitch += 0.001*3;
			lRoll  += 0.001*5;

			Vector3DD lEuler;
			lQ.GetEulerAngles(lEuler);
			QuaternionD lQ2;
			lQ2.SetEulerAngles(lEuler);
			const Vector3DD lTest(1, 1, 1);
			Vector3DD lTest1 = lTest*lQ;
			Vector3DD lTest2 = lTest*lQ2;
			double lDistance = lTest1.GetDistance(lTest2);
			lTestOk = (Math::IsEpsEqual(lDistance, 0.0, 2e-11));
			deb_assert(lTestOk);
		}
	}

	ReportTestResult(pAccount, "Transformation", lContext, lTestOk);
	return (lTestOk);
}

bool TestTimers(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	// Basic timer test.
	if (lTestOk)
	{
		lContext = "timer";
		Thread::Sleep(0.001);	// Bail early on cpu time slice.
		HiResTimer lHiTimer(false);
		Timer lLoTimer;
		Thread::Sleep(0.100);
		const double lHiTime = lHiTimer.QueryTimeDiff();
		const double lLoTime = lLoTimer.QueryTimeDiff();
		lTestOk = (lHiTime > 0.090 && lHiTime < 0.150 &&
			lLoTime > 0.090 && lLoTime < 0.150);
		deb_assert(lTestOk);
	}

	ReportTestResult(pAccount, "Timers", lContext, lTestOk);
	return (lTestOk);
}

bool TestSystemManager(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	// Directory test.
	if (lTestOk)
	{
		lContext = "root directory";
		str lRootDir = SystemManager::GetRootDirectory();

#if defined(LEPRA_WINDOWS)
		// Hugge: This test will fail if this app runs from another partition.
		// lTestOk = (lRootDir =="C:/"); 
		str lDriveLetter = lRootDir.substr(0, 1);
		str lRest = lRootDir.substr(1);
		lTestOk = lDriveLetter >= "A" && 
				   lDriveLetter <= "Z" &&
				   lRest == ":/";
#elif defined(LEPRA_POSIX)
		lTestOk = (lRootDir == "/");
#else // <Unknown target>
#error "Unknown target!"
#endif // LEPRA_WINDOWS/LEPRA_POSIX/<Unknown target>
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "current/documents directory";
		lTestOk = (SystemManager::GetCurrentDirectory().length() >= 4 &&
			SystemManager::GetUserDirectory().length() >= 4);
		deb_assert(lTestOk);
	}

	// Platform/user test.
	if (lTestOk)
	{
		lContext = "Os name";
		str lOs = SystemManager::GetOsName();
#if defined(LEPRA_WINDOWS)
		lTestOk = (lOs == "Windows NT");
#elif defined(LEPRA_POSIX)
		lTestOk = (lOs == "Darwin" || lOs == "Linux");
#else // <Unknown target>
#error "Not implemented for this platform!"
#endif // LEPRA_WINDOWS/LEPRA_POSIX/<Unknown target>
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "login name";
		lTestOk = (SystemManager::GetLoginName().length() >= 1);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "user name";
		lTestOk = (SystemManager::QueryFullUserName().length() >= 1);
		deb_assert(lTestOk);
	}

	// Cpu test.
	if (lTestOk)
	{
		lContext = "physical CPU count";
		lTestOk = (SystemManager::GetPhysicalCpuCount() >= 1 && SystemManager::GetPhysicalCpuCount() <= 4);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "logical CPU count";
		lTestOk = (SystemManager::GetLogicalCpuCount() >= 1 && SystemManager::GetLogicalCpuCount() <= 16);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "CPU core count";
		lTestOk = (SystemManager::GetCoreCount() >= 1 && SystemManager::GetCoreCount() <= 4);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "Cpu type";
		str lCpuName = SystemManager::GetCpuName();
		lTestOk = (lCpuName == "GenuineIntel" || lCpuName == "AuthenticAMD" ||
			lCpuName == "x64" || lCpuName == "x86" || lCpuName == "PowerPC");
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "Cpu frequency";
		uint64 lCpuFrequency = SystemManager::QueryCpuFrequency();
		lTestOk = (lCpuFrequency >= 700*1000*1000 &&
			lCpuFrequency <= (uint64)6*1000*1000*1000);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "Cpu MIPS";
		unsigned lCpuMips = SystemManager::QueryCpuMips();
		lTestOk = (lCpuMips >= 50 && lCpuMips <= 3500);
		deb_assert(lTestOk);
	}

	// Memory test.
	if (lTestOk)
	{
		lContext = "Ram size";
		lTestOk = (SystemManager::GetAmountRam() >= 64*1024*1024 &&
			SystemManager::GetAmountRam() <= (uint64)4*1024*1024*1024);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "available Ram >= total Ram";
		lTestOk = (SystemManager::GetAvailRam() <= SystemManager::GetAmountRam());
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "available Ram size";
		lTestOk = (SystemManager::GetAvailRam() >= 30*1024*1024 &&
			SystemManager::GetAvailRam() <= (uint64)4*1024*1024*1024);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "virtual memory size";
		lTestOk = (SystemManager::GetAmountVirtualMemory() >= 150*1024*1024 &&
			SystemManager::GetAmountVirtualMemory() <= (uint64)20*1024*1024*1024);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "available virtual memory > total virtual memory";
		lTestOk = (SystemManager::GetAvailVirtualMemory() <= SystemManager::GetAmountVirtualMemory());
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "available virtual memory size";
		lTestOk = (SystemManager::GetAvailVirtualMemory() >= 100*1024*1024 &&
			SystemManager::GetAvailVirtualMemory() <= (uint64)19*1024*1024*1024);
		deb_assert(lTestOk);
	}
#ifdef TEST_WEB_AND_MAIL
	if (lTestOk)
	{
		// Just make sure we don't crash. Need manual verification that it works anyhoo.
		SystemManager::WebBrowseTo("http://trialepicfail.blogspot.com/");
	}
	if (lTestOk)
	{
		// Just make sure we don't crash. Need manual verification that it works anyhoo.
		SystemManager::EmailTo("info@pixeldoctrine.com", "Test subject?", "Hi,\n\nHow are you? Hope you're good!\n\nLater,\nJonas");
	}
#endif // !TEST_WEB_AND_MAIL

	ReportTestResult(pAccount, "System", lContext, lTestOk);
	return (lTestOk);
}

bool TestNetwork(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	if (lTestOk)
	{
		lContext = "network start";
		lTestOk = Network::Start();
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "local resolve";
		SocketAddress lAddress;
		lTestOk = lAddress.Resolve(":1024");
		deb_assert(lTestOk);
		if (lTestOk)
		{
			const str lLocalAddress = lAddress.GetAsString();
			lTestOk = ((strutil::StartsWith(lLocalAddress, "127.0.") &&
				strutil::EndsWith(lLocalAddress, ".1:1024")) ||
				(strutil::StartsWith(lLocalAddress, "192.168.") &&
				strutil::EndsWith(lLocalAddress, ":1024")));
			deb_assert(lTestOk);
		}
	}
	if (lTestOk)
	{
		lContext = "DNS resolve";
		IPAddress lAddress;
		lTestOk = Network::ResolveHostname("ftp.sunet.se", lAddress);
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "UDP exclusive bind";
		SocketAddress lAddress;
		lAddress.Resolve(":1025");
		UdpSocket lSocket1(lAddress, true);
		UdpSocket lSocket2(lAddress, true);
		lTestOk = (lSocket1.IsOpen() && !lSocket2.IsOpen());
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "UDP send";
		SocketAddress lReceiveAddress;
		lReceiveAddress.Resolve(":47346");
		SocketAddress lSendAddress;
		lSendAddress.Resolve(":47347");
		UdpSocket lReceiver(lReceiveAddress, true);
		deb_assert(lReceiver.IsOpen());
		UdpSocket lSender(lSendAddress, true);
		deb_assert(lSender.IsOpen());
		lTestOk = (lSender.SendTo((const uint8*)"Hello World", 12, lReceiveAddress) == 12);
		deb_assert(lTestOk);
		if (lTestOk)
		{
			lContext = "UDP receive";
			uint8 lMessage[12];
			SocketAddress lSourceAddress;
			int lRecvCount = lReceiver.ReceiveFrom(lMessage, sizeof(lMessage), lSourceAddress);
			lTestOk = (lRecvCount == 12 && ::strncmp("Hello World", (const char*)lMessage, 12) == 0 &&
				lSourceAddress.GetIP() == lSendAddress.GetIP() && lSourceAddress.GetPort() == lSendAddress.GetPort());
			deb_assert(lTestOk);
		}
	}

	if (lTestOk)
	{
		lContext = "TCP server init";
		SocketAddress lReceiveAddress;
		lReceiveAddress.Resolve(":47346");
		SocketAddress lSendAddress;
		lSendAddress.Resolve(":47347");
		TcpListenerSocket lServer(lReceiveAddress, true);
		lTestOk = lServer.IsOpen();
		deb_assert(lTestOk);

		class DummyAcceptor: public Thread
		{
		public:
			DummyAcceptor(TcpListenerSocket* pServerSocket):
				Thread("DummyAcceptor"),
				mServerSocket(pServerSocket),
				mConnectSocket(0)
			{
			}
			void Run()
			{
				mConnectSocket = mServerSocket->Accept();
				deb_assert(mConnectSocket);
			}
			TcpListenerSocket* mServerSocket;
			TcpSocket* mConnectSocket;
		};
		DummyAcceptor* lAcceptor = 0;
		if (lTestOk)
		{
			lContext = "TCP acceptor start";
			lAcceptor = new DummyAcceptor(&lServer);
			lTestOk = lAcceptor->Start();
			deb_assert(lTestOk);
		}
		TcpSocket lSender(lSendAddress);
		if (lTestOk)
		{
			lContext = "TCP connect";
			lTestOk = false;
			for (int x = 0; x < 3 && !lTestOk; ++x)
			{
				Thread::Sleep(0.01);
				lTestOk	= lSender.Connect(lReceiveAddress);
			}
			deb_assert(lTestOk);
		}
		TcpSocket* lReceiver = 0;
		if (lTestOk)
		{
			lContext = "TCP accept";
			for (int x = 0; !lReceiver && x < 300; ++x)
			{
				lReceiver = lAcceptor->mConnectSocket;
				if (!lReceiver)
				{
					Thread::Sleep(0.001);
				}
			}
			lTestOk = (lReceiver != 0);
			deb_assert(lTestOk);
		}
		const int lPacketCount = 300;
		const int lPacketByteCount = 100;
		if (lTestOk)
		{
			lContext = "TCP overflow sends";
			for (int x = 0; lTestOk && x < lPacketCount; ++x)
			{
				uint8 lValue[lPacketByteCount];
				for (int y = 0; y < lPacketByteCount; ++y)
				{
					lValue[y] = (uint8)x;
				}
				lTestOk = (lSender.Send(lValue, lPacketByteCount) == lPacketByteCount);
				deb_assert(lTestOk);
			}
		}
		if (lTestOk)
		{
			lContext = "TCP underflow receive";
			Thread::Sleep(0.2f);	// On POSIX, it actually takes some time to pass through firewall and stuff.
			for (int x = 0; lTestOk && x < lPacketCount/10; ++x)
			{
				const int lReadSize = lPacketByteCount*10;
				uint8 lValue[lReadSize];
				int lActualReadSize = lReceiver->Receive(lValue, lReadSize);
				lTestOk = (lActualReadSize == lReadSize);
				deb_assert(lTestOk);
				if (lTestOk)
				{
					for (int y = 0; lTestOk && y < lReadSize; ++y)
					{
						int z = x*10+y/lPacketByteCount;
						lTestOk = (lValue[y] == (uint8)z);
						deb_assert(lTestOk);
					}
				}
			}
		}
		delete (lAcceptor);

		if (lTestOk)
		{
			lContext = "TCP algo receiver";
			lSender.Send("A1B2C3", 6);
			class TestReceiver: public DatagramReceiver
			{
				int Receive(TcpSocket* pSocket, void* pBuffer, int)
				{
					static int x = 6;
					x -= 2;
					return (pSocket->Receive(pBuffer, x));
				}
			};
			TestReceiver lAlgo;
			lReceiver->SetDatagramReceiver(&lAlgo);
			char lBuffer[6] = {0, 1, 2, 3, 4, 5};
			int lReceiveCount = lReceiver->ReceiveDatagram(lBuffer, 6);
			lTestOk = (lReceiveCount == 4 && lBuffer[0] == 'A' && lBuffer[3] == '2');
			deb_assert(lTestOk);
			if (lTestOk)
			{
				lReceiveCount = lReceiver->ReceiveDatagram(lBuffer, 6);
				lTestOk = (lReceiveCount == 2 && lBuffer[0] == 'C' && lBuffer[1] == '3');
				deb_assert(lTestOk);
			}
		}
	}

	if (lTestOk)
	{
		lContext = "network stop";
		lTestOk = Network::Stop();
		deb_assert(lTestOk);
	}

	ReportTestResult(pAccount, "Network", lContext, lTestOk);
	return (lTestOk);
}

bool TestTcpMuxSocket(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = Network::Start();

	//uint16 lAcceptPort = (uint16)Random::Uniform(40000, 50000);
	SocketAddress lAcceptAddress;
	TcpMuxSocket* lAcceptSocket = 0;
	if (lTestOk)
	{
		lContext = "resolving acceptor";
		lTestOk = lAcceptAddress.Resolve(":46767");
		//lAcceptAddress.SetPort(lAcceptPort);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "opening TCP MUX";
		lAcceptSocket = new TcpMuxSocket("T1", lAcceptAddress, true);
		lAcceptSocket->SetConnectIdTimeout(5.0);
		lTestOk = lAcceptSocket->IsOpen();
		deb_assert(lTestOk);
	}
	//uint16 lConnectPort = (uint16)Random::Uniform(40000, 50000);
	SocketAddress lConnectorAddress;
	TcpSocket* lConnectSocket = 0;
	if (lTestOk)
	{
		lContext = "resolving connector";
		lTestOk = lConnectorAddress.Resolve(":47676");
		//lConnectorAddress.SetPort(lConnectPort);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "creating TCP";
		lConnectSocket = new TcpSocket(lConnectorAddress);
		lTestOk = lConnectSocket->IsOpen();
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "connecting TCP";
		lTestOk = false;
		for (int x = 0; !lTestOk && x < 1000; ++x)
		{
			lTestOk = lConnectSocket->Connect(lAcceptAddress);
			Thread::Sleep(0.001);
		}
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "verifying TCP MUX connection count";
		unsigned lConnectionCount = 0;
		for (int x = 0; lConnectionCount == 0 && x < 500; ++x)
		{
			lConnectionCount = lAcceptSocket->GetConnectionCount();
			Thread::Sleep(0.001f);
		}
		lTestOk = (lConnectionCount == 1);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "dropping non-V TCP connect";
		lAcceptSocket->SetConnectIdTimeout(0.01);
		lTestOk = false;
		for (int x = 0; x < 3 && !lTestOk; ++x)
		{
			Thread::Sleep(0.02);
			lAcceptSocket->PollAccept();
			unsigned lConnectionCount = lAcceptSocket->GetConnectionCount();
			lTestOk = (lConnectionCount == 0);
		}
		deb_assert(lTestOk);
	}

	if (lConnectSocket)
	{
		delete (lConnectSocket);
		lConnectSocket = 0;
	}

	if (lAcceptSocket)
	{
		delete (lAcceptSocket);
		lAcceptSocket = 0;
	}

	Network::Stop();

	ReportTestResult(pAccount, "TcpMuxSocket", lContext, lTestOk);
	return (lTestOk);
}

class DualSocketClientTest
{
public:
	bool Test();

private:
	template<class _Server> bool TestClientServerTransmit(str& pContext, _Server& pServer,
		DualMuxSocket& pClientMuxSocket, DualSocket* pClientSocket, bool pSafe);

	logclass();
};

template<class _MuxSocket, class _VSocket> class ServerSocketHandler: public Thread
{
public:
	typedef _VSocket VSocket;
	typedef _VSocket* (_MuxSocket::*_AcceptMethod)();
	ServerSocketHandler(const str& pName, _MuxSocket& pServerSocket, _AcceptMethod pAcceptMethod, bool pLoop):
		Thread(pName),
		mServerSocket(0),
		mServerMuxSocket(pServerSocket),
		mAcceptMethod(pAcceptMethod),
		mLoop(pLoop)
	{
		bool started = Start();
		if (!started)
		{
			deb_assert(false);
		}
	}
	~ServerSocketHandler()
	{
		CloseSocket();
	}
	void CloseSocket()
	{
		if (mServerSocket)
		{
			mServerMuxSocket.CloseSocket(mServerSocket);
			mServerSocket = 0;
		}
	}
	_VSocket* mServerSocket;
	_MuxSocket& mServerMuxSocket;
private:
	void Run()
	{
		do
		{
			do
			{
				mServerSocket = (mServerMuxSocket.*mAcceptMethod)();
			}
			while (mLoop && !mServerSocket && !GetStopRequest());
			while (mLoop && mServerSocket && !GetStopRequest())
			{
				Thread::Sleep(0.01);
			}
		}
		while (mLoop && !GetStopRequest());
	}
	_AcceptMethod mAcceptMethod;
	bool mLoop;
	void operator=(const ServerSocketHandler&) {};
};

bool DualSocketClientTest::Test()
{
	str lContext;
	bool lTestOk = true;

	if (lTestOk)
	{
		lContext = "network startup error";
		lTestOk = Network::Start();
		deb_assert(lTestOk);
	}

	SocketAddress lLocalAddress;
	if (lTestOk)
	{
		lContext = "address resolve";
		lTestOk = lLocalAddress.Resolve(":1025");
		deb_assert(lTestOk);
	}

	// Create client.
	SocketAddress lClientAddress(lLocalAddress);
	lClientAddress.SetPort(55113);
	DualMuxSocket lClientMuxSocket("Client ", lClientAddress, false);
	if (lTestOk)
	{
		lContext = "client socket open";
		lTestOk = lClientMuxSocket.IsOpen();
		deb_assert(lTestOk);
	}

	// Make sure server connect fails (server not up yet).
	SocketAddress lServerAddress(lLocalAddress);
	lServerAddress.SetPort(55112);
	DualSocket* lClientSocket = 0;
	if (lTestOk)
	{
		mLog.Headline("Connect without TCP+UDP.");
		lContext = "client forced invalid connect";
		const std::string lId = SystemManager::GetRandomId();
		lClientSocket = lClientMuxSocket.Connect(lServerAddress, lId, 0.5);
		lTestOk = (lClientSocket == 0);
		deb_assert(lTestOk);
	}

	// Create and start TCP server (connect should fail if not UDP is up).
	{
		mLog.Headline("Connect without UDP.");
		TcpMuxSocket* lServerTcpMuxSocket = new TcpMuxSocket("Srv ", lServerAddress, true);
		if (lTestOk)
		{
			lContext = "server TCP socket open";
			lTestOk = lServerTcpMuxSocket->IsOpen();
			deb_assert(lTestOk);
		}
		ServerSocketHandler<TcpMuxSocket, TcpVSocket>* lServer =
			new ServerSocketHandler<TcpMuxSocket, TcpVSocket>(
				"TcpDummyServerSocket", *lServerTcpMuxSocket, &TcpMuxSocket::Accept, false);
		if (lTestOk)
		{
			lContext = "client connected without UDP";
			const std::string lId = SystemManager::GetRandomId();
			lClientSocket = lClientMuxSocket.Connect(lServerAddress, lId, 1.0);
			lTestOk = (lClientSocket == 0);
			deb_assert(lTestOk);
		}
		if (lTestOk)
		{
			lContext = "server did not drop client TCP connection in time";
			Thread::Sleep(0.01);
			TcpVSocket* lConnectorSocket = lServerTcpMuxSocket->PopReceiverSocket();
			if (lConnectorSocket != 0)	// Already dropped?
			{
				char a[1];
				lTestOk = (lConnectorSocket->Receive(a, 1, false) < 0);
				deb_assert(lTestOk);
			}
		}
		delete (lServer);	// Must delete manually, due to dependency on scope MUX socket.
		delete (lServerTcpMuxSocket);
	}

	// Create and start UDP server (connect should fail if not TCP is up).
	mLog.Headline("Connect without TCP.");
	UdpMuxSocket lServerUdpMuxSocket("Srv ", lServerAddress, true);
	if (lTestOk)
	{
		lContext = "server UDP socket open";
		lTestOk = lServerUdpMuxSocket.IsOpen();
		deb_assert(lTestOk);
	}
	ServerSocketHandler<UdpMuxSocket, UdpVSocket>* lUdpServer =
		new ServerSocketHandler<UdpMuxSocket, UdpVSocket>(
			"UDP Server", lServerUdpMuxSocket, &UdpMuxSocket::Accept, true);
	if (lTestOk)
	{
		lContext = "client connected without TCP";
		const std::string lId = SystemManager::GetRandomId();
		lClientSocket = lClientMuxSocket.Connect(lServerAddress, lId, 0.5);
		lTestOk = (lClientSocket == 0);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "UDP server did not accept MUX connection";
		lTestOk = (lServerUdpMuxSocket.GetConnectionCount() == 1);
		deb_assert(lTestOk);
		lUdpServer->CloseSocket();
	}
	if (lTestOk)
	{
		lContext = "server did not drop client UDP connection";
		lTestOk = (lServerUdpMuxSocket.GetConnectionCount() == 0);
		deb_assert(lTestOk);
	}

	// With both TCP and UDP sockets setup, connect should pull through.
	mLog.Headline("Connect for real.");
	TcpMuxSocket lServerTcpMuxSocket("Srv ", lServerAddress, true);
	if (lTestOk)
	{
		lContext = "server TCP socket open";
		lTestOk = lServerTcpMuxSocket.IsOpen();
		deb_assert(lTestOk);
	}
	ServerSocketHandler<TcpMuxSocket, TcpVSocket>* lTcpServer =
		new ServerSocketHandler<TcpMuxSocket, TcpVSocket>(
			"TCP Server", lServerTcpMuxSocket, &TcpMuxSocket::Accept, true);
	if (lTestOk)
	{
		lContext = "client TCP+UDP connect";
		Thread::Sleep(0.2);
		const std::string lId = SystemManager::GetRandomId();
		lClientSocket = lClientMuxSocket.Connect(lServerAddress, lId, 2.0);
		lTestOk = (lClientSocket != 0);
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lTestOk = TestClientServerTransmit(lContext, *lUdpServer, lClientMuxSocket, lClientSocket, false);
	}

	if (lTestOk)
	{
		lTestOk = TestClientServerTransmit(lContext, *lTcpServer, lClientMuxSocket, lClientSocket, true);
	}

	delete (lUdpServer);	// Must delete manually, due to dependency on scope MUX socket.
	delete (lTcpServer);	// Must delete manually, due to dependency on scope MUX socket.

	if (lTestOk)
	{
		lContext = "network shutdown error";
		lTestOk = Network::Stop();
		deb_assert(lTestOk);
	}

	ReportTestResult(mLog, "GameClientSocket", lContext, lTestOk);
	return (lTestOk);
}

template<class _Server> bool DualSocketClientTest::TestClientServerTransmit(str& pContext, _Server& pServer,
		DualMuxSocket& pClientMuxSocket, DualSocket* pClientSocket, bool pSafe)
{
	bool lTestOk = true;
	if (lTestOk)
	{
		pContext = "server append send";
		IOError lError = pServer.mServerSocket->AppendSendBuffer("Hi", 3);
		lTestOk = (lError == IO_OK);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		pContext = "server pop sender";
		typename _Server::VSocket* lSocket = pServer.mServerMuxSocket.PopSenderSocket();
		lTestOk = (lSocket == pServer.mServerSocket);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		pContext = "server send";
		int lLength = pServer.mServerSocket->SendBuffer();
		lTestOk = (lLength == 3);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		pContext = "client pop receiver";
		DualSocket* lSocket = 0;
		for (int x = 0; lSocket == 0 && x < 500; ++x)
		{
			lSocket = pClientMuxSocket.PopReceiverSocket(pSafe);
			Thread::Sleep(0.001f);
		}
		lTestOk = (lSocket == pClientSocket);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		pContext = "client receive";
		uint8 lBuffer[16];
		int lLength = pClientSocket->Receive(pSafe, lBuffer, 16);
		lTestOk = (lLength == 3 && memcmp("Hi", lBuffer, 3) == 0);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		pContext = "client append send";
		IOError lError = pClientSocket->AppendSendBuffer(pSafe, "Hi", 3);
		lTestOk = (lError == IO_OK);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		pContext = "client pop sender";
		DualSocket* lSocket = pClientMuxSocket.PopSenderSocket();
		lTestOk = (lSocket == pClientSocket);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		pContext = "client send";
		int lLength = pClientSocket->SendBuffer();
		lTestOk = (lLength == 3);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		pContext = "server pop receiver";
		typename _Server::VSocket* lSocket = 0;
		for (int x = 0; lSocket == 0 && x < 500; ++x)
		{
			lSocket = pServer.mServerMuxSocket.PopReceiverSocket();
			Thread::Sleep(0.001f);
		}
		lTestOk = (lSocket != 0 && lSocket == pServer.mServerSocket);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		pContext = "server receive";
		uint8 lBuffer[16];
		int lLength = pServer.mServerSocket->Receive(lBuffer, 16);
		lTestOk = (lLength == 3 && memcmp("Hi", lBuffer, 3) == 0);
		deb_assert(lTestOk);
	}
	return (lTestOk);
}



class DualSocketServerTest
{
public:
	bool Test();

	logclass();
};

bool DualSocketServerTest::Test()
{
	str lContext;
	bool lTestOk = true;

	if (lTestOk)
	{
		lContext = "network startup error";
		lTestOk = Network::Start();
		deb_assert(lTestOk);
	}

	SocketAddress lLocalAddress;
	if (lTestOk)
	{
		lContext = "address resolve";
		lTestOk = lLocalAddress.Resolve(":1025");
		deb_assert(lTestOk);
	}

	// Create server.
	SocketAddress lServerAddress(lLocalAddress);
	lServerAddress.SetPort(55113);
	DualMuxSocket lServerMuxSocket("Server ", lServerAddress, true);
	lServerMuxSocket.SetConnectDualTimeout(0.5f);
	if (lTestOk)
	{
		lContext = "server socket open";
		lTestOk = lServerMuxSocket.IsOpen();
		deb_assert(lTestOk);
	}
	class ServerSocketHandler: public Thread
	{
	public:
		ServerSocketHandler(DualMuxSocket& pServerSocket):
			Thread("GameServerSocket"),
			mServerMuxSocket(pServerSocket)
		{
			bool started = Start();
			if (!started)
			{
				deb_assert(false);
			}
		}
	private:
		void Run()
		{
			while (!GetStopRequest() && mServerMuxSocket.PollAccept() == 0)
			{
				Thread::Sleep(0.001);
			}
		}
		void operator=(const ServerSocketHandler&) {};
		DualMuxSocket& mServerMuxSocket;
	};

	// Check one-sided client connect.
	SocketAddress lClientAddress(lLocalAddress);
	lClientAddress.SetPort(55112);
	TcpMuxSocket lClientTcpSocket("Client", lClientAddress, false);
	if (lTestOk)
	{
		lContext = "client TCP connect failed";
		ServerSocketHandler lHandler(lServerMuxSocket);
		lTestOk = (lClientTcpSocket.Connect(lServerAddress, "Hejsan", 0.5) != 0);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "server didn't let TCP connect through";
		for (int x = 0; lServerMuxSocket.GetConnectionCount() == 0 && x < 500; ++x)
		{
			Thread::Sleep(0.001);
		}
		lTestOk = (lServerMuxSocket.GetConnectionCount() == 1);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "server let TCP-only connect stay";
		ServerSocketHandler lHandler(lServerMuxSocket);
		for (int x = 0; lServerMuxSocket.GetConnectionCount() == 1 && x < 1000; ++x)
		{
			Thread::Sleep(0.001);
		}
		lTestOk = (lServerMuxSocket.GetConnectionCount() == 0);
		deb_assert(lTestOk);
	}


	if (lTestOk)
	{
		lContext = "network shutdown error";
		lTestOk = Network::Stop();
		deb_assert(lTestOk);
	}

	ReportTestResult(mLog, "GameServerSocket", lContext, lTestOk);
	return (lTestOk);
}

loginstance(TEST, DualSocketClientTest);
loginstance(TEST, DualSocketServerTest);



bool TestArchive(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	if (lTestOk)
	{
		// TODO: implement!
		lContext = "?";
		lTestOk = true;
		deb_assert(lTestOk);
	}

	ReportTestResult(pAccount, "Archive", lContext, lTestOk);
	return (lTestOk);
}
/*
bool TestFFT(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	enum
	{
		NUM_POINTS = 256
	};

	FFT lFFT(NUM_POINTS, NUM_POINTS);
	float32 lSignal[NUM_POINTS];

	if (lTestOk)
	{
		lContext = "FFT Direct Current failed.";

		// Generate a direct current signal.
		int i;
		for (i = 0; i < NUM_POINTS; i++)
		{
			lSignal[i] = 1.234f;
		}

		lFFT.Transform(lSignal, NUM_POINTS);

		// Check if it's close to what we expected...
		float32 lAmp = (float32)lFFT.GetAmp(0);
		lTestOk = abs(lAmp - 1.234f) < 1e-5f;

		if (lTestOk)
		{
			// The rest should be zero.
			for (i = 1; lTestOk && i < NUM_POINTS; i++)
			{
				lTestOk = abs(lFFT.GetAmp(i)) < 1e-5f;
			}
		}

		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "FFT single sine failed.";

		// Generate a cosine wave signal.
		int i;
		for (i = 0; i < NUM_POINTS; i++)
		{
			lSignal[i] = (float32)(cos(2.0 * PI * (float64)i / (float64)NUM_POINTS + 0.4f) * 1.234);
		}

		lFFT.Transform(lSignal, NUM_POINTS);

		// Check if near...
		float32 lAmp = (float32)(lFFT.GetAmp(1) + lFFT.GetAmp(-1));
		lTestOk = abs(lAmp - 1.234f) < 1e-5f;

		if (lTestOk)
		{
			float32 lPhase1 = (float32)lFFT.GetPhase(1);
			float32 lPhase2 = (float32)lFFT.GetPhase(-1);
			lTestOk = (abs(lPhase1 - 0.4f) < 1e-5f) &&
					(abs(lPhase2 + 0.4f) < 1e-5f);
		}

		if (lTestOk)
		{
			// The rest should be zero.
			for (i = -NUM_POINTS / 2; i < NUM_POINTS / 2; i++)
			{
				if (i != 1 && i != -1)
				{
					lAmp = (float32)lFFT.GetAmp(i);
					lTestOk = abs(lAmp) < 1e-5f;
				}
			}
		}

		deb_assert(lTestOk);
	}

	ReportTestResult(pAccount, "FFT", lContext, lTestOk);
	return (lTestOk);
}
*/

bool TestCrypto(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	if (lTestOk)
	{
		lContext = "DES encrypt failed.";
		DES lDES;
		lDES.SetKey(LEPRA_ULONGLONG(0x1234567890FEDABC));
		const char lString[16] = "Hello World!!!!";
		uint8 lData[16];
		::memcpy(lData, lString, sizeof(lData));
		lDES.Encrypt(lData, 16);
		lTestOk = (::memcmp(lData, lString, sizeof(lData)) != 0);
		deb_assert(lTestOk);
		if (lTestOk)
		{
			lContext = "DES decrypt failed.";
			lDES.Decrypt(lData, 16);
			lTestOk = (::memcmp(lData, lString, sizeof(lData)) == 0);
			deb_assert(lTestOk);
		}
	}

	if (lTestOk)
	{
		lContext = "RSA encrypt failed.";
		// TODO: implement!
//		RSA lRSA;
//		lDES.SetKey(0x1234567890FEDABC);
//		const char* lString = "Hello World 123!";
//		uint8 lData[17];
//		::memcpy(lData, lString, sizeof(lData));
//		lDES.Encrypt(lData, 17);
//		lTestOk = (::memcmp(lData, lString, sizeof(lData)) != 0);
//		deb_assert(lTestOk);
//		if (lTestOk)
//		{
//			lContext = "RSA decrypt failed.";
//			lDES.Decrypt(lData, 17);
//			lTestOk = (::memcmp(lData, lString, sizeof(lData)) == 0);
//			deb_assert(lTestOk);
//		}
	}

	if (lTestOk)
	{
		lContext = "SHA-1 hashing failed.";
		SHA1 lSHA1;
		uint8 lHash[20];
		lSHA1.Hash((const uint8*)"Hello World!", 12, lHash);
		uint8 lWanted[20] = { 0x2e,0xf7,0xbd,0xe6,0x08,0xce,0x54,0x04,0xe9,0x7d,0x5f,0x04,0x2f,0x95,0xf8,0x9f,0x1c,0x23,0x28,0x71 };
		lTestOk = (::memcmp(lHash, lWanted, sizeof(lHash)) == 0);
		deb_assert(lTestOk);
	}

	ReportTestResult(pAccount, "Crypto", lContext, lTestOk);
	return (lTestOk);
}

void DummyThread(void*)
{
}

Lock* gMutex;
Condition* gCondition;

void ConditionThread(void*)
{
	gMutex->Acquire();
	gCondition->Wait();
	gMutex->Release();
	Thread::Sleep(0.1);
}

bool TestPerformance(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	Network::Start();

	// Loop twice: first time to register nodes in
	// the performance tree, second time to do the
	// actual measuring with high accuracy.
	for (unsigned z = 0; z < 2; ++z)
	{
		if (z == 1)	// Reset time from first loop (only measure time second loop).
		{
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
				for (int x = 0; x < 1000; ++x)
				{
					y += x;
					--y;
				}
			}

			{
				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(String);
				str lString("Apa");
				lString += "Esau";
				lString.rfind("e", lString.length()-1);
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
				Lock lMutex;
				{
					LEPRA_MEASURE_SCOPE(Acquire);
					lMutex.Acquire();
				}
				{
					LEPRA_MEASURE_SCOPE(Release);
					lMutex.Release();
				}
			}

#ifdef LEPRA_WINDOWS
			{
				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(CRITICAL_SECTION);
				CRITICAL_SECTION lSection;
				::InitializeCriticalSection(&lSection);
				{
					LEPRA_MEASURE_SCOPE(Enter);
					::EnterCriticalSection(&lSection);
				}
				{
					LEPRA_MEASURE_SCOPE(Leave);
					::LeaveCriticalSection(&lSection);
				}
				::DeleteCriticalSection(&lSection);
			}
#endif // LEPRA_WINDOWS

#ifdef LEPRA_WINDOWS
			{
				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(SpinLock);
				SpinLock lLock;
				{
					LEPRA_MEASURE_SCOPE(Acquire);
					lLock.Acquire();
				}
				{
					LEPRA_MEASURE_SCOPE(Release);
					lLock.Release();
				}
			}
#endif // LEPRA_WINDOWS

			{
				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(Semaphore);
				Semaphore lSemaphore;
				{
					LEPRA_MEASURE_SCOPE(Signal);
					lSemaphore.Signal();
				}
				{
					LEPRA_MEASURE_SCOPE(Wait);
					lSemaphore.Wait();
				}
			}

			{
				StaticThread lThread("Condition");
				Lock lMutex;
				Condition lCondition(&lMutex);
				gMutex = &lMutex;
				gCondition = &lCondition;
				lThread.Start(ConditionThread, 0);
				Thread::Sleep(0.1);	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(Condition);
				{
					LEPRA_MEASURE_SCOPE(Signal);
					lCondition.Signal();
				}
				{
					LEPRA_MEASURE_SCOPE(SignalAll);
					lCondition.SignalAll();
				}
				{
					LEPRA_MEASURE_SCOPE(Wait(0));
					lMutex.Acquire();
					lCondition.Wait(0);
					lMutex.Release();
				}
				lThread.Join();	// Ensure thread terminates before condition is destroyed.
			}

			{
				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(StaticThread);
				StaticThread lThread("PerformanceTest");
				{
					LEPRA_MEASURE_SCOPE(Start);
					lThread.Start(DummyThread, 0);
				}
				{
					LEPRA_MEASURE_SCOPE(Join);
					lThread.Join();
				}
				deb_assert(!lThread.IsRunning());
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
				char lHostname[256] = "";
				::gethostname(lHostname, sizeof(lHostname));
				sa.sin_addr.s_addr = *(unsigned int*)(gethostbyname(lHostname)->h_addr_list[0]);
				sa.sin_port = Endian::HostToBig((uint16)46666);
				lTestOk = (::bind(fd, (sockaddr*)&sa, sizeof(sa)) >= 0);
				deb_assert(lTestOk);
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
				SocketAddress lAddress;
				lAddress.Resolve(":46666");
				UdpSocket lSocket(lAddress, true);
				{
					LEPRA_MEASURE_SCOPE(SendTo);
					lSocket.SendTo((const uint8*)"Hello World", 12, lAddress);
				}
				{
					LEPRA_MEASURE_SCOPE(ReceiveFrom);
					char lBuffer[12] = "";
					lSocket.ReceiveFrom((uint8*)lBuffer, 12, lAddress);
					deb_assert(::strcmp(lBuffer, "Hello World") == 0);
				}
			}

			{
				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(UdpMuxSocket);
				SocketAddress lAddress1;
				lAddress1.Resolve(":46666");
				SocketAddress lAddress2;
				lAddress2.Resolve(":46667");
				UdpVSocket* lSocket = 0;
				UdpMuxSocket lMuxSocket1("#1 ", lAddress1, false);
				UdpMuxSocket lMuxSocket2("#2 ", lAddress2, true);
				class DummyAcceptor: public Thread
				{
				public:
					DummyAcceptor(UdpMuxSocket* pSocket):
						Thread("DummyAcceptor"),
						mSocket(pSocket)
					{
					}
				private:
					void Run()
					{
						UdpVSocket* lSocket = mSocket->Accept();
						deb_assert(lSocket);
						IOError lIo = lSocket->AppendSendBuffer("Hello Client", 13);
						deb_assert(lIo == IO_OK);
						if (lIo == IO_OK) {}	// TRICKY.
						int r = lSocket->SendBuffer();
						deb_assert(r == 13);
						if (r == 13) {}	// TRICKY.
						lSocket->WaitAvailable(0.5);
						char lBuffer[13] = "";
						lSocket->ReadRaw(lBuffer, 13);
						bool lTestOk = (::strcmp(lBuffer, "Hello Server") == 0);
						deb_assert(lTestOk);
						if (lTestOk) {}	// TRICKY.
						mSocket->CloseSocket(lSocket);
					}
					UdpMuxSocket* mSocket;
				};
				DummyAcceptor lAcceptor(&lMuxSocket2);
				lAcceptor.Start();
				{
					LEPRA_MEASURE_SCOPE(Connect);
					lSocket = lMuxSocket1.Connect(lAddress2, "", 0.5);
					deb_assert(lSocket);
				}
				{
					LEPRA_MEASURE_SCOPE(WriteRaw+Flush);
					IOError lIo = lSocket->AppendSendBuffer("Hello Server", 13);
					deb_assert(lIo == IO_OK);
					if (lIo == IO_OK) {}	// TRICKY.
					int r = lSocket->SendBuffer();
					deb_assert(r == 13);
					if (r == 13) {}	// TRICKY.
				}
				{
					LEPRA_MEASURE_SCOPE(Refill+ReadRaw);
					lSocket->WaitAvailable(0.5);
					char lBuffer[13] = "";
					lSocket->ReadRaw(lBuffer, 13);
					lTestOk = (::strcmp(lBuffer, "Hello Client") == 0);
					deb_assert(lTestOk);
				}
				{
					LEPRA_MEASURE_SCOPE(Close);
					for (int x = 0; lAcceptor.IsRunning() && x < 10; ++x)
					{
						Thread::Sleep(0.06);
					}
					lMuxSocket1.CloseSocket(lSocket);
				}
			}
		}
	}

	Network::Stop();

	if (lTestOk)
	{
		lContext = "?";
		lTestOk = true;
		deb_assert(lTestOk);
	}

	ReportTestResult(pAccount, "Performance", lContext, lTestOk);
	return (lTestOk);
}

bool TestPath(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;
	const char* lTestPath1 = "/usr/bin/.hid";
	const char* lTestPath2 = "C:\\Documents and settings\\Sverker\\Mina dokument\\.skit...apansson";
	const char* lTestPath3 = "apa_bepa0.cepa";
	const char* lTestPath4 = "d1/f3";

	if (lTestOk)
	{
		lContext = "extension 1";
		str lExtension = Path::GetExtension(lTestPath1);
		lTestOk = (lExtension == "");
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "file 1";
		str lFile = Path::GetFileBase(lTestPath1);
		lTestOk = (lFile == ".hid");
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "file 2";
		str lDir = Path::GetDirectory(lTestPath3);
		str lFile = Path::GetFileBase(lTestPath3);
		str lExt = Path::GetExtension(lTestPath3);
		lTestOk = (lDir == "" && lFile == "apa_bepa0" && lExt == "cepa");
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "file 3";
		str lDir = Path::GetDirectory(lTestPath4);
		str lFile = Path::GetFileBase(lTestPath4);
		str lExt = Path::GetExtension(lTestPath4);
		lTestOk = (lDir == "d1/" && lFile == "f3" && lExt == "");
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "directory 1";
		str lDirectory = Path::GetDirectory(lTestPath1);
		lTestOk = (lDirectory == "/usr/bin/");
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "extension 2";
		str lExtension = Path::GetExtension(lTestPath2);
		lTestOk = (lExtension == "apansson");
		deb_assert(lTestOk);
	}
#ifdef LEPRA_WINDOWS
	if (lTestOk)
	{
		lContext = "file 2";
		str lFile = Path::GetFileBase(lTestPath2);
		lTestOk = (lFile == ".skit..");
		deb_assert(lTestOk);
	}
	str lDirectory;
	if (lTestOk)
	{
		lContext = "directory 2";
		lDirectory = Path::GetDirectory(lTestPath2);
		lTestOk = (lDirectory == "C:\\Documents and settings\\Sverker\\Mina dokument\\");
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "directory 3";
		strutil::strvec lDirectoryArray = Path::SplitNodes(lDirectory);
		lTestOk = (lDirectoryArray.size() == 4 && lDirectoryArray[0] == "C:" &&
			lDirectoryArray[1] == "Documents and settings" && lDirectoryArray[2] == "Sverker" &&
			lDirectoryArray[3] == "Mina dokument");
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "directory 4";
		strutil::strvec lDirectoryArray = Path::SplitNodes("\\WINDOWS.0\\");
		lTestOk = (lDirectoryArray.size() == 1 && lDirectoryArray[0] == "WINDOWS.0");
		deb_assert(lTestOk);
	}
#endif // Windows
	if (lTestOk)
	{
		lContext = "directory 5";
		strutil::strvec lDirectoryArray = Path::SplitNodes(lTestPath1);
		lTestOk = (lDirectoryArray.size() == 3 && lDirectoryArray[0] == "usr" &&
			lDirectoryArray[1] == "bin" && lDirectoryArray[2] == ".hid");
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "split";
		strutil::strvec lPathParts = Path::SplitPath("a/b.c/d.e");
		lTestOk = (lPathParts.size() == 3 && lPathParts[0] == "a/b.c/" &&
			lPathParts[1] == "d" && lPathParts[2] == "e");
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "normalize 1";
		str lPath;
		lTestOk = Path::NormalizePath("/./apa/../", lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == "/");
		}
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "normalize 2";
		str lPath;
		lTestOk = Path::NormalizePath("/sune./apa/../", lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == "/sune./");
		}
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "normalize 3";
		str lPath;
		lTestOk = Path::NormalizePath("./apa/../", lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == "./");
		}
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "normalize 4";
		str lPath;
		lTestOk = Path::NormalizePath("./apa/..", lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == ".");
		}
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "normalize 5";
		str lPath;
		lTestOk = Path::NormalizePath("./apa/../sunk/..", lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == ".");
		}
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "normalize 6";
		str lPath;
		lTestOk = Path::NormalizePath("/apa/../sunk/..", lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == "/");
		}
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "normalize 7";
		str lPath;
		lTestOk = Path::NormalizePath("/apa/../fagott/sunk/..", lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == "/fagott");
		}
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "normalize 8";
		str lPath;
		lTestOk = Path::NormalizePath("./..", lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == "..");
		}
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "normalize 9";
		str lPath;
		lTestOk = Path::NormalizePath(".//", lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == "./");
		}
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "normalize 10";
		str lPath;
		lTestOk = Path::NormalizePath("//a", lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == "/a");
		}
		deb_assert(lTestOk);
	}
#ifdef LEPRA_WINDOWS
	if (lTestOk)
	{
		lContext = "normalize 11";
		str lPath;
		lTestOk = Path::NormalizePath("C:\\a..\\b\\..\\c", lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == "C:/a../c");
		}
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "normalize 12";
		str lPath;
		lTestOk = Path::NormalizePath("\\\\.\\C:\\a\\\\b\\.\\.\\c\\", lPath);	// UNC name.
		if (lTestOk)
		{
			lTestOk = (lPath == "\\\\.\\C:/a/b/c/");
		}
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "normalize 13";
		str lPath;
		lTestOk = Path::NormalizePath("\\\\MyServer\\$Share_1$\\.\\.\\Porn\\..\\Bible\\NT.txt", lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == "\\\\MyServer/$Share_1$/Bible/NT.txt");
		}
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "normalize 14";
		str lPath;
		lTestOk = Path::NormalizePath("..\\..\\", lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == "../../");
		}
		deb_assert(lTestOk);
	}
#endif // Windows
	if (lTestOk)
	{
		lContext = "normalize error 1";
		str lPath;
		lTestOk = !Path::NormalizePath("/../", lPath);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "wildcard error";
		lTestOk &=  Path::IsWildcardMatch("/a/b/c.txt",	"/a/b/c.txt");	deb_assert(lTestOk);
		lTestOk &=  Path::IsWildcardMatch("/a/*/c.txt",	"/a/b/c.txt");	deb_assert(lTestOk);
		lTestOk &=  Path::IsWildcardMatch("/a/b/*.txt",	"/a/b/c.txt");	deb_assert(lTestOk);
		lTestOk &=  Path::IsWildcardMatch("/*/*/c.txt",	"/a/b/c.txt");	deb_assert(lTestOk);
		lTestOk &=  Path::IsWildcardMatch("/a/*/c.txt",	"/a/bug/c.txt");	deb_assert(lTestOk);
		lTestOk &=  Path::IsWildcardMatch("*/c.txt",	"banana/c.txt");	deb_assert(lTestOk);
		lTestOk &=  Path::IsWildcardMatch("c.*",		"c.txt");		deb_assert(lTestOk);
		lTestOk &=  Path::IsWildcardMatch("*.*",		"c.txt");		deb_assert(lTestOk);
		lTestOk &=  Path::IsWildcardMatch("*.txt",		"c.txt");		deb_assert(lTestOk);
		lTestOk &=  Path::IsWildcardMatch("*.t*t",		"c.txt");		deb_assert(lTestOk);
		lTestOk &= !Path::IsWildcardMatch("/a/c/c.txt",	"/a/b/c.txt");	deb_assert(lTestOk);
		lTestOk &= !Path::IsWildcardMatch("/a*/c.txt",	"/a/b/c.txt");	deb_assert(lTestOk);
		lTestOk &= !Path::IsWildcardMatch("/*/c.txt",	"/a/b/c.txt");	deb_assert(lTestOk);
		lTestOk &= !Path::IsWildcardMatch("*/*.txt",	"/a/b/c.txt");	deb_assert(lTestOk);
		lTestOk &= !Path::IsWildcardMatch("*.txt",		"/a/b/c.txt");	deb_assert(lTestOk);
		lTestOk &= !Path::IsWildcardMatch("/a/b*",		"/a/bug/c.txt");	deb_assert(lTestOk);
		lTestOk &= !Path::IsWildcardMatch("/a/b/c.t*x",	"banana/c.txt");	deb_assert(lTestOk);
		lTestOk &= !Path::IsWildcardMatch("t*",		"c.txt");		deb_assert(lTestOk);
		lTestOk &= !Path::IsWildcardMatch("*c",		"c.txt");		deb_assert(lTestOk);
	}

	ReportTestResult(pAccount, "Path", lContext, lTestOk);
	return (lTestOk);
}

bool TestMemFileConcurrency(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	if (lTestOk)
	{
		lContext = "thread trashing";
		class FileTrasher: public Thread
		{
		public:
			FileTrasher(LogListener& pFile, Semaphore& pSemaphore):
				Thread("Trasher"),
				mFile(pFile),
				mSemaphore(pSemaphore)
			{
				Start();
			}
			void Run()
			{
				mSemaphore.Wait();
				Logger* lLog = LogType::GetLogger(LogType::GENERAL_RESOURCES);
				for (int x = 0; x < 1000; ++x)
				{
					mFile.OnLog(lLog, "?", LEVEL_TRACE);
				}
			}
			LogListener& mFile;
			Semaphore& mSemaphore;
			void operator=(const FileTrasher&) {};
		};
		const int lCount = 20;
		FileTrasher* lTrashers[lCount];
		::memset(lTrashers, 0, sizeof(lTrashers));
		MemFileLogListener lFile(5*1024);
		lFile.Clear();
		Semaphore lSemaphore;
		for (int x = 0; x < lCount; ++x)
		{
			if (!lTrashers[x])
			{
				lTrashers[x] = new FileTrasher(lFile, lSemaphore);
			}
		}
		for (int u = 0; u < lCount; ++u)
		{
			lSemaphore.Signal();
		}
		Thread::Sleep(0.1);
		for (int z = 0; z < lCount; ++z)
		{
			if (lTrashers[z]->IsRunning())
			{
				Thread::Sleep(0.001);
				z = -1;
			}
		}
		for (int y = 0; y < lCount; ++y)
		{
			delete (lTrashers[y]);
		}
	}

	ReportTestResult(pAccount, "MemFileConcurrency", lContext, lTestOk);
	return (lTestOk);
}

void ShowTestResult(const LogDecorator& pAccount, bool pTestOk)
{
	::printf("\n");
	ReportTestResult(pAccount, "Total test result", "?", pTestOk);

	// Print termination "progress bar".
	const char* lEndMessage = "Termination progress: [";
	const int lProgressBarSteps = 45;
	const double lTerminationTime = pTestOk? 2.0 : 35.0;
	::printf("\n\n%s", lEndMessage);
	for (int x = 0; x < lProgressBarSteps; ++x)
	{
		::printf(" ");
	}
	::printf("]\r%s", lEndMessage);
	for (int y = 0; y < lProgressBarSteps; ++y)
	{
		::printf("*");
		::fflush(stdout);
		Thread::Sleep(lTerminationTime/lProgressBarSteps);
	}
	printf("]\n");
}

bool TestLepra()
{
	bool lTestOk = true;

	if (lTestOk)
	{
		lTestOk = TestString(gLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestRandom(gLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestMath(gLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestNumber(gLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestVector3D(gLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestOrderedMap(gLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestBinTree(gLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestLooseBintree(gLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestLooseQuadtree(gLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestLooseOctree(gLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestIdManager(gLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestTransformation(gLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestTimers(gLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestThreading(gLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestSystemManager(gLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestNetwork(gLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestTcpMuxSocket(gLLog);
	}
	if (lTestOk)
	{
		// A special test for the mux- and virtual sockets.
		lTestOk = TestUDPSockets(gLLog);
	}
	if (lTestOk)
	{
		DualSocketClientTest lDualSocketClientTest;
		lTestOk = lDualSocketClientTest.Test();
	}
	if (lTestOk)
	{
		DualSocketServerTest lDualSocketServerTest;
		lTestOk = lDualSocketServerTest.Test();
	}
	if (lTestOk)
	{
		lTestOk = TestArchive(gLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestCrypto(gLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestPerformance(gLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestPath(gLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestMemFileConcurrency(gLLog);
	}

	return (lTestOk);
}
