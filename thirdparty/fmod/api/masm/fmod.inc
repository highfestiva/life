;=====================================================================
; Translation of fmod.h to MASM 6.14 assembly language.
; Translation done by Ricardo Santos
; dxantos@yahoo.com
; Updated to 3.6 by Brett Paterson 6/04/2002
;=====================================================================

ifndef __FMOD_INC__
__FMOD_INC__ = 0

;=====================================================================
;Equates
;=====================================================================

; Version
FMOD_VERSION equ 3.6f

;---------------------------------------------------------------------
; FMOD_ERRORS
;
; Description: On failure of commands in FMOD, use FSOUND_GetError to
;              attain what happened.
;
; See Also: FSOUND_GetError
;---------------------------------------------------------------------
FMOD_ERR_NONE               equ 0   ; No errors
FMOD_ERR_BUSY               equ 1   ; Cannot call this command after FSOUND_Init.  Call FSOUND_Close first.
FMOD_ERR_UNINITIALIZED      equ 2   ; This command failed because FSOUND_Init was not called
FMOD_ERR_INIT               equ 3   ; Error initializing output device.
FMOD_ERR_ALLOCATED          equ 4   ; Error initializing output device, but more specifically, the output device is already in use and cannot be reused.
FMOD_ERR_PLAY               equ 5   ; Playing the sound failed.
FMOD_ERR_OUTPUT_FORMAT      equ 6   ; Soundcard does not support the features needed for this soundsystem (16bit stereo output)
FMOD_ERR_COOPERATIVELEVEL   equ 7   ; Error setting cooperative level for hardware.
FMOD_ERR_CREATEBUFFER       equ 8   ; Error creating hardware sound buffer.
FMOD_ERR_FILE_NOTFOUND      equ 9   ; File not found
FMOD_ERR_FILE_FORMAT        equ 10  ; Unknown file format
FMOD_ERR_FILE_BAD           equ 11  ; Error loading file
FMOD_ERR_MEMORY             equ 12  ; Not enough memory
FMOD_ERR_VERSION            equ 13  ; The version number of this file format is not supported
FMOD_ERR_INVALID_PARAM      equ 14  ; An invalid parameter was passed to this function
FMOD_ERR_NO_EAX             equ 15  ; Tried to use an EAX command on a non EAX enabled channel or output.
FMOD_ERR_CHANNEL_ALLOC      equ 17  ; Failed to allocate a new channel
FMOD_ERR_RECORD             equ 18  ; Recording is not supported on this machine
FMOD_ERR_MEDIAPLAYER		equ 19	; Required Mediaplayer codec is not installed
FMOD_ERR_CDDEVICE           equ 20  ; An error occured trying to open the specified CD device


;---------------------------------------------------------------------
; FSOUND_OUTPUTTYPES
;
; Description:  
;    These output types are used with FSOUND_SetOutput, to choose which output driver to use.
;    
;    FSOUND_OUTPUT_DSOUND will not support hardware 3d acceleration if the sound card driver 
;    does not support DirectX 6 Voice Manager Extensions.
;
;    FSOUND_OUTPUT_WINMM is recommended for NT and CE.
;
;
; See Also:     FSOUND_SetOutput
;               FSOUND_GetOutput
;---------------------------------------------------------------------
FSOUND_OUTPUT_NOSOUND   equ 0 ; NoSound driver, all calls to this succeed but do nothing.
FSOUND_OUTPUT_WINMM     equ 1 ; Windows Multimedia driver.
FSOUND_OUTPUT_DSOUND    equ 2 ; DirectSound driver.  You need this to get EAX or EAX2 support, or FX api support.
FSOUND_OUTPUT_A3D       equ 3 ; A3D driver.  not supported any more.

FSOUND_OUTPUT_OSS		equ 4 ; Linux/Unix OSS (Open Sound System) driver, i.e. the kernel sound drivers.
FSOUND_OUTPUT_ESD		equ 5 ; Linux/Unix ESD (Enlightment Sound Daemon) driver.
FSOUND_OUTPUT_ALSA		equ 6 ; Linux Alsa driver.

FSOUND_OUTPUT_ASIO      equ 7 ; Low latency ASIO driver
FSOUND_OUTPUT_XBOX      equ 8 ; Xbox driver
FSOUND_OUTPUT_PS2       equ 9 ; PlayStation 2 driver
FSOUND_OUTPUT_MAC       equ 10 ; Mac SoundMager driver


;---------------------------------------------------------------------
; FSOUND_MIXERTYPE
;
; Description:  These mixer types are used with FSOUND_SetMixer, to
;               choose which mixer to use, or to act upon for other
;               reasons using FSOUND_GetMixer.
;
; See Also:     FSOUND_SetMixer
;               FSOUND_GetMixer
;---------------------------------------------------------------------
FSOUND_MIXER_AUTODETECT                 equ 0   ; Windows CE only - Non interpolating/low quality mixer. 
FSOUND_MIXER_BLENDMODE                  equ 1   ; removed / obsolete.
FSOUND_MIXER_MMXP5                      equ 2   ; removed / obsolete. 
FSOUND_MIXER_MMXP6                      equ 3   ; removed / obsolete. 

FSOUND_MIXER_QUALITY_AUTODETECT         equ 4   ; All platforms - Autodetect the fastest quality mixer based on your cpu. 
FSOUND_MIXER_QUALITY_FPU                equ 5   ; Win32/Linux only - Interpolating/volume ramping FPU mixer.  
FSOUND_MIXER_QUALITY_MMXP5              equ 6   ; Win32/Linux only - Interpolating/volume ramping P5 MMX mixer.  
FSOUND_MIXER_QUALITY_MMXP6              equ 7   ; Win32/Linux only - Interpolating/volume ramping ppro+ MMX mixer. 

