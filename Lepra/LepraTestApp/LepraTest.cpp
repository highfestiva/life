
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



// Unreachable code warning below (MSVC8). For some reason just this file happens to temper with some shitty template.
#define LEPRA_INCLUDE_NO_OS
#include "../../Lepra/Include/LepraTarget.h"
#undef LEPRA_INCLUDE_NO_OS
#ifdef LEPRA_MSVC
#pragma warning(push)
#pragma warning(disable: 4702)
#endif // LEPRA_MSVC
#include "../Include/Lepra.h"
#ifdef LEPRA_MSVC
#pragma warning(pop)
#endif // LEPRA_MSVC

#include <assert.h>
#include <math.h>
#include "../Include/Canvas.h"
#include "../Include/DES.h"
#include "../Include/DiskFile.h"
#include "../Include/IdManager.h"
#include "../Include/IOBuffer.h"
#include "../Include/Lepra.h"
#include "../Include/Log.h"
#include "../Include/LogListener.h"
#include "../Include/Math.h"
#include "../Include/Network.h"
#include "../Include/Number.h"
#include "../Include/Path.h"
#include "../Include/Performance.h"
#include "../Include/Random.h"
#include "../Include/RotationMatrix.h"
#include "../Include/SHA1.h"
#include "../Include/String.h"
#include "../Include/SystemManager.h"
#include "../Include/Transformation.h"


using namespace Lepra;

class LepraTest{};
static LogDecorator gLLog(LogType::GetLog(LogType::SUB_TEST), typeid(LepraTest));



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
		pLog.Headlinef(_T("%20s: passed."), pTestName.c_str());
	}
	else
	{
		pLog.Fatalf(_T("%20s: failed (%s)!"), pTestName.c_str(), pContext.c_str());
	}
}

bool TestString(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	// Verify str basics.
	if (lTestOk)
	{
		lContext = _T("String basics");
		str lString = _T("ABCdefghijklmnopqrstUvwxyz");
		lString += _T("97531") + lString + _T("02468");
		lString.erase(31, 4);	// Remove second ABCd.
		lString = strutil::ReplaceAll(lString, _T('x'), _T(' '));
		str lTmp1 = _T("oA");
		strutil::ToUpper(lTmp1);
		str lTmp2 = _T("Oa");
		strutil::ToLower(lTmp2);
		lString.insert(10, lTmp1+lTmp2);
		lTmp1 = _T("*!\" #\t%&/\r()=\n\v");
		strutil::StripWhiteSpaces(lTmp1);
		lString.insert(30, lTmp1);
		lString.insert(0, strutil::Right(_T("Johannes"), 6) + str(_T("Sune")).substr(0, 1));
		lTestOk = (lString == _T("hannesSABCdefghijOAoaklmnopqrstUvw yz*!\"#%&/()=97531efghijklmnopqrstUvw yz02468") &&
			lString.find(_T('S'), 0) == 6 &&
			lString.rfind(_T('S'), lString.length()-1) == 6);
		assert(lTestOk);
	}

	// Verify str basics.
	if (lTestOk)
	{
		lContext = _T("String start/end");
		str lData(_T("This is the start of something new!"));
		lTestOk = (strutil::StartsWith(lData, _T("This is")) &&
			!strutil::StartsWith(lData, _T("That is")) &&
			strutil::EndsWith(lData, _T(" new!")) &&
			!strutil::EndsWith(lData, _T(" old!")));
		assert(lTestOk);
	}

	// Verify strutil::Format.
	if (lTestOk)
	{
		lContext = _T("strutil::Format()");
		int i = 123;
		float f = 123.321f;
		str lString = _T("String ") + strutil::Format(_T("format test: %i, %.4f, %s"), i, f, _T("Hello World!"));
		lTestOk = (lString == _T("String format test: 123, 123.3210, Hello World!"));
		assert(lTestOk);
	}

	// Verify str conversion to/from Ansi/Unicode.
	if (lTestOk)
	{
		lContext = _T("Ansi/Unicode conversion");
		const char* lTestData1	=  "Knastest !\"#%&/()=?'-_+\\}][{$@'*.:,;~^<>|";
		const wchar_t* lTestData2	= L"Knastest !\"#%&/()=?'-_+\\}][{$@'*.:,;~^<>|";
		wstr lUnicode = wstrutil::ToOwnCode(astr(lTestData1));
		astr lAnsi = astrutil::ToOwnCode(wstr(lTestData2));
		lTestOk = (lAnsi == lTestData1 && lUnicode == lTestData2);
		assert(lTestOk);
	}

	// Test empty int conversion.
	if (lTestOk)
	{
		lContext = _T("empty int conversion");
		int lValue = 0;
		lTestOk = !strutil::StringToInt(_T(""), lValue);
		assert(lTestOk);
	}

	// Test floating point conversion.
	if (lTestOk)
	{
		lContext = _T("float conversion");
		float64 lValue = 1.5;
		str lString = strutil::Format(_T("%.5f"), lValue);
		lTestOk = strutil::StringToDouble(lString, lValue);
		if (lTestOk)
		{
			lTestOk = (lString == _T("1.50000") && lValue == 1.5f);
		}
		assert(lTestOk);
	}

	// Test empty floating point conversion.
	if (lTestOk)
	{
		lContext = _T("empty float conversion");
		double lValue = 0;
		lTestOk = !strutil::StringToDouble(_T(""), lValue);
		assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = _T("string -> C string");
		lTestOk = (strutil::StringToCString(_T("Hej\"\\\n'!#\r\t")) == _T("Hej\\\"\\\\\\n'!#\\r\\t"));
		assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = _T("C string -> string");
		str lValue;
		lTestOk = strutil::CStringToString(_T("Hej\\\"\\\\\\n'!#\\r\\t"), lValue);
		assert(lTestOk);
		if (lTestOk)
		{
			lTestOk = (lValue == _T("Hej\"\\\n'!#\r\t"));
			assert(lTestOk);
		}
	}

	strutil::strvec lTestWords;
	if (lTestOk)
	{
		lContext = _T("plain string splitting");
		lTestWords = strutil::Split(_T("Den \"kyliga Trazan'\"\tapansson\r\n\v.\t\t"), _T(" \t\v\r\n"));
		lTestOk = (lTestWords.size() == 6 && lTestWords[0] == _T("Den") && lTestWords[1] == _T("\"kyliga") &&
			lTestWords[2] == _T("Trazan'\"") && lTestWords[3] == _T("apansson") && lTestWords[4] == _T(".") &&
			lTestWords[5].empty());
		assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = _T("string stripping");
		lTestWords[1] = strutil::StripLeft(lTestWords[1], _T("\""));
		lTestOk = (lTestWords[1] == _T("kyliga"));
		assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = _T("string joining");
		str lString = strutil::Join(lTestWords, _T(" "));
		lTestOk = (lString == _T("Den kyliga Trazan'\" apansson . "));
		assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = _T("block string splitting 1");
		strutil::strvec lWords = strutil::BlockSplit(_T("\"Hej du glade\" sade jag  \ttill\n\n\r\vhonom igen."), _T(" \t\v\r\n"), false, false, 4);
		size_t lPhraseCount = lWords.size();
		const str& lWord0 = lWords[0];
		const str& lWord1 = lWords[1];
		const str& lWord2 = lWords[2];
		const str& lWord3 = lWords[3];
		const str& lWord4 = lWords[4];
		lTestOk = (lPhraseCount == 5 && lWord0 == _T("Hej du glade") && lWord1 == _T("sade") &&
			lWord2 == _T("jag") && lWord3 == _T("till") && lWord4 == _T("\n\n\r\vhonom igen."));
		assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = _T("block string splitting 2");
		strutil::strvec lWords = strutil::BlockSplit(_T("\"Hej du glade \" sade jag  \t\"till\n\"\n\r\vhan..\nhonom igen."), _T(" \t\v\r\n"), true, false, 4);
		lTestOk = (lWords.size() == 5 && lWords[0] == _T("\"Hej du glade \"") && lWords[1] == _T("sade") &&
			lWords[2] == _T("jag") && lWords[3] == _T("\"till\n\"") && lWords[4] == _T("\n\r\vhan..\nhonom igen."));
		assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = _T("block string splitting 3");
		strutil::strvec lWords = strutil::BlockSplit(_T("\"a\\\"b\""), _T(" \t\v\r\n\""), false, true, 4);
		lTestOk = (lWords.size() == 1 && lWords[0] == _T("a\\\"b"));
		assert(lTestOk);
	}

	ReportTestResult(pAccount, _T("String"), lContext, lTestOk);
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
		lDesiredResult = _T("1.00 ");
		lContext = _T("testing ")+lDesiredResult;
		lResult = Number::ConvertToPostfixNumber(1, 2);
		lTestOk = (lResult == lDesiredResult);
		assert(lTestOk);
	}

	if (lTestOk)
	{
		lDesiredResult = _T("1.41 k");
		lContext = _T("testing ")+lDesiredResult;
		lResult = Number::ConvertToPostfixNumber(1.414444e3, 2);
		lTestOk = (lResult == lDesiredResult);
		assert(lTestOk);
	}

	if (lTestOk)
	{
		lDesiredResult = _T("7.6667 m");
		lContext = _T("rounding ")+lDesiredResult;
		lResult = Number::ConvertToPostfixNumber(7.66666666666666e-3, 4);
		lTestOk = (lResult == lDesiredResult);
		assert(lTestOk);
	}

	if (lTestOk)
	{
		lDesiredResult = _T("10 M");
		lContext = _T("testing ")+lDesiredResult;
		lResult = Number::ConvertToPostfixNumber(10e6, 0);
		lTestOk = (lResult == lDesiredResult);
		assert(lTestOk);
	}

	ReportTestResult(pAccount, _T("Number"), lContext, lTestOk);
	return (lTestOk);
}

