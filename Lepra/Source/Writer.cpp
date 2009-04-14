/*
	Class:  Writer
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	I needed to put this function here since including String.h results in
	further includes which are not compatible with the jpeg library...

	And what does the jpeg library have to do with anything? Well, dunno, but
	the compilation will fail, and that's not a good thing. :)
*/



#include "../Include/Writer.h"
#include "../Include/OutputStream.h"



namespace Lepra
{



IOError Writer::WriteData(const void* pBuffer, unsigned pSize)
{
	return mOutStream->WriteRaw(pBuffer, pSize);
}

const String& Writer::GetStreamName()
{
	return mOutStream->GetName();
}



}
