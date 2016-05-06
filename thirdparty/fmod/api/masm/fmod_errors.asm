;=====================================================================
; Fmod Error Strings
; Translated to assembly by Ricardo Santos
; dxantos@yahoo.com
;=====================================================================

.586
.model flat, stdcall
include fmod.inc

;---------------------------------------------------------------------
.const
;---------------------------------------------------------------------
	align 4
err00 db "No errors", 0
	align 4
err01 db "Cannot call this command after FSOUND_Init.  Call FSOUND_Close first.", 0
	align 4
err02 db "This command failed because FSOUND_Init was not called", 0
	align 4
err03 db "Playing the sound failed.", 0
	align 4
err04 db "Error initializing output device.", 0
	align 4
err05 db "The output device is already in use and cannot be reused.", 0
	align 4
err06 db "Soundcard does not support the features needed for this soundsystem (16bit stereo output)", 0
	align 4
err07 db "Error setting cooperative level for hardware.", 0
	align 4
err08 db "Error creating hardware sound buffer.", 0
	align 4
err09 db "File not found.", 0
	align 4
err10 db "Unknown file format.", 0
	align 4
err11 db "Error loading file.", 0
	align 4
err12 db "Not enough memory.", 0
	align 4
err13 db "The version number of this file format is not supported.", 0
	align 4
err14 db "An invalid parameter was passed to this function", 0
	align 4
err15 db "Tried to use an EAX command on a non EAX enabled channel or output.", 0
	align 4
err16 db "Failed to allocate a new channel", 0
	align 4
err17 db "Recording not supported on this device", 0
	align 4
err18 db "Required Mediaplayer codec is not installed", 0
	align 4
errUnknown db "Unknown error", 0

;---------------------------------------------------------------------
.code
;---------------------------------------------------------------------
	align 4
;=====================================================================
FMOD_ErrorString proc near public,
	errcode:dword
;=====================================================================
;{
	mov edx, errcode

	.if edx == FMOD_ERR_NONE
	;{
		mov eax, offset err00
	;}
	.elseif edx == FMOD_ERR_BUSY
	;{
		mov eax, offset err01
	;}
	.elseif edx == FMOD_ERR_UNINITIALIZED
	;{
		mov eax, offset err02
	;}
	.elseif edx == FMOD_ERR_PLAY
	;{
		mov eax, offset err03
	;}
	.elseif edx == FMOD_ERR_INIT
	;{
		mov eax, offset err04
	;}
	.elseif edx == FMOD_ERR_ALLOCATED
	;{
		mov eax, offset err05
	;}
	.elseif edx == FMOD_ERR_OUTPUT_FORMAT
	;{
		mov eax, offset err06
	;}
	.elseif edx == FMOD_ERR_COOPERATIVELEVEL
	;{
		mov eax, offset err07
	;}
	.elseif edx == FMOD_ERR_CREATEBUFFER
	;{
		mov eax, offset err08
	;}
	.elseif edx == FMOD_ERR_FILE_NOTFOUND
	;{
		mov eax, offset err09
	;}
	.elseif edx == FMOD_ERR_FILE_FORMAT
	;{
		mov eax, offset err10
	;}
	.elseif edx == FMOD_ERR_FILE_BAD
	;{
		mov eax, offset err11
	;}
	.elseif edx == FMOD_ERR_MEMORY
	;{
		mov eax, offset err12
	;}
	.elseif edx == FMOD_ERR_VERSION
	;{
		mov eax, offset err13
	;}
	.elseif edx == FMOD_ERR_INVALID_PARAM
	;{
		mov eax, offset err14
	;}
	.elseif edx == FMOD_ERR_NO_EAX
	;{
		mov eax, offset err15
	;}
	.elseif edx == FMOD_ERR_CHANNEL_ALLOC
	;{
		mov eax, offset err16
	;}
	.elseif edx == FMOD_ERR_RECORD
	;{
		mov eax, offset err17
	;}
	.elseif edx == FMOD_ERR_MEDIAPLAYER
	;{
		mov eax, offset err18
	;}
	.else
	;{
		mov eax, offset	errUnknown
	;}
	.endif
	ret
;}
FMOD_ErrorString endp

end
