
// Thanks Jesse Maurais for "OpenAL Lesson 8: OggVorbis Streaming Using The Source Queue"!



#pragma once

#include "UiLepra.h"
#if defined(LEPRA_MAC)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else // !Mac
#include <AL/al.h>
#endif // Mac/!Mac
#include <vorbis/vorbisfile.h>
#include "UiLepra.h"
#include "UiSoundStream.h"



namespace UiLepra
{



class OggAlStream: public SoundStream
{
public:
	OggAlStream(const str& pFilename, bool pLoop);
	virtual ~OggAlStream();
	virtual bool Playback();		// play the Ogg stream
	virtual bool Rewind();
	virtual bool IsPlaying() const;		// check if the source is playing
	virtual bool Stop();
	virtual bool Update();			// update the stream if necessary

protected:
	bool Open(const str& pFilename);	// obtain a handle to the file
	bool Release();				// release the file handle
	bool Stream(ALuint buffer);		// reloads a buffer
	bool Clear();				// empties the queue

private:
	FILE*		mOggFile;		// file handle
	OggVorbis_File	mOggStream;		// stream handle
	vorbis_info*	mVorbisInfo;		// some formatting data

	ALuint mAlBuffers[2];			// front and back buffers
	ALuint mAlSource;			// audio source
	ALenum mAlFormat;			// internal format
};



}
