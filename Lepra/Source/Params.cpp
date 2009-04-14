/*
	Class:  Params
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include "../Include/LepraTypes.h"
#include "../Include/Params.h"

namespace Lepra
{

Params::Params()
{
	mVarTable = new VarTable;
}

Params::Params(const Params& pParams)
{
	mVarTable = new VarTable;
	Copy(pParams);
}

Params::~Params()
{
	RemoveAll();
	delete mVarTable;
	mVarTable = 0;
}

void Params::Copy(const Params& pParams)
{
	RemoveAll();

	VarTable::Iterator lIter;
	
	for (lIter = pParams.mVarTable->First(); lIter != pParams.mVarTable->End(); ++lIter)
	{
		Variable* lVar = *lIter;

		switch(lVar->mType)
		{
			case VT_INT:
			{
				switch(lVar->mSubType)
				{
				case ST_INT8:
					AddVariableInt(lVar->mName, lVar->mCount, (int8*)lVar->mIntValue, lVar->mSubType);
					break;
				case ST_INT16:
					AddVariableInt(lVar->mName, lVar->mCount, (int16*)lVar->mIntValue, lVar->mSubType);
					break;
				case ST_INT32:
				case ST_INT:
					AddVariableInt(lVar->mName, lVar->mCount, (int*)lVar->mIntValue, lVar->mSubType);
					break;
				}
				break;
			}
			case VT_FLOAT:
			{
				switch(lVar->mSubType)
				{
				case ST_FLOAT32:
					AddVariableFloat(lVar->mName, lVar->mCount, (float32*)lVar->mFloatValue, lVar->mSubType);
					break;
				case ST_FLOAT64:
				case ST_FLOAT:
					AddVariableFloat(lVar->mName, lVar->mCount, (float64*)lVar->mFloatValue, lVar->mSubType);
					break;
				}
				break;
			}
			case VT_STRING:
			{
				AddVariableString(lVar->mName, lVar->mCount, lVar->mString);
				break;
			}
			case VT_STRUCT:
			{
				AddVariableStruct(lVar->mName, lVar->mCount, lVar->mParams);
				break;
			}
			case VT_TUPLE:
			{
				AddVariableTuple(lVar->mName, lVar->mCount);
				for (int i = 0; i < lVar->mCount; i++)
				{
					switch(lVar->mTupleMember[i].mType)
					{
					case VT_INT:
						SetTupleMember(lVar->mName, i, lVar->mTupleMember[i].mIntValue);
						break;
					case VT_FLOAT:
						SetTupleMember(lVar->mName, i, lVar->mTupleMember[i].mFloatValue);
						break;
					case VT_STRING:
						SetTupleMember(lVar->mName, i, lVar->mTupleMember[i].mStringValue);
						break;
					}
				}
				break;
			}
		}
	}

	mName = pParams.mName;
}

bool Params::GetFirstVariable(String& pName, VariableType& pType, int& pCount, SubType* pSubType)
{
	mIter = mVarTable->First();

	if (mIter != mVarTable->End())
	{
		Variable* lVar = *mIter;

		pName = lVar->mName;
		pType = lVar->mType;
		pCount = lVar->mCount;

		if (pSubType != 0)
		{
			*pSubType = lVar->mSubType;
		}

		return true;
	}

	return false;
}

bool Params::GetNextVariable(String& pName, VariableType& pType, int& pCount, SubType* pSubType)
{
	++mIter;

	if (mIter != mVarTable->End())
	{
		Variable* lVar = *mIter;

		pName = lVar->mName;
		pType = lVar->mType;
		pCount = lVar->mCount;

		if (pSubType != 0)
		{
			*pSubType = lVar->mSubType;
		}

		return true;
	}

	return false;
}

bool Params::FindVariable(const String& pName, VariableType& pType, int& pCount) const
{
	VarTable::Iterator lIter = mVarTable->Find(pName);

	if (lIter != mVarTable->End())
	{
		Variable* lVar = *mIter;

		pType = lVar->mType;
		pCount = lVar->mCount;
		return true;
	}

	return false;
}

int Params::ConditionalFind(const String& pName, VariableType pType, int pMinCount) const
{
	VarTable::Iterator lIter = mVarTable->Find(pName);

	if (lIter != mVarTable->End() && 
	   (*lIter)->mType == pType &&
	   (*lIter)->mCount >= pMinCount)
	{
		return (*lIter)->mCount;
	}

	return 0;
}

int Params::GetIntValue(const String& pVariableName, int pIndex) const
{
	VarTable::Iterator lIter = mVarTable->Find(pVariableName);

	if (lIter != mVarTable->End())
	{
		Variable* lVar = *lIter;
		if (lVar->mType == VT_INT)
		{
			switch(lVar->mSubType)
			{
			case ST_INT8:
				return (int)((int8*)lVar->mIntValue)[pIndex];
			case ST_INT16:
				return (int)((int16*)lVar->mIntValue)[pIndex];
			case ST_INT32:
			case ST_INT:
				return (int)((int*)lVar->mIntValue)[pIndex];
			}
		}
		else if(lVar->mType == VT_TUPLE)
		{
			if (lVar->mTupleMember[pIndex].mType == VT_INT)
			{
				return lVar->mTupleMember[pIndex].mIntValue;
			}
		}
	}

	return 0;
}

float64 Params::GetFloatValue(const String& pVariableName, int pIndex) const
{
	VarTable::Iterator lIter = mVarTable->Find(pVariableName);

	if (lIter != mVarTable->End())
	{
		Variable* lVar = *lIter;
		if (lVar->mType == VT_FLOAT)
		{
			switch(lVar->mSubType)
			{
			case ST_FLOAT32:
				return (float64)((float32*)lVar->mFloatValue)[pIndex];
			case ST_FLOAT64:
			case ST_FLOAT:
				return (float64)((float64*)lVar->mFloatValue)[pIndex];
			}
		}
		else if(lVar->mType == VT_TUPLE)
		{
			if (lVar->mTupleMember[pIndex].mType == VT_FLOAT)
			{
				return lVar->mTupleMember[pIndex].mFloatValue;
			}
		}
	}

	return 0;
}

String Params::GetStringValue(const String& pVariableName, int pIndex) const
{
	VarTable::Iterator lIter = mVarTable->Find(pVariableName);

	if (lIter != mVarTable->End())
	{
		Variable* lVar = *lIter;
		if (lVar->mType == VT_STRING)
		{
			return lVar->mString[pIndex];
		}
		else if(lVar->mType == VT_TUPLE)
		{
			if (lVar->mTupleMember[pIndex].mType == VT_STRING)
			{
				return lVar->mTupleMember[pIndex].mStringValue;
			}
		}
	}

	return _T("");
}

Params* Params::GetStruct(const String& pVariableName, int pIndex) const
{
	VarTable::Iterator lIter = mVarTable->Find(pVariableName);

	if (lIter != mVarTable->End())
	{
		return &(*lIter)->mParams[pIndex];
	}

	return 0;
}

void Params::RemoveAll()
{
	if (mVarTable == 0)
	{
		return;
	}

	VarTable::Iterator lIter;

	// Delete all variables.
	for (lIter = mVarTable->First(); lIter != mVarTable->End();)
	{
		Variable* lVar = *lIter;
		mVarTable->Remove(lIter++);

		delete lVar;
		lVar = 0;
	}
}

void Params::RemoveVariable(const String& pVariableName)
{
	VarTable::Iterator lIter = mVarTable->Find(pVariableName);

	if (lIter != mVarTable->End())
	{
		mVarTable->Remove(lIter);
	}
}

bool Params::AddVariableInt(const String& pVariableName, int pCount, int8* pData, SubType pSubType)
{
	bool lOk = false;

	if (pCount > 0)
	{
		if (pSubType != ST_INT8 && 
		   pSubType != ST_INT16 &&
		   pSubType != ST_INT32 &&
		   pSubType != ST_INT)
		{
			pSubType = ST_INT;
		}

		VarTable::Iterator lIter = mVarTable->Find(pVariableName);

		if (lIter == mVarTable->End())
		{
			Variable* lVar = new Variable;
			lVar->mType = VT_INT;
			lVar->mSubType = pSubType;
			lVar->mCount = pCount;
			lVar->mName = pVariableName;

			int i;
			switch(pSubType)
			{
			case ST_INT8:
				lVar->mIntValue = new int8[pCount];
				for (i = 0; i < pCount; i++)
				{
					((int8*)lVar->mIntValue)[i] = (int8)pData[i];
				}
				break;
			case ST_INT16:
				lVar->mIntValue = new int16[pCount];
				for (i = 0; i < pCount; i++)
				{
					((int16*)lVar->mIntValue)[i] = (int16)pData[i];
				}
				break;
			case ST_INT32:
			case ST_INT:
				lVar->mIntValue = new int[pCount];
				for (i = 0; i < pCount; i++)
				{
					((int*)lVar->mIntValue)[i] = (int)pData[i];
				}
				break;
			}

			mVarTable->Insert(lVar->mName, lVar);
			lOk = true;
		}
	}

	return lOk;
}


bool Params::AddVariableInt(const String& pVariableName, int pCount, int16* pData, SubType pSubType)
{
	bool lOk = false;

	if (pCount > 0)
	{
		if (pSubType != ST_INT8 && 
		   pSubType != ST_INT16 &&
		   pSubType != ST_INT32 &&
		   pSubType != ST_INT)
		{
			pSubType = ST_INT;
		}

		VarTable::Iterator lIter = mVarTable->Find(pVariableName);

		if (lIter == mVarTable->End())
		{
			Variable* lVar = new Variable;
			lVar->mType = VT_INT;
			lVar->mSubType = pSubType;
			lVar->mCount = pCount;
			lVar->mName = pVariableName;

			int i;
			switch(pSubType)
			{
			case ST_INT8:
				lVar->mIntValue = new int8[pCount];
				for (i = 0; i < pCount; i++)
				{
					((int8*)lVar->mIntValue)[i] = (int8)pData[i];
				}
				break;
			case ST_INT16:
				lVar->mIntValue = new int16[pCount];
				for (i = 0; i < pCount; i++)
				{
					((int16*)lVar->mIntValue)[i] = (int16)pData[i];
				}
				break;
			case ST_INT32:
			case ST_INT:
				lVar->mIntValue = new int[pCount];
				for (i = 0; i < pCount; i++)
				{
					((int*)lVar->mIntValue)[i] = (int)pData[i];
				}
				break;
			}

			mVarTable->Insert(lVar->mName, lVar);
			lOk = true;
		}
	}

	return lOk;
}

bool Params::AddVariableInt(const String& pVariableName, int pCount, int32* pData, SubType pSubType)
{
	bool lOk = false;

	if (pCount > 0)
	{
		if (pSubType != ST_INT8 && 
		   pSubType != ST_INT16 &&
		   pSubType != ST_INT32 &&
		   pSubType != ST_INT)
		{
			pSubType = ST_INT;
		}

		VarTable::Iterator lIter = mVarTable->Find(pVariableName);

		if (lIter == mVarTable->End())
		{
			Variable* lVar = new Variable;
			lVar->mType = VT_INT;
			lVar->mSubType = pSubType;
			lVar->mCount = pCount;
			lVar->mName = pVariableName;

			int i;
			switch(pSubType)
			{
			case ST_INT8:
				lVar->mIntValue = new int8[pCount];
				for (i = 0; i < pCount; i++)
				{
					((int8*)lVar->mIntValue)[i] = (int8)pData[i];
				}
				break;
			case ST_INT16:
				lVar->mIntValue = new int16[pCount];
				for (i = 0; i < pCount; i++)
				{
					((int16*)lVar->mIntValue)[i] = (int16)pData[i];
				}
				break;
			case ST_INT32:
			case ST_INT:
				lVar->mIntValue = new int32[pCount];
				for (i = 0; i < pCount; i++)
				{
					((int32*)lVar->mIntValue)[i] = (int32)pData[i];
				}
				break;
			}

			mVarTable->Insert(lVar->mName, lVar);
			lOk = true;
		}
	}

	return lOk;
}

bool Params::AddVariableFloat(const String& pVariableName, int pCount, float32* pData, SubType pSubType)
{
	bool lOk = false;

	if (pCount > 0)
	{
		if (pSubType != ST_FLOAT32 && 
		   pSubType != ST_FLOAT64 &&
		   pSubType != ST_FLOAT)
		{
			pSubType = ST_FLOAT;
		}

		VarTable::Iterator lIter = mVarTable->Find(pVariableName);

		if (lIter == mVarTable->End())
		{
			Variable* lVar = new Variable;
			lVar->mType = VT_FLOAT;
			lVar->mSubType = pSubType;
			lVar->mCount = pCount;
			lVar->mName = pVariableName;

			int i;
			switch(pSubType)
			{
			case ST_FLOAT32:
				lVar->mFloatValue = new float32[pCount];
				for (i = 0; i < pCount; i++)
				{
					((float32*)lVar->mFloatValue)[i] = (float32)pData[i];
				}
				break;
			case ST_FLOAT64:
			case ST_FLOAT:
				lVar->mFloatValue = new float64[pCount];
				for (i = 0; i < pCount; i++)
				{
					((float64*)lVar->mFloatValue)[i] = (float64)pData[i];
				}
				break;
			}

			mVarTable->Insert(lVar->mName, lVar);
			lOk = true;
		}
	}

	return lOk;
}

bool Params::AddVariableFloat(const String& pVariableName, int pCount, float64* pData, SubType pSubType)
{
	bool lOk = false;

	if (pCount > 0)
	{
		if (pSubType != ST_FLOAT32 && 
		   pSubType != ST_FLOAT64 &&
		   pSubType != ST_FLOAT)
		{
			pSubType = ST_FLOAT;
		}

		VarTable::Iterator lIter = mVarTable->Find(pVariableName);

		if (lIter == mVarTable->End())
		{
			Variable* lVar = new Variable;
			lVar->mType = VT_FLOAT;
			lVar->mSubType = pSubType;
			lVar->mCount = pCount;
			lVar->mName = pVariableName;

			int i;
			switch(pSubType)
			{
			case ST_FLOAT32:
				lVar->mFloatValue = new float32[pCount];
				for (i = 0; i < pCount; i++)
				{
					((float32*)lVar->mFloatValue)[i] = (float32)pData[i];
				}
				break;
			case ST_FLOAT64:
			case ST_FLOAT:
				lVar->mFloatValue = new float64[pCount];
				for (i = 0; i < pCount; i++)
				{
					((float64*)lVar->mFloatValue)[i] = (float64)pData[i];
				}
				break;
			}

			mVarTable->Insert(lVar->mName, lVar);
			lOk = true;
		}
	}

	return lOk;
}

bool Params::AddVariableString(const String& pVariableName, int pCount, const String* pData)
{
	bool lOk = false;

	if (pCount > 0)
	{
		VarTable::Iterator lIter = mVarTable->Find(pVariableName);

		if (lIter == mVarTable->End())
		{
			Variable* lVar = new Variable;
			lVar->mType = VT_STRING;
			lVar->mCount = pCount;
			lVar->mName = pVariableName;
			lVar->mString = new String[pCount];

			for (int i = 0; i < pCount; i++)
			{
				lVar->mString[i] = pData[i];
			}

			mVarTable->Insert(lVar->mName, lVar);
			lOk = true;
		}
	}

	return lOk;
}

bool Params::AddVariableStruct(const String& pVariableName, int pCount, const Params* pData)
{
	bool lOk = false;

	if (pCount > 0)
	{
		VarTable::Iterator lIter = mVarTable->Find(pVariableName);

		if (lIter == mVarTable->End())
		{
			Variable* lVar = new Variable;
			lVar->mType = VT_STRUCT;
			lVar->mCount = pCount;
			lVar->mName = pVariableName;
			lVar->mParams = new Params[pCount];
			lVar->mParams->SetName(String(pVariableName));

			for (int i = 0; i < pCount; i++)
			{
				lVar->mParams[i] = pData[i];
			}

			mVarTable->Insert(lVar->mName, lVar);
			lOk = true;
		}
	}

	return lOk;
}

bool Params::AddVariableInt(const String& pVariableName, std::list<int32>& pList, SubType pSubType)
{
	bool lOk = false;

	if (pList.empty() == false)
	{
		if (pSubType != ST_INT8 && 
		   pSubType != ST_INT16 &&
		   pSubType != ST_INT32 &&
		   pSubType != ST_INT)
		{
			pSubType = ST_INT;
		}

		VarTable::Iterator lTIter = mVarTable->Find(pVariableName);

		if (lTIter == mVarTable->End())
		{
			Variable* lVar = new Variable;
			lVar->mType = VT_INT;
			lVar->mSubType = pSubType;
			lVar->mCount = (int)pList.size();
			lVar->mName = pVariableName;

			std::list<int32>::iterator lIter;
			int i;
			switch(pSubType)
			{
			case ST_INT8:
				lVar->mIntValue = new int8[pList.size()];
				for (lIter = pList.begin(), i = 0; lIter != pList.end(); ++lIter, ++i)
				{
					((int8*)lVar->mIntValue)[i] = (int8)*lIter;
				}
				break;
			case ST_INT16:
				lVar->mIntValue = new int16[pList.size()];
				for (lIter = pList.begin(), i = 0; lIter != pList.end(); ++lIter, ++i)
				{
					((int16*)lVar->mIntValue)[i] = (int16)*lIter;
				}
				break;
			case ST_INT32:
			case ST_INT:
				lVar->mIntValue = new int32[pList.size()];
				for (lIter = pList.begin(), i = 0; lIter != pList.end(); ++lIter, ++i)
				{
					((int32*)lVar->mIntValue)[i] = (int32)*lIter;
				}
				break;
			}

			mVarTable->Insert(lVar->mName, lVar);
			lOk = true;
		}
	}

	return lOk;
}

bool Params::AddVariableFloat(const String& pVariableName, std::list<float64>& pList, SubType pSubType)
{
	bool lOk = false;

	if (pList.empty() == false)
	{
		if (pSubType != ST_FLOAT32 && 
		   pSubType != ST_FLOAT64 &&
		   pSubType != ST_FLOAT)
		{
			pSubType = ST_FLOAT;
		}

		VarTable::Iterator lTIter = mVarTable->Find(pVariableName);

		if (lTIter == mVarTable->End())
		{
			Variable* lVar = new Variable;
			lVar->mType = VT_FLOAT;
			lVar->mSubType = pSubType;
			lVar->mCount = (int)pList.size();
			lVar->mName = pVariableName;

			std::list<float64>::iterator lIter;
			int i;
			switch(pSubType)
			{
			case ST_FLOAT32:
				lVar->mFloatValue = new float32[pList.size()];
				for (lIter = pList.begin(), i = 0; lIter != pList.end(); ++lIter, ++i)
				{
					((float32*)lVar->mFloatValue)[i] = (float32)*lIter;
				}
				break;
			case ST_FLOAT64:
			case ST_FLOAT:
				lVar->mFloatValue = new float64[pList.size()];
				for (lIter = pList.begin(), i = 0; lIter != pList.end(); ++lIter, ++i)
				{
					((float64*)lVar->mFloatValue)[i] = (float64)*lIter;
				}
				break;
			}

			mVarTable->Insert(lVar->mName, lVar);
			lOk = true;
		}
	}

	return lOk;
}

bool Params::AddVariableString(const String& pVariableName, std::list<String>& pList)
{
	bool lOk = false;

	if (pList.empty() == false)
	{
		VarTable::Iterator lTIter = mVarTable->Find(pVariableName);

		if (lTIter == mVarTable->End())
		{
			Variable* lVar = new Variable;
			lVar->mType = VT_STRING;
			lVar->mCount = (int)pList.size();
			lVar->mName = pVariableName;
			lVar->mString = new String[pList.size()];

			std::list<String>::iterator lIter;
			int i;
			for (lIter = pList.begin(), i = 0; lIter != pList.end(); ++lIter, ++i)
			{
				lVar->mString[i] = *lIter;
			}

			mVarTable->Insert(lVar->mName, lVar);
			lOk = true;
		}
	}

	return lOk;
}

bool Params::AddVariableStruct(const String& pVariableName, std::list<Params*>& pList)
{
	bool lOk = false;

	if (pList.empty() == false)
	{
		VarTable::Iterator lTIter = mVarTable->Find(pVariableName);

		if (lTIter == mVarTable->End())
		{
			Variable* lVar = new Variable;
			lVar->mType = VT_STRUCT;
			lVar->mCount = (int)pList.size();
			lVar->mName = pVariableName;
			lVar->mParams = new Params[pList.size()];
			lVar->mParams->SetName(String(pVariableName));

			std::list<Params*>::iterator lIter;
			int i;
			for (lIter = pList.begin(), i = 0; lIter != pList.end(); ++lIter, ++i)
			{
				lVar->mParams[i] = *(*lIter);
			}

			mVarTable->Insert(lVar->mName, lVar);
			lOk = true;
		}
	}

	return lOk;
}

bool Params::AddVariableTuple(const String& pVariableName, int pCount)
{
	bool lOk = false;
	VarTable::Iterator lTIter = mVarTable->Find(pVariableName);

	if (lTIter == mVarTable->End())
	{
		Variable* lVar = new Variable;
		lVar->mType = VT_TUPLE;
		lVar->mCount = pCount;
		lVar->mName = pVariableName;
		lVar->mTupleMember = new TupleMember[lVar->mCount];

		mVarTable->Insert(lVar->mName, lVar);
		lOk = true;
	}

	return lOk;
}

bool Params::SetTupleMember(const String& pVariableName, int pIndex, int32 pData)
{
	bool lOk = false;
	VarTable::Iterator lTIter = mVarTable->Find(pVariableName);

	if (lTIter != mVarTable->End())
	{
		Variable* lVar = *lTIter;
		if (pIndex >= 0 && pIndex < lVar->mCount)
		{
			lVar->mTupleMember[pIndex].mType = VT_INT;
			lVar->mTupleMember[pIndex].mIntValue = pData;
			lVar->mTupleMember[pIndex].mFloatValue = 0;
			lVar->mTupleMember[pIndex].mStringValue = _T("");
			lOk = true;
		}
	}
	return lOk;
}

bool Params::SetTupleMember(const String& pVariableName, int pIndex, float64 pData)
{
	bool lOk = false;
	VarTable::Iterator lTIter = mVarTable->Find(pVariableName);

	if (lTIter != mVarTable->End())
	{
		Variable* lVar = *lTIter;
		if (pIndex >= 0 && pIndex < lVar->mCount)
		{
			lVar->mTupleMember[pIndex].mType = VT_FLOAT;
			lVar->mTupleMember[pIndex].mIntValue = 0;
			lVar->mTupleMember[pIndex].mFloatValue = pData;
			lVar->mTupleMember[pIndex].mStringValue = _T("");
			lOk = true;
		}
	}
	return lOk;
}

bool Params::SetTupleMember(const String& pVariableName, int pIndex, const String& pData)
{
	bool lOk = false;
	VarTable::Iterator lTIter = mVarTable->Find(pVariableName);

	if (lTIter != mVarTable->End())
	{
		Variable* lVar = *lTIter;
		if (pIndex >= 0 && pIndex < lVar->mCount)
		{
			lVar->mTupleMember[pIndex].mType = VT_STRING;
			lVar->mTupleMember[pIndex].mIntValue = 0;
			lVar->mTupleMember[pIndex].mFloatValue = 0;
			lVar->mTupleMember[pIndex].mStringValue = pData;
			lOk = true;
		}
	}
	return lOk;
}

} // End namespace.