bool TestOrderedMap(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	OrderedMap<astr, int> lMap;
	if (lTestOk)
	{
		lContext = _T("empty start");
		lTestOk = (lMap.GetCount() == 0 && lMap.Find("4") == lMap.End());
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("insert unique");
		lMap.PushBack("4",  5);
		lTestOk = (lMap.GetCount() == 1 && lMap.Find("4").GetObject() == 5);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("erase -> empty");
		lMap.Remove(lMap.Find("4"));
		lTestOk = (lMap.GetCount() == 0 && lMap.Find("4") == lMap.End());
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("insert two");
		lMap.PushBack("4",  5);
		lMap.PushBack("5",  4);
		lTestOk = (lMap.GetCount() == 2 && lMap.Find("4").GetObject() == 5 && lMap.Find("5").GetObject() == 4);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("pop front");
		astr lKey;
		int lValue;
		lMap.PopFront(lKey, lValue);
		lTestOk = (lKey == "4" && lValue == 5 && lMap.GetCount() == 1);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("pop back");
		astr lKey;
		int lValue;
		lMap.PopBack(lKey, lValue);
		lTestOk = (lKey == "5" && lValue == 4 && lMap.GetCount() == 0);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("insert three");
		lMap.PushFront("5",  4);
		lMap.PushFront("4",  5);
		lMap.PushFront("3",  3);
		lTestOk = (lMap.GetCount() == 3 && lMap.Find("3").GetObject() == 3 &&
			lMap.Find("4").GetObject() == 5 && lMap.Find("5").GetObject() == 4);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("erase mid");
		lMap.Remove("4");
		lTestOk = (lMap.GetCount() == 2 && lMap.Find("3").GetObject() == 3 &&
			lMap.Find("4") == lMap.End() && lMap.Find("5").GetObject() == 4);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("erase end");
		lMap.Remove("5");
		lTestOk = (lMap.GetCount() == 1 && lMap.Find("3").GetObject() == 3 &&
			lMap.Find("4") == lMap.End() && lMap.Find("5") == lMap.End());
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("erase last");
		lMap.Remove("3");
		lTestOk = (lMap.GetCount() == 0 && lMap.Find("3") == lMap.End() &&
			lMap.Find("4") == lMap.End() && lMap.Find("5") == lMap.End());
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("random add/remove");
		const int lEntries = 11;
		astr lSet[lEntries];
		size_t lSetCount = 0;
		for (int x = 1; lTestOk && x < 10000; ++x)
		{
			assert(lMap.GetCount() == lSetCount);
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
				assert(lTestOk);
			}
			else if (lSet[lIndex].empty())
			{
				lSet[lIndex] = ":"+astrutil::Format("%i", x);
				lTestOk = lMap.Find(lSet[lIndex]) == lMap.End();
				assert(lTestOk);
				if (lTestOk)
				{
					lMap.PushBack(lSet[lIndex], x);
					++lSetCount;
					lTestOk = (lMap.GetCount() == lSetCount &&
						lMap.Find(lSet[lIndex]).GetObject() == x &&
						lMap.Find(lSet[lIndex]) == lMap.Last());
					assert(lTestOk);
				}
			}
			else
			{
				lTestOk = (lMap.Find(lSet[lIndex]) != lMap.End());
				assert(lTestOk);
				if (lTestOk)
				{
					int lValue = -1;
					astrutil::StringToInt(lSet[lIndex].c_str()+1, lValue, 10);
					lTestOk = (lTestOk && lMap.Find(lSet[lIndex]).GetObject() == lValue);
					assert(lTestOk);
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
					assert(lTestOk);
				}
			}
		}
		lTestOk = (lMap.GetCount() == lSetCount);
		assert(lTestOk);
	}

	ReportTestResult(pAccount, _T("OrderedMap"), lContext, lTestOk);
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
		lContext = _T("min ID");
		lTestOk = (lIdManager.GetMinId() == 3);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("max ID");
		lTestOk = (lIdManager.GetMaxId() == 100);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("invalid ID");
		lTestOk = (lIdManager.GetInvalidId() == -2);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("reserve ID (4)");
		lTestOk = lIdManager.ReserveId(4);
		assert(lTestOk);
		lDebugState = lIdManager.GetDebugState();
		lTestOk = (lDebugState == _T("3-2, 3-3,\n4-4, 5-100,\n"));
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("alloc ID (3)");
		lTestOk = (lIdManager.GetFreeId() == 3);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("alloc ID (5)");
		lTestOk = (lIdManager.GetFreeId() == 5);
		lDebugState = lIdManager.GetDebugState();
		lTestOk = (lDebugState == _T("3-5, 6-100,\n"));
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("recycle ID (4)");
		lTestOk = lIdManager.RecycleId(4);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("invalid recycle ID (4)");
		lTestOk = !lIdManager.RecycleId(4);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("invalid recycle ID (4)");
		lTestOk = !lIdManager.RecycleId(4);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("invalid recycle ID (7)");
		lTestOk = !lIdManager.RecycleId(7);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("invalid recycle ID (1000)");
		lTestOk = !lIdManager.RecycleId(1000);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("invalid recycle ID (-2)");
		lTestOk = !lIdManager.RecycleId(-2);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("reserve ID (4)");
		lTestOk = lIdManager.ReserveId(4);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("invalid reserve ID (3)");
		lTestOk = !lIdManager.ReserveId(3);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("invalid reserve ID (4)");
		lTestOk = !lIdManager.ReserveId(4);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("invalid reserve ID (2)");
		lTestOk = !lIdManager.ReserveId(2);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("invalid reserve ID (-2)");
		lTestOk = !lIdManager.ReserveId(-2);
		assert(lTestOk);
	}
	for (int x = 6; lTestOk && x <= 100; ++x)
	{
		lContext = _T("alloc many IDs");
		lTestOk = (lIdManager.GetFreeId() == x);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("no free IDs");
		lTestOk = (lIdManager.GetFreeId() == -2);
		assert(lTestOk);
		lDebugState = lIdManager.GetDebugState();
		lTestOk = (lDebugState == _T("3-100, 101-100,\n"));
		assert(lTestOk);
	}
	for (int y = 0; y < 1000; ++y)
	{
		for (int z = 100; lTestOk && z >= 3; z -= 2)
		{
			lContext = _T("free dec-loop");
			lTestOk = lIdManager.RecycleId(z);
			assert(lTestOk);
		}
		for (int u = 100; lTestOk && u >= 3; u -= 2)
		{
			lContext = _T("reserve dec-loop");
			lTestOk = lIdManager.ReserveId(u);
			assert(lTestOk);
		}
		for (int v = 3; lTestOk && v <= 100; ++v)
		{
			lContext = _T("free inc-loop");
			lTestOk = lIdManager.RecycleId(v);
			assert(lTestOk);
		}
		for (int w = 3; lTestOk && w <= 100; ++w)
		{
			lContext = _T("alloc inc-loop");
			lTestOk = (lIdManager.GetFreeId() == w);
			assert(lTestOk);
		}
	}
	if (lTestOk)
	{
		// Just simple an anti-crash test.
		IdManager<int> lIdManager2(1, 0x7FFFFFFF-1, 0xFFFFFFFF);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-0, 1-2147483646,\n"));
		assert(lTestOk);

		lTestOk = lIdManager2.ReserveId(1);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-1, 2-2147483646,\n"));
		assert(lTestOk);

		lTestOk = lIdManager2.ReserveId(5);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-1, 2-4,\n5-5, 6-2147483646,\n"));
		assert(lTestOk);

		lTestOk = lIdManager2.ReserveId(3);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-1, 2-2,\n3-3, 4-4,\n5-5, 6-2147483646,\n"));
		assert(lTestOk);

		lTestOk = lIdManager2.RecycleId(1);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-0, 1-2,\n3-3, 4-4,\n5-5, 6-2147483646,\n"));
		assert(lTestOk);

		lTestOk = lIdManager2.ReserveId(2147483646);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-0, 1-2,\n3-3, 4-4,\n5-5, 6-2147483645,\n2147483646-2147483646, 2147483647-2147483646,\n"));
		assert(lTestOk);

		lTestOk = !lIdManager2.RecycleId(4);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-0, 1-2,\n3-3, 4-4,\n5-5, 6-2147483645,\n2147483646-2147483646, 2147483647-2147483646,\n"));
		assert(lTestOk);

		lTestOk = !lIdManager2.RecycleId(6);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-0, 1-2,\n3-3, 4-4,\n5-5, 6-2147483645,\n2147483646-2147483646, 2147483647-2147483646,\n"));
		assert(lTestOk);

		lTestOk = !lIdManager2.RecycleId(1);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-0, 1-2,\n3-3, 4-4,\n5-5, 6-2147483645,\n2147483646-2147483646, 2147483647-2147483646,\n"));
		assert(lTestOk);

		lTestOk = !lIdManager2.RecycleId(2147483645);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-0, 1-2,\n3-3, 4-4,\n5-5, 6-2147483645,\n2147483646-2147483646, 2147483647-2147483646,\n"));
		assert(lTestOk);

		lTestOk = lIdManager2.RecycleId(5);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-0, 1-2,\n3-3, 4-2147483645,\n2147483646-2147483646, 2147483647-2147483646,\n"));
		assert(lTestOk);

		lTestOk = lIdManager2.RecycleId(2147483646);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-0, 1-2,\n3-3, 4-2147483646,\n"));
		assert(lTestOk);

		lTestOk = lIdManager2.RecycleId(3);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-0, 1-2147483646,\n"));
		assert(lTestOk);

		lTestOk = lIdManager2.ReserveId(1);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-1, 2-2147483646,\n"));
		assert(lTestOk);

		lTestOk = lIdManager2.ReserveId(7);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-1, 2-6,\n7-7, 8-2147483646,\n"));
		assert(lTestOk);

		lTestOk = lIdManager2.ReserveId(8);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-1, 2-6,\n7-8, 9-2147483646,\n"));
		assert(lTestOk);

		lTestOk = lIdManager2.ReserveId(4);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-1, 2-3,\n4-4, 5-6,\n7-8, 9-2147483646,\n"));
		assert(lTestOk);

		lTestOk = lIdManager2.RecycleId(1);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-0, 1-3,\n4-4, 5-6,\n7-8, 9-2147483646,\n"));
		assert(lTestOk);

		lTestOk = lIdManager2.RecycleId(4);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-0, 1-6,\n7-8, 9-2147483646,\n"));
		assert(lTestOk);

		lTestOk = lIdManager2.RecycleId(8);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-0, 1-6,\n7-7, 8-2147483646,\n"));
		assert(lTestOk);

		lTestOk = lIdManager2.RecycleId(7);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-0, 1-2147483646,\n"));
		assert(lTestOk);

		lTestOk = lIdManager2.ReserveId(8);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-0, 1-7,\n8-8, 9-2147483646,\n"));
		assert(lTestOk);

		lTestOk = lIdManager2.ReserveId(7);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-0, 1-6,\n7-8, 9-2147483646,\n"));
		assert(lTestOk);

		lTestOk = lIdManager2.RecycleId(8);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-0, 1-6,\n7-7, 8-2147483646,\n"));
		assert(lTestOk);

		lTestOk = lIdManager2.RecycleId(7);
		assert(lTestOk);
		lDebugState = lIdManager2.GetDebugState();
		lTestOk = (lDebugState == _T("1-0, 1-2147483646,\n"));
		assert(lTestOk);
	}

	ReportTestResult(pAccount, _T("IdManager"), lContext, lTestOk);
	return (lTestOk);
}