FSOUND_MIXER_MONO                       equ 8   ; Windows CE only - MONO non interpolating/low quality mixer. For speed
FSOUND_MIXER_QUALITY_MONO               equ 9   ; Windows CE only - MONO Interpolating mixer.  For speed

;---------------------------------------------------------------------
; FMUSIC_TYPES
;
; Description:  These definitions describe the type of song being played.
;
; See Also:     FMUSIC_GetType
;---------------------------------------------------------------------
FMUSIC_TYPE_NONE    equ 0       ; No song being played
FMUSIC_TYPE_MOD     equ 1       ; Protracker / Fasttracker
FMUSIC_TYPE_S3M     equ 2       ; ScreamTracker 3
FMUSIC_TYPE_XM      equ 3       ; FastTracker 2
FMUSIC_TYPE_IT      equ 4       ; Impulse Tracker.
FMUSIC_TYPE_MIDI	equ 5		; MIDI file

;---------------------------------------------------------------------
; FSOUND_DSP_PRIORITIES
;
; Description:  These default priorities are
;
; See Also:     FSOUND_DSP_Create
;               FSOUND_DSP_SetPriority
;---------------------------------------------------------------------
FSOUND_DSP_DEFAULTPRIORITY_CLEARUNIT        equ 0       ; DSP CLEAR unit - done first
FSOUND_DSP_DEFAULTPRIORITY_SFXUNIT          equ 100     ; DSP SFX unit - done second
FSOUND_DSP_DEFAULTPRIORITY_MUSICUNIT        equ 200     ; DSP MUSIC unit - done third
FSOUND_DSP_DEFAULTPRIORITY_USER             equ 300     ; User priority, use this as reference
FSOUND_DSP_DEFAULTPRIORITY_FFTUNIT          equ 900     ; This reads data for FSOUND_DSP_GetSpectrum, so it comes after user units
FSOUND_DSP_DEFAULTPRIORITY_CLIPANDCOPYUNIT  equ 1000    ; DSP CLIP AND COPY unit - last

;---------------------------------------------------------------------
; FSOUND_CAPS
;
; Description:  Driver description bitfields.  Use FSOUND_Driver_GetCaps
;               to determine if a driver enumerated has the settings you
;               are after.  The enumerated driver depends on the output
;               mode, see FSOUND_OUTPUTTYPES
;
; See Also:     FSOUND_GetDriverCaps
;               FSOUND_OUTPUTTYPES
;---------------------------------------------------------------------
FSOUND_CAPS_HARDWARE                equ 01h ; This driver supports hardware accelerated 3d sound.
FSOUND_CAPS_EAX2                    equ 02h ; This driver supports EAX2 reverb
FSOUND_CAPS_EAX3                    equ 10h ; This driver supports EAX3 reverb

;---------------------------------------------------------------------
; FSOUND_MODES
;
; Description:  Sample description bitfields, OR them together for
;               loading and describing samples.
;---------------------------------------------------------------------
FSOUND_LOOP_OFF     equ 0000001h  ; For non looping samples.
FSOUND_LOOP_NORMAL  equ 0000002h  ; For forward looping samples.
FSOUND_LOOP_BIDI    equ 0000004h  ; For bidirectional looping samples.  (no effect if in hardware).
FSOUND_8BITS        equ 0000008h  ; For 8 bit samples.
FSOUND_16BITS       equ 0000010h  ; For 16 bit samples.
FSOUND_MONO         equ 0000020h  ; For mono samples.
FSOUND_STEREO       equ 0000040h  ; For stereo samples.
FSOUND_UNSIGNED     equ 0000080h  ; For source data containing unsigned samples.
FSOUND_SIGNED       equ 0000100h  ; For source data containing signed data.
FSOUND_DELTA        equ 0000200h  ; For source data stored as delta values.
FSOUND_IT214        equ 0000400h  ; For source data stored using IT214 compression.
FSOUND_IT215        equ 0000800h  ; For source data stored using IT215 compression.
FSOUND_HW3D         equ 0001000h  ; Attempts to make samples use 3d hardware acceleration. (if the card supports it)
FSOUND_2D           equ 0002000h  ; Ignores any 3d processing.  overrides FSOUND_HW3D.  Located in software.
FSOUND_STREAMABLE   equ 0004000h  ; For realtime streamable samples. If you dont supply this sound may come out corrupted.
FSOUND_LOADMEMORY	equ 0008000h  ; 'name' will be interpreted as a pointer to data for streaming and samples.
FSOUND_LOADRAW		equ 0010000h  ; For  will ignore file format and treat as raw pcm.
FSOUND_MPEGACCURATE	equ 0020000h  ; For FSOUND_Stream_OpenFile - for accurate FSOUND_Stream_GetLengthMs/FSOUND_Stream_SetTime.  WARNING, see FSOUNDStream_OpenFile for inital opening time performance issues.
FSOUND_FORCEMONO    equ 0040000h  ; For forcing stereo streams and samples to be mono - needed with FSOUND_HW3D - incurs speed hit
FSOUND_HW2D         equ 0080000h  ; 2d hardware sounds.  allows hardware specific effects
FSOUND_ENABLEFX     equ 0100000h  ; Allows DX8 FX to be played back on a sound.  Requires DirectX 8 - Note these sounds cant be played more than once, or have a changing frequency
FSOUND_MPEGHALFRATE equ 0200000h  ; For FMODCE only - decodes mpeg streams using a lower quality decode, but faster execution
FSOUND_XADPCM       equ 0400000h  ; For XBOX only - Describes a user sample that its contents are compressed as XADPCM
FSOUND_VAG          equ 0800000h  ; For PS2 only - Describes a user sample that its contents are compressed as Sony VAG format */
FSOUND_NONBLOCKING  equ 1000000h  ; For FSOUND_Stream_OpenFile - Causes stream to open in the background and not block the foreground app - stream plays only when ready. */

