/*
	Class:  Params
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	A class used to store parameters loaded from a param-file.
	(Read more about param files in ParamLoader.h).

	You can iterate over all variables using GetFirstVariable() and
	GetNextVariable(), which will return the name of the variable,
	the type, and the count. "Count" is the number of elements in a
	list, and will always be 1 if the variable is a single int, float64,
	string or struct.
	
	You can search after a variable by calling FindVariable(). This isn't
	as convenient as ConditionalFind() though, which will probably be the
	function that you'll use the most.

	When a variable is known to exist, and when you know its type, you can
	safely read its value by calling GetIntValue(), GetFloatValue(), etc.
*/

#ifndef LEPRA_PARAMS_H
#define LEPRA_PARAMS_H

#include "LepraTypes.h"
#include "HashTable.h"
#include "String.h"
#include <list>

namespace Lepra
{

class Params
{
public:

	friend class ParamLoader;

	enum VariableType
	{
		VT_INVALID = 0,
		VT_INT,
		VT_FLOAT,
		VT_STRING,
		VT_STRUCT,
		VT_TUPLE,
	};

	enum SubType
	{
		ST_INVALID = 0,
		ST_INT8,
		ST_INT16,
		ST_INT32,
		ST_INT,		// INT32 ascii.
		ST_FLOAT32,
		ST_FLOAT64,
		ST_FLOAT,	// FLOAT64 ascii.
	};

	Params();
	Params(const Params& pParams);
	virtual ~Params();

	void Copy(const Params& pParams);

	bool GetFirstVariable(String& pName, VariableType& pType, int& pCount, SubType* pSubType = 0);
	bool GetNextVariable(String& pName, VariableType& pType, int& pCount, SubType* pSubType = 0);

	bool FindVariable(const String& pName, VariableType& pType, int& pCount) const;

	// Returns "Count" if a variable with the name pName is found, and if it is
	// of the type pType, and count is greater or equal to pMinCount.
	// Returns 0 otherwise.
	int ConditionalFind(const String& pName, VariableType pType, int pMinCount = 0) const;

	int GetIntValue(const String& pVariableName, int pIndex = 0) const;
	float64 GetFloatValue(const String& pVariableName, int pIndex = 0) const;
	String GetStringValue(const String& pVariableName, int pIndex = 0) const;
	Params* GetStruct(const String& pVariableName, int pIndex = 0) const;

	void RemoveAll();
	void RemoveVariable(const String& pVariableName);

	// Array version...
	bool AddVariableInt(const String& pVariableName, int pCount, int8* pData, SubType pSubType = ST_INT);
	bool AddVariableInt(const String& pVariableName, int pCount, int16* pData, SubType pSubType = ST_INT);
	bool AddVariableInt(const String& pVariableName, int pCount, int32* pData, SubType pSubType = ST_INT);
	bool AddVariableFloat(const String& pVariableName, int pCount, float32* pData, SubType pSubType = ST_FLOAT);
	bool AddVariableFloat(const String& pVariableName, int pCount, float64* pData, SubType pSubType = ST_FLOAT);
	bool AddVariableString(const String& pVariableName, int pCount, const String* pData);
	bool AddVariableStruct(const String& pVariableName, int pCount, const Params* pData);

	// Linked list version...
	bool AddVariableInt(const String& pVariableName, std::list<int32>& pList, SubType pSubType = ST_INT32);
	bool AddVariableFloat(const String& pVariableName, std::list<float64>& pList, SubType pSubType = ST_FLOAT64);
	bool AddVariableString(const String& pVariableName, std::list<String>& pList);
	bool AddVariableStruct(const String& pVariableName, std::list<Params*>& pList);

	bool AddVariableTuple(const String& pVariableName, int pCount);
	bool SetTupleMember(const String& pVariableName, int pIndex, int32 pData);
	bool SetTupleMember(const String& pVariableName, int pIndex, float64 pData);
	bool SetTupleMember(const String& pVariableName, int pIndex, const String& pString);

	inline const Params& operator = (const Params& pParams);

	// Set and get struct name, if this class is representing a struct.
	inline void SetName(const String& pName);
	inline const String& GetName();

	inline int GetNumVariables();

private:
	class TupleMember
	{
	public:
		TupleMember() :
			mType(VT_INVALID),
			mIntValue(0),
			mFloatValue(0)
		{
		}

		void Copy(TupleMember* pTupleMember)
		{
			mType = pTupleMember->mType;
			mIntValue = pTupleMember->mIntValue;
			mFloatValue = pTupleMember->mFloatValue;
			mStringValue = pTupleMember->mStringValue;
		}

		VariableType mType;
		int32 mIntValue;
		float64 mFloatValue;
		String mStringValue;
	};
	
	class Variable
	{
	public:

		Variable()
		{
			mName = _T("");
			mType = VT_INVALID;
			mSubType = ST_INVALID;
			mCount = 0;
			mIntValue = 0;
			mFloatValue = 0;
			mString = 0;
			mParams = 0;
			mTupleMember = 0;
		}

		virtual ~Variable()
		{
			if (mIntValue != 0)
			{
				delete[] (int*)mIntValue;
				mIntValue = 0;
			}

			if (mFloatValue != 0)
			{
				delete[] (float*)mFloatValue;
				mFloatValue = 0;
			}

			if (mString != 0)
			{
				delete[] mString;
				mString = 0;
			}

			if (mParams != 0)
			{
				delete[] mParams;
				mParams = 0;
			}

			if (mTupleMember != 0)
			{
				delete[] mTupleMember;
				mTupleMember = 0;
			}
		}

		String mName;
		VariableType mType;
		SubType mSubType;
		int mCount; // > 1 if array or tuple
		
		void* mIntValue;
		void* mFloatValue;
		String* mString;
		Params* mParams;
		TupleMember* mTupleMember;
	};

	typedef HashTable<String, Variable*, std::hash<String> , 128> VarTable;

	inline void* GetCurrentIntPointer();
	inline void* GetCurrentFloatPointer();

	VarTable::Iterator mIter;

	VarTable* mVarTable;

	String mName;
};

const Params& Params::operator = (const Params& pParams)
{
	if (this == &pParams)
	{
		return *this;
	}

	Copy(pParams);

	return *this;
}

void Params::SetName(const String& pName)
{
	mName = pName;
}

const String& Params::GetName()
{
	return mName;
}

void* Params::GetCurrentIntPointer()
{
	if (mVarTable == 0 || mIter == mVarTable->End())
		return 0;
	return (*mIter)->mIntValue;
}

void* Params::GetCurrentFloatPointer()
{
	if (mVarTable == 0 || mIter == mVarTable->End())
		return 0;
	return (*mIter)->mFloatValue;
}

int Params::GetNumVariables()
{
	return mVarTable->GetCount();
}

} // End namespace.

#endif // !LEPRA_PARAMS_H