bool TestTransformation(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;
	str lDebugState;

	if (lTestOk)
	{
		lContext = _T("anchor rotate 1");
		TransformationD lTransformation;
		lTransformation.SetPosition(Vector3DD(4, 1, 1));
		lTransformation.RotateAroundAnchor(Vector3DD(3, 0, -0.41), Vector3DD(0, 0, 1), PIF/2);
		lTestOk = (lTransformation.GetPosition().GetDistance(Vector3DD(2, 1, 1)) < 0.0001);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("matrix rotate");
		const Vector3DD lAxis(1, -1, 0);
		RotationMatrixD lMatrix1;
		lMatrix1.RotateAroundVector(lAxis, PI/4);
		RotationMatrixD lMatrix2;
		lMatrix2.RotateAroundVector(lAxis, -PI*3/4);
		Vector3DD lNewPosition1 = lMatrix1*Vector3DD(2, 2, 2);
		Vector3DD lNewPosition2 = lMatrix2*Vector3DD(-2, -2, -2);
		lTestOk = (lNewPosition1.GetDistance(lNewPosition2) < 0.0001 &&
			lNewPosition1.GetDistance(Vector3DD(0.4142, 0.4142, 3.4142)) < 0.0001);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("quaternion rotate");
		const Vector3DD lAxis(1, -1, 0);
		QuaternionD lQuaternion1(PI/4, lAxis);
		QuaternionD lQuaternion2(-PI*3/4, lAxis);
		Vector3DD lNewPosition1 = lQuaternion1*Vector3DD(2, 2, 2);
		Vector3DD lNewPosition2 = lQuaternion2*Vector3DD(-2, -2, -2);
		lTestOk = (lNewPosition1.GetDistance(lNewPosition2) < 0.0001 &&
			lNewPosition1.GetDistance(Vector3DD(0.4142, 0.4142, 3.4142)) < 0.0001);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("quaternion multiply");
		Vector3DD x(1, 0, 0);
		QuaternionD q1(PI/2, Vector3DD(0, -1, 0));
		x = q1*x;
		lTestOk = (x.GetDistance(Vector3DD(0, 0, 1)) < 0.0001);
		assert(lTestOk);
		if (lTestOk)
		{
			QuaternionD q2(-PI*1.5, Vector3DD(1, 0, 0));
			x = (q1*q2)*x;
			lTestOk = (x.GetDistance(Vector3DD(0, -1, 0)) < 0.0001);
			assert(lTestOk);
		}
	}
	if (lTestOk)
	{
		lContext = _T("anchor rotate 2");
		const Vector3DD lAnchor(0, -1, -1);
		TransformationD lTransformation1;
		lTransformation1.SetPosition(Vector3DD(2, 1, -1));
		const Vector3DD lDiff1(lTransformation1.GetPosition()-lAnchor);
		TransformationD lTransformation2;
		double l2Side = lDiff1.GetLength()/::sqrt(2.0);
		lTransformation2.SetPosition(Vector3DD(-l2Side, l2Side-1, -1));
		const Vector3DD lDiff2(lTransformation2.GetPosition()-lAnchor);
		assert(::fabs(lDiff1.GetLength()-lDiff2.GetLength()) < 0.0001);

		const Vector3DD lAxis1(1, -1, 0);
		lTransformation1.RotateAroundAnchor(lAnchor, lAxis1, PI/2);
		const Vector3DD lAxis2(1, 1, 0);
		lTransformation2.RotateAroundAnchor(lAnchor, lAxis2, PI/2);
		lTestOk = (lTransformation1.GetPosition().GetDistance(lTransformation2.GetPosition()) < 0.0001 &&
			lTransformation1.GetPosition().GetDistance(Vector3DD(0, -1, lDiff1.GetLength()-1)) < 0.0001);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("quaternion rotate 2");
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
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("quaternion back&forth 1");
		const QuaternionD lParentQ(0.95, Vector3DD(1, 1, 0));
		const QuaternionD lChildRelativeQ(PI/4, Vector3DD(1, 0, 0));
		const QuaternionD lChildAbsoluteQ = lChildRelativeQ*lParentQ;
		Vector3DD lVector = lChildAbsoluteQ*Vector3DD(-1, 1, 1);
		lTestOk = (lVector.GetDistance(Vector3DD(0, -1.2, 1.2)) < 0.06);
		assert(lTestOk);
		if (lTestOk)
		{
			lVector = (lChildAbsoluteQ/lParentQ)*Vector3DD(0, 1, 0);
			lTestOk = (lVector.GetDistance(Vector3DD(0, 0.707, 0.707)) < 0.001);
			assert(lTestOk);
		}
		Vector3DD lEulerAngles1;
		if (lTestOk)
		{
			(lChildAbsoluteQ/lParentQ).GetEulerAngles(lEulerAngles1);
			lTestOk = (lEulerAngles1.GetDistance(Vector3DD(0, PI/4, 0)) < 2e-11);
			assert(lTestOk);
		}
		if (lTestOk)
		{
			Vector3DD lEulerAngles2;
			lChildRelativeQ.GetEulerAngles(lEulerAngles2);
			lTestOk = (lEulerAngles1.GetDistance(lEulerAngles1) < 2e-11);
			assert(lTestOk);
		}
	}
	if (lTestOk)
	{
		lContext = _T("quaternion/matrix angle 1");
		QuaternionD lQ;
		lQ.RotateAroundOwnZ(PI/3);
		lQ.RotateAroundOwnX(-PI/7);
		lQ.RotateAroundOwnY(PI/5);
		const Vector3DD lAx = lQ.GetAxisX();
		const Vector3DD lAy = lQ.GetAxisY();
		const Vector3DD lAz = lQ.GetAxisZ();
		RotationMatrixD lMatrix(lAx, lAy, lAz);
		QuaternionD lTarget(lMatrix);
		lTestOk = (Math::IsEpsEqual(lQ.GetA(), lTarget.GetA()) &&
			Math::IsEpsEqual(lQ.GetB(), lTarget.GetB()) &&
			Math::IsEpsEqual(lQ.GetC(), lTarget.GetC()) &&
			Math::IsEpsEqual(lQ.GetD(), lTarget.GetD()));
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("vector spherical angles 1");
		double lTheta = -1;
		double lPhi = -1;
		const double l45 = ::sin(PI/4);
		Vector3DD(l45, l45, 1).GetSphericalAngles(lTheta, lPhi);
		lTestOk = (Math::IsEpsEqual(lTheta, PI/4) &&
			Math::IsEpsEqual(lPhi, PI/4));
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("vector spherical angles 2");
		double lTheta = -1;
		double lPhi = -1;
		const double l45 = ::sin(PI/4);
		Vector3DD(-l45, -l45, -1).GetSphericalAngles(lTheta, lPhi);
		lTestOk = (Math::IsEpsEqual(lTheta, PI*3/4) &&
			Math::IsEpsEqual(lPhi, PI*5/4));
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("vector spherical angles 3");
		double lTheta = -1;
		double lPhi = -1;
		Vector3DD(1, 0, 0).GetSphericalAngles(lTheta, lPhi);
		lTestOk = (Math::IsEpsEqual(lTheta, PI/2) &&
			Math::IsEpsEqual(lPhi, 0.0));
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("vector spherical angles 3");
		double lTheta = -1;
		double lPhi = -1;
		Vector3DD(0, 0, 1).GetSphericalAngles(lTheta, lPhi);
		lTestOk = (Math::IsEpsEqual(lTheta, 0.0) &&
			Math::IsEpsEqual(lPhi, 0.0));
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("vector spherical angles 4");
		double lTheta = -1;
		double lPhi = -1;
		Vector3DD(0, -1, -1).GetSphericalAngles(lTheta, lPhi);
		lTestOk = (Math::IsEpsEqual(lTheta, PI*3/4) &&
			Math::IsEpsEqual(lPhi, PI*6/4));
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("quaternion euler angles 1");

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
			assert(lTestOk);
		}
	}

	ReportTestResult(pAccount, _T("Transformation"), lContext, lTestOk);
	return (lTestOk);
}