;---------------------------------------------------------------------
; FSOUND_CDPLAYMODES
;
; Description:  Playback method for a CD Audio track, using
;               FSOUND_CD_Play
;
; See Also:     FSOUND_CD_Play
;---------------------------------------------------------------------
FSOUND_CD_PLAYCONTINUOUS    equ 0   ; Starts from the current track and plays to end of CD.
FSOUND_CD_PLAYONCE          equ 1   ; Plays the specified track then stops.
FSOUND_CD_PLAYLOOPED        equ 2   ; Plays the specified track looped, forever until stopped manually.
FSOUND_CD_PLAYRANDOM        equ 3   ; Plays tracks in random order

;---------------------------------------------------------------------
; FSOUND_SAMPLEMODE
;
; Description:  Misccellaneous values for FMOD functions.
;
; See Also:     FSOUND_PlaySound
;               FSOUND_PlaySoundEx
;				FSOUND_Sample_Alloc
;               FSOUND_Sample_Load
;               FSOUND_SetPan
;               
;---------------------------------------------------------------------
FSOUND_FREE          equ -1     ; value to play on any free channel, or to allocate a sample in a free sample slot.
FSOUND_UNMANAGED     equ -2     ; value to allocate a sample that is NOT managed by FSOUND or placed in a sample slot.
FSOUND_ALL			 equ -3	    ; for a channel index , this flag will affect ALL channels available!  Not supported by every function.
FSOUND_STEREOPAN     equ -1     ; value for FSOUND_SetPan so that stereo sounds are not played at half volume.  See FSOUND_SetPan for more on this.
FSOUND_SYSTEMCHANNEL equ -1000  ; special channel ID for channel based functions that want to alter the global FSOUND software mixing output channel
FSOUND_SYSTEMSAMPLE  equ -1000  ; special 'sample' ID for all sample based functions that want to alter the global FSOUND software mixing output sample

;---------------------------------------------------------------------
; FSOUND_REVERB_PROPERTIES
;
; Description:  Structure defining a reverb environment.  See FMOD.H for environment presets.
;
; Remarks:
;    For more indepth descriptions of the reverb properties under win32, please see the EAX3
;    documentation at http://developer.creative.com/ under the 'downloads' section.
;    If they do not have the EAX3 documentation, then most information can be attained from
;    the EAX2 documentation, as EAX3 only adds some more parameters and functionality on top of 
;    EAX2.
;    Note the default reverb properties are the same as the FSOUND_PRESET_GENERIC preset.
;    Note that integer values that typically range from -10,000 to 1000 are represented in 
;    decibels, and are of a logarithmic scale, not linear, wheras float values are typically linear.
;    PORTABILITY: Each member has the platform it supports in braces ie (win32/xbox).  
;    Some reverb parameters are only supported in win32 and some only on xbox. If all parameters are set then
;    the reverb should product a similar effect on either platform.
;    Linux and FMODCE do not support the reverb api.
;
; See Also:     FSOUND_Reverb_SetProperties
;               FSOUND_Reverb_GetProperties
;
;---------------------------------------------------------------------

FSOUND_REVERB_PROPERTIES struct     ; MIN     MAX    DEFAULT DESCRIPTION
    Environment       dword ?       ; 0       25     0       sets all listener properties (win32 only)
    EnvSize           real4 ?       ; 1.0     100.0  7.5     environment size in meters (win32 only)
    EnvDiffusion      real4 ?       ; 0.0     1.0    1.0     environment diffusion (win32/xbox)
    Room              dword ?       ; -10000  0      -1000   room effect level (at mid frequencies) (win32/xbox)
    RoomHF            dword ?       ; -10000  0      -100    relative room effect level at high frequencies (win32/xbox)
    RoomLF            dword ?       ; -10000  0      0       relative room effect level at low frequencies (win32 only)
    DecayTime         real4 ?       ; 0.1     20.0   1.49    reverberation decay time at mid frequencies (win32/xbox)
    DecayHFRatio      real4 ?       ; 0.1     2.0    0.83    high-frequency to mid-frequency decay time ratio (win32/xbox)
    DecayLFRatio      real4 ?       ; 0.1     2.0    1.0     low-frequency to mid-frequency decay time ratio (win32 only)
    Reflections       dword ?       ; -10000  1000   -2602   early reflections level relative to room effect (win32/xbox)
    ReflectionsDelay  real4 ?       ; 0.0     0.3    0.007   initial reflection delay time (win32/xbox)
    ReflectionsPan_x  real4 ?       ;                0,0,0   early reflections panning vector x component (win32 only)
    ReflectionsPan_y  real4 ?       ;                0,0,0   early reflections panning vector y component (win32 only)
    ReflectionsPan_z  real4 ?       ;                0,0,0   early reflections panning vector z component (win32 only)
    Reverb            dword ?       ; -10000  2000   200     late reverberation level relative to room effect (win32/xbox)
    ReverbDelay       real4 ?       ; 0.0     0.1    0.011   late reverberation delay time relative to initial reflection (win32/xbox)
    ReverbPan_x       real4 ?       ;                0,0,0   late reverberation panning vector x component (win32 only)
    ReverbPan_y       real4 ?       ;                0,0,0   late reverberation panning vector y component (win32 only)
    ReverbPan_z       real4 ?       ;                0,0,0   late reverberation panning vector z component (win32 only)
    EchoTime          real4 ?       ; .075    0.25   0.25    echo time (win32 only)
    EchoDepth         real4 ?       ; 0.0     1.0    0.0     echo depth (win32 only)
    ModulationTime    real4 ?       ; 0.04    4.0    0.25    modulation time (win32 only)
    ModulationDepth   real4 ?       ; 0.0     1.0    0.0     modulation depth (win32 only)
    AirAbsorptionHF   real4 ?       ; -100    0.0    -5.0    change in level per meter at high frequencies (win32 only)
    HFReference       real4 ?       ; 1000.0  20000  5000.0  reference high frequency (hz) (win32/xbox)
    LFReference       real4 ?       ; 20.0    1000.0 250.0   reference low frequency (hz) (win32 only)
    RoomRolloffFactor real4 ?       ; 0.0     10.0   0.0     like FSOUND_3D_Listener_SetRolloffFactor but for room effect (win32/xbox)
    Diffusion         real4 ?       ; 0.0     100.0  100.0   Value that controls the echo density in the late reverberation decay. (xbox only)
    Density           real4 ?       ; 0.0     100.0  100.0   Value that controls the modal density in the late reverberation decay (xbox only)
    Flags             dword ?       ; FSOUND_REVERB_PROPERTYFLAGS - modifies the behavior of above properties (win32 only)
