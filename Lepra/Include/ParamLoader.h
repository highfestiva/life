/*
	Class:  ParamLoader
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	A parameter loader. Loads specially formatted textfiles (uff-files) into
	a Param-instance. Since this is such a powerful tool and such a general file 
	format, I prefer to call it the "Universal File Format", with .uff as the
	file extention. Following the file format specification (described below)
	any data can be stored, and thus, all files of different types (images, sounds 
	etc) only need to define the STRUCTURE of the data.

	An uff-file is (in the common case) a regular text file used to store arbitrary 
	data. It's no script- or programming-language, since it doesn't contain any code 
	that'll be executed.

	The contents of the file 

	The file format is using a C-like syntax. Line comments can be written in the 
	file using // in front of the comment. Semicolon (';') must be at the end of
	each expression.

	The data is stored as various variables, and each variable can be of the type
	int, float64 or string. There's also a type which is called a "struct". A struct
	can be compared to a struct in C or C++.

	The type of a variable is determined by the following data. Here's an example
	of three different variables:

	int myInt = 4				// This is a comment.
	float myFloat = 1.3			
	string myString = "Hello"

	int myIntArray = [1, 2, 3, 4, 5, 6]
	float myFloatArray = [1.0, 2.0, 3.14159265, 1.2345]
	string myStringArray = ["One",		// It is allowed to
				"Two",		// split this up on
				"Three"]	// several lines like this.

	It is important that all indices in the array are of the same type. An array
	containing different types will be ignored.

	The final type that you can use is a struct:

	struct myStruct
	{
		// Inside a struct, you can define any variables you want,
		// even substructs...
		int memberInt = 123;
		struct subStruct
		{
			bla  = 2
			bla2 = 2.3
		}
	}

	All variables (even structs) must have unique names at the level at which they
	are declared.

	Multiple line comments can be written as usual in C and C++, with one important
	difference - multi line comments are recursive.

	That's all! Good luck!
*/

#ifndef LEPRA_PARAMLOADER_H
#define LEPRA_PARAMLOADER_H

#include "LepraTypes.h"
#include "Params.h"
#include "String.h"
#include "DiskFile.h"

namespace Lepra
{

class ParamLoader
{
public:

	ParamLoader();
	virtual ~ParamLoader();

	void Load(const String& pFileName, Params& pParams);
	void Save(const String& pFileName, Params& pParams);

	void Load(const String& pArchiveName, const String& pFileName, Params& pParams);
	void Save(const String& pArchiveName, const String& pFileName, Params& pParams);

	void Load(Reader& pReader, Params& pParams);
	void Save(Writer& pWriter, Params& pParams);

	void LoadOneExpression(Reader& pReader, Params& pParams);

private:
	enum Type
	{
		TYPE_INVALID = 0,
		TYPE_INT8,
		TYPE_INT16,
		TYPE_INT32,
		TYPE_FLOAT32,
		TYPE_FLOAT64,
	};

	void Save(Writer& pWriter, Params& pParams, int pTabs, bool pSkipFirstTab = false);

	// Parsing stuff.

	void ReadNewExpression(Reader& pReader, Params& pParams, tchar& pChar, unsigned& pLine);
	tchar ReadIntVar(Reader& pReader, Params& pParams, const String& pVarName, unsigned& pLine);
	tchar ReadFloatVar(Reader& pReader, Params& pParams, const String& pVarName, unsigned& pLine);
	tchar ReadStringVar(Reader& pReader, Params& pParams, const String& pVarName, unsigned& pLine);
	tchar ReadStructVar(Reader& pReader, Params& pParams, const String& pVarName, unsigned& pLine);
	tchar ReadStructVarDirect(Reader& pReader, Params& pParams, unsigned& pLine);
	tchar ReadStructArr(Reader& pReader, Params& pParams, const String& pVarName, unsigned& pLine);
	void SkipLine(Reader& pReader, unsigned& pLine);
	void SkipMultiLineComment(Reader& pReader, unsigned& pLine);
	void SkipSpaces(Reader& pReader, tchar& pCurrentChar, unsigned& pLine);
	void SkipComments(Reader& pReader, tchar& pCurrentChar, unsigned& pLine);
	void ReadType(Reader& pReader, String& pType, tchar& pCurrentChar, unsigned& pLine);
	void ReadInt(Reader& pReader, int& pInt, tchar& pCurrentChar, unsigned& pLine);
	void ReadFloat(Reader& pReader, float64& pFloat, tchar& pCurrentChar, unsigned& pLine);
	void ReadString(Reader& pReader, String& pString, tchar& pCurrentChar, unsigned& pLine);
	void ReadVariableName(Reader& pReader, String& pName, tchar& pCurrentChar, unsigned& pLine);
	void ReadBinary(Reader& pReader, Params& pParams, const String pVarName, tchar& pChar, Type pType, unsigned& pLine);

	LOG_CLASS_DECLARE();
};

}

#endif // !LEPRA_PARAMLOADER_H