bool TestSystemManager(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	// Directory test.
	if (lTestOk)
	{
		lContext = _T("root directory");
		str lRootDir = SystemManager::GetRootDirectory();

#if defined(LEPRA_WINDOWS)
		// Hugge: This test will fail if this app runs from another partition.
		// lTestOk = (lRootDir ==_T("C:/")); 
		str lDriveLetter = lRootDir.substr(0, 1);
		str lRest = lRootDir.substr(1);
		lTestOk = lDriveLetter >= _T("A") && 
				   lDriveLetter <= _T("Z") &&
				   lRest == _T(":/");
#elif defined(LEPRA_POSIX)
		lTestOk = (lRootDir == _T("/"));
#else // <Unknown target>
#error "Unknown target!"
#endif // LEPRA_WINDOWS/LEPRA_POSIX/<Unknown target>
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("current/documents directory");
		lTestOk = (SystemManager::GetCurrentDirectory().length() >= 4 &&
			SystemManager::GetUserDirectory().length() >= 4);
		assert(lTestOk);
	}

	// Platform/user test.
	if (lTestOk)
	{
		lContext = _T("Os name");
		lTestOk = (SystemManager::GetOsName() ==
#if defined(LEPRA_WINDOWS)
			_T("Windows NT"));
#elif defined(LEPRA_POSIX)
			_T("Posix"));
#elif defined(LEPRA_MACOSX)
			_T("MacOS X"));
#else // <Unknown target>
#error "Not implemented for this platform!"
#endif // LEPRA_WINDOWS/LEPRA_LINUX/LEPRA_MACOSX/<Unknown target>
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("login name");
		lTestOk = (SystemManager::GetLoginName().length() >= 1);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("user name");
		lTestOk = (SystemManager::QueryFullUserName().length() >= 1);
		assert(lTestOk);
	}

	// Cpu test.
	if (lTestOk)
	{
		lContext = _T("physical CPU count");
		lTestOk = (SystemManager::GetPhysicalCpuCount() >= 1 && SystemManager::GetPhysicalCpuCount() <= 4);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("logical CPU count");
		lTestOk = (SystemManager::GetLogicalCpuCount() >= 1 && SystemManager::GetLogicalCpuCount() <= 8);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("CPU core count");
		lTestOk = (SystemManager::GetCoreCount() >= 1 && SystemManager::GetCoreCount() <= 4);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("Cpu type");
		str lCpuName = SystemManager::GetCpuName();
		lTestOk = (lCpuName == _T("GenuineIntel") ||
			lCpuName == _T("AuthenticAMD"));
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("Cpu frequency");
		uint64 lCpuFrequency = SystemManager::QueryCpuFrequency();
		lTestOk = (lCpuFrequency >= 700*1000*1000 &&
			lCpuFrequency <= (uint64)6*1000*1000*1000);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("Cpu MIPS");
		unsigned lCpuMips = SystemManager::QueryCpuMips();
		lTestOk = (lCpuMips >= 50 && lCpuMips <= 3500);
		assert(lTestOk);
	}

	// Memory test.
	if (lTestOk)
	{
		lContext = _T("Ram size");
		lTestOk = (SystemManager::GetAmountRam() >= 64*1024*1024 &&
			SystemManager::GetAmountRam() <= (uint64)4*1024*1024*1024);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("available Ram >= total Ram");
		lTestOk = (SystemManager::GetAvailRam() <= SystemManager::GetAmountRam());
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("available Ram size");
		lTestOk = (SystemManager::GetAvailRam() >= 30*1024*1024 &&
			SystemManager::GetAvailRam() <= (uint64)4*1024*1024*1024);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("virtual memory size");
		lTestOk = (SystemManager::GetAmountVirtualMemory() >= 150*1024*1024 &&
			SystemManager::GetAmountVirtualMemory() <= (uint64)20*1024*1024*1024);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("available virtual memory > total virtual memory");
		lTestOk = (SystemManager::GetAvailVirtualMemory() <= SystemManager::GetAmountVirtualMemory());
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("available virtual memory size");
		lTestOk = (SystemManager::GetAvailVirtualMemory() >= 100*1024*1024 &&
			SystemManager::GetAvailVirtualMemory() <= (uint64)19*1024*1024*1024);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		// Just make sure we don't crash. Need manual verification that it works anyhoo.
		//SystemManager::WebBrowseTo(_T("http://trialepicfail.blogspot.com/"));
	}

	ReportTestResult(pAccount, _T("System"), lContext, lTestOk);
	return (lTestOk);
}

