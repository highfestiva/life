/**
 * OpenAL cross platform audio library
 * Copyright (C) 1999-2007 by authors.
 * This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA  02111-1307, USA.
 * Or go to http://www.gnu.org/copyleft/lgpl.html
 */

#include "config.h"

#include <stdlib.h>

#include "AL/al.h"
#include "AL/alc.h"
#include "alMain.h"
#include "alFilter.h"
#include "alThunk.h"
#include "alError.h"


static void InitFilterParams(ALfilter *filter, ALenum type);


ALvoid AL_APIENTRY alGenFilters(ALsizei n, ALuint *filters)
{
    ALCcontext *Context;
    ALsizei i;

    Context = GetContextSuspended();
    if(!Context) return;

    if (n > 0)
    {
        ALCdevice *device = Context->Device;

        // Check that enough memory has been allocted in the 'filters' array for n Filters
        if (!IsBadWritePtr((void*)filters, n * sizeof(ALuint)))
        {
            ALfilter **list = &device->FilterList;
            while(*list)
                list = &(*list)->next;

            i = 0;
            while(i < n)
            {
                *list = calloc(1, sizeof(ALfilter));
                if(!(*list))
                {
                    // We must have run out or memory
                    alDeleteFilters(i, filters);
                    alSetError(AL_OUT_OF_MEMORY);
                    break;
                }

                filters[i] = (ALuint)ALTHUNK_ADDENTRY(*list);
                (*list)->filter = filters[i];

                InitFilterParams(*list, AL_FILTER_NULL);
                device->FilterCount++;
                i++;

                list = &(*list)->next;
            }
        }
    }

    ProcessContext(Context);
}

ALvoid AL_APIENTRY alDeleteFilters(ALsizei n, ALuint *filters)
{
    ALCcontext *Context;
    ALfilter *ALFilter;
    ALsizei i;

    Context = GetContextSuspended();
    if(!Context) return;

    if (n >= 0)
    {
        ALCdevice *device = Context->Device;

        // Check that all filters are valid
        for (i = 0; i < n; i++)
        {
            if (!alIsFilter(filters[i]))
            {
                alSetError(AL_INVALID_NAME);
                break;
            }
        }

        if (i == n)
        {
            // All filters are valid
            for (i = 0; i < n; i++)
            {
                // Recheck that the filter is valid, because there could be duplicated names
                if (filters[i] && alIsFilter(filters[i]))
                {
                    ALfilter **list;

                    ALFilter = ((ALfilter*)ALTHUNK_LOOKUPENTRY(filters[i]));

                    // Remove Source from list of Sources
                    list = &device->FilterList;
                    while(*list && *list != ALFilter)
                         list = &(*list)->next;

                    if(*list)
                        *list = (*list)->next;
                    ALTHUNK_REMOVEENTRY(ALFilter->filter);

                    memset(ALFilter, 0, sizeof(ALfilter));
                    free(ALFilter);

                    device->FilterCount--;
                }
            }
        }
    }
    else
        alSetError(AL_INVALID_VALUE);

    ProcessContext(Context);
}

ALboolean AL_APIENTRY alIsFilter(ALuint filter)
{
    ALCcontext *Context;
    ALfilter *list;

    Context = GetContextSuspended();
    if(!Context) return AL_FALSE;

    list = Context->Device->FilterList;
    while(list && list->filter != filter)
        list = list->next;

    ProcessContext(Context);

    return ((list || !filter) ? AL_TRUE : AL_FALSE);
}

ALvoid AL_APIENTRY alFilteri(ALuint filter, ALenum param, ALint iValue)
{
    ALCcontext *Context;

    Context = GetContextSuspended();
    if(!Context) return;

    if (filter && alIsFilter(filter))
    {
        ALfilter *ALFilter = (ALfilter*)ALTHUNK_LOOKUPENTRY(filter);

        switch(param)
        {
        case AL_FILTER_TYPE:
            if(iValue == AL_FILTER_NULL ||
               iValue == AL_FILTER_LOWPASS)
                InitFilterParams(ALFilter, iValue);
            else
                alSetError(AL_INVALID_VALUE);
            break;

        default:
            alSetError(AL_INVALID_ENUM);
            break;
        }
    }
    else
        alSetError(AL_INVALID_NAME);

    ProcessContext(Context);
}

ALvoid AL_APIENTRY alFilteriv(ALuint filter, ALenum param, ALint *piValues)
{
    ALCcontext *Context;

    Context = GetContextSuspended();
    if(!Context) return;

    if (filter && alIsFilter(filter))
    {
        switch(param)
        {
        case AL_FILTER_TYPE:
            alFilteri(filter, param, piValues[0]);
            break;

        default:
            alSetError(AL_INVALID_ENUM);
            break;
        }
    }
    else
        alSetError(AL_INVALID_NAME);

    ProcessContext(Context);
}

