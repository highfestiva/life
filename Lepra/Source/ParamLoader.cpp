/*
	Class:  ParamLoader
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include "../Include/LepraTypes.h"
#include "../Include/ParamLoader.h"
#include "../Include/DiskFile.h"
#include "../Include/ArchiveFile.h"
#include "../Include/String.h"
#include "../Include/String.h"
#include "../Include/Thread.h"
#include "../Include/Log.h"

#include <math.h>

namespace Lepra
{

ParamLoader::ParamLoader()
{
}

ParamLoader::~ParamLoader()
{
}

void ParamLoader::Load(Reader& pReader, Params& pParams)
{
	unsigned lLine = 1;
	tchar lChar;
	while (pReader.Read(lChar) == IO_OK)
	{
		ReadNewExpression(pReader, pParams, lChar, lLine);
	}
}

void ParamLoader::Save(Writer& pWriter, Params& pParams)
{
	Save(pWriter, pParams, 0);
}

void ParamLoader::LoadOneExpression(Reader& pReader, Params& pParams)
{
	unsigned lLine = 1;
	tchar lChar;
	ReadNewExpression(pReader, pParams, lChar, lLine);
}

void ParamLoader::Load(const String& pFileName, Params& pParams)
{
	DiskFile lFile;
	if (lFile.Open(pFileName, DiskFile::MODE_READ_ONLY) == true)
	{
		Load(lFile, pParams);
		lFile.Close();
	}
}

void ParamLoader::Save(const String& pFileName, Params& pParams)
{
	DiskFile lFile;
	if (lFile.Open(pFileName, DiskFile::MODE_WRITE_ONLY) == true)
	{
		Save(lFile, pParams);
		lFile.Close();
	}
}

void ParamLoader::Load(const String& pArchiveName, const String& pFileName, Params& pParams)
{
	ArchiveFile lFile(pArchiveName);
	if (lFile.Open(pFileName, ArchiveFile::READ_ONLY) == true)
	{
		Load(lFile, pParams);
		lFile.Close();
	}
}

void ParamLoader::Save(const String& pArchiveName, const String& pFileName, Params& pParams)
{
	ArchiveFile lFile(pArchiveName);
	if (lFile.Open(pFileName, ArchiveFile::WRITE_ONLY) == true)
	{
		Save(lFile, pParams);
		lFile.Close();
	}
}

void ParamLoader::Save(Writer& pWriter, Params& pParams, int pTabs, bool pSkipFirstTab)
{
	String lVarName;
	Params::VariableType lType;
	Params::SubType lSubType;
	int lCount;

	bool lContinue = pParams.GetFirstVariable(lVarName, lType, lCount, &lSubType);
	bool lFirst = true;

	while (lContinue == true)
	{
		int i;

		if (lFirst == false || pSkipFirstTab == false)
		{
			for (i = 0; i < pTabs; i++)
				pWriter.Write('\t');
		}

		lFirst = false;

		bool lBinary = false;
		switch(lType)
		{
		case Params::VT_INT:
			switch(lSubType)
			{
			case Params::ST_INT8:
				pWriter.WriteLine(_T("int8 "));
				pWriter.WriteLine(lVarName);
				pWriter.WriteLine(StringUtility::Format(_T("[%i] =\r\n"), lCount));
				lBinary = true;
				break;
			case Params::ST_INT16:
				pWriter.WriteLine(_T("int16 "));
				pWriter.WriteLine(lVarName);
				pWriter.WriteLine(StringUtility::Format(_T("[%i] =\r\n"), lCount));
				lBinary = true;
				break;
			case Params::ST_INT32:
				pWriter.WriteLine(_T("int "));
				pWriter.WriteLine(lVarName);
				pWriter.WriteLine(StringUtility::Format(_T("[%i] =\r\n"), lCount));
				lBinary = true;
				break;
			case Params::ST_INT:
			default:
				pWriter.WriteLine(_T("int "));
				pWriter.WriteLine(lVarName);
				break;
			}
			break;
		case Params::VT_FLOAT:
			switch(lSubType)
			{
			case Params::ST_FLOAT32:
				pWriter.WriteLine(_T("float32 "));
				pWriter.WriteLine(lVarName);
				pWriter.WriteLine(StringUtility::Format(_T("[%i] =\r\n"), lCount));
				lBinary = true;
				break;
			case Params::ST_FLOAT64:
				pWriter.WriteLine(_T("float64 "));
				pWriter.WriteLine(lVarName);
				pWriter.WriteLine(StringUtility::Format(_T("[%i] =\r\n"), lCount));
				lBinary = true;
				break;
			case Params::ST_FLOAT:
			default:
				pWriter.WriteLine(_T("float "));
				pWriter.WriteLine(lVarName);
				break;
			}
			break;
		case Params::VT_STRING:
			pWriter.WriteLine(_T("string "));
			pWriter.WriteLine(lVarName);
			break;
		case Params::VT_STRUCT:
			pWriter.WriteLine(_T("struct "));
			pWriter.WriteLine(lVarName);
			break;
		}

		if (lBinary == true)
		{
			int i;
			switch(lSubType)
			{
			case Params::ST_INT8:
				{
					int8* lInt = (int8*)pParams.GetCurrentIntPointer();
					for (i = 0; i < lCount; i++)
					{
						pWriter.Write(lInt[i]);
					}
				}
				break;
			case Params::ST_INT16:
				{
					int16* lInt = (int16*)pParams.GetCurrentIntPointer();
					for (i = 0; i < lCount; i++)
					{
						pWriter.Write(lInt[i]);
					}
				}
				break;
			case Params::ST_INT32:
				{
					int* lInt = (int*)pParams.GetCurrentIntPointer();
					for (i = 0; i < lCount; i++)
					{
						pWriter.Write(lInt[i]);
					}
				}
				break;
			case Params::ST_FLOAT32:
				{
					float32* lFloat = (float32*)pParams.GetCurrentFloatPointer();
					for (i = 0; i < lCount; i++)
					{
						pWriter.Write(lFloat[i]);
					}
				}
				break;
			case Params::ST_FLOAT64:
				{
					float64* lFloat = (float64*)pParams.GetCurrentFloatPointer();
					for (i = 0; i < lCount; i++)
					{
						pWriter.Write(lFloat[i]);
					}
				}
				break;
			}
		}
		else
		{
			if (lCount == 1)
			{
				switch(lType)
				{
				case Params::VT_INT:
					{
						String lString = StringUtility::Format(_T(" = %i"), pParams.GetIntValue(lVarName, 0));
						pWriter.WriteLine(lString);
					}
					break;
				case Params::VT_FLOAT:
					{
						String lString = StringUtility::Format(_T(" = %f"), pParams.GetFloatValue(lVarName, 0));
						pWriter.WriteLine(lString);
					}
					break;
				case Params::VT_STRING:
					pWriter.WriteLine(_T(" = \""));
					pWriter.WriteLine(pParams.GetStringValue(lVarName, 0));
					pWriter.WriteLine(_T("\""));
					break;
				case Params::VT_STRUCT:
					pWriter.WriteLine(_T("\r\n"));
					for (i = 0; i < pTabs; i++)
					{
						pWriter.WriteLine(_T("\t"));
					}
					pWriter.WriteLine(_T("{\r\n"));

					Save(pWriter, *pParams.GetStruct(lVarName, 0), pTabs + 1);

					for (i = 0; i < pTabs; i++)
					{
						pWriter.WriteLine(_T("\t"));
					}
					pWriter.WriteLine(_T("}\r\n"));
					break;
				};
			}
			else
			{
				switch(lType)
				{
				case Params::VT_INT:
				case Params::VT_FLOAT:
					pWriter.WriteLine(_T(" = ["));
					break;
				case Params::VT_STRING:
				case Params::VT_STRUCT:
					pWriter.WriteLine(_T(" = [\r\n"));
					break;
				}

				for (i = 0; i < lCount; i++)
				{
					int j;
					switch(lType)
					{
					case Params::VT_INT:
						{
							String lString = StringUtility::Format(_T("%i"), pParams.GetIntValue(lVarName, i));
							pWriter.WriteLine(lString);
							if (i != (lCount - 1))
							{
								pWriter.WriteLine(_T(", "));
							}
							else
							{
								pWriter.WriteLine(_T("]\r\n"));
							}
						}
						break;
					case Params::VT_FLOAT:
						{
							String lString = StringUtility::Format(_T("%f"), pParams.GetFloatValue(lVarName, i));
							pWriter.WriteLine(lString);
							if (i != (lCount - 1))
							{
								pWriter.WriteLine(_T(", "));
							}
							else
							{
								pWriter.WriteLine(_T("]\r\n"));
							}
						}
						break;
					case Params::VT_STRING:
						for (j = 0; j < pTabs; j++)
						{
							pWriter.WriteLine(_T("\t"));
						}
						pWriter.WriteLine(_T("\""));
						pWriter.WriteLine(pParams.GetStringValue(lVarName, i));
						pWriter.WriteLine(_T("\""));
						if (i != (lCount - 1))
						{
							pWriter.WriteLine(_T(",\r\n"));
						}
						else
						{
							pWriter.WriteLine(_T("\r\n"));
							for (j = 0; j < pTabs; j++)
							{
								pWriter.WriteLine(_T("\t"));
							}
							pWriter.WriteLine(_T("]\r\n"));
						}
						break;
					case Params::VT_STRUCT:
						for (j = 0; j < pTabs; j++)
						{
							pWriter.WriteLine(_T("\t"));
						}
						pWriter.WriteLine(_T("{\t"));
						Save(pWriter, *pParams.GetStruct(lVarName, i), pTabs + 1, true);
						for (j = 0; j < pTabs; j++)
						{
							pWriter.WriteLine(_T("\t"));
						}
						if (i != (lCount - 1))
						{
							pWriter.WriteLine(_T("},\r\n"));
						}
						else
						{
							pWriter.WriteLine(_T("}]\r\n\r\n"));
						}
						break;
					};
				}
			}
		}

		pWriter.WriteLine(_T("\r\n"));
		lContinue = pParams.GetNextVariable(lVarName, lType, lCount, &lSubType);
	}
}

void ParamLoader::ReadNewExpression(Reader& pReader, Params& pParams, tchar& pChar, unsigned& pLine)
{
	String lVarType;
	String lVarName;

	ReadType(pReader, lVarType, pChar, pLine);
	ReadVariableName(pReader, lVarName, pChar, pLine);

	if (lVarType.length() > 0 && lVarName.empty())
	{
		mLog.Warningf(_T("Invalid type or name in file %s at line %i."), pReader.GetStreamName().c_str(), pLine);
		return;
	}
	else if(lVarType.empty() && lVarName.empty())
	{
		return;
	}

	SkipSpaces(pReader, pChar, pLine);

	if (StringUtility::CompareIgnoreCase(lVarType, _T("int")) == 0)
	{
		if (pChar != _T('='))
		{
			mLog.Warningf(_T("Expected '=' in file %s at line %i, found '%c'."), pReader.GetStreamName().c_str(), pLine, pChar);
			return;
		}
		pChar = ReadIntVar(pReader, pParams, lVarName, pLine);
		return;
	}
	else if(StringUtility::CompareIgnoreCase(lVarType, _T("float")) == 0)
	{
		if (pChar != _T('='))
		{
			mLog.Warningf(_T("Expected '=' in file %s at line %i, found '%c'."), pReader.GetStreamName().c_str(), pLine, pChar);
			return;
		}
		pChar = ReadFloatVar(pReader, pParams, lVarName, pLine);
		return;
	}
	else if(StringUtility::CompareIgnoreCase(lVarType, _T("string")) == 0)
	{
		if (pChar != _T('='))
		{
			mLog.Warningf(_T("Expected '=' in file %s at line %i, found '%c'."), pReader.GetStreamName().c_str(), pLine, pChar);
			return;
		}
		pChar = ReadStringVar(pReader, pParams, lVarName, pLine);
		return;
	}
	else if(StringUtility::CompareIgnoreCase(lVarType, _T("struct")) == 0)
	{
		if (pChar != _T('{') && pChar != _T('='))
		{
			mLog.Warningf(_T("Expected '{' in file %s at line %i, found '%c'."), pReader.GetStreamName().c_str(), pLine, pChar);
			return;
		}
		if (pChar == _T('{'))
		{
			pChar = ReadStructVar(pReader, pParams, lVarName, pLine);
			if (pChar == _T('}'))
			{
				pReader.Read(pChar);
			}
		}
		else
		{
			pChar = ReadStructArr(pReader, pParams, lVarName, pLine);
		}
		return;
	}
	else if(StringUtility::CompareIgnoreCase(lVarType, _T("int8")) == 0)
	{
		ReadBinary(pReader, pParams, lVarName, pChar, TYPE_INT8, pLine);
	}
	else if(StringUtility::CompareIgnoreCase(lVarType, _T("int16")) == 0)
	{
		ReadBinary(pReader, pParams, lVarName, pChar, TYPE_INT16, pLine);
	}
	else if(StringUtility::CompareIgnoreCase(lVarType, _T("int")) == 0)
	{
		ReadBinary(pReader, pParams, lVarName, pChar, TYPE_INT32, pLine);
	}
	else if(StringUtility::CompareIgnoreCase(lVarType, _T("float32")) == 0)
	{
		ReadBinary(pReader, pParams, lVarName, pChar, TYPE_FLOAT32, pLine);
	}
	else if(StringUtility::CompareIgnoreCase(lVarType, _T("float64")) == 0)
	{
		ReadBinary(pReader, pParams, lVarName, pChar, TYPE_FLOAT64, pLine);
	}
	else
	{
		const tchar* lFileName = pReader.GetStreamName().c_str();
		String lMsg(StringUtility::Format(_T("Unknown type \"%s\" in file %s at line %i.\r\n"), lVarType.c_str(), lFileName, pLine));
		lMsg += _T("Allowed types are:\r\nASCII:  int, float, string, struct\r\nBINARY: int8, int16, int, float32, float64\r\n");
		mLog.Warning(lMsg);
		return;
	}

}

tchar ParamLoader::ReadIntVar(Reader& pReader, Params& pParams, const String& pVarName, unsigned& pLine)
{
	if (pReader.GetAvailable() > 0)
	{
		tchar lChar;
		pReader.Read(lChar);
		SkipSpaces(pReader, lChar, pLine);

		if (lChar == _T('['))
		{
			std::list<int> lIntList;
			while (pReader.GetAvailable() > 0)
			{
				pReader.Read(lChar);

				int lInt;
				ReadInt(pReader, lInt, lChar, pLine);
				SkipSpaces(pReader, lChar, pLine);

				lIntList.push_back(lInt);

				if (lChar == _T(']'))
				{
					pReader.Read(lChar);
					break;
				}
				else if(lChar != _T(','))
				{
					mLog.Warningf(_T("Expected ',' in file %s at line %i, found '%c'."), pReader.GetStreamName().c_str(), pLine, lChar);
				}
			}

			if (lIntList.empty() == false)
			{
				pParams.AddVariableInt(String(pVarName), lIntList);
			}

			return lChar;
		}
		else
		{
			int lInt;
			ReadInt(pReader, lInt, lChar, pLine);
			pParams.AddVariableInt(String(pVarName), 1, &lInt);
			return lChar;
		}
	}

	return 0;
}

tchar ParamLoader::ReadFloatVar(Reader& pReader, Params& pParams, const String& pVarName, unsigned& pLine)
{
	if (pReader.GetAvailable() > 0)
	{
		tchar lChar;
		pReader.Read(lChar);
		SkipSpaces(pReader, lChar, pLine);
		if (lChar == _T('['))
		{
			std::list<float64> lFloatList;
			while (pReader.GetAvailable() > 0)
			{
				pReader.Read(lChar);

				float64 lFloat;
				ReadFloat(pReader, lFloat, lChar, pLine);
				SkipSpaces(pReader, lChar, pLine);

				lFloatList.push_back(lFloat);

				if (lChar == _T(']'))
				{
					pReader.Read(lChar);
					break;
				}
				else if(lChar != _T(','))
				{
					mLog.Warningf(_T("Expected ',' in file %s at line %i, found '%c'."), pReader.GetStreamName().c_str(), pLine, lChar);
				}
			}

			if (lFloatList.empty() == false)
			{
				pParams.AddVariableFloat(String(pVarName), lFloatList);
			}

			return lChar;
		}
		else
		{
			float64 lFloat;
			ReadFloat(pReader, lFloat, lChar, pLine);
			pParams.AddVariableFloat(String(pVarName), 1, &lFloat);
			return lChar;
		}
	}
	return 0;
}

tchar ParamLoader::ReadStringVar(Reader& pReader, Params& pParams, const String& pVarName, unsigned& pLine)
{
	if (pReader.GetAvailable() > 0)
	{
		tchar lChar;
		pReader.Read(lChar);
		SkipSpaces(pReader, lChar, pLine);
		if (lChar == _T('['))
		{
			std::list<String> lStringList;
			while (pReader.GetAvailable() > 0)
			{
				pReader.Read(lChar);

				String lString;
				ReadString(pReader, lString, lChar, pLine);
				SkipSpaces(pReader, lChar, pLine);

				lStringList.push_back(lString);

				if (lChar == _T(']'))
				{
					pReader.Read(lChar);
					break;
				}
				else if(lChar != _T(','))
				{
					mLog.Warningf(_T("Expected ',' in file %s at line %i, found '%c'."), pReader.GetStreamName().c_str(), pLine, lChar);
				}
			}

			if (lStringList.empty() == false)
			{
				pParams.AddVariableString(String(pVarName), lStringList);
			}

			return lChar;
		}
		else
		{
			String lString;
			ReadString(pReader, lString, lChar, pLine);
			pParams.AddVariableString(String(pVarName), 1, &lString);
			return lChar;
		}
	}

	return 0;
}

tchar ParamLoader::ReadStructVar(Reader& pReader, Params& pParams, const String& pVarName, unsigned& pLine)
{
	if (pReader.GetAvailable() > 0)
	{
		tchar lChar;
		pReader.Read(lChar);
		SkipSpaces(pReader, lChar, pLine);

		String lVarName(pVarName);
		Params lParams;
		lParams.SetName(lVarName);

		while (pReader.GetAvailable() > 0)
		{
			ReadNewExpression(pReader, lParams, lChar, pLine);
			if (lChar == _T('}'))
			{
				break;
			}
		}
		pParams.AddVariableStruct(lVarName, 1, &lParams);
		return lChar;
	}
	return 0;
}

tchar ParamLoader::ReadStructVarDirect(Reader& pReader, Params& pParams, unsigned& pLine)
{
	if (pReader.GetAvailable() > 0)
	{
		tchar lChar;
		pReader.Read(lChar);
		SkipSpaces(pReader, lChar, pLine);

		while (pReader.GetAvailable() > 0)
		{
			ReadNewExpression(pReader, pParams, lChar, pLine);
			if (lChar == _T('}'))
			{
				break;
			}
		}
		return lChar;
	}
	return 0;
}

tchar ParamLoader::ReadStructArr(Reader& pReader, Params& pParams, const String& pVarName, unsigned& pLine)
{
	if (pReader.GetAvailable() > 0)
	{
		tchar lChar;
		pReader.Read(lChar);
		SkipSpaces(pReader, lChar, pLine);

		if (lChar != _T('['))
		{
			mLog.Warningf(_T("Expected '[' in file %s at line %i, found '%c'."), pReader.GetStreamName().c_str(), pLine, lChar);
			return lChar;
		}

		std::list<Params*> lParamsList;
		while (pReader.GetAvailable() > 0)
		{
			pReader.Read(lChar);
			SkipSpaces(pReader, lChar, pLine);

			String lVarNameStr;

			if (lChar != _T('{'))
			{
				ReadVariableName(pReader, lVarNameStr, lChar, pLine);
				SkipSpaces(pReader, lChar, pLine);
			}
			else
			{
				lVarNameStr = StringUtility::IntToString((int)lParamsList.size(), 10);
			}

			if (lChar != _T('{'))
			{
				mLog.Warningf(_T("Expected '{' in file %s at line %i, found '%c'."), pReader.GetStreamName().c_str(), pLine, lChar);
				break;
			}

			String lVarName(lVarNameStr);
			Params* lParams = new Params;
			lParams->SetName(lVarName);

			lChar = ReadStructVarDirect(pReader, *lParams, pLine);
			if (lChar != _T('}'))
			{
				mLog.Warningf(_T("Expected '}' in file %s at line %i, found '%c'."), pReader.GetStreamName().c_str(), pLine, lChar);
				break;
			}
			else
			{
				pReader.Read(lChar);
			}

			SkipSpaces(pReader, lChar, pLine);

			lParamsList.push_back(lParams);

			if (lChar == _T(']'))
			{
				pReader.Read(lChar);
				break;
			}
			else if(lChar != _T(','))
			{
				mLog.Warningf(_T("Expected ',' in file %s at line %i, found '%c'."), pReader.GetStreamName().c_str(), pLine, lChar);
			}
		}

		if (lParamsList.empty() == false)
		{
			pParams.AddVariableStruct(String(pVarName), lParamsList);
			std::list<Params*>::iterator lIter;
			for (lIter = lParamsList.begin(); lIter != lParamsList.end(); ++lIter)
			{
				Params* lParams = *lIter;
				delete lParams;
			}
		}

		return lChar;
	}
	return 0;
}

void ParamLoader::SkipLine(Reader& pReader, unsigned& pLine)
{
	tchar lChar;
	do
	{
		pReader.Read(lChar);
	}
	while (lChar != _T('\n') && pReader.GetAvailable() > 0);
	pLine++;
}

void ParamLoader::SkipMultiLineComment(Reader& pReader, unsigned& pLine)
{
	tchar lChar;
	pReader.Read(lChar);

	while (pReader.GetAvailable() > 0)
	{
		switch(lChar)
		{
			case _T('*'):
			{
				// This may be the end of the comment. Look one char ahead.
				if (pReader.GetAvailable() > 0)
				{
					tchar lChar2;
					pReader.Read(lChar2);
					if (lChar2 == _T('/'))
					{
						// Yep, we are done.
						return;
					}
					else
					{
						lChar = lChar2;
					}
				}
			}
			break;
			case _T('/'):
			{
				// This may be the start of a new multiline comment. Look one char ahead.
				if (pReader.GetAvailable() > 0)
				{
					tchar lChar2;
					pReader.Read(lChar2);
					if (lChar2 == _T('*'))
					{
						// Yes, parse recursively.
						SkipMultiLineComment(pReader, pLine);
					}
					else
					{
						lChar = lChar2;
					}
				}
			}
			break;
			case _T('\n'):
			{
				pLine++;
			}
			// TRICKY: falls through (to read next character).
			default:
			{
				pReader.Read(lChar);
			}
			break;
		}
	}
}

void ParamLoader::SkipSpaces(Reader& pReader, tchar& pCurrentChar, unsigned& pLine)
{
	bool lMoreAvailable = true;
	// Skip spaces.
	do
	{
		if (pCurrentChar == _T('\n'))
		{
			pLine++;
		}
		
		if ( pCurrentChar != _T(' ')  &&
			pCurrentChar != _T('\r') &&
			pCurrentChar != _T('\n') &&
			pCurrentChar != _T('\t'))
		{
			// This wasn't whitespace... Check if it is a comment.
			if (pCurrentChar == _T('/'))
			{
				if (pReader.GetAvailable() > 0)
				{
					pReader.Read(pCurrentChar);
					if (pCurrentChar == _T('/'))
					{
						SkipLine(pReader, pLine);
					}
					else if(pCurrentChar == _T('*'))
					{
						SkipMultiLineComment(pReader, pLine);
					}
					else
					{
						mLog.Warningf(_T("Unexpected / in file %s at line %i."), pReader.GetStreamName().c_str(), pLine);
					}
				}
			}
			else
			{
				// We are done.
				if (pReader.GetAvailable() <= 0)
				{
					pCurrentChar = 0;
				}
				return;
			}
		}

		lMoreAvailable = (pReader.GetAvailable() > 0);
		if (lMoreAvailable == true)
		{
			pReader.Read(pCurrentChar);
		}
	}while(lMoreAvailable == true);
}

void ParamLoader::ReadType(Reader& pReader, String& pType, tchar& pCurrentChar, unsigned& pLine)
{
	SkipSpaces(pReader, pCurrentChar, pLine);

	if ((pCurrentChar >= _T('a') && pCurrentChar <= _T('z')) ||
		(pCurrentChar >= _T('A') && pCurrentChar <= _T('Z')))
	{
		pType += pCurrentChar;
		while (pReader.GetAvailable() > 0)
		{
			pReader.Read(pCurrentChar);
			if ((pCurrentChar >= _T('a') && pCurrentChar <= _T('z')) ||
				(pCurrentChar >= _T('A') && pCurrentChar <= _T('Z')))
			{
				pType += pCurrentChar;
			}
			else
			{
				break;
			}
		}
	}
}

void ParamLoader::ReadInt(Reader& pReader, int& pInt, tchar& pCurrentChar, unsigned& pLine)
{
	SkipSpaces(pReader, pCurrentChar, pLine);

	pInt = 0;
	bool lSign = false;

	if (pCurrentChar == _T('-'))
	{
		lSign = true;

		if (pReader.GetAvailable() > 0)
		{
			pReader.Read(pCurrentChar);
		}
		else
		{
			return;
		}
	}

	if (pCurrentChar >= _T('0') && pCurrentChar <= _T('9'))
	{
		pInt = (int)(pCurrentChar - _T('0'));

		while (pReader.GetAvailable() > 0)
		{
			pReader.Read(pCurrentChar);
			if (pCurrentChar >= _T('0') && pCurrentChar <= _T('9'))
			{
				pInt *= 10;
				pInt += (int)(pCurrentChar - _T('0'));
			}
			else
			{
				break;
			}
		}
	}

	if (lSign == true)
	{
		pInt = -pInt;
	}
}

void ParamLoader::ReadFloat(Reader& pReader, float64& pFloat, tchar& pCurrentChar, unsigned& pLine)
{
	SkipSpaces(pReader, pCurrentChar, pLine);

	pFloat = 0;
	bool lSign = false;

	if (pCurrentChar == _T('-'))
	{
		lSign = true;

		if (pReader.GetAvailable() > 0)
		{
			pReader.Read(pCurrentChar);
		}
		else
		{
			return;
		}
	}

	enum Part
	{
		INTEGER_PART = 0,
		DECIMAL_PART,
		EXPSIGN_PART,
		EXPINT_PART,
		EXPDEC_PART,
	};

	Part lPart = INTEGER_PART;
	bool lExpSign = false;
	float64 lExp = 0;
	float64 lDecValue = 0.1;
	bool lDone = false;

	if (pCurrentChar >= _T('0') && pCurrentChar <= _T('9'))
	{
		pFloat = (float64)(pCurrentChar - _T('0'));

		while (pReader.GetAvailable() > 0 && lDone == false)
		{
			pReader.Read(pCurrentChar);

			switch(lPart)
			{
			case INTEGER_PART:
				if (pCurrentChar >= _T('0') && pCurrentChar <= _T('9'))
				{
					pFloat *= 10.0;
					pFloat += (float64)(pCurrentChar - _T('0'));
				}
				else if(pCurrentChar == _T('.'))
				{
					lPart = DECIMAL_PART;
				}
				else if(pCurrentChar == _T('e') || pCurrentChar == _T('E'))
				{
					lPart = EXPSIGN_PART;
				}
				else
				{
					lDone = true;
				}
				break;
			case DECIMAL_PART:
				if (pCurrentChar >= _T('0') && pCurrentChar <= _T('9'))
				{
					pFloat += (float64)(pCurrentChar - _T('0')) * lDecValue;
					lDecValue *= 0.1;
				}
				else if(pCurrentChar == _T('e') || pCurrentChar == _T('E'))
				{
					lPart = EXPSIGN_PART;
				}
				else
				{
					lDone = true;
				}
				break;
			case EXPSIGN_PART:
				if (pCurrentChar == _T('-'))
				{
					lExpSign = true;
					lPart = EXPINT_PART;
				}
				else if(pCurrentChar >= _T('0') && pCurrentChar <= _T('9'))
				{
					// Start integer part.
					lExp *= 10.0;
					lExp += (float64)(pCurrentChar - _T('0'));
					lPart = EXPINT_PART;
				}
				else
				{
					lDone = true;
				}
				break;
			case EXPINT_PART:
				if (pCurrentChar >= _T('0') && pCurrentChar <= _T('9'))
				{
					lExp *= 10.0;
					lExp += (float64)(pCurrentChar - _T('0'));
				}
				else if(pCurrentChar == _T('.'))
				{
					lPart = EXPDEC_PART;
					lDecValue = 0.1;
				}
				else
				{
					lDone = true;
				}
				break;
			case EXPDEC_PART:
				if (pCurrentChar >= _T('0') && pCurrentChar <= _T('9'))
				{
					lExp += (float64)(pCurrentChar - _T('0')) * lDecValue;
					lDecValue *= 0.1;
				}
				else
				{
					lDone = true;
				}
				break;
			}
		}
	}

	if (lExpSign == true)
	{
		lExp = -lExp;
	}

	pFloat *= pow(10.0, lExp);

	if (lSign == true)
	{
		pFloat = -pFloat;
	}
}

void ParamLoader::ReadString(Reader& pReader, String& pString, tchar& pCurrentChar, unsigned& pLine)
{
	SkipSpaces(pReader, pCurrentChar, pLine);

	if (pCurrentChar != _T('"'))
	{
		mLog.Warningf(_T("Unexpected '%c' in file %s at line %i."), pCurrentChar, pReader.GetStreamName().c_str(), pLine);
		return;
	}

	while (pReader.GetAvailable() > 0)
	{
		pReader.Read(pCurrentChar);
		if (pCurrentChar == _T('"'))
		{
			if (pReader.GetAvailable() > 0)
			{
				// Read one more...
				pReader.Read(pCurrentChar);
			}
			break;
		}
		else if(pCurrentChar == _T('\\'))
		{
			// An escape character...
			tchar lString[3];
			lString[0] = pCurrentChar;
			if (pReader.GetAvailable() > 0)
			{
				pReader.Read(pCurrentChar);
				lString[1] = pCurrentChar;
				lString[2] = 0;
				pString += StringUtility::Format(lString);
			}
		}
		else
		{
			pString += pCurrentChar;
		}
	}
}

void ParamLoader::ReadVariableName(Reader& pReader, String& pName, tchar& pCurrentChar, unsigned& pLine)
{
	SkipSpaces(pReader, pCurrentChar, pLine);

	if ((pCurrentChar >= _T('a') && pCurrentChar <= _T('z')) ||
		(pCurrentChar >= _T('A') && pCurrentChar <= _T('Z')) ||
		pCurrentChar == _T('_'))
	{
		pName += pCurrentChar;

		while (pReader.GetAvailable() > 0)
		{
			pReader.Read(pCurrentChar);

			if ((pCurrentChar >= _T('a') && pCurrentChar <= _T('z')) ||
			   (pCurrentChar >= _T('A') && pCurrentChar <= _T('Z')) ||
			   (pCurrentChar >= _T('0') && pCurrentChar <= _T('9')) ||
				pCurrentChar == _T('_'))
			{
				pName += pCurrentChar;
			}
			else
			{
				return;
			}
		}
	}
}

void ParamLoader::ReadBinary(Reader& pReader, Params& pParams, const String pVarName, tchar& pChar, Type pType, unsigned& pLine)
{
	if (pChar != _T('['))
	{
		mLog.Warningf(_T("Expected '[' in file %s at line %i, found '%c'."), pReader.GetStreamName().c_str(), pLine, pChar);
		return;
	}
	int lCount;
	ReadInt(pReader, lCount, pChar, pLine);
	SkipSpaces(pReader, pChar, pLine);

	if (lCount <= 0)
	{
		mLog.Warningf(_T("Invalid count value in file %s at line %i."), pReader.GetStreamName().c_str(), pLine);
		lCount = 0;
		return;
	}

	if (pChar != _T(']'))
	{
		mLog.Warningf(_T("Expected ']' in file %s at line %i, found '%c'."), pReader.GetStreamName().c_str(), pLine, pChar);
		return;
	}

	if (pReader.GetAvailable() > 0)
	{
		pReader.Read(pChar);
		SkipSpaces(pReader, pChar, pLine);

		if (pChar != _T('='))
		{
			mLog.Warningf(_T("Expected '=' in file %s at line %i, found '%c'."), pReader.GetStreamName().c_str(), pLine, pChar);
			return;
		}

		// Read on until first newline.
		while (pReader.GetAvailable() > 0 && pChar != _T('\n'))
		{
			pReader.Read(pChar);
		}

		if (pReader.GetAvailable() > 0)
		{
			int i;

			String lVarName(pVarName);

			switch(pType)
			{
			case TYPE_INT8:
				{
					int* lDataBuffer = new int[lCount];
					for (i = 0; i < lCount; i++)
					{
						int8 lData;
						pReader.Read(lData);
						lDataBuffer[i] = (int)lData;
					}
					pParams.AddVariableInt(lVarName, lCount, lDataBuffer);
				}
				break;
			case TYPE_INT16:
				{
					int* lDataBuffer = new int[lCount];
					for (i = 0; i < lCount; i++)
					{
						int16 lData;
						pReader.Read(lData);
						lDataBuffer[i] = (int)lData;
					}
					pParams.AddVariableInt(lVarName, lCount, lDataBuffer);
				}
				break;
			case TYPE_INT32:
				{
					int* lDataBuffer = new int[lCount];
					for (i = 0; i < lCount; i++)
					{
						pReader.Read(lDataBuffer[i]);
					}
					pParams.AddVariableInt(lVarName, lCount, lDataBuffer);
				}
				break;
			case TYPE_FLOAT32:
				{
					float64* lDataBuffer = new float64[lCount];
					for (i = 0; i < lCount; i++)
					{
						float32 lData;
						pReader.Read(lData);
						lDataBuffer[i] = (float64)lData;
					}
					pParams.AddVariableFloat(lVarName, lCount, lDataBuffer);
				}
				break;
			case TYPE_FLOAT64:
					float64* lDataBuffer = new float64[lCount];
					for (i = 0; i < lCount; i++)
					{
						pReader.Read(lDataBuffer[i]);
					}
					pParams.AddVariableFloat(lVarName, lCount, lDataBuffer);
				break;
			}
		}
	}
}


} // End namespace.