bool TestNetwork(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	if (lTestOk)
	{
		lContext = _T("network start");
		lTestOk = Network::Start();
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("local resolve");
		SocketAddress lAddress;
		lTestOk = lAddress.Resolve(_T(":1024"));
		assert(lTestOk);
		if (lTestOk)
		{
			const str lLocalAddress = lAddress.GetAsString();
			lTestOk = ((strutil::StartsWith(lLocalAddress, _T("127.0.")) &&
				strutil::EndsWith(lLocalAddress, _T(".1:1024"))) ||
				(strutil::StartsWith(lLocalAddress, _T("192.168.")) &&
				strutil::EndsWith(lLocalAddress, _T(":1024"))));
			assert(lTestOk);
		}
	}
	if (lTestOk)
	{
		lContext = _T("DNS resolve");
		IPAddress lAddress;
		lTestOk = Network::ResolveHostname(_T("ftp.sunet.se"), lAddress);
		assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = _T("UDP send");
		SocketAddress lReceiveAddress;
		lReceiveAddress.Resolve(_T(":47346"));
		SocketAddress lSendAddress;
		lSendAddress.Resolve(_T(":47347"));
		UdpSocket lReceiver(lReceiveAddress);
                assert(lReceiver.IsOpen());
		UdpSocket lSender(lSendAddress);
                assert(lSender.IsOpen());
		lTestOk = (lSender.SendTo((const uint8*)"Hello World", 12, lReceiveAddress) == 12);
		assert(lTestOk);
		if (lTestOk)
		{
			lContext = _T("UDP receive");
			uint8 lMessage[12];
			SocketAddress lSourceAddress;
			int lRecvCount = lReceiver.ReceiveFrom(lMessage, sizeof(lMessage), lSourceAddress);
			lTestOk = (lRecvCount == 12 && ::strncmp("Hello World", (const char*)lMessage, 12) == 0 &&
				lSourceAddress.GetIP() == lSendAddress.GetIP() && lSourceAddress.GetPort() == lSendAddress.GetPort());
			assert(lTestOk);
		}
	}

	if (lTestOk)
	{
		lContext = _T("TCP server init");
		SocketAddress lReceiveAddress;
		lReceiveAddress.Resolve(_T(":47346"));
		SocketAddress lSendAddress;
		lSendAddress.Resolve(_T(":47347"));
		TcpListenerSocket lServer(lReceiveAddress);
		lTestOk = lServer.IsOpen();
		assert(lTestOk);

		class DummyAcceptor: public Thread
		{
		public:
			DummyAcceptor(TcpListenerSocket* pServerSocket):
				Thread(_T("DummyAcceptor")),
				mServerSocket(pServerSocket),
				mConnectSocket(0)
			{
			}
			void Run()
			{
				mConnectSocket = mServerSocket->Accept();
				assert(mConnectSocket);
			}
			TcpListenerSocket* mServerSocket;
			TcpSocket* mConnectSocket;
		};
		DummyAcceptor* lAcceptor = 0;
		if (lTestOk)
		{
			lContext = _T("TCP acceptor start");
			lAcceptor = new DummyAcceptor(&lServer);
			lTestOk = lAcceptor->Start();
			assert(lTestOk);
		}
		TcpSocket lSender(lSendAddress);
		if (lTestOk)
		{
			lContext = _T("TCP connect");
			Thread::Sleep(0.01);
			lTestOk = lSender.Connect(lReceiveAddress);
			assert(lTestOk);
		}
		TcpSocket* lReceiver = 0;
		if (lTestOk)
		{
			lContext = _T("TCP accept");
			for (int x = 0; !lReceiver && x < 300; ++x)
			{
				lReceiver = lAcceptor->mConnectSocket;
				if (!lReceiver)
				{
					Thread::Sleep(0.001);
				}
			}
			lTestOk = (lReceiver != 0);
			assert(lTestOk);
		}
		const int lPacketCount = 300;
		const int lPacketByteCount = 100;
		if (lTestOk)
		{
			lContext = _T("TCP overflow sends");
			for (int x = 0; lTestOk && x < lPacketCount; ++x)
			{
				uint8 lValue[lPacketByteCount];
				for (int y = 0; y < lPacketByteCount; ++y)
				{
					lValue[y] = (uint8)x;
				}
				lTestOk = (lSender.Send(lValue, lPacketByteCount) == lPacketByteCount);
				assert(lTestOk);
			}
		}
		if (lTestOk)
		{
			lContext = _T("TCP underflow receive");
			Thread::Sleep(0.2f);	// On POSIX, it actually takes some time to pass through firewall and stuff.
			for (int x = 0; lTestOk && x < lPacketCount/10; ++x)
			{
				const int lReadSize = lPacketByteCount*10;
				uint8 lValue[lReadSize];
				int lActualReadSize = lReceiver->Receive(lValue, lReadSize);
				lTestOk = (lActualReadSize == lReadSize);
				assert(lTestOk);
				if (lTestOk)
				{
					for (int y = 0; lTestOk && y < lReadSize; ++y)
					{
						int z = x*10+y/lPacketByteCount;
						lTestOk = (lValue[y] == (uint8)z);
						assert(lTestOk);
					}
				}
			}
		}
		delete (lAcceptor);

		if (lTestOk)
		{
			lContext = _T("TCP algo receiver");
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
			assert(lTestOk);
			if (lTestOk)
			{
				lReceiveCount = lReceiver->ReceiveDatagram(lBuffer, 6);
				lTestOk = (lReceiveCount == 2 && lBuffer[0] == 'C' && lBuffer[1] == '3');
				assert(lTestOk);
			}
		}
	}

	if (lTestOk)
	{
		lContext = _T("network stop");
		lTestOk = Network::Stop();
		assert(lTestOk);
	}

	ReportTestResult(pAccount, _T("Network"), lContext, lTestOk);
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
		lContext = _T("resolving acceptor");
		lTestOk = lAcceptAddress.Resolve(_T("localhost:46767"));
		//lAcceptAddress.SetPort(lAcceptPort);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("opening TCP MUX");
		lAcceptSocket = new TcpMuxSocket(_T("T1"), lAcceptAddress, true);
		lAcceptSocket->SetConnectIdTimeout(5.0);
		lTestOk = lAcceptSocket->IsOpen();
		assert(lTestOk);
	}
	//uint16 lConnectPort = (uint16)Random::Uniform(40000, 50000);
	SocketAddress lConnectorAddress;
	TcpSocket* lConnectSocket = 0;
	if (lTestOk)
	{
		lContext = _T("resolving connector");
		lTestOk = lConnectorAddress.Resolve(_T("localhost:47676"));
		//lConnectorAddress.SetPort(lConnectPort);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("creating TCP");
		lConnectSocket = new TcpSocket(lConnectorAddress);
		lTestOk = lConnectSocket->IsOpen();
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("connecting TCP");
		lTestOk = false;
		for (int x = 0; !lTestOk && x < 1000; ++x)
		{
			lTestOk = lConnectSocket->Connect(lAcceptAddress);
			Thread::Sleep(0.001);
		}
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("verifying TCP MUX connection count");
		unsigned lConnectionCount = 0;
		for (int x = 0; lConnectionCount == 0 && x < 500; ++x)
		{
			lConnectionCount = lAcceptSocket->GetConnectionCount();
			Thread::Sleep(0.001f);
		}
		lTestOk = (lConnectionCount == 1);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("dropping non-V TCP connect");
		unsigned lConnectionCount = 1;
		lAcceptSocket->SetConnectIdTimeout(0.01);
		Thread::Sleep(0.02);
		lAcceptSocket->PollAccept();
		lConnectionCount = lAcceptSocket->GetConnectionCount();
		lTestOk = (lConnectionCount == 0);
		assert(lTestOk);
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

	ReportTestResult(pAccount, _T("TcpMuxSocket"), lContext, lTestOk);
	return (lTestOk);
}

class DualSocketClientTest
{
public:
	bool Test();

private:
	template<class _Server> bool TestClientServerTransmit(str& pContext, _Server& pServer,
		DualMuxSocket& pClientMuxSocket, DualSocket* pClientSocket, bool pSafe);