FSOUND_REVERB_PROPERTIES ends

;---------------------------------------------------------------------
;  FSOUND_REVERB_FLAGS
;    
;  Description : Values for the Flags member of the FSOUND_REVERB_PROPERTIES structure.
;
;  See also : FSOUND_REVERB_PROPERTIES
;---------------------------------------------------------------------
FSOUND_REVERB_FLAGS_DECAYTIMESCALE        equ 01h       ; 'EnvSize' affects reverberation decay time
FSOUND_REVERB_FLAGS_REFLECTIONSSCALE      equ 02h       ; 'EnvSize' affects reflection level
FSOUND_REVERB_FLAGS_REFLECTIONSDELAYSCALE equ 04h       ; 'EnvSize' affects initial reflection delay time
FSOUND_REVERB_FLAGS_REVERBSCALE           equ 08h       ; 'EnvSize' affects reflections level
FSOUND_REVERB_FLAGS_REVERBDELAYSCALE      equ 10h       ; 'EnvSize' affects late reverberation delay time
FSOUND_REVERB_FLAGS_DECAYHFLIMIT          equ 20h       ; AirAbsorptionHF affects DecayHFRatio
FSOUND_REVERB_FLAGS_ECHOTIMESCALE         equ 40h       ; 'EnvSize' affects echo time
FSOUND_REVERB_FLAGS_MODULATIONTIMESCALE   equ 80h       ; 'EnvSize' affects modulation time
FSOUND_REVERB_FLAGS_CORE0                 equ 100h      ;  PS2 Only - Reverb is applied to CORE0 (hw voices 0-23) */
FSOUND_REVERB_FLAGS_CORE1                 equ 200h      ;  PS2 Only - Reverb is applied to CORE1 (hw voices 24-47) */

;FSOUND_REVERB_FLAGS_DEFAULT              equ (FSOUND_REVERB_FLAGS_DECAYTIMESCALE | FSOUND_REVERB_FLAGS_REFLECTIONSSCALE | FSOUND_REVERB_FLAGS_REFLECTIONSDELAYSCALE | FSOUND_REVERB_FLAGS_REVERBSCALE | FSOUND_REVERB_FLAGS_REVERBDELAYSCALE | FSOUND_REVERB_FLAGS_DECAYHFLIMIT | FSOUND_REVERB_FLAGS_CORE0 | FSOUND_REVERB_FLAGS_CORE1)

;---------------------------------------------------------------------
; FSOUND_REVERB_CHANNELPROPERTIES
;
; Description:  Structure defining the properties for a reverb source, related to a FSOUND channel.
;
; Remarks:
;    For more indepth descriptions of the reverb properties under win32, please see the EAX3
;    documentation at http://developer.creative.com/ under the 'downloads' section.
;    If they do not have the EAX3 documentation, then most information can be attained from
;    the EAX2 documentation, as EAX3 only adds some more parameters and functionality on top of 
;    EAX2.
;    Note the default reverb properties are the same as the FSOUND_PRESET_GENERIC preset.
;    Note that integer values that typically range from -10,000 to 1000 are represented in 
;    decibels, and are of a logarithmic scale, not linear, wheras float values are typically linear.
;    PORTABILITY: Each member has the platform it supports in braces ie (win32/xbox).  
;    Some reverb parameters are only supported in win32 and some only on xbox. If all parameters are set then
;    the reverb should product a similar effect on either platform.
;    Linux and FMODCE do not support the reverb api.
;
;
; See Also:     FSOUND_Reverb_SetChannelProperties
;               FSOUND_Reverb_GetChannelProperties
;
;---------------------------------------------------------------------

