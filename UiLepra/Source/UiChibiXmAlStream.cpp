
// Thanks David Churchill for "Module Music on iPhone/iPad with ChibiXM " and many thanks to ChibiXM!



#include "../Include/UiChibiXmAlStream.h"
#include <assert.h>
#include "../../Lepra/Include/File.h"
#include "../../Lepra/Include/FileOpener.h"
#include "../Include/UiSoundManager.h"

#ifdef LEPRA_MSVC
#pragma warning(disable: 4996)	// fopen unsafe.
#endif // Visual
#define AL_CHECK()	if (alGetError() != AL_NO_ERROR) return false;
#define SAMPLE_RATE	22050
#define BUFFER_SIZE	(4096 * 8)



namespace UiLepra
{



// Memory manager function bindings used by chibi to allocate/free memory
static XM_MemoryManager s_memory_manager;

// File I/O function bindings used by chibi to do file i/o operations
static XM_FileIO        s_file_io;

///////////////////////////////////////////////////////////////////////////////
// Memory Manager Interface
///////////////////////////////////////////////////////////////////////////////
static void* alloc_mem(xm_u32 p_size, XM_MemoryAllocType) {
	return malloc(p_size);
}

static void free_mem(void *p_mem, XM_MemoryAllocType) {
	free(p_mem);
}


///////////////////////////////////////////////////////////////////////////////
// File System Interface
///////////////////////////////////////////////////////////////////////////////
static FileOpener* gFileOpener = 0;
static File* gFile = 0;

static xm_bool fileio_in_use()
{
	return gFile? xm_true : xm_false;		
}

static XM_FileIOError fileio_open(const char *p_file, xm_bool p_big_endian_mode) 
{
	if (gFile)
	{
		return XM_FILE_ERROR_IN_USE;
	}

	gFile = gFileOpener->Open(strutil::Encode(p_file));
	if (!gFile)
	{
		return XM_FILE_ERROR_CANT_OPEN;
	}

	gFile->SetReaderEndian(p_big_endian_mode? Endian::TYPE_BIG_ENDIAN : Endian::TYPE_LITTLE_ENDIAN);

	return XM_FILE_OK;
}

static xm_u8 fileio_get_u8() 
{
	if (!gFile)
	{
		return 0;
	}
	xm_u8 b;
	gFile->Read(b);
	return b;
}

static xm_u16 fileio_get_u16() 
{
	if (!gFile)
	{
		return 0;
	}
	xm_u16 w;
	gFile->Read(w);
	return w;
}

static xm_u32 fileio_get_u32() 
{
	if (!gFile)
	{
		return 0;
	}
	xm_u32 dw;
	gFile->Read(dw);
	return dw;
}

static void fileio_get_byte_array(xm_u8 *p_dst, xm_u32 p_count)
{
	if (!gFile)
	{
		return;
	}
	gFile->ReadData(p_dst, p_count);
}

static void fileio_seek_pos(xm_u32 p_offset) 
{
	if (!gFile)
	{
		return;
	}
	gFile->SeekSet(p_offset);
}

static xm_u32 fileio_get_pos() 
{
	if (!gFile)
	{
		return 0;
	}
	return (xm_u32)gFile->Tell();
}

static xm_bool fileio_eof_reached() 
{
	if (!gFile)
	{
		return xm_true;
	}
	return (gFile->Tell() == gFile->GetSize())? xm_true : xm_false;
}

static void fileio_close() 
{
	if (gFile)
	{
		gFile->Close();
		delete gFile;
		gFile = 0;
	}
}



void ChibiXmAlStream::SetFileOpener(FileOpener* pOpener)
{
	delete gFileOpener;
	gFileOpener = pOpener;
}



ChibiXmAlStream::ChibiXmAlStream(SoundManager* pSoundManager, const str& pFilename, bool pLoop):
	Parent(pSoundManager)
{
	assert(gFileOpener);

	mIsLooping = pLoop;

	// Setup ChibiXM memory & file IO.
	memset(&s_memory_manager, 0, sizeof(XM_MemoryManager));
	s_memory_manager.alloc = alloc_mem;
	s_memory_manager.free  = free_mem;
	xm_set_memory_manager(&s_memory_manager);
	memset(&s_file_io, 0, sizeof(XM_FileIO));
	s_file_io.in_use         = fileio_in_use;
	s_file_io.open           = fileio_open;
	s_file_io.get_u8         = fileio_get_u8;
	s_file_io.get_u16        = fileio_get_u16;
	s_file_io.get_u32        = fileio_get_u32;
	s_file_io.get_byte_array = fileio_get_byte_array;
	s_file_io.seek_pos       = fileio_seek_pos;
	s_file_io.get_pos        = fileio_get_pos;
	s_file_io.eof_reached    = fileio_eof_reached;
	s_file_io.close          = fileio_close;
	xm_loader_set_fileio(&s_file_io);

	xm_create_software_mixer(SAMPLE_RATE, 32);

	mSong = 0;

	memset(mAlBuffers, 0, sizeof(mAlBuffers));
	mAlSource = 0;
	mAlFormat = AL_FORMAT_MONO8;

	Open(pFilename);
}

ChibiXmAlStream::~ChibiXmAlStream()
{
	Release();
}

bool ChibiXmAlStream::Playback()
{
	if (IsPlaying())
	{
		return true;
	}

	xm_player_play();

	if (!Stream(mAlBuffers[0]) || !Stream(mAlBuffers[1]))
	{
		return false;
	}

	alSourceQueueBuffers(mAlSource, 2, mAlBuffers);
	alSourcePlay(mAlSource);
	return true;
}

bool ChibiXmAlStream::Rewind()
{
	const bool lIsPlaying = IsPlaying();
	xm_player_stop();
	if (lIsPlaying)
	{
		xm_player_play();
	}
	return true;
}

bool ChibiXmAlStream::IsPlaying() const
{
	ALenum state = !AL_PLAYING;
	alGetSourcei(mAlSource, AL_SOURCE_STATE, &state);
	return (state == AL_PLAYING && xm_player_is_playing());
}

bool ChibiXmAlStream::Stop()
{
	Pause();
	return Rewind();
}

bool ChibiXmAlStream::Pause()
{
	alSourceStop(mAlSource);
	return Clear();
}

bool ChibiXmAlStream::Update()
{
	bool lIsActive = true;
	int lProcessedBufferCount;
	alGetSourcei(mAlSource, AL_BUFFERS_PROCESSED, &lProcessedBufferCount);
	alSourcef(mAlSource, AL_GAIN, mVolume * mSoundManager->GetMasterVolume());
	AL_CHECK();
	while (lProcessedBufferCount--)
	{
		ALuint buffer;
		alSourceUnqueueBuffers(mAlSource, 1, &buffer);
		AL_CHECK();

		lIsActive &= Stream(buffer);

		alSourceQueueBuffers(mAlSource, 1, &buffer);
		AL_CHECK();
	}
	if (!lIsActive && mIsLooping)
	{
		Clear();
		lIsActive = Rewind();
	}
	TimeoutAutoResume();
	return lIsActive;
}

bool ChibiXmAlStream::Open(const str& pFilename)
{
	Release();

	mSong = xm_song_alloc();
	if (xm_loader_open_song(astrutil::Encode(pFilename).c_str(), mSong) != XM_LOADER_OK)
	{
		Release();
		return false;
	}

	xm_player_set_song(mSong);

	alGenBuffers(2, mAlBuffers);
	AL_CHECK();
	alGenSources(1, &mAlSource);
	AL_CHECK();

	alSource3f(mAlSource, AL_POSITION,		0.0, 0.0, 0.0);
	alSource3f(mAlSource, AL_VELOCITY,		0.0, 0.0, 0.0);
	alSource3f(mAlSource, AL_DIRECTION,		0.0, 0.0, 0.0);
	alSourcef (mAlSource, AL_ROLLOFF_FACTOR,	0.0);
	alSourcei (mAlSource, AL_SOURCE_RELATIVE,	AL_TRUE);
	mIsOpen = true;
	return mIsOpen;
}

bool ChibiXmAlStream::Release()
{
	xm_player_stop();
	if (mSong)
	{
		xm_song_free(mSong);
		mSong = 0;
	}

	alSourceStop(mAlSource);
	if (!Clear())
	{
		return false;
	}

	alDeleteSources(1, &mAlSource);
	AL_CHECK();
	alDeleteBuffers(1, mAlBuffers);
	AL_CHECK();

	mIsOpen = false;
	return true;
}

bool ChibiXmAlStream::Stream(ALuint buffer)
{
	char lData[BUFFER_SIZE];
	int lSize = 0;
	while (lSize < BUFFER_SIZE)
	{
		int lResult = XmRead(lData + lSize, BUFFER_SIZE - lSize);
		if (lResult > 0)
		{
			lSize += lResult;
		}
		else if (lResult < 0)
		{
			return false;
		}
		else
		{
			break;
		}
	}

	if (lSize == 0)
	{
		return false;
	}

	alBufferData(buffer, mAlFormat, lData, lSize, SAMPLE_RATE);
	AL_CHECK();
	return true;
}

bool ChibiXmAlStream::Clear()
{
	int lQueuedBufferCount;
	alGetSourcei(mAlSource, AL_BUFFERS_QUEUED, &lQueuedBufferCount);
	while (lQueuedBufferCount--)
	{
		ALuint buffer;
		alSourceUnqueueBuffers(mAlSource, 1, &buffer);
		AL_CHECK();
	}
	return true;
}

void ChibiXmAlStream::TimeoutAutoResume()
{
	ALenum state;
	alGetSourcei(mAlSource, AL_SOURCE_STATE, &state);
	if (state == AL_STOPPED)
	{
		alSourcePlay(mAlSource);
	}
}

int ChibiXmAlStream::XmRead(char* pBuffer, int pBufferByteCount)
{
	xm_s32 lMixBuffer[BUFFER_SIZE * 2];		// Chibi is always in stereo.
	::memset(lMixBuffer, 0, sizeof(lMixBuffer));	// Format is stereo-interleaved 32-bit, so multiply by 8.
	const int lFrameCount = pBufferByteCount;
	xm_software_mix_to_buffer(lMixBuffer, lFrameCount);
	for (int x = 0; x < pBufferByteCount; ++x)
	{
		// Average stereo into mono.
		pBuffer[x] = (unsigned char)(unsigned)((((lMixBuffer[x*2+0]>>1) + (lMixBuffer[x*2+1]>>1)) >> 24) + 128);
	}
	return pBufferByteCount;
}



LOG_CLASS_DEFINE(UI_SOUND, ChibiXmAlStream);



}