/**
 * OpenAL cross platform audio library
 * Copyright (C) 1999-2000 by authors.
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

#include "alMain.h"
#include "AL/alc.h"
#include "alError.h"
#include "alListener.h"

ALAPI ALvoid ALAPIENTRY alListenerf(ALenum eParam, ALfloat flValue)
{
    ALCcontext *pContext;

    pContext = GetContextSuspended();
    if(!pContext) return;

    switch(eParam)
    {
        case AL_GAIN:
            if(flValue >= 0.0f)
                pContext->Listener.Gain = flValue;
            else
                alSetError(AL_INVALID_VALUE);
            break;

        case AL_METERS_PER_UNIT:
            if(flValue > 0.0f)
                pContext->Listener.MetersPerUnit = flValue;
            else
                alSetError(AL_INVALID_VALUE);
            break;

        default:
            alSetError(AL_INVALID_ENUM);
            break;
    }

    ProcessContext(pContext);
}


ALAPI ALvoid ALAPIENTRY alListener3f(ALenum eParam, ALfloat flValue1, ALfloat flValue2, ALfloat flValue3)
{
    ALCcontext *pContext;

    pContext = GetContextSuspended();
    if(!pContext) return;

    switch(eParam)
    {
        case AL_POSITION:
            pContext->Listener.Position[0] = flValue1;
            pContext->Listener.Position[1] = flValue2;
            pContext->Listener.Position[2] = flValue3;
            break;

        case AL_VELOCITY:
            pContext->Listener.Velocity[0] = flValue1;
            pContext->Listener.Velocity[1] = flValue2;
            pContext->Listener.Velocity[2] = flValue3;
            break;

        default:
            alSetError(AL_INVALID_ENUM);
            break;
    }

    ProcessContext(pContext);
}


ALAPI ALvoid ALAPIENTRY alListenerfv(ALenum eParam, const ALfloat *pflValues)
{
    ALCcontext *pContext;

    pContext = GetContextSuspended();
    if(!pContext) return;

    if(pflValues)
    {
        switch(eParam)
        {
            case AL_GAIN:
                if(pflValues[0] >= 0.0f)
                    pContext->Listener.Gain = pflValues[0];
                else
                    alSetError(AL_INVALID_VALUE);
                break;

            case AL_METERS_PER_UNIT:
                if(pflValues[0] > 0.0f)
                    pContext->Listener.MetersPerUnit = pflValues[0];
                else
                    alSetError(AL_INVALID_VALUE);
                break;

            case AL_POSITION:
                pContext->Listener.Position[0] = pflValues[0];
                pContext->Listener.Position[1] = pflValues[1];
                pContext->Listener.Position[2] = pflValues[2];
                break;

            case AL_VELOCITY:
                pContext->Listener.Velocity[0] = pflValues[0];
                pContext->Listener.Velocity[1] = pflValues[1];
                pContext->Listener.Velocity[2] = pflValues[2];
                break;

            case AL_ORIENTATION:
                // AT then UP
                pContext->Listener.Forward[0] = pflValues[0];
                pContext->Listener.Forward[1] = pflValues[1];
                pContext->Listener.Forward[2] = pflValues[2];
                pContext->Listener.Up[0] = pflValues[3];
                pContext->Listener.Up[1] = pflValues[4];
                pContext->Listener.Up[2] = pflValues[5];
                break;

            default:
                alSetError(AL_INVALID_ENUM);
                break;
        }
    }
    else
        alSetError(AL_INVALID_VALUE);

    ProcessContext(pContext);
}


ALAPI ALvoid ALAPIENTRY alListeneri(ALenum eParam, ALint lValue)
{
    ALCcontext *pContext;

    (void)lValue;

    pContext = GetContextSuspended();
    if(!pContext) return;

    switch(eParam)
    {
        default:
            alSetError(AL_INVALID_ENUM);
            break;
    }

    ProcessContext(pContext);
}


ALAPI void ALAPIENTRY alListener3i(ALenum eParam, ALint lValue1, ALint lValue2, ALint lValue3)
{
    ALCcontext *pContext;

    pContext = GetContextSuspended();
    if(!pContext) return;

    switch(eParam)
    {
        case AL_POSITION:
        case AL_VELOCITY:
            alListener3f(eParam, (ALfloat)lValue1, (ALfloat)lValue2, (ALfloat)lValue3);
            break;

        default:
            alSetError(AL_INVALID_ENUM);
            break;
    }

    ProcessContext(pContext);
}


ALAPI void ALAPIENTRY alListeneriv( ALenum eParam, const ALint* plValues )
{
    ALCcontext *pContext;
    ALfloat flValues[6];

    pContext = GetContextSuspended();
    if(!pContext) return;

    if(plValues)
    {
        switch(eParam)
        {
            case AL_POSITION:
            case AL_VELOCITY:
                flValues[0] = (ALfloat)plValues[0];
                flValues[1] = (ALfloat)plValues[1];
                flValues[2] = (ALfloat)plValues[2];
                alListenerfv(eParam, flValues);
                break;

            case AL_ORIENTATION:
                flValues[0] = (ALfloat)plValues[0];
                flValues[1] = (ALfloat)plValues[1];
                flValues[2] = (ALfloat)plValues[2];
                flValues[3] = (ALfloat)plValues[3];
                flValues[4] = (ALfloat)plValues[4];
                flValues[5] = (ALfloat)plValues[5];
                alListenerfv(eParam, flValues);
                break;

            default:
                alSetError(AL_INVALID_ENUM);
                break;
        }
    }
    else
        alSetError(AL_INVALID_VALUE);

    ProcessContext(pContext);
}


ALAPI ALvoid ALAPIENTRY alGetListenerf(ALenum eParam, ALfloat *pflValue)
{
    ALCcontext *pContext;

    pContext = GetContextSuspended();
    if(!pContext) return;

    if(pflValue)
    {
        switch(eParam)
        {
            case AL_GAIN:
                *pflValue = pContext->Listener.Gain;
                break;

            case AL_METERS_PER_UNIT:
                *pflValue = pContext->Listener.MetersPerUnit;
                break;

            default:
                alSetError(AL_INVALID_ENUM);
                break;
        }
    }
    else
        alSetError(AL_INVALID_VALUE);

    ProcessContext(pContext);
}


ALAPI ALvoid ALAPIENTRY alGetListener3f(ALenum eParam, ALfloat *pflValue1, ALfloat *pflValue2, ALfloat *pflValue3)
{
    ALCcontext *pContext;

    pContext = GetContextSuspended();
    if(!pContext) return;

    if(pflValue1 && pflValue2 && pflValue3)
    {
        switch(eParam)
        {
            case AL_POSITION:
                *pflValue1 = pContext->Listener.Position[0];
                *pflValue2 = pContext->Listener.Position[1];
                *pflValue3 = pContext->Listener.Position[2];
                break;

            case AL_VELOCITY:
                *pflValue1 = pContext->Listener.Velocity[0];
                *pflValue2 = pContext->Listener.Velocity[1];
                *pflValue3 = pContext->Listener.Velocity[2];
                break;

            default:
                alSetError(AL_INVALID_ENUM);
                break;
        }
    }
    else
        alSetError(AL_INVALID_VALUE);

    ProcessContext(pContext);
}


ALAPI ALvoid ALAPIENTRY alGetListenerfv(ALenum eParam, ALfloat *pflValues)
{
    ALCcontext *pContext;

    pContext = GetContextSuspended();
    if(!pContext) return;

    if(pflValues)
    {
        switch(eParam)
        {
            case AL_GAIN:
                pflValues[0] = pContext->Listener.Gain;
                break;

            case AL_METERS_PER_UNIT:
                pflValues[0] = pContext->Listener.MetersPerUnit;
                break;

            case AL_POSITION:
                pflValues[0] = pContext->Listener.Position[0];
                pflValues[1] = pContext->Listener.Position[1];
                pflValues[2] = pContext->Listener.Position[2];
                break;

            case AL_VELOCITY:
                pflValues[0] = pContext->Listener.Velocity[0];
                pflValues[1] = pContext->Listener.Velocity[1];
                pflValues[2] = pContext->Listener.Velocity[2];
                break;

            case AL_ORIENTATION:
                // AT then UP
                pflValues[0] = pContext->Listener.Forward[0];
                pflValues[1] = pContext->Listener.Forward[1];
                pflValues[2] = pContext->Listener.Forward[2];
                pflValues[3] = pContext->Listener.Up[0];
                pflValues[4] = pContext->Listener.Up[1];
                pflValues[5] = pContext->Listener.Up[2];
                break;

            default:
                alSetError(AL_INVALID_ENUM);
                break;
        }
    }
    else
        alSetError(AL_INVALID_VALUE);

    ProcessContext(pContext);
}


ALAPI ALvoid ALAPIENTRY alGetListeneri(ALenum eParam, ALint *plValue)
{
    ALCcontext *pContext;

    pContext = GetContextSuspended();
    if(!pContext) return;

    if(plValue)
    {
        switch(eParam)
        {
            default:
                alSetError(AL_INVALID_ENUM);
                break;
        }
    }
    else
        alSetError(AL_INVALID_VALUE);

    ProcessContext(pContext);
}


ALAPI void ALAPIENTRY alGetListener3i(ALenum eParam, ALint *plValue1, ALint *plValue2, ALint *plValue3)
{
    ALCcontext *pContext;

    pContext = GetContextSuspended();
    if(!pContext) return;

    if(plValue1 && plValue2 && plValue3)
    {
        switch (eParam)
        {
            case AL_POSITION:
                *plValue1 = (ALint)pContext->Listener.Position[0];
                *plValue2 = (ALint)pContext->Listener.Position[1];
                *plValue3 = (ALint)pContext->Listener.Position[2];
                break;

            case AL_VELOCITY:
                *plValue1 = (ALint)pContext->Listener.Velocity[0];
                *plValue2 = (ALint)pContext->Listener.Velocity[1];
                *plValue3 = (ALint)pContext->Listener.Velocity[2];
                break;

            default:
                alSetError(AL_INVALID_ENUM);
                break;
        }
    }
    else
        alSetError(AL_INVALID_VALUE);

    ProcessContext(pContext);
}


ALAPI void ALAPIENTRY alGetListeneriv(ALenum eParam, ALint* plValues)
{
    ALCcontext *pContext;

    pContext = GetContextSuspended();
    if(!pContext) return;

    if(plValues)
    {
        switch(eParam)
        {
            case AL_POSITION:
                plValues[0] = (ALint)pContext->Listener.Position[0];
                plValues[1] = (ALint)pContext->Listener.Position[1];
                plValues[2] = (ALint)pContext->Listener.Position[2];
                break;

            case AL_VELOCITY:
                plValues[0] = (ALint)pContext->Listener.Velocity[0];
                plValues[1] = (ALint)pContext->Listener.Velocity[1];
                plValues[2] = (ALint)pContext->Listener.Velocity[2];
                break;

            case AL_ORIENTATION:
                // AT then UP
                plValues[0] = (ALint)pContext->Listener.Forward[0];
                plValues[1] = (ALint)pContext->Listener.Forward[1];
                plValues[2] = (ALint)pContext->Listener.Forward[2];
                plValues[3] = (ALint)pContext->Listener.Up[0];
                plValues[4] = (ALint)pContext->Listener.Up[1];
                plValues[5] = (ALint)pContext->Listener.Up[2];
                break;

            default:
                alSetError(AL_INVALID_ENUM);
                break;
        }
    }
    else
        alSetError(AL_INVALID_VALUE);

    ProcessContext(pContext);
}
