
// Thanks Jesse Maurais for "OpenAL Lesson 8: OggVorbis Streaming Using The Source Queue"!



#pragma once

#include "uilepra.h"
#if defined(LEPRA_MAC)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else // !Mac
#include <AL/al.h>
#endif // Mac/!Mac
#include <vorbis/vorbisfile.h>
#include "uilepra.h"
#include "uisoundstream.h"



namespace uilepra {



class OggAlStream: public SoundStream {
public:
	typedef SoundStream Parent;

	OggAlStream(SoundManager* sound_manager, const str& filename, bool loop);
	virtual ~OggAlStream();
	virtual bool Playback();		// play the Ogg stream
	virtual bool Rewind();
	virtual bool IsPlaying() const;		// check if the source is playing
	virtual bool Stop();
	virtual bool Pause();
	virtual bool Update();			// update the stream if necessary

protected:
	bool Open(const str& filename);	// obtain a handle to the file
	bool Release();				// release the file handle
	bool Stream(ALuint buffer);		// reloads a buffer
	bool Clear();				// empties the queue
	void TimeoutAutoResume();		// If too high latencies between updates, OpenAL stops playing. This resumes.

private:
	FILE*		ogg_file_;		// file handle
	OggVorbis_File	ogg_stream_;		// stream handle
	vorbis_info*	vorbis_info_;		// some formatting data

	ALuint al_buffers_[2];			// front and back buffers
	ALuint al_source_;			// audio source
	ALenum al_format_;			// internal format
};



}