FSOUND_REVERB_CHANNELPROPERTIES struct        ; MIN     MAX    DEFAULT
    int    Direct;                  dword ?   ; -10000  1000   0       direct path level (at low and mid frequencies) (win32/xbox) 
    int    DirectHF;                dword ?   ; -10000  0      0       relative direct path level at high frequencies (win32/xbox) 
    int    Room;                    dword ?   ; -10000  1000   0       room effect level (at low and mid frequencies) (win32/xbox) 
    int    RoomHF;                  dword ?   ; -10000  0      0       relative room effect level at high frequencies (win32/xbox) 
    int    Obstruction;             dword ?   ; -10000  0      0       main obstruction control (attenuation at high frequencies)  (win32/xbox) 
    float  ObstructionLFRatio;      real4 ?   ; 0.0     1.0    0.0     obstruction low-frequency level re. main control (win32/xbox) 
    int    Occlusion;               dword ?   ; -10000  0      0       main occlusion control (attenuation at high frequencies) (win32/xbox) 
    float  OcclusionLFRatio;        real4 ?   ; 0.0     1.0    0.25    occlusion low-frequency level re. main control (win32/xbox) 
    float  OcclusionRoomRatio;      real4 ?   ; 0.0     10.0   1.5     relative occlusion control for room effect (win32) 
    float  OcclusionDirectRatio;    real4 ?   ; 0.0     10.0   1.0     relative occlusion control for direct path (win32) 
    int    Exclusion;               dword ?   ; -10000  0      0       main exlusion control (attenuation at high frequencies) (win32) 
    float  ExclusionLFRatio;        real4 ?   ; 0.0     1.0    1.0     exclusion low-frequency level re. main control (win32) 
    int    OutsideVolumeHF;         dword ?   ; -10000  0      0       outside sound cone level at high frequencies (win32) 
    float  DopplerFactor;           real4 ?   ; 0.0     10.0   0.0     like DS3D flDopplerFactor but per source (win32) 
    float  RolloffFactor;           real4 ?   ; 0.0     10.0   0.0     like DS3D flRolloffFactor but per source (win32) 
    float  RoomRolloffFactor;       real4 ?   ; 0.0     10.0   0.0     like DS3D flRolloffFactor but for room effect (win32/xbox) 
    float  AirAbsorptionFactor;     real4 ?   ; 0.0     10.0   1.0     multiplies AirAbsorptionHF member of FSOUND_REVERB_PROPERTIES (win32) 
    int    Flags;                   dword ?   ; FSOUND_REVERB_CHANNELFLAGS - modifies the behavior of properties (win32) 
FSOUND_REVERB_CHANNELPROPERTIES ends

;---------------------------------------------------------------------
;  FSOUND_REVERB_CHANNELFLAGS
;    
;  Description : Values for the Flags member of the FSOUND_REVERB_CHANNELPROPERTIES structure.
;
;  See also : FSOUND_REVERB_CHANNELPROPERTIES
;---------------------------------------------------------------------
FSOUND_REVERB_CHANNELFLAGS_DIRECTHFAUTO  equ 1   ; Automatic setting of 'Direct'  due to distance from listener
FSOUND_REVERB_CHANNELFLAGS_ROOMAUTO      equ 2   ; Automatic setting of 'Room'  due to distance from listener
FSOUND_REVERB_CHANNELFLAGS_ROOMHFAUTO    equ 4   ; Automatic setting of 'RoomHF' due to distance from listener
; FSOUND_REVERB_CHANNELFLAGS_DEFAULT       (FSOUND_REVERB_CHANNELFLAGS_DIRECTHFAUTO | FSOUND_REVERB_CHANNELFLAGS_ROOMAUTO | FSOUND_REVERB_CHANNELFLAGS_ROOMHFAUTO)


;---------------------------------------------------------------------
; FSOUND_FX_MODES
;
; Description:  These flags are used with FSOUND_FX_Enable to enable or disable DirectX 8 FX for a channel.
;
; See Also:     
;    FSOUND_FX_Enable
;    FSOUND_FX_SetChorus
;    FSOUND_FX_SetCompressor
;    FSOUND_FX_SetDistortion
;    FSOUND_FX_SetEcho
;    FSOUND_FX_SetFlanger
;    FSOUND_FX_SetGargle
;    FSOUND_FX_SetI3DL2Reverb
;    FSOUND_FX_SetParamEQ
;    FSOUND_FX_SetWavesReverb
;---------------------------------------------------------------------

FSOUND_FX_CHORUS                    equ 0
FSOUND_FX_COMPRESSOR                equ 1 
FSOUND_FX_DISTORTION                equ 2
FSOUND_FX_ECHO                      equ 3
FSOUND_FX_FLANGER                   equ 4
FSOUND_FX_GARGLE                    equ 5
FSOUND_FX_I3DL2REVERB               equ 6
FSOUND_FX_PARAMEQ                   equ 7
FSOUND_FX_WAVES_REVERB              equ 8


;---------------------------------------------------------------------
; FSOUND_SPEAKERMODES
;
; Description:  These are speaker types defined for use with the FSOUND_SetSpeakerMode command.
;
; See Also:     FSOUND_SetSpeakerMode
;
; Remarks:      Only works with FSOUND_OUTPUT_DSOUND output mode.
;
;---------------------------------------------------------------------

FSOUND_SPEAKERMODE_DOLBYDIGITAL     equ 0   ; The audio is played through a speaker arrangement of surround speakers with a subwoofer.
FSOUND_SPEAKERMODE_HEADPHONE        equ 1   ; The speakers are headphones.
FSOUND_SPEAKERMODE_MONO             equ 2   ; The speakers are monaural.
FSOUND_SPEAKERMODE_QUAD             equ 3   ; The speakers are quadraphonic.
FSOUND_SPEAKERMODE_STEREO           equ 4   ; The speakers are stereo (default value).
FSOUND_SPEAKERMODE_SURROUND         equ 5   ; The speakers are surround sound.
FSOUND_SPEAKERMODE_DTS              equ 6   ; The audio is played through a speaker arrangement of surround speakers with a subwoofer.


;---------------------------------------------------------------------
; FSOUND_INIT_FLAGS
;	
; Description:  Initialization flags.  Use them with FSOUND_Init in the flags parameter to change various behaviour.
;
; See Also:     FSOUND_Init
;---------------------------------------------------------------------
FSOUND_INIT_USEDEFAULTMIDISYNTH		equ 001h   ; Causes MIDI playback to force software decoding.
FSOUND_INIT_GLOBALFOCUS				equ 002h   ; For DirectSound output - sound is not muted when window is out of focus.
FSOUND_INIT_ENABLEOUTPUTFX          equ 004h   ; For DirectSound output - Allows FSOUND_FX api to be used on global software mixer output!
FSOUND_INIT_ACCURATEVULEVELS        equ 008h   ; This latency adjusts FSOUND_GetCurrentVU, but incurs a cpu and very small memory hit
FSOUND_INIT_DISABLE_CORE0_REVERB    equ 010h   ; PS2 only - Disable reverb on CORE 0 to regain SRAM
FSOUND_INIT_DISABLE_CORE1_REVERB    equ 020h   ; PS2 only - Disable reverb on CORE 1 to regain SRAM

