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
#include <math.h>

#include "AL/al.h"
#include "AL/alc.h"
#include "alMain.h"
#include "alAuxEffectSlot.h"
#include "alThunk.h"
#include "alError.h"


static ALvoid InitializeEffect(ALCcontext *Context, ALeffectslot *ALEffectSlot, ALeffect *effect);


ALvoid AL_APIENTRY alGenAuxiliaryEffectSlots(ALsizei n, ALuint *effectslots)
{
    ALCcontext *Context;
    ALsizei i, j;

    Context = GetContextSuspended();
    if(!Context) return;

    if (n > 0)
    {
        ALCdevice *Device = Context->Device;

        if(Context->AuxiliaryEffectSlotCount+n <= Device->AuxiliaryEffectSlotMax)
        {
            // Check that enough memory has been allocted in the 'effectslots' array for n Effect Slots
            if (!IsBadWritePtr((void*)effectslots, n * sizeof(ALuint)))
            {
                ALeffectslot **list = &Context->AuxiliaryEffectSlot;
                while(*list)
                    list = &(*list)->next;

                i = 0;
                while(i < n)
                {
                    *list = calloc(1, sizeof(ALeffectslot));
                    if(!(*list) || !((*list)->EffectState=NoneCreate()))
                    {
                        // We must have run out or memory
                        free(*list); *list = NULL;
                        alDeleteAuxiliaryEffectSlots(i, effectslots);
                        alSetError(AL_OUT_OF_MEMORY);
                        break;
                    }

                    (*list)->Gain = 1.0;
                    (*list)->AuxSendAuto = AL_TRUE;
                    for(j = 0;j < BUFFERSIZE;j++)
                        (*list)->WetBuffer[j] = 0.0f;
                    (*list)->refcount = 0;

                    effectslots[i] = (ALuint)ALTHUNK_ADDENTRY(*list);
                    (*list)->effectslot = effectslots[i];

                    Context->AuxiliaryEffectSlotCount++;
                    i++;

                    list = &(*list)->next;
                }
            }
        }
        else
            alSetError(AL_INVALID_OPERATION);
    }

    ProcessContext(Context);
}

ALvoid AL_APIENTRY alDeleteAuxiliaryEffectSlots(ALsizei n, ALuint *effectslots)
{
    ALCcontext *Context;
    ALeffectslot *ALAuxiliaryEffectSlot;
    ALsizei i;

    Context = GetContextSuspended();
    if(!Context) return;

    if (n >= 0)
    {
        // Check that all effectslots are valid
        for (i = 0; i < n; i++)
        {
            if (!alIsAuxiliaryEffectSlot(effectslots[i]))
            {
                alSetError(AL_INVALID_NAME);
                break;
            }
            else
            {
                ALAuxiliaryEffectSlot = (ALeffectslot*)ALTHUNK_LOOKUPENTRY(effectslots[i]);
                if(ALAuxiliaryEffectSlot->refcount > 0)
                {
                    alSetError(AL_INVALID_NAME);
                    break;
                }
            }
        }

        if (i == n)
        {
            // All effectslots are valid
            for (i = 0; i < n; i++)
            {
                // Recheck that the effectslot is valid, because there could be duplicated names
                if (alIsAuxiliaryEffectSlot(effectslots[i]))
                {
                    ALeffectslot **list;

                    ALAuxiliaryEffectSlot = ((ALeffectslot*)ALTHUNK_LOOKUPENTRY(effectslots[i]));

                    // Remove Source from list of Sources
                    list = &Context->AuxiliaryEffectSlot;
                    while(*list && *list != ALAuxiliaryEffectSlot)
                         list = &(*list)->next;

                    if(*list)
                        *list = (*list)->next;
                    ALTHUNK_REMOVEENTRY(ALAuxiliaryEffectSlot->effectslot);

                    if(ALAuxiliaryEffectSlot->EffectState)
                        ALEffect_Destroy(ALAuxiliaryEffectSlot->EffectState);

                    memset(ALAuxiliaryEffectSlot, 0, sizeof(ALeffectslot));
                    free(ALAuxiliaryEffectSlot);

                    Context->AuxiliaryEffectSlotCount--;
                }
            }
        }
    }
    else
        alSetError(AL_INVALID_VALUE);

    ProcessContext(Context);
}