	LOG_CLASS_DECLARE();
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
			assert(false);
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
		lContext = _T("network startup error");
		lTestOk = Network::Start();
		assert(lTestOk);
	}

	SocketAddress lLocalAddress;
	if (lTestOk)
	{
		lContext = _T("address resolve");
		lTestOk = lLocalAddress.Resolve(_T("localhost:1025"));
		assert(lTestOk);
	}

	// Create client.
	SocketAddress lClientAddress(lLocalAddress);
	lClientAddress.SetPort(55113);
	DualMuxSocket lClientMuxSocket(_T("Client "), lClientAddress, false);
	if (lTestOk)
	{
		lContext = _T("client socket open");
		lTestOk = lClientMuxSocket.IsOpen();
		assert(lTestOk);
	}

	// Make sure server connect fails (server not up yet).
	SocketAddress lServerAddress(lLocalAddress);
	lServerAddress.SetPort(55112);
	DualSocket* lClientSocket = 0;
	if (lTestOk)
	{
		mLog.AHeadline("Connect without TCP+UDP.");
		lContext = _T("client forced invalid connect");
		lClientSocket = lClientMuxSocket.Connect(lServerAddress, 0.5);
		lTestOk = (lClientSocket == 0);
		assert(lTestOk);
	}

	// Create and start TCP server (connect should fail if not UDP is up).
	{
		mLog.AHeadline("Connect without UDP.");
		TcpMuxSocket* lServerTcpMuxSocket = new TcpMuxSocket(_T("Srv "), lServerAddress, true);
		if (lTestOk)
		{
			lContext = _T("server TCP socket open");
			lTestOk = lServerTcpMuxSocket->IsOpen();
			assert(lTestOk);
		}
		ServerSocketHandler<TcpMuxSocket, TcpVSocket>* lServer =
			new ServerSocketHandler<TcpMuxSocket, TcpVSocket>(
				_T("TcpDummyServerSocket"), *lServerTcpMuxSocket, &TcpMuxSocket::Accept, false);
		if (lTestOk)
		{
			lContext = _T("client connected without UDP");
			lClientSocket = lClientMuxSocket.Connect(lServerAddress, 1.0);
			lTestOk = (lClientSocket == 0);
			assert(lTestOk);
		}
		if (lTestOk)
		{
			lContext = _T("server did not drop client TCP connection in time");
			Thread::Sleep(0.01);
			TcpVSocket* lConnectorSocket = lServerTcpMuxSocket->PopReceiverSocket();
			if (lConnectorSocket != 0)	// Already dropped?
			{
				char a[1];
				lTestOk = (lConnectorSocket->Receive(a, 1, false) < 0);
				assert(lTestOk);
			}
		}
		delete (lServer);	// Must delete manually, due to dependency on scope MUX socket.
		delete (lServerTcpMuxSocket);
	}

	// Create and start UDP server (connect should fail if not TCP is up).
	mLog.AHeadline("Connect without TCP.");
	UdpMuxSocket lServerUdpMuxSocket(_T("Srv "), lServerAddress);
	if (lTestOk)
	{
		lContext = _T("server UDP socket open");
		lTestOk = lServerUdpMuxSocket.IsOpen();
		assert(lTestOk);
	}
	ServerSocketHandler<UdpMuxSocket, UdpVSocket>* lUdpServer =
		new ServerSocketHandler<UdpMuxSocket, UdpVSocket>(
			_T("UDP Server"), lServerUdpMuxSocket, &UdpMuxSocket::Accept, true);
	if (lTestOk)
	{
		lContext = _T("client connected without TCP");
		lClientSocket = lClientMuxSocket.Connect(lServerAddress, 0.5);
		lTestOk = (lClientSocket == 0);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("UDP server did not accept MUX connection");
		lTestOk = (lServerUdpMuxSocket.GetConnectionCount() == 1);
		assert(lTestOk);
		lUdpServer->CloseSocket();
	}
	if (lTestOk)
	{
		lContext = _T("server did not drop client UDP connection");
		lTestOk = (lServerUdpMuxSocket.GetConnectionCount() == 0);
		assert(lTestOk);
	}

	// With both TCP and UDP sockets setup, connect should pull through.
	mLog.AHeadline("Connect for real.");
	TcpMuxSocket lServerTcpMuxSocket(_T("Srv "), lServerAddress, true);
	if (lTestOk)
	{
		lContext = _T("server TCP socket open");
		lTestOk = lServerTcpMuxSocket.IsOpen();
		assert(lTestOk);
	}
	ServerSocketHandler<TcpMuxSocket, TcpVSocket>* lTcpServer =
		new ServerSocketHandler<TcpMuxSocket, TcpVSocket>(
			_T("TCP Server"), lServerTcpMuxSocket, &TcpMuxSocket::Accept, true);
	if (lTestOk)
	{
		lContext = _T("client TCP+UDP connect");
		Thread::Sleep(0.2);
		LogType::GetLog(LogType::SUB_ROOT)->SetLevelThreashold(Log::LEVEL_TRACE);
		lClientSocket = lClientMuxSocket.Connect(lServerAddress, 2.0);
		lTestOk = (lClientSocket != 0);
		assert(lTestOk);
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
		lContext = _T("network shutdown error");
		lTestOk = Network::Stop();
		assert(lTestOk);
	}

	ReportTestResult(mLog, _T("GameClientSocket"), lContext, lTestOk);
	return (lTestOk);
}

template<class _Server> bool DualSocketClientTest::TestClientServerTransmit(str& pContext, _Server& pServer,
		DualMuxSocket& pClientMuxSocket, DualSocket* pClientSocket, bool pSafe)
{
	bool lTestOk = true;
	if (lTestOk)
	{
		pContext = _T("server append send");
		IOError lError = pServer.mServerSocket->AppendSendBuffer("Hi", 3);
		lTestOk = (lError == IO_OK);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		pContext = _T("server pop sender");
		typename _Server::VSocket* lSocket = pServer.mServerMuxSocket.PopSenderSocket();
		lTestOk = (lSocket == pServer.mServerSocket);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		pContext = _T("server send");
		int lLength = pServer.mServerSocket->SendBuffer();
		lTestOk = (lLength == 3);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		pContext = _T("client pop receiver");
		DualSocket* lSocket = 0;
		for (int x = 0; lSocket == 0 && x < 500; ++x)
		{
			lSocket = pClientMuxSocket.PopReceiverSocket(pSafe);
			Thread::Sleep(0.001f);
		}
		lTestOk = (lSocket == pClientSocket);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		pContext = _T("client receive");
		uint8 lBuffer[16];
		int lLength = pClientSocket->Receive(pSafe, lBuffer, 16);
		lTestOk = (lLength == 3 && memcmp("Hi", lBuffer, 3) == 0);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		pContext = _T("client append send");
		IOError lError = pClientSocket->AppendSendBuffer(pSafe, "Hi", 3);
		lTestOk = (lError == IO_OK);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		pContext = _T("client pop sender");
		DualSocket* lSocket = pClientMuxSocket.PopSenderSocket();
		lTestOk = (lSocket == pClientSocket);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		pContext = _T("client send");
		int lLength = pClientSocket->SendBuffer();
		lTestOk = (lLength == 3);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		pContext = _T("server pop receiver");
		typename _Server::VSocket* lSocket = 0;
		for (int x = 0; lSocket == 0 && x < 500; ++x)
		{
			lSocket = pServer.mServerMuxSocket.PopReceiverSocket();
			Thread::Sleep(0.001f);
		}
		lTestOk = (lSocket != 0 && lSocket == pServer.mServerSocket);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		pContext = _T("server receive");
		uint8 lBuffer[16];
		int lLength = pServer.mServerSocket->Receive(lBuffer, 16);
		lTestOk = (lLength == 3 && memcmp("Hi", lBuffer, 3) == 0);
		assert(lTestOk);
	}
	return (lTestOk);
}



class DualSocketServerTest
{
public:
	bool Test();

	LOG_CLASS_DECLARE();
};