;=====================================================================
; Function prototypes.
;=====================================================================

;
;    PRE - FSOUND_Init functions. These can't be called after FSOUND_Init is 
;    called (they will fail). They set up FMOD system functionality. 
;
FSOUND_SetOutput                PROTO :DWORD
FSOUND_SetDriver                PROTO :DWORD
FSOUND_SetMixer                 PROTO :DWORD
FSOUND_SetBufferSize            PROTO :DWORD
FSOUND_SetHWND                  PROTO :DWORD
FSOUND_SetMinHardwareChannels   PROTO :DWORD
FSOUND_SetMaxHardwareChannels   PROTO :DWORD
FSOUND_SetMemorySystem          PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD



;
;    Main initialization / closedown functions.
;    Note : Use FSOUND_INIT_USEDEFAULTMIDISYNTH with FSOUND_Init for software override 
;           with MIDI playback.
;         : Use FSOUND_INIT_GLOBALFOCUS with FSOUND_Init to make sound audible no matter 
;           which window is in focus. (FSOUND_OUTPUT_DSOUND only)
;

FSOUND_Init     PROTO :DWORD,:DWORD,:DWORD
FSOUND_Close    PROTO

;
; Runtime system level functions 
;

FSOUND_SetSpeakerMode       PROTO :DWORD
FSOUND_SetSFXMasterVolume   PROTO :DWORD
FSOUND_SetPanSeperation     PROTO :DWORD
FSOUND_File_SetCallbacks    PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD

;
; System information functions. 
;

FSOUND_GetError					PROTO
FSOUND_GetVersion				PROTO
FSOUND_GetOutput				PROTO
FSOUND_GetOutputHandle			PROTO
FSOUND_GetDriver				PROTO
FSOUND_GetMixer					PROTO
FSOUND_GetNumDrivers			PROTO
FSOUND_GetDriverName			PROTO :DWORD
FSOUND_GetDriverCaps			PROTO :DWORD,:DWORD
FSOUND_GetOutputRate            PROTO
FSOUND_GetMaxChannels           PROTO
FSOUND_GetMaxSamples            PROTO
FSOUND_GetSFXMasterVolume       PROTO
FSOUND_GetNumHardwareChannels   PROTO
FSOUND_GetChannelsPlaying       PROTO
FSOUND_GetCPUUsage              PROTO
FSOUND_GetMemoryStats           PROTO :DWORD,:DWORD

;---------------------------------------------------------------------
; Sample management / load functions.
;---------------------------------------------------------------------

;
;   Sample creation and management functions
;   Note : Use FSOUND_LOADMEMORY   flag with FSOUND_Sample_Load to load from memory.
;          Use FSOUND_LOADRAW      flag with FSOUND_Sample_Load to treat as as raw pcm data.
;

FSOUND_Sample_Load		PROTO :DWORD,:DWORD,:DWORD,:DWORD
FSOUND_Sample_Alloc     PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_Sample_Free      PROTO :DWORD
FSOUND_Sample_Upload    PROTO :DWORD,:DWORD,:DWORD
FSOUND_Sample_Lock      PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_Sample_Unlock    PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD

;
; Sample control functions
;

FSOUND_Sample_SetMode           PROTO :DWORD,:DWORD
FSOUND_Sample_SetLoopPoints     PROTO :DWORD,:DWORD,:DWORD
FSOUND_Sample_SetDefaults       PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_Sample_SetMinMaxDistance PROTO :DWORD,:DWORD,:DWORD

;
; Sample information
;

FSOUND_Sample_Get           PROTO :DWORD
FSOUND_Sample_GetName		PROTO :DWORD
FSOUND_Sample_GetLength     PROTO :DWORD
FSOUND_Sample_GetLoopPoints PROTO :DWORD,:DWORD,:DWORD
FSOUND_Sample_GetDefaults   PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_Sample_GetMode       PROTO :DWORD

;---------------------------------------------------------------------
; Channel control functions.
;---------------------------------------------------------------------

;
;    Playing and stopping sounds.  
;    Note : Use FSOUND_FREE as the 'channel' variable, to let FMOD pick a free channel for you.
;           Use FSOUND_ALL as the 'channel' variable to control ALL channels with one function call!
;

FSOUND_PlaySound            PROTO :DWORD,:DWORD
FSOUND_PlaySoundEx          PROTO :DWORD,:DWORD,:DWORD,:DWORD
FSOUND_StopSound            PROTO :DWORD

;
;    Functions to control playback of a channel.
;    Note : FSOUND_ALL can be used on most of these functions as a channel value.
;

FSOUND_SetFrequency         PROTO :DWORD,:DWORD
FSOUND_SetVolume            PROTO :DWORD,:DWORD
FSOUND_SetVolumeAbsolute    PROTO :DWORD,:DWORD
FSOUND_SetPan               PROTO :DWORD,:DWORD
FSOUND_SetSurround          PROTO :DWORD,:DWORD
FSOUND_SetMute              PROTO :DWORD,:DWORD
FSOUND_SetPriority          PROTO :DWORD,:DWORD
FSOUND_SetReserved          PROTO :DWORD,:DWORD
FSOUND_SetPaused            PROTO :DWORD,:DWORD
FSOUND_SetLoopMode			PROTO :DWORD,:DWORD
FSOUND_SetCurrentPosition   PROTO :DWORD,:DWORD

;
;   Channel information functions
;

