
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once



namespace Lepra
{



class ListUtil
{
public:
	template<class _ListType> static void DeleteAll(_ListType& pList)
	{
		while(!pList.empty())
		{
			delete pList.front();
			pList.pop_front();
		}
	}
	template<class _ListType> static typename _ListType::iterator FindByIndex(_ListType& pList, size_t pIndex)
	{
		if(pIndex < 0 || pIndex >= pList.size())
			return pList.end();

		typename _ListType::iterator it;
		if(pIndex <= pList.size() / 2)
		{
			size_t i;
			for(i = 0, it = pList.begin(); i < pIndex; ++i, ++it)
			{}
		}
		else
		{
			size_t i;
			for(i = pList.size() - 1, it = --pList.end(); i > pIndex; --i, --it)
			{}
		}

		return it;
	}
	// Returns -1 if not found.
	template<class _ListType> static int FindIndexOf(_ListType& pList, const typename _ListType::value_type& pValue)
	{
		int lIndex = 0;
		typename _ListType::iterator it;
		for(it = pList.begin(); it != pList.end(); ++it, ++lIndex)
		{
			if(*it == pValue)
				return lIndex;
		}

		return -1;
	}
};


template<class _Type, class _ListType = std::vector<_Type*> >
class VectorUtil
{
public:
	static void CloneList(_ListType& pTarget, const _ListType& pSource)
	{
		const size_t cnt = pSource.size();
		pTarget.resize(cnt);
		for (size_t x = 0; x < cnt; ++x)
		{
			pTarget[x] = new _Type(*pSource[x]);
		}
	}

	static void CloneListFactoryMethod(_ListType& pTarget, const _ListType& pSource)
	{
		const size_t cnt = pSource.size();
		pTarget.resize(cnt);
		for (size_t x = 0; x < cnt; ++x)
		{
			pTarget[x] = _Type::Create(*pSource[x]);
		}
	}
};



}