bool DualSocketServerTest::Test()
{
	str lContext;
	bool lTestOk = true;

	if (lTestOk)
	{
		lContext = _T("network startup error");
		lTestOk = Network::Start();
		assert(lTestOk);
	}

	SocketAddress lLocalAddress;
	if (lTestOk)
	{
		lContext = _T("address resolve");
		lTestOk = lLocalAddress.Resolve(_T("localhost:1025"));
		assert(lTestOk);
	}

	// Create server.
	SocketAddress lServerAddress(lLocalAddress);
	lServerAddress.SetPort(55113);
	DualMuxSocket lServerMuxSocket(_T("Server "), lServerAddress, true);
	lServerMuxSocket.SetConnectDualTimeout(0.5f);
	if (lTestOk)
	{
		lContext = _T("server socket open");
		lTestOk = lServerMuxSocket.IsOpen();
		assert(lTestOk);
	}
	class ServerSocketHandler: public Thread
	{
	public:
		ServerSocketHandler(DualMuxSocket& pServerSocket):
			Thread(_T("GameServerSocket")),
			mServerMuxSocket(pServerSocket)
		{
			bool started = Start();
			if (!started)
			{
				assert(false);
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
	TcpMuxSocket lClientTcpSocket(_T("Client"), lClientAddress, false);
	if (lTestOk)
	{
		lContext = _T("client TCP connect failed");
		ServerSocketHandler lHandler(lServerMuxSocket);
		lTestOk = (lClientTcpSocket.Connect(lServerAddress, "Hejsan", 0.5) != 0);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("server didn't let TCP connect through");
		for (int x = 0; lServerMuxSocket.GetConnectionCount() == 0 && x < 500; ++x)
		{
			Thread::Sleep(0.001);
		}
		lTestOk = (lServerMuxSocket.GetConnectionCount() == 1);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("server let TCP-only connect stay");
		ServerSocketHandler lHandler(lServerMuxSocket);
		for (int x = 0; lServerMuxSocket.GetConnectionCount() == 1 && x < 1000; ++x)
		{
			Thread::Sleep(0.001);
		}
		lTestOk = (lServerMuxSocket.GetConnectionCount() == 0);
		assert(lTestOk);
	}


	if (lTestOk)
	{
		lContext = _T("network shutdown error");
		lTestOk = Network::Stop();
		assert(lTestOk);
	}

	ReportTestResult(mLog, _T("GameServerSocket"), lContext, lTestOk);
	return (lTestOk);
}

LOG_CLASS_DEFINE(TEST, DualSocketClientTest);
LOG_CLASS_DEFINE(TEST, DualSocketServerTest);



bool TestArchive(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	if (lTestOk)
	{
		// TODO: implement!
		lContext = _T("?");
		lTestOk = true;
		assert(lTestOk);
	}

	ReportTestResult(pAccount, _T("Archive"), lContext, lTestOk);
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
		lContext = _T("FFT Direct Current failed.");

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

		assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = _T("FFT single sine failed.");

		// Generate a cosine wave signal.
		int i;
		for (i = 0; i < NUM_POINTS; i++)
		{
			lSignal[i] = (float32)(cos(2.0 * PI * (float64)i / (float64)NUM_POINTS + 0.4f) * 1.234);
		}

		lFFT.Transform(lSignal, NUM_POINTS);

		// Check if near...
		float32 lAmp   = (float32)(lFFT.GetAmp(1) + lFFT.GetAmp(-1));
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

		assert(lTestOk);
	}

	ReportTestResult(pAccount, _T("FFT"), lContext, lTestOk);
	return (lTestOk);
}

bool TestCrypto(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	if (lTestOk)
	{
		lContext = _T("DES encrypt failed.");
		DES lDES;
		lDES.SetKey(0x1234567890FEDABC);
		const char lString[16] = "Hello World!!!!";
		uint8 lData[16];
		::memcpy(lData, lString, sizeof(lData));
		lDES.Encrypt(lData, 16);
		lTestOk = (::memcmp(lData, lString, sizeof(lData)) != 0);
		assert(lTestOk);
		if (lTestOk)
		{
			lContext = _T("DES decrypt failed.");
			lDES.Decrypt(lData, 16);
			lTestOk = (::memcmp(lData, lString, sizeof(lData)) == 0);
			assert(lTestOk);
		}
	}

	if (lTestOk)
	{
		lContext = _T("RSA encrypt failed.");
		// TODO: implement!
//		RSA lRSA;
//		lDES.SetKey(0x1234567890FEDABC);
//		const char* lString = "Hello World 123!";
//		uint8 lData[17];
//		::memcpy(lData, lString, sizeof(lData));
//		lDES.Encrypt(lData, 17);
//		lTestOk = (::memcmp(lData, lString, sizeof(lData)) != 0);
//		assert(lTestOk);
//		if (lTestOk)
//		{
//			lContext = _T("RSA decrypt failed.");
//			lDES.Decrypt(lData, 17);
//			lTestOk = (::memcmp(lData, lString, sizeof(lData)) == 0);
//			assert(lTestOk);
//		}
	}

	if (lTestOk)
	{
		lContext = _T("SHA-1 hashing failed.");
		SHA1 lSHA1;
		uint8 lHash[20];
		lSHA1.Hash((const uint8*)"Hello World!", 13, lHash);
		// TODO: implement!
//		lTestOk = (::memcmp(lData, lString, sizeof(lData)) != 0);
//		assert(lTestOk);
	}

	ReportTestResult(pAccount, _T("Crypto"), lContext, lTestOk);
	return (lTestOk);
}
*/

void DummyThread(void*)
{
}

Condition* gCondition;

void ConditionThread(void*)
{
	gCondition->Wait();
	Thread::Sleep(0.1);
}

CompatibleCondition* gCompatibleCondition;

void CompatibleConditionThread(void*)
{
	gCompatibleCondition->Wait();
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
			ScopePerformanceData::ClearAll(ScopePerformanceData::GetRoots());
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
				str lString(_T("Apa"));
				lString += _T("Esau");
				lString.rfind(_T("e"), lString.length()-1);
			}

//			{
//				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
//				LEPRA_MEASURE_SCOPE(BigInt);
//				BigInt lBigInt(_T("84879234798733231872345687123101"));
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

			{
				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(CompatibleLock);
				CompatibleLock lMutex;
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
				StaticThread lThread(_T("Condition"));
				Condition lCondition;
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
					lCondition.Wait(0);
				}
			}

			{
				StaticThread lThread(_T("CompatibleCondition"));
				CompatibleCondition lCondition;
				gCompatibleCondition = &lCondition;
				lThread.Start(CompatibleConditionThread, 0);
				Thread::Sleep(0.1);	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(CompatibleCondition);
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
					lCondition.Wait(0);
				}
			}

			{
				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(StaticThread);
				StaticThread lThread(_T("PerformanceTest"));
				{
					LEPRA_MEASURE_SCOPE(Start);
					lThread.Start(DummyThread, 0);
				}
				{
					LEPRA_MEASURE_SCOPE(Join);
					lThread.Join();
				}
				assert(!lThread.IsRunning());
			}
		}

		{
			LEPRA_MEASURE_SCOPE(Networkcategory);

			{
				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(sys_socket(UDP));
				sys_socket fd = ::socket(PF_INET, SOCK_DGRAM, 0);
				assert(fd > 0);
				sockaddr_in sa;
				::memset(&sa, 0, sizeof(sa));
				sa.sin_family = AF_INET;
				char lHostname[256] = "";
				::gethostname(lHostname, sizeof(lHostname));
				sa.sin_addr.s_addr = *(unsigned int*)(gethostbyname(lHostname)->h_addr_list[0]);
				sa.sin_port = Endian::HostToBig((uint16)46666);
				lTestOk = (::bind(fd, (sockaddr*)&sa, sizeof(sa)) >= 0);
				assert(lTestOk);
				{
					LEPRA_MEASURE_SCOPE(sendto);
					::sendto(fd, "Hello World", 12, 0, (sockaddr*)&sa, sizeof(sa));
				}
				{
					LEPRA_MEASURE_SCOPE(recvfrom);
					char buf[12] = "";
					socklen_t fromlen = sizeof(sa);
					::recvfrom(fd, buf, 12, 0, (sockaddr*)&sa, &fromlen);
					assert(::strcmp(buf, "Hello World") == 0);
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
				lAddress.Resolve(_T("localhost:46666"));
				UdpSocket lSocket(lAddress);
				{
					LEPRA_MEASURE_SCOPE(SendTo);
					lSocket.SendTo((const uint8*)"Hello World", 12, lAddress);
				}
				{
					LEPRA_MEASURE_SCOPE(ReceiveFrom);
					char lBuffer[12] = "";
					lSocket.ReceiveFrom((uint8*)lBuffer, 12, lAddress);
					assert(::strcmp(lBuffer, "Hello World") == 0);
				}
			}

			{
				Thread::YieldCpu();	// Yield to not get a starved Cpu time slice.
				LEPRA_MEASURE_SCOPE(UdpMuxSocket);
				SocketAddress lAddress1;
				lAddress1.Resolve(_T("localhost:46666"));
				SocketAddress lAddress2;
				lAddress2.Resolve(_T("localhost:46667"));
				UdpVSocket* lSocket = 0;
				UdpMuxSocket lMuxSocket1(_T("#1 "), lAddress1);
				UdpMuxSocket lMuxSocket2(_T("#2 "), lAddress2);
				class DummyAcceptor: public Thread
				{
				public:
					DummyAcceptor(UdpMuxSocket* pSocket):
						Thread(_T("DummyAcceptor")),
						mSocket(pSocket)
					{
					}
				private:
					void Run()
					{
						UdpVSocket* lSocket = mSocket->Accept();
						assert(lSocket);
						IOError lIo = lSocket->AppendSendBuffer("Hello Client", 13);
						assert(lIo == IO_OK);
						if (lIo == IO_OK) {}	// TRICKY.
						int r = lSocket->SendBuffer();
						assert(r == 13);
						if (r == 13) {}	// TRICKY.
						lSocket->WaitAvailable(0.5);
						char lBuffer[13] = "";
						lSocket->ReadRaw(lBuffer, 13);
						bool lTestOk = (::strcmp(lBuffer, "Hello Server") == 0);
						assert(lTestOk);
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
					assert(lSocket);
				}
				{
					LEPRA_MEASURE_SCOPE(WriteRaw+Flush);
					IOError lIo = lSocket->AppendSendBuffer("Hello Server", 13);
					assert(lIo == IO_OK);
					if (lIo == IO_OK) {}	// TRICKY.
					int r = lSocket->SendBuffer();
					assert(r == 13);
					if (r == 13) {}	// TRICKY.
				}
				{
					LEPRA_MEASURE_SCOPE(Refill+ReadRaw);
					lSocket->WaitAvailable(0.5);
					char lBuffer[13] = "";
					lSocket->ReadRaw(lBuffer, 13);
					lTestOk = (::strcmp(lBuffer, "Hello Client") == 0);
					assert(lTestOk);
				}
				{
					LEPRA_MEASURE_SCOPE(Close);
					lMuxSocket1.CloseSocket(lSocket);
				}
			}
		}
	}

	Network::Stop();

	if (lTestOk)
	{
		lContext = _T("?");
		lTestOk = true;
		assert(lTestOk);
	}

	ReportTestResult(pAccount, _T("Performance"), lContext, lTestOk);
	return (lTestOk);
}

