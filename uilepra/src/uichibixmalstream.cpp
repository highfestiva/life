
// Thanks David Churchill for "Module Music on iPhone/iPad with ChibiXM " and many thanks to ChibiXM!



#include "pch.h"
#include "../include/uichibixmalstream.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/file.h"
#include "../../lepra/include/fileopener.h"
#include "../include/uisoundmanager.h"

#ifdef LEPRA_MSVC
#pragma warning(disable: 4996)	// fopen unsafe.
#endif // Visual
#define AL_CHECK()	if (alGetError() != AL_NO_ERROR) return false;
#define kSampleRate	22050
#define kBufferSize	(4096 * 8)



namespace uilepra {



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
// File system Interface
///////////////////////////////////////////////////////////////////////////////
static FileOpener* g_file_opener = 0;
static File* g_file = 0;

static xm_bool fileio_in_use() {
	return g_file? xm_true : xm_false;
}

static XM_FileIOError fileio_open(const char *p_file, xm_bool p_big_endian_mode) {
	if (g_file) {
		return XM_FILE_ERROR_IN_USE;
	}

	g_file = g_file_opener->Open(p_file);
	if (!g_file) {
		return XM_FILE_ERROR_CANT_OPEN;
	}

	g_file->SetReaderEndian(p_big_endian_mode? Endian::kTypeBigEndian : Endian::kTypeLittleEndian);

	return XM_FILE_OK;
}

static xm_u8 fileio_get_u8() {
	if (!g_file) {
		return 0;
	}
	xm_u8 b;
	g_file->Read(b);
	return b;
}

static xm_u16 fileio_get_u16() {
	if (!g_file) {
		return 0;
	}
	xm_u16 w;
	g_file->Read(w);
	return w;
}

static xm_u32 fileio_get_u32() {
	if (!g_file) {
		return 0;
	}
	xm_u32 dw;
	g_file->Read(dw);
	return dw;
}

static void fileio_get_byte_array(xm_u8 *p_dst, xm_u32 p_count) {
	if (!g_file) {
		return;
	}
	g_file->ReadData(p_dst, p_count);
}

static void fileio_seek_pos(xm_u32 p_offset) {
	if (!g_file) {
		return;
	}
	g_file->SeekSet(p_offset);
}

static xm_u32 fileio_get_pos() {
	if (!g_file) {
		return 0;
	}
	return (xm_u32)g_file->Tell();
}

static xm_bool fileio_eof_reached() {
	if (!g_file) {
		return xm_true;
	}
	return (g_file->Tell() == g_file->GetSize())? xm_true : xm_false;
}

static void fileio_close() {
	if (g_file) {
		g_file->Close();
		delete g_file;
		g_file = 0;
	}
}



void ChibiXmAlStream::SetFileOpener(FileOpener* opener) {
	delete g_file_opener;
	g_file_opener = opener;
}



ChibiXmAlStream::ChibiXmAlStream(SoundManager* sound_manager, const str& filename, bool loop):
	Parent(sound_manager) {
	deb_assert(g_file_opener);

	is_looping_ = loop;

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

	xm_create_software_mixer(kSampleRate, 32);

	song_ = 0;

	memset(al_buffers_, 0, sizeof(al_buffers_));
	al_source_ = 0;
	al_format_ = AL_FORMAT_MONO8;

	Open(filename);
}

ChibiXmAlStream::~ChibiXmAlStream() {
	Release();
}

bool ChibiXmAlStream::Playback() {
	if (IsPlaying()) {
		return true;
	}

	xm_player_play();

	if (!Stream(al_buffers_[0]) || !Stream(al_buffers_[1])) {
		return false;
	}

	alSourceQueueBuffers(al_source_, 2, al_buffers_);
	alSourcef(al_source_, AL_GAIN, volume_ * sound_manager_->GetMusicVolume());
	alSourcePlay(al_source_);
	return true;
}

bool ChibiXmAlStream::Rewind() {
	const bool is_playing = IsPlaying();
	xm_player_stop();
	if (is_playing) {
		xm_player_play();
	}
	return true;
}

bool ChibiXmAlStream::IsPlaying() const {
	ALenum state = !AL_PLAYING;
	alGetSourcei(al_source_, AL_SOURCE_STATE, &state);
	return (state == AL_PLAYING && xm_player_is_playing());
}

bool ChibiXmAlStream::Stop() {
	Pause();
	return Rewind();
}

bool ChibiXmAlStream::Pause() {
	alSourceStop(al_source_);
	return Clear();
}

bool ChibiXmAlStream::Update() {
	bool is_active = true;
	int processed_buffer_count;
	alGetSourcei(al_source_, AL_BUFFERS_PROCESSED, &processed_buffer_count);
	alSourcef(al_source_, AL_GAIN, volume_ * sound_manager_->GetMusicVolume());
	AL_CHECK();
	while (processed_buffer_count--) {
		ALuint buffer;
		alSourceUnqueueBuffers(al_source_, 1, &buffer);
		AL_CHECK();

		is_active &= Stream(buffer);

		alSourceQueueBuffers(al_source_, 1, &buffer);
		AL_CHECK();
	}
	if (!is_active && is_looping_) {
		Clear();
		is_active = Rewind();
	}
	TimeoutAutoResume();
	return is_active;
}

bool ChibiXmAlStream::Open(const str& filename) {
	Release();

	song_ = xm_song_alloc();
	if (xm_loader_open_song(filename.c_str(), song_) != XM_LOADER_OK) {
		Release();
		return false;
	}

	xm_player_set_song(song_);

	alGenBuffers(2, al_buffers_);
	AL_CHECK();
	alGenSources(1, &al_source_);
	AL_CHECK();

	alSource3f(al_source_, AL_POSITION,		0.0, 0.0, 0.0);
	alSource3f(al_source_, AL_VELOCITY,		0.0, 0.0, 0.0);
	alSource3f(al_source_, AL_DIRECTION,		0.0, 0.0, 0.0);
	alSourcef (al_source_, AL_ROLLOFF_FACTOR,	0.0);
	alSourcei (al_source_, AL_SOURCE_RELATIVE,	AL_TRUE);
	is_open_ = true;
	return is_open_;
}

bool ChibiXmAlStream::Release() {
	xm_player_stop();
	if (song_) {
		xm_song_free(song_);
		song_ = 0;
	}

	alSourceStop(al_source_);
	if (!Clear()) {
		return false;
	}

	alDeleteSources(1, &al_source_);
	AL_CHECK();
	alDeleteBuffers(1, al_buffers_);
	AL_CHECK();

	is_open_ = false;
	return true;
}

bool ChibiXmAlStream::Stream(ALuint buffer) {
	char data[kBufferSize];
	int size = 0;
	while (size < kBufferSize) {
		int result = XmRead(data + size, kBufferSize - size);
		if (result > 0) {
			size += result;
		} else if (result < 0) {
			return false;
		} else {
			break;
		}
	}

	if (size == 0) {
		return false;
	}

	alBufferData(buffer, al_format_, data, size, kSampleRate);
	AL_CHECK();
	return true;
}

bool ChibiXmAlStream::Clear() {
	int queued_buffer_count;
	alGetSourcei(al_source_, AL_BUFFERS_QUEUED, &queued_buffer_count);
	while (queued_buffer_count--) {
		ALuint buffer;
		alSourceUnqueueBuffers(al_source_, 1, &buffer);
		AL_CHECK();
	}
	return true;
}

void ChibiXmAlStream::TimeoutAutoResume() {
	ALenum state;
	alGetSourcei(al_source_, AL_SOURCE_STATE, &state);
	if (state == AL_STOPPED) {
		alSourcePlay(al_source_);
	}
}

int ChibiXmAlStream::XmRead(char* _buffer, int buffer_byte_count) {
	xm_s32 mix_buffer[kBufferSize * 2];		// Chibi is always in stereo.
	::memset(mix_buffer, 0, sizeof(mix_buffer));	// Format is stereo-interleaved 32-bit, so multiply by 8.
	const int frame_count = buffer_byte_count;
	xm_software_mix_to_buffer(mix_buffer, frame_count);
	for (int x = 0; x < buffer_byte_count; ++x) {
		// Average stereo into mono.
		_buffer[x] = (unsigned char)(unsigned)((((mix_buffer[x*2+0]>>1) + (mix_buffer[x*2+1]>>1)) >> 24) + 128);
	}
	return buffer_byte_count;
}



loginstance(kUiSound, ChibiXmAlStream);



}
