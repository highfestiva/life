#ifndef _AL_FILTER_H_
#define _AL_FILTER_H_

#include "AL/al.h"
#include "alu.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ALfloat coeff;
#ifndef _MSC_VER
    ALfloat history[0];
#else
    ALfloat history[1];
#endif
} FILTER;

static __inline ALfloat lpFilter4P(FILTER *iir, ALuint offset, ALfloat input)
{
    ALfloat *history = &iir->history[offset];
    ALfloat a = iir->coeff;
    ALfloat output = input;

    output = output + (history[0]-output)*a;
    history[0] = output;
    output = output + (history[1]-output)*a;
    history[1] = output;
    output = output + (history[2]-output)*a;
    history[2] = output;
    output = output + (history[3]-output)*a;
    history[3] = output;

    return output;
}

static __inline ALfloat lpFilter2P(FILTER *iir, ALuint offset, ALfloat input)
{
    ALfloat *history = &iir->history[offset];
    ALfloat a = iir->coeff;
    ALfloat output = input;

    output = output + (history[0]-output)*a;
    history[0] = output;
    output = output + (history[1]-output)*a;
    history[1] = output;

    return output;
}


#define AL_FILTER_TYPE                                     0x8001

#define AL_FILTER_NULL                                     0x0000
#define AL_FILTER_LOWPASS                                  0x0001
#define AL_FILTER_HIGHPASS                                 0x0002
#define AL_FILTER_BANDPASS                                 0x0003

#define AL_LOWPASS_GAIN                                    0x0001
#define AL_LOWPASS_GAINHF                                  0x0002


typedef struct ALfilter
{
    // Filter type (AL_FILTER_NULL, ...)
    ALenum type;

    ALfloat Gain;
    ALfloat GainHF;

    // Index to itself
    ALuint filter;

    struct ALfilter *next;
} ALfilter;

ALvoid AL_APIENTRY alGenFilters(ALsizei n, ALuint *filters);
ALvoid AL_APIENTRY alDeleteFilters(ALsizei n, ALuint *filters);
ALboolean AL_APIENTRY alIsFilter(ALuint filter);

ALvoid AL_APIENTRY alFilteri(ALuint filter, ALenum param, ALint iValue);
ALvoid AL_APIENTRY alFilteriv(ALuint filter, ALenum param, ALint *piValues);
ALvoid AL_APIENTRY alFilterf(ALuint filter, ALenum param, ALfloat flValue);
ALvoid AL_APIENTRY alFilterfv(ALuint filter, ALenum param, ALfloat *pflValues);

ALvoid AL_APIENTRY alGetFilteri(ALuint filter, ALenum param, ALint *piValue);
ALvoid AL_APIENTRY alGetFilteriv(ALuint filter, ALenum param, ALint *piValues);
ALvoid AL_APIENTRY alGetFilterf(ALuint filter, ALenum param, ALfloat *pflValue);
ALvoid AL_APIENTRY alGetFilterfv(ALuint filter, ALenum param, ALfloat *pflValues);

ALvoid ReleaseALFilters(ALCdevice *device);

#ifdef __cplusplus
}
#endif

#endif