bool TestPath(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;
	const tchar* lTestPath1 = _T("/usr/bin/.hid");
	const tchar* lTestPath2 = _T("C:\\Documents and settings\\Sverker\\Mina dokument\\.skit...apansson");

	if (lTestOk)
	{
		lContext = _T("extension 1");
		str lExtension = Path::GetExtension(lTestPath1);
		lTestOk = (lExtension == _T(""));
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("file 1");
		str lFile = Path::GetFileBase(lTestPath1);
		lTestOk = (lFile == _T(".hid"));
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("directory 1");
		str lDirectory = Path::GetDirectory(lTestPath1);
		lTestOk = (lDirectory == _T("/usr/bin/"));
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("extension 2");
		str lExtension = Path::GetExtension(lTestPath2);
		lTestOk = (lExtension == _T("apansson"));
		assert(lTestOk);
	}
#ifdef LEPRA_WINDOWS
	if (lTestOk)
	{
		lContext = _T("file 2");
		str lFile = Path::GetFileBase(lTestPath2);
		lTestOk = (lFile == _T(".skit.."));
		assert(lTestOk);
	}
	str lDirectory;
	if (lTestOk)
	{
		lContext = _T("directory 2");
		lDirectory = Path::GetDirectory(lTestPath2);
		lTestOk = (lDirectory == _T("C:\\Documents and settings\\Sverker\\Mina dokument\\"));
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("directory 3");
		strutil::strvec lDirectoryArray = Path::SplitNodes(lDirectory);
		lTestOk = (lDirectoryArray.size() == 4 && lDirectoryArray[0] == _T("C:") &&
			lDirectoryArray[1] == _T("Documents and settings") && lDirectoryArray[2] == _T("Sverker") &&
			lDirectoryArray[3] == _T("Mina dokument"));
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("directory 4");
		strutil::strvec lDirectoryArray = Path::SplitNodes(_T("\\WINDOWS.0\\"));
		lTestOk = (lDirectoryArray.size() == 1 && lDirectoryArray[0] == _T("WINDOWS.0"));
		assert(lTestOk);
	}
#endif // Windows
	if (lTestOk)
	{
		lContext = _T("directory 5");
		strutil::strvec lDirectoryArray = Path::SplitNodes(lTestPath1);
		lTestOk = (lDirectoryArray.size() == 3 && lDirectoryArray[0] == _T("usr") &&
			lDirectoryArray[1] == _T("bin") && lDirectoryArray[2] == _T(".hid"));
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("split");
		strutil::strvec lPathParts = Path::SplitPath(_T("a/b.c/d.e"));
		lTestOk = (lPathParts.size() == 3 && lPathParts[0] == _T("a/b.c/") &&
			lPathParts[1] == _T("d") && lPathParts[2] == _T("e"));
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("normalize 1");
		str lPath;
		lTestOk = Path::NormalizePath(_T("/./apa/../"), lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == _T("/"));
		}
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("normalize 2");
		str lPath;
		lTestOk = Path::NormalizePath(_T("/sune./apa/../"), lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == _T("/sune./"));
		}
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("normalize 3");
		str lPath;
		lTestOk = Path::NormalizePath(_T("./apa/../"), lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == _T("./"));
		}
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("normalize 4");
		str lPath;
		lTestOk = Path::NormalizePath(_T("./apa/.."), lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == _T("."));
		}
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("normalize 5");
		str lPath;
		lTestOk = Path::NormalizePath(_T("./apa/../sunk/.."), lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == _T("."));
		}
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("normalize 6");
		str lPath;
		lTestOk = Path::NormalizePath(_T("/apa/../sunk/.."), lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == _T("/"));
		}
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("normalize 7");
		str lPath;
		lTestOk = Path::NormalizePath(_T("/apa/../fagott/sunk/.."), lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == _T("/fagott"));
		}
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("normalize 8");
		str lPath;
		lTestOk = Path::NormalizePath(_T("./.."), lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == _T(".."));
		}
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("normalize 9");
		str lPath;
		lTestOk = Path::NormalizePath(_T(".//"), lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == _T("./"));
		}
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("normalize 10");
		str lPath;
		lTestOk = Path::NormalizePath(_T("//a"), lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == _T("/a"));
		}
		assert(lTestOk);
	}
#ifdef LEPRA_WINDOWS
	if (lTestOk)
	{
		lContext = _T("normalize 11");
		str lPath;
		lTestOk = Path::NormalizePath(_T("C:\\a..\\b\\..\\c"), lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == _T("C:/a../c"));
		}
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("normalize 12");
		str lPath;
		lTestOk = Path::NormalizePath(_T("\\\\.\\C:\\a\\\\b\\.\\.\\c\\"), lPath);	// UNC name.
		if (lTestOk)
		{
			lTestOk = (lPath == _T("\\\\.\\C:/a/b/c/"));
		}
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("normalize 13");
		str lPath;
		lTestOk = Path::NormalizePath(_T("\\\\MyServer\\$Share_1$\\.\\.\\Porn\\..\\Bible\\NT.txt"), lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == _T("\\\\MyServer/$Share_1$/Bible/NT.txt"));
		}
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("normalize 14");
		str lPath;
		lTestOk = Path::NormalizePath(_T("..\\..\\"), lPath);
		if (lTestOk)
		{
			lTestOk = (lPath == _T("../../"));
		}
		assert(lTestOk);
	}
#endif // Windows
	if (lTestOk)
	{
		lContext = _T("normalize error 1");
		str lPath;
		lTestOk = !Path::NormalizePath(_T("/../"), lPath);
		assert(lTestOk);
	}

	ReportTestResult(pAccount, _T("Path"), lContext, lTestOk);
	return (lTestOk);
}

bool TestMemFileConcurrency(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	if (lTestOk)
	{
		lContext = _T("thread trashing");
		class FileTrasher: public Thread
		{
		public:
			FileTrasher(LogListener& pFile, Semaphore& pSemaphore):
				Thread(_T("Trasher")),
				mFile(pFile),
				mSemaphore(pSemaphore)
			{
				Start();
			}
			void Run()
			{
				mSemaphore.Wait();
				Log* lLog = LogType::GetLog(LogType::SUB_GENERAL_RESOURCES);
				for (int x = 0; x < 1000; ++x)
				{
					mFile.OnLog(lLog, _T("?"), Log::LEVEL_TRACE);
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

	ReportTestResult(pAccount, _T("MemFileConcurrency"), lContext, lTestOk);
	return (lTestOk);
}

void ShowTestResult(const LogDecorator& pAccount, bool pTestOk)
{
	::printf("\n");
	ReportTestResult(pAccount, _T("Total test result"), _T("?"), pTestOk);

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
		lTestOk = TestNumber(gLLog);
	}
	/*if (lTestOk)
	{
		lTestOk = TestSkipList(gLLog);
	}*/
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
	/*if (lTestOk)
	{
		DualSocketClientTest lDualSocketClientTest;
		lTestOk = lDualSocketClientTest.Test();
	}
	if (lTestOk)
	{
		DualSocketServerTest lDualSocketServerTest;
		lTestOk = lDualSocketServerTest.Test();
	}*/
	if (lTestOk)
	{
		lTestOk = TestArchive(gLLog);
	}
//	if (lTestOk)
//	{
//		lTestOk = TestFFT(gLLog);
//	}
//	if (lTestOk)
//	{
//		lTestOk = TestCrypto(gLLog);
//	}
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
