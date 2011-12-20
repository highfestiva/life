
// Thanks Jesse Maurais for "OpenAL Lesson 8: OggVorbis Streaming Using The Source Queue"!



#pragma once

#include "UiLepra.h"
#include <xmplay.h>
#if defined(LEPRA_MAC)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else // !Mac
#include <AL/al.h>
#endif // Mac/!Mac
#include "UiSoundStream.h"



namespace Lepra
{
class FileOpener;
}



namespace UiLepra
{



class ChibiXmAlStream: public SoundStream
{
public:
	typedef SoundStream Parent;

	static void SetFileOpener(FileOpener* pOpener);

	ChibiXmAlStream(SoundManager* pSoundManager, const str& pFilename, bool pLoop);
	virtual ~ChibiXmAlStream();
	virtual bool Playback();		// play the Ogg stream
	virtual bool Rewind();
	virtual bool IsPlaying() const;		// check if the source is playing
	virtual bool Stop();
	virtual bool Pause();
	virtual bool Update();			// update the stream if necessary

protected:
	bool Open(const str& pFilename);	// obtain a handle to the file
	bool Release();				// release the file handle
	bool Stream(ALuint buffer);		// reloads a buffer
	bool Clear();				// empties the queue
	void TimeoutAutoResume();		// If too high latencies between updates, OpenAL stops playing. This resumes.
	int XmRead(char* pBuffer, int pBufferByteCount);

private:
	XM_Song* mSong;

	ALuint mAlBuffers[2];			// front and back buffers
	ALuint mAlSource;			// audio source
	ALenum mAlFormat;			// internal format

	LOG_CLASS_DECLARE();
};



}
