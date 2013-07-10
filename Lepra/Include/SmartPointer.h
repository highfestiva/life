/*
	Class:  SmartPointer
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/



#ifndef LEPRA_SMARTPOINTER_H
#define LEPRA_SMARTPOINTER_H



namespace Lepra
{



#define	SMART_POINTER_REFERENCE	public:	SmartPointerReference _mRef;



class SmartPointerReference
{
public:
	mutable int mReferenceCount;	// Public for optimization reasons only.
	inline SmartPointerReference():	// Inlined for optimization reasons only.
		mReferenceCount(0)
	{
	};
};

#define TEMPLATE template<class Type>
#define QUAL SmartPointer<Type>

TEMPLATE class SmartPointer
{
public:

	SmartPointer(Type* pObject = 0);
	SmartPointer(const SmartPointer& pCopy);
	// TRICKY: (optimized) this class does not have a virtual destructor.
	~SmartPointer();

	void operator=(Type* pObject);
	void operator=(const SmartPointer& pCopy);
	operator Type*() const;
	Type* operator->() const;

	void SetObject(Type* pObject);
	void ReleaseObject();

private:
	Type* mObject;
};



TEMPLATE QUAL::SmartPointer(Type* pObject):
	mObject(0)
{
	SetObject(pObject);
}

TEMPLATE QUAL::SmartPointer(const SmartPointer& pCopy):
	mObject(0)
{
	SetObject(pCopy.mObject);
}

TEMPLATE QUAL::~SmartPointer()
{
	ReleaseObject();
}



TEMPLATE void QUAL::operator=(Type* pObject)
{
	SetObject(pObject);
}

TEMPLATE void QUAL::operator=(const SmartPointer& pCopy)
{
	SetObject(pCopy.mObject);
}

TEMPLATE QUAL::operator Type*() const
{
	return (mObject);
}

TEMPLATE Type* QUAL::operator->() const
{
	return (mObject);
}



TEMPLATE void QUAL::SetObject(Type* pObject)
{
	if (pObject != mObject)
	{
		ReleaseObject();

		mObject = pObject;
		if (mObject)
		{
			++mObject->_mRef.mReferenceCount;
		}
	}
}

TEMPLATE void QUAL::ReleaseObject()
{
	if (mObject)
	{
		if (--mObject->_mRef.mReferenceCount == 0)
		{
			delete (mObject);
		}
		mObject = 0;
	}
}

#undef TEMPLATE
#undef QUAL


}



#endif // !LEPRA_SMARTPOINTER_H