ALvoid AL_APIENTRY alFilterf(ALuint filter, ALenum param, ALfloat flValue)
{
    ALCcontext *Context;

    Context = GetContextSuspended();
    if(!Context) return;

    if (filter && alIsFilter(filter))
    {
        ALfilter *ALFilter = (ALfilter*)ALTHUNK_LOOKUPENTRY(filter);

        switch(ALFilter->type)
        {
        case AL_FILTER_LOWPASS:
            switch(param)
            {
            case AL_LOWPASS_GAIN:
                if(flValue >= 0.0f && flValue <= 1.0f)
                    ALFilter->Gain = flValue;
                else
                    alSetError(AL_INVALID_VALUE);
                break;

            case AL_LOWPASS_GAINHF:
                if(flValue >= 0.0f && flValue <= 1.0f)
                    ALFilter->GainHF = flValue;
                else
                    alSetError(AL_INVALID_VALUE);
                break;

            default:
                alSetError(AL_INVALID_ENUM);
                break;
            }
            break;

        default:
            alSetError(AL_INVALID_ENUM);
            break;
        }
    }
    else
        alSetError(AL_INVALID_NAME);

    ProcessContext(Context);
}

ALvoid AL_APIENTRY alFilterfv(ALuint filter, ALenum param, ALfloat *pflValues)
{
    ALCcontext *Context;

    Context = GetContextSuspended();
    if(!Context) return;

    if (filter && alIsFilter(filter))
    {
        switch(param)
        {
        default:
            alFilterf(filter, param, pflValues[0]);
            break;
        }
    }
    else
        alSetError(AL_INVALID_NAME);

    ProcessContext(Context);
}

ALvoid AL_APIENTRY alGetFilteri(ALuint filter, ALenum param, ALint *piValue)
{
    ALCcontext *Context;

    Context = GetContextSuspended();
    if(!Context) return;

    if (filter && alIsFilter(filter))
    {
        ALfilter *ALFilter = (ALfilter*)ALTHUNK_LOOKUPENTRY(filter);

        switch(param)
        {
        case AL_FILTER_TYPE:
            *piValue = ALFilter->type;
            break;

        default:
            alSetError(AL_INVALID_ENUM);
            break;
        }
    }
    else
        alSetError(AL_INVALID_NAME);

    ProcessContext(Context);
}

ALvoid AL_APIENTRY alGetFilteriv(ALuint filter, ALenum param, ALint *piValues)
{
    ALCcontext *Context;

    Context = GetContextSuspended();
    if(!Context) return;

    if (filter && alIsFilter(filter))
    {
        switch(param)
        {
        case AL_FILTER_TYPE:
            alGetFilteri(filter, param, piValues);
            break;

        default:
            alSetError(AL_INVALID_ENUM);
            break;
        }
    }
    else
        alSetError(AL_INVALID_NAME);

    ProcessContext(Context);
}

ALvoid AL_APIENTRY alGetFilterf(ALuint filter, ALenum param, ALfloat *pflValue)
{
    ALCcontext *Context;

    Context = GetContextSuspended();
    if(!Context) return;

    if (filter && alIsFilter(filter))
    {
        ALfilter *ALFilter = (ALfilter*)ALTHUNK_LOOKUPENTRY(filter);

        switch(ALFilter->type)
        {
        case AL_FILTER_LOWPASS:
            switch(param)
            {
            case AL_LOWPASS_GAIN:
                *pflValue = ALFilter->Gain;
                break;

            case AL_LOWPASS_GAINHF:
                *pflValue = ALFilter->GainHF;
                break;

            default:
                alSetError(AL_INVALID_ENUM);
                break;
            }
            break;

        default:
            alSetError(AL_INVALID_ENUM);
            break;
        }
    }
    else
        alSetError(AL_INVALID_NAME);

    ProcessContext(Context);
}

ALvoid AL_APIENTRY alGetFilterfv(ALuint filter, ALenum param, ALfloat *pflValues)
{
    ALCcontext *Context;

    Context = GetContextSuspended();
    if(!Context) return;

    if (filter && alIsFilter(filter))
    {
        switch(param)
        {
        default:
            alGetFilterf(filter, param, pflValues);
            break;
        }
    }
    else
        alSetError(AL_INVALID_NAME);

    ProcessContext(Context);
}


ALvoid ReleaseALFilters(ALCdevice *device)
{
    ALfilter *list = device->FilterList;
    while(list)
    {
        ALfilter *temp = list;
        list = list->next;

        // Release filter structure
        memset(temp, 0, sizeof(ALfilter));
        free(temp);
    }
    device->FilterList = NULL;
    device->FilterCount = 0;
}


static void InitFilterParams(ALfilter *filter, ALenum type)
{
    filter->type = type;

    filter->Gain = 1.0;
    filter->GainHF = 1.0;
}