FSOUND_IsPlaying            PROTO :DWORD
FSOUND_GetFrequency         PROTO :DWORD
FSOUND_GetVolume            PROTO :DWORD
FSOUND_GetPan               PROTO :DWORD
FSOUND_GetSurround          PROTO :DWORD
FSOUND_GetMute              PROTO :DWORD
FSOUND_GetPriority          PROTO :DWORD
FSOUND_GetReserved          PROTO :DWORD
FSOUND_GetPaused            PROTO :DWORD
FSOUND_GetLoopMode          PROTO :DWORD
FSOUND_GetCurrentPosition   PROTO :DWORD
FSOUND_GetCurrentSample     PROTO :DWORD
FSOUND_GetCurrentLevels     PROTO :DWORD

;---------------------------------------------------------------------
; FX functions.
;---------------------------------------------------------------------

;    Functions to control DX8 only effects processing.
;
;    - FX enabled samples can only be played once at a time, not multiple times at once.
;    - Sounds have to be created with FSOUND_HW2D or FSOUND_HW3D for this to work.
;    - FSOUND_INIT_ENABLEOUTPUTFX can be used to apply hardware effect processing to the
;      global mixed output of FMOD's software channels.
;    - FSOUND_FX_Enable returns an FX handle that you can use to alter fx parameters.
;    - FSOUND_FX_Enable can be called multiple times in a row, even on the same FX type,
;      it will return a unique handle for each FX.
;    - FSOUND_FX_Enable cannot be called if the sound is playing or locked.
;    - FSOUND_FX_Disable must be called to reset/clear the FX from a channel.
;

FSOUND_FX_Enable            PROTO :DWORD,:DWORD
FSOUND_FX_Disable           PROTO :DWORD

FSOUND_FX_SetChorus         PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_FX_SetCompressor     PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_FX_SetDistortion     PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_FX_SetEcho           PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_FX_SetFlanger        PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_FX_SetGargle         PROTO :DWORD,:DWORD,:DWORD
FSOUND_FX_SetI3DL2Reverb    PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_FX_SetParamEQ        PROTO :DWORD,:DWORD,:DWORD,:DWORD
FSOUND_FX_SetWavesReverb    PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD

;---------------------------------------------------------------------
; 3D sound functions.
;---------------------------------------------------------------------

;
; see also FSOUND_Sample_SetMinMaxDistance (above)
;

FSOUND_3D_Update                        PROTO
FSOUND_3D_SetAttributes                 PROTO :DWORD,:DWORD,:DWORD
FSOUND_3D_GetAttributes                 PROTO :DWORD,:DWORD,:DWORD
FSOUND_3D_Listener_SetAttributes        PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_3D_Listener_GetAttributes        PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_3D_Listener_SetDopplerFactor     PROTO :DWORD
FSOUND_3D_Listener_SetDistanceFactor    PROTO :DWORD
FSOUND_3D_Listener_SetRolloffFactor     PROTO :DWORD

;---------------------------------------------------------------------
; File Streaming functions.
;---------------------------------------------------------------------

;
; Note : Use FSOUND_LOADMEMORY   flag with FSOUND_Stream_OpenFile to stream from memory.
;        Use FSOUND_LOADRAW      flag with FSOUND_Stream_OpenFile to treat stream as raw pcm data.
;        Use FSOUND_MPEGACCURATE flag with FSOUND_Stream_OpenFile to open mpegs in 'accurate mode' for seeking etc.
;        Use FSOUND_FREE as the 'channel' variable, to let FMOD pick a free channel for you.
;

FSOUND_Stream_OpenFile			PROTO :DWORD,:DWORD,:DWORD
FSOUND_Stream_Create			PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_Stream_Play				PROTO :DWORD,:DWORD
FSOUND_Stream_PlayEx            PROTO :DWORD,:DWORD,:DWORD,:DWORD

FSOUND_Stream_Stop				PROTO :DWORD
FSOUND_Stream_Close				PROTO :DWORD
FSOUND_Stream_SetEndCallback	PROTO :DWORD,:DWORD,:DWORD
FSOUND_Stream_SetSynchCallback	PROTO :DWORD,:DWORD,:DWORD
FSOUND_Stream_GetSample			PROTO :DWORD
FSOUND_Stream_CreateDSP			PROTO :DWORD,:DWORD,:DWORD,:DWORD
FSOUND_Stream_SetBufferSize     PROTO :DWORD

FSOUND_Stream_SetPosition   PROTO :DWORD,:DWORD
FSOUND_Stream_GetPosition   PROTO :DWORD
FSOUND_Stream_SetTime		PROTO :DWORD,:DWORD
FSOUND_Stream_GetTime       PROTO :DWORD
FSOUND_Stream_GetLength     PROTO :DWORD
FSOUND_Stream_GetLengthMs	PROTO :DWORD

;---------------------------------------------------------------------
; CD audio functions.
;---------------------------------------------------------------------

;
;   Note : 0 = default cdrom.  Otherwise specify the drive letter, for example. 'D'. 
;

FSOUND_CD_Play				PROTO :DWORD,:DWORD
FSOUND_CD_SetPlayMode		PROTO :DWORD,:DWORD
FSOUND_CD_Stop				PROTO :DWORD
FSOUND_CD_SetPaused			PROTO :DWORD,:DWORD
FSOUND_CD_SetVolume			PROTO :DWORD,:DWORD
FSOUND_CD_Eject				PROTO :DWORD
							
FSOUND_CD_GetPaused			PROTO :DWORD
FSOUND_CD_GetTrack			PROTO :DWORD
FSOUND_CD_GetNumTracks		PROTO :DWORD
FSOUND_CD_GetVolume			PROTO :DWORD
FSOUND_CD_GetTrackLength	PROTO :DWORD,:DWORD
FSOUND_CD_GetTrackTime		PROTO :DWORD

;---------------------------------------------------------------------
; DSP functions.
;---------------------------------------------------------------------