ALboolean AL_APIENTRY alIsAuxiliaryEffectSlot(ALuint effectslot)
{
    ALCcontext *Context;
    ALeffectslot **list;

    Context = GetContextSuspended();
    if(!Context) return AL_FALSE;

    list = &Context->AuxiliaryEffectSlot;
    while(*list && (*list)->effectslot != effectslot)
        list = &(*list)->next;

    ProcessContext(Context);

    return (*list ? AL_TRUE : AL_FALSE);
}

ALvoid AL_APIENTRY alAuxiliaryEffectSloti(ALuint effectslot, ALenum param, ALint iValue)
{
    ALCcontext *Context;

    Context = GetContextSuspended();
    if(!Context) return;

    if (alIsAuxiliaryEffectSlot(effectslot))
    {
        ALeffectslot *ALEffectSlot = (ALeffectslot*)ALTHUNK_LOOKUPENTRY(effectslot);

        switch(param)
        {
        case AL_EFFECTSLOT_EFFECT:
            if(alIsEffect(iValue))
            {
                ALeffect *effect = (ALeffect*)ALTHUNK_LOOKUPENTRY(iValue);
                InitializeEffect(Context, ALEffectSlot, effect);
            }
            else
                alSetError(AL_INVALID_VALUE);
            break;

        case AL_EFFECTSLOT_AUXILIARY_SEND_AUTO:
            if(iValue == AL_TRUE || iValue == AL_FALSE)
                ALEffectSlot->AuxSendAuto = iValue;
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

ALvoid AL_APIENTRY alAuxiliaryEffectSlotiv(ALuint effectslot, ALenum param, ALint *piValues)
{
    ALCcontext *Context;

    Context = GetContextSuspended();
    if(!Context) return;

    if (alIsAuxiliaryEffectSlot(effectslot))
    {
        switch(param)
        {
        case AL_EFFECTSLOT_EFFECT:
        case AL_EFFECTSLOT_AUXILIARY_SEND_AUTO:
            alAuxiliaryEffectSloti(effectslot, param, piValues[0]);
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

ALvoid AL_APIENTRY alAuxiliaryEffectSlotf(ALuint effectslot, ALenum param, ALfloat flValue)
{
    ALCcontext *Context;

    Context = GetContextSuspended();
    if(!Context) return;

    if (alIsAuxiliaryEffectSlot(effectslot))
    {
        ALeffectslot *ALEffectSlot = (ALeffectslot*)ALTHUNK_LOOKUPENTRY(effectslot);

        switch(param)
        {
        case AL_EFFECTSLOT_GAIN:
            if(flValue >= 0.0f && flValue <= 1.0f)
                ALEffectSlot->Gain = flValue;
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

ALvoid AL_APIENTRY alAuxiliaryEffectSlotfv(ALuint effectslot, ALenum param, ALfloat *pflValues)
{
    ALCcontext *Context;

    Context = GetContextSuspended();
    if(!Context) return;

    if (alIsAuxiliaryEffectSlot(effectslot))
    {
        switch(param)
        {
        case AL_EFFECTSLOT_GAIN:
            alAuxiliaryEffectSlotf(effectslot, param, pflValues[0]);
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

ALvoid AL_APIENTRY alGetAuxiliaryEffectSloti(ALuint effectslot, ALenum param, ALint *piValue)
{
    ALCcontext *Context;

    Context = GetContextSuspended();
    if(!Context) return;

    if (alIsAuxiliaryEffectSlot(effectslot))
    {
        ALeffectslot *ALEffectSlot = (ALeffectslot*)ALTHUNK_LOOKUPENTRY(effectslot);

        switch(param)
        {
        case AL_EFFECTSLOT_EFFECT:
            *piValue = ALEffectSlot->effect.effect;
            break;

        case AL_EFFECTSLOT_AUXILIARY_SEND_AUTO:
            *piValue = ALEffectSlot->AuxSendAuto;
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

ALvoid AL_APIENTRY alGetAuxiliaryEffectSlotiv(ALuint effectslot, ALenum param, ALint *piValues)
{
    ALCcontext *Context;

    Context = GetContextSuspended();
    if(!Context) return;

    if (alIsAuxiliaryEffectSlot(effectslot))
    {
        switch(param)
        {
        case AL_EFFECTSLOT_EFFECT:
        case AL_EFFECTSLOT_AUXILIARY_SEND_AUTO:
            alGetAuxiliaryEffectSloti(effectslot, param, piValues);
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

ALvoid AL_APIENTRY alGetAuxiliaryEffectSlotf(ALuint effectslot, ALenum param, ALfloat *pflValue)
{
    ALCcontext *Context;

    Context = GetContextSuspended();
    if(!Context) return;

    if (alIsAuxiliaryEffectSlot(effectslot))
    {
        ALeffectslot *ALEffectSlot = (ALeffectslot*)ALTHUNK_LOOKUPENTRY(effectslot);

        switch(param)
        {
        case AL_EFFECTSLOT_GAIN:
            *pflValue = ALEffectSlot->Gain;
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

ALvoid AL_APIENTRY alGetAuxiliaryEffectSlotfv(ALuint effectslot, ALenum param, ALfloat *pflValues)
{
    ALCcontext *Context;

    Context = GetContextSuspended();
    if(!Context) return;

    if (alIsAuxiliaryEffectSlot(effectslot))
    {
        switch(param)
        {
        case AL_EFFECTSLOT_GAIN:
            alGetAuxiliaryEffectSlotf(effectslot, param, pflValues);
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


static ALvoid NoneDestroy(ALeffectState *State)
{ free(State); }
static ALboolean NoneDeviceUpdate(ALeffectState *State, ALCdevice *Device)
{
    return AL_TRUE;
    (void)State;
    (void)Device;
}
static ALvoid NoneUpdate(ALeffectState *State, ALCcontext *Context, const ALeffect *Effect)
{
    (void)State;
    (void)Context;
    (void)Effect;
}
static ALvoid NoneProcess(ALeffectState *State, const ALeffectslot *Slot, ALuint SamplesToDo, const ALfloat *SamplesIn, ALfloat (*SamplesOut)[OUTPUTCHANNELS])
{
    (void)State;
    (void)Slot;
    (void)SamplesToDo;
    (void)SamplesIn;
    (void)SamplesOut;
}
ALeffectState *NoneCreate(void)
{
    ALeffectState *state;

    state = calloc(1, sizeof(*state));
    if(!state)
    {
        alSetError(AL_OUT_OF_MEMORY);
        return NULL;
    }

    state->Destroy = NoneDestroy;
    state->DeviceUpdate = NoneDeviceUpdate;
    state->Update = NoneUpdate;
    state->Process = NoneProcess;

    return state;
}

static ALvoid InitializeEffect(ALCcontext *Context, ALeffectslot *ALEffectSlot, ALeffect *effect)
{
    if(ALEffectSlot->effect.type != (effect?effect->type:AL_EFFECT_NULL))
    {
        ALeffectState *NewState = NULL;
        if(!effect || effect->type == AL_EFFECT_NULL)
            NewState = NoneCreate();
        else if(effect->type == AL_EFFECT_EAXREVERB)
            NewState = EAXVerbCreate();
        else if(effect->type == AL_EFFECT_REVERB)
            NewState = VerbCreate();
        else if(effect->type == AL_EFFECT_ECHO)
            NewState = EchoCreate();
        /* No new state? An error occured.. */
        if(NewState == NULL ||
           ALEffect_DeviceUpdate(NewState, Context->Device) == AL_FALSE)
        {
            if(NewState)
                ALEffect_Destroy(NewState);
            return;
        }
        if(ALEffectSlot->EffectState)
            ALEffect_Destroy(ALEffectSlot->EffectState);
        ALEffectSlot->EffectState = NewState;
    }
    if(!effect)
    {
        memset(&ALEffectSlot->effect, 0, sizeof(ALEffectSlot->effect));
        return;
    }
    memcpy(&ALEffectSlot->effect, effect, sizeof(*effect));
    ALEffect_Update(ALEffectSlot->EffectState, Context, effect);
}


ALvoid ReleaseALAuxiliaryEffectSlots(ALCcontext *Context)
{
    while(Context->AuxiliaryEffectSlot)
    {
        ALeffectslot *temp = Context->AuxiliaryEffectSlot;
        Context->AuxiliaryEffectSlot = Context->AuxiliaryEffectSlot->next;

        // Release effectslot structure
        if(temp->EffectState)
            ALEffect_Destroy(temp->EffectState);
        ALTHUNK_REMOVEENTRY(temp->effectslot);

        memset(temp, 0, sizeof(ALeffectslot));
        free(temp);
    }
    Context->AuxiliaryEffectSlotCount = 0;
}