;
;   DSP Unit control and information functions. 
;   These functions allow you access to the mixed stream that FMOD uses to play back sound on.
;

FSOUND_DSP_Create       PROTO :DWORD,:DWORD,:DWORD
FSOUND_DSP_Free         PROTO :DWORD
FSOUND_DSP_SetPriority  PROTO :DWORD,:DWORD
FSOUND_DSP_GetPriority  PROTO :DWORD
FSOUND_DSP_SetActive    PROTO :DWORD,:DWORD
FSOUND_DSP_GetActive    PROTO :DWORD

;
; Functions to get hold of FSOUND 'system DSP unit' handles.
;

FSOUND_DSP_GetClearUnit         PROTO
FSOUND_DSP_GetSFXUnit           PROTO
FSOUND_DSP_GetMusicUnit         PROTO
FSOUND_DSP_GetFFTUnit           PROTO
FSOUND_DSP_GetClipAndCopyUnit   PROTO

;
;   Miscellaneous DSP functions 
;   Note for the spectrum analysis function to work, you have to enable the FFT DSP unit with 
;   the following code FSOUND_DSP_SetActive(FSOUND_DSP_GetFFTUnit(), TRUE);
;   It is off by default to save cpu usage.
;

FSOUND_DSP_MixBuffers           PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_DSP_ClearMixBuffer       PROTO
FSOUND_DSP_GetBufferLength      PROTO
FSOUND_DSP_GetBufferLengthTotal PROTO   ; Total buffer length due to FSOUND_SetBufferSize
FSOUND_DSP_GetSpectrum          PROTO   ; Array of 512 floats - call FSOUND_DSP_SetActive(FSOUND_DSP_GetFFTUnit(), TRUE)) for this to work.

;---------------------------------------------------------------------
; Reverb functions. (eax, eax2 reverb)
;---------------------------------------------------------------------

;
;   See top of file for definitions and information on the reverb parameters.
;

FSOUND_Reverb_SetProperties         PROTO :DWORD
FSOUND_Reverb_GetProperties         PROTO :DWORD
FSOUND_Reverb_SetChannelProperties  PROTO :DWORD,:DWORD
FSOUND_Reverb_GetChannelProperties  PROTO :DWORD,:DWORD

;---------------------------------------------------------------------
; Recording functions
;---------------------------------------------------------------------

;
; Recording initialization functions
;

FSOUND_Record_SetDriver         PROTO :DWORD
FSOUND_Record_GetNumDrivers     PROTO
FSOUND_Record_GetDriverName     PROTO :DWORD
FSOUND_Record_GetDriver         PROTO

;
; Recording functionality.  Only one recording session will work at a time
;

FSOUND_Record_StartSample       PROTO :DWORD,:DWORD
FSOUND_Record_GetPosition       PROTO
FSOUND_Record_Stop              PROTO

;---------------------------------------------------------------------
; FMUSIC API
;---------------------------------------------------------------------

;
; Song management / playback functions.
;

FMUSIC_LoadSong             PROTO :DWORD
FMUSIC_LoadSongMemory       PROTO :DWORD,:DWORD
FMUSIC_FreeSong             PROTO :DWORD
FMUSIC_PlaySong             PROTO :DWORD
FMUSIC_StopSong             PROTO :DWORD
FMUSIC_StopAllSongs         PROTO

FMUSIC_SetZxxCallback       PROTO :DWORD,:DWORD
FMUSIC_SetRowCallback       PROTO :DWORD,:DWORD,:DWORD
FMUSIC_SetOrderCallback     PROTO :DWORD,:DWORD,:DWORD
FMUSIC_SetInstCallback		PROTO :DWORD,:DWORD,:DWORD

FMUSIC_SetSample			PROTO :DWORD,:DWORD,:DWORD
FMUSIC_OptimizeChannels     PROTO :DWORD,:DWORD,:DWORD

;
; Runtime song functions.
;

FMUSIC_SetReverb			PROTO :DWORD			; MIDI only.
FMUSIC_SetLooping           PROTO :DWORD,:DWORD
FMUSIC_SetOrder             PROTO :DWORD,:DWORD
FMUSIC_SetPaused            PROTO :DWORD,:DWORD
FMUSIC_SetMasterVolume      PROTO :DWORD,:DWORD
FMUSIC_SetPanSeperation     PROTO :DWORD,:DWORD

;
; Static song information functions.
;

FMUSIC_GetName                  PROTO :DWORD
FMUSIC_GetType                  PROTO :DWORD
FMUSIC_GetNumOrders             PROTO :DWORD
FMUSIC_GetNumPatterns           PROTO :DWORD
FMUSIC_GetNumInstruments        PROTO :DWORD
FMUSIC_GetNumSamples            PROTO :DWORD
FMUSIC_GetNumChannels           PROTO :DWORD
FMUSIC_GetSample                PROTO :DWORD,:DWORD
FMUSIC_GetPatternLength         PROTO :DWORD,:DWORD

;
; Runtime song information.
;

FMUSIC_IsFinished       PROTO :DWORD
FMUSIC_IsPlaying        PROTO :DWORD
FMUSIC_GetMasterVolume  PROTO :DWORD
FMUSIC_GetGlobalVolume  PROTO :DWORD
FMUSIC_GetOrder         PROTO :DWORD
FMUSIC_GetPattern       PROTO :DWORD
FMUSIC_GetSpeed         PROTO :DWORD
FMUSIC_GetBPM           PROTO :DWORD
FMUSIC_GetRow           PROTO :DWORD
FMUSIC_GetPaused        PROTO :DWORD
FMUSIC_GetTime          PROTO :DWORD
FMUSIC_GetRealChannel   PROTO :DWORD,:DWORD

;
; Error string
;
FMOD_ErrorString proto :dword

endif ;__FMOD_INC__

