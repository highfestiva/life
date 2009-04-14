//===============================================================================================
// RECORD.EXE
// Copyright (c), Firelight Multimedia, 1999-2000.
//
// This example shows how to record data to a static sample, or record dynamically, and have
// a dsp unit processing the result.
// The reverb below is taken from /samples/fmod/fmod.c
//===============================================================================================
//Converted to Delphi by Bocevski Dragan, d_bocevski@hotpop.com   ,   d_bocevski@yahoo.com
//===============================================================================================
//  History
//
//  2001/09/09 by Steve 'Sly' Williams
//  - Updated to version 3.40
//  - Warning!  Recording with reverb has a crash bug.  It is in the DSP callback function somewhere
//
//  2002/02/13 by Steve 'Sly' Williams
//  - Updated for FMOD 3.50
//===============================================================================================

program record1;

uses
  FMod, FModErrors, Windows;

{$APPTYPE CONSOLE}

const
  ENABLEREVERB = True; //Reverb not yet implemented
  RECORDLEN = (44100 * 5); // 5 seconds at 44khz
  REVERB_NUMTAPS = 7;

  // increase this value if the sound sounds corrupted or the time between recording
 // and hearing the result is longer than it should be..
  RECORD_DELAY_MS = 80;
  RECORD_DELAY_SAMPLES = (44100 * RECORD_DELAY_MS / 1000);

type
  REVERBTAP = record
    Unt: PFSoundDSPUnit;
    Historybuff: PChar; // storage space for tap history
    Workarea: PChar; // a place to hold 1 buffer worth of data (for preverb)
    Delayms: Integer; // delay of p/reverb tab in milliseconds
    Volume: Integer; // volume of p/reverb tab
    Pan: Integer; // pan of p/reverb tab
    Historyoffset: Integer; // running offset into history buffer
    Historylen: Integer; // size of history buffer in SAMPLES
  end;

  // reverb stuff
var
  DSP_ReverbTap: array[0..REVERB_NUMTAPS - 1] of REVERBTAP;

  {
  [
   [DESCRIPTION]
   Callback to mix in one reverb tap.  It copies the buffer into its own history buffer also.

   [PARAMETERS]
   'originalbuffer'	Pointer to the original mixbuffer, not any buffers passed down
        through the dsp chain.  They are in newbuffer.
   'newbuffer'			Pointer to buffer passed from previous DSP unit.
   'length'			Length in SAMPLES of buffer being passed.
   'param'				User parameter.  In this case it is a pointer to DSP_LowPassBuffer.

   [RETURN_VALUE]
   a pointer to the buffer that was passed in, with a tap mixed into it.

   [REMARKS]
  ]
  }

function DSP_ReverbCallback(originalbuffer, newbuffer: Pointer; length, param:
  Integer): pointer;
var
  mixertype: TFSoundMixerTypes;
  tap: ^REVERBTAP;
  FEEDBACK: Boolean;
  taillen: Integer;
  startlen: Integer;
  ptemp: Pointer;
begin
  mixertype := FSOUND_GetMixer();
  tap := pointer(param);

  // must be 16bit stereo integer buffer.. sorry blendmode (32bit) and fpu (32bit float) dont support this.
  if (mixertype = FSOUND_MIXER_BLENDMODE) or (mixertype =
    FSOUND_MIXER_QUALITY_FPU) then
    result := newbuffer
  else
  begin
    FEEDBACK := true;
      // this determines whether to use already pre-reverbed data for the tap, or the original clean buffer

    // reverb history buffer is a ringbuffer.  If the length makes the copy wrap, then split the copy
    // into end part, and start part..
    if (tap.Historyoffset + length > tap.historylen) then
    begin
      taillen := tap.Historylen - tap.Historyoffset; //ova treba ???
      //taillen:=tap.Historyoffset-tap.Historylen;
      startlen := length - taillen;

      // mix a scaled version of history buffer into output
      FSOUND_DSP_MixBuffers(newbuffer, tap.historybuff + (tap.historyoffset shl
        2), taillen, 44100, tap.volume, tap.pan, FSOUND_STEREO or FSOUND_16BITS);
      FSOUND_DSP_MixBuffers(pchar(newbuffer) + ((tap.historylen -
        tap.historyoffset) shl 2), tap.historybuff, startlen, 44100, tap.volume,
        tap.pan, FSOUND_STEREO or FSOUND_16BITS);

      // now copy input into reverb/history buffer
      if FEEDBACK then
      begin
        ptemp := tap.historybuff + (tap.historyoffset shl 2);
        move(newbuffer, ptemp, taillen shl 2);
        ptemp := pchar(newbuffer) + ((tap.Historylen - tap.Historyoffset) shl
          2);
        move(ptemp, tap.historybuff, startlen shl 2);
      end
      else
      begin
        ptemp := tap.historybuff + (tap.historyoffset shl 2);
        move(ptemp, originalbuffer, taillen shl 2);
        ptemp := pchar(originalbuffer) + ((tap.historylen - tap.historyoffset)
          shl 2);
        move(tap.historybuff, ptemp, startlen shl 2);
      end
        // no wrapping reverb buffer, just write dest
    end
    else
    begin
      // mix a scaled version of history buffer into output
      FSOUND_DSP_MixBuffers(newbuffer, tap.historybuff + (tap.historyoffset shl
        2), length, 44100, tap.volume, tap.pan, FSOUND_STEREO or FSOUND_16BITS);

      // now copy input into reverb/history buffer
      if FEEDBACK then
      begin
        ptemp := tap.historybuff + (tap.historyoffset shl 2);
        move(ptemp, newbuffer, length shl 2);
      end
      else
      begin
        ptemp := tap.historybuff + (tap.historyoffset shl 2);
        move(ptemp, originalbuffer, length shl 2);
      end
    end;
    inc(tap.Historyoffset, length);
    if (tap.historyoffset >= tap.historylen) then
      dec(tap.Historyoffset, tap.historylen);

    // reverb history has been mixed into new buffer, so return it.
    result := newbuffer;
  end;
end;

procedure SetupReverb();
var
  delay: array[0..REVERB_NUMTAPS - 1] of Integer;
  volume: array[0..REVERB_NUMTAPS - 1] of Integer;
  pan: array[0..REVERB_NUMTAPS - 1] of Integer;
  count: Integer;
  ptemp: PChar;
begin
  delay[0] := 131;
  delay[1] := 149;
  delay[2] := 173;
  delay[3] := 211;
  delay[4] := 281;
  delay[5] := 401;
  delay[6] := 457; //prime numbers!
  volume[0] := 120;
  volume[1] := 100;
  volume[2] := 95;
  volume[3] := 90;
  volume[4] := 80;
  volume[5] := 60;
  volume[6] := 50;
  pan[0] := 100;
  pan[1] := 128;
  pan[2] := 128;
  pan[3] := 152;
  pan[4] := 128;
  pan[5] := 100;
  pan[6] := 152;
  // ====================================================================================================================
  // REVERB SETUP
  // ====================================================================================================================
  // something to fiddle with..

  for count := 0 to REVERB_NUMTAPS - 1 do
  begin
    DSP_ReverbTap[count].delayms := delay[count];
    DSP_ReverbTap[count].volume := volume[count];
    DSP_ReverbTap[count].pan := pan[count];
    DSP_ReverbTap[count].historyoffset := 0;
    DSP_ReverbTap[count].historylen := round((DSP_ReverbTap[count].delayms *
      44100 / 1000));

    if (DSP_ReverbTap[count].historylen < FSOUND_DSP_GetBufferLength()) then
      DSP_ReverbTap[count].historylen := FSOUND_DSP_GetBufferLength();
        // just in case our calc is not the same.

    GetMem(ptemp, DSP_ReverbTap[count].historylen * 4);
      // * 4 is for 16bit stereo (mmx only)
    DSP_ReverbTap[count].historybuff := ptemp;
    DSP_ReverbTap[count].workarea := nil;

    {???} DSP_ReverbTap[count].Unt := FSOUND_DSP_Create(@DSP_ReverbCallback,
      FSOUND_DSP_DEFAULTPRIORITY_USER + 20 + (count * 2),
      DWORD(@DSP_ReverbTap[count]));
    //	DSP_ReverbTap[count].Unt		:= FSOUND_DSP_Create(&DSP_ReverbCallback, FSOUND_DSP_DEFAULTPRIORITY_USER+20+(count*2),	(int)&DSP_ReverbTap[count]);

    if DSP_ReverbTap[count].Unt <> nil then
      FSOUND_DSP_SetActive(DSP_ReverbTap[count].Unt, TRUE);
  end;
end;

procedure CloseReverb();
var
  count: Integer;
begin
  for count := 0 to REVERB_NUMTAPS - 1 do
  begin
    FSOUND_DSP_Free(DSP_ReverbTap[count].Unt);
    DSP_ReverbTap[count].Unt := nil;

    Freemem(DSP_ReverbTap[count].historybuff);
    DSP_ReverbTap[count].historybuff := nil;

    Freemem(DSP_ReverbTap[count].workarea);
    DSP_ReverbTap[count].workarea := nil;
  end;
end;

var
  samp1: PFSoundSample;
  key: DWORD;
  driver, i, channel, originalfreq: Integer;
  dw: dword;
  enm: TFSoundOutputTypes;
  h, h1: Thandle;
  buf: input_record;
  c: coord;
  s, position, splaypos, srecordpos, sdiff, sfreq: string;
  playpos, recordpos: Cardinal;
  diff: Longint;
  oldrecordpos, oldplaypos: Cardinal;
  vu: string[16];
  vuval: single;
  smoothedvu: single;

const
  VUSPEED = 0.2;

begin
  oldrecordpos := 0;
  oldplaypos := 0;
  smoothedvu := 0;
  diff := 0;
  SetLength(position, 80);
  SetConsoleTitle(pchar('Example Record'));
  h := GetStdHandle(STD_INPUT_HANDLE);
  h1 := GetStdHandle(STD_OUTPUT_HANDLE);
  Buf.EventType := Key_Event;
  c.X := 1;
  c.Y := 24;
  if FSOUND_GetVersion() < FMOD_VERSION then
  begin
    writeln('Error : You are using the wrong DLL version!  You should be using FMOD ', FMOD_VERSION: 3: 2);
    exit;
  end;

  // ==========================================================================================
  // SELECT OUTPUT METHOD
  // ==========================================================================================

  writeln;
  writeln('---------------------------------------------------------');
  writeln('Output Type');
  writeln('---------------------------------------------------------');

  writeln('1 - Direct Sound');
  writeln('2 - Windows Multimedia Waveout');
  writeln('3 - NoSound');
  writeln('---------------------------------------------------------');
    // print driver names
  writeln('Press a corresponding number or ESC to quit');

  repeat
    Sleep(50);
    FlushConsoleInputBuffer(h);
    repeat
      ReadConsoleInput(h, buf, 1, dw);
    until buf.Event.KeyEvent.bKeyDown = false;
    key := buf.Event.KeyEvent.wVirtualKeyCode;
    case key of
      ord('1'): FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);
      ord('2'): FSOUND_SetOutput(FSOUND_OUTPUT_WINMM);
      ord('3'): FSOUND_SetOutput(FSOUND_OUTPUT_NOSOUND);
      27: exit;
    end;
  until ((key >= ord('1')) and (key <= ord('3')));

  // ==========================================================================================
  // SELECT OUTPUT DRIVER
  // ==========================================================================================

  // The following list are the drivers for the output method selected above.
  writeln;
  writeln('---------------------------------------------------------');
  enm := FSOUND_GetOutput();
  case enm of
    FSOUND_OUTPUT_NOSOUND: write('NoSound');
    FSOUND_OUTPUT_WINMM: write('Windows Multimedia Waveout');
    FSOUND_OUTPUT_DSOUND: write('Direct Sound');
    FSOUND_OUTPUT_A3D: write('A3D');
  end;
  writeln;
  writeln(' Driver list');
  writeln('---------------------------------------------------------');
  for i := 0 to FSOUND_GetNumDrivers() - 1 do
    writeln(i + 1, ' - ', FSOUND_GetDriverName(i)); // print driver names
  writeln('---------------------------------------------------------');
    // print driver names
  writeln('Press a corresponding number or ESC to quit');
  repeat
    FlushConsoleInputBuffer(h);
    Sleep(50);
    repeat
      ReadConsoleInput(h, buf, 1, dw);
    until buf.Event.KeyEvent.bKeyDown = false;
    key := buf.Event.KeyEvent.wVirtualKeyCode;
    if (key = 27) then
      exit;
    driver := key - ord('1');
  until driver < FSOUND_GetNumDrivers();
  FSOUND_SetDriver(driver); // Select sound card (0 = default)

  // ==========================================================================================
  // SELECT MIXER
  // ==========================================================================================

  FSOUND_SetMixer(FSOUND_MIXER_QUALITY_AUTODETECT);

  // ==========================================================================================
  // INITIALIZE
  // ==========================================================================================
  if not FSOUND_Init(44100, 64, 0) then
  begin
    writeln('Error! Initializing');
    writeln(FMOD_ErrorString(FSOUND_GetError()));
    FSOUND_Close();
    exit;
  end;

  // ==========================================================================================
  // SELECT INPUT DRIVER (can be done before or after init)
  // ==========================================================================================
     // The following list are the drivers for the output method selected above.
  writeln;
  writeln('---------------------------------------------------------');
  enm := FSOUND_GetOutput();
  case enm of

    FSOUND_OUTPUT_NOSOUND: write('NoSound');
    FSOUND_OUTPUT_WINMM: write('Windows Multimedia Waveout');
    FSOUND_OUTPUT_DSOUND: write('Direct Sound');
    FSOUND_OUTPUT_A3D: write('A3D');
  end;
  writeln;
  writeln(' Recording device driver list');
  writeln('---------------------------------------------------------');
  for i := 0 to FSOUND_Record_GetNumDrivers() - 1 do
    writeln(i + 1, ' - ', FSOUND_Record_GetDriverName(i)); // print driver names
  writeln('---------------------------------------------------------');
    // print driver names
  writeln('Press a corresponding number or ESC to quit');
  repeat
    FlushConsoleInputBuffer(h);
    Sleep(50);
    repeat
      ReadConsoleInput(h, buf, 1, dw);
    until buf.Event.KeyEvent.bKeyDown = false;
    key := buf.Event.KeyEvent.wVirtualKeyCode;
    if (ord(key) = 27) then
      exit;
    driver := ord(key) - ord('1');
  until driver <= FSOUND_Record_GetNumDrivers();
  FSOUND_Record_SetDriver(driver); // Select sound card (0 = default)

  // ==========================================================================================
  // DISPLAY HELP
  // ==========================================================================================

  writeln;
  write('FSOUND Output Method : ');
  case (FSOUND_GetOutput()) of

    FSOUND_OUTPUT_NOSOUND: writeln('FSOUND_OUTPUT_NOSOUND');
    FSOUND_OUTPUT_WINMM: writeln('FSOUND_OUTPUT_WINMM');
    FSOUND_OUTPUT_DSOUND: writeln('FSOUND_OUTPUT_DSOUND');
  end;

  write('FSOUND Mixer         : ');
  case (FSOUND_GetMixer()) of
    FSOUND_MIXER_BLENDMODE: writeln('FSOUND_MIXER_BLENDMODE');
    FSOUND_MIXER_MMXP5: writeln('FSOUND_MIXER_MMXP5');
    FSOUND_MIXER_MMXP6: writeln('FSOUND_MIXER_MMXP6');
    FSOUND_MIXER_QUALITY_FPU: writeln('FSOUND_MIXER_QUALITY_FPU');
    FSOUND_MIXER_QUALITY_MMXP5: writeln('FSOUND_MIXER_QUALITY_MMXP5');
    FSOUND_MIXER_QUALITY_MMXP6: writeln('FSOUND_MIXER_QUALITY_MMXP6');
  end;
  writeln('FSOUND Driver        : ' + FSOUND_GetDriverName(FSOUND_GetDriver()));
  writeln('FSOUND Record Driver : ' +
    FSOUND_Record_GetDriverName(FSOUND_Record_GetDriver()));

  // ==========================================================================================
  // RECORD INTO A STATIC SAMPLE
  // ==========================================================================================
     // create a sample to record into
  samp1 := FSOUND_Sample_Alloc(FSOUND_UNMANAGED, RECORDLEN, FSOUND_MONO or
    FSOUND_16BITS, 44100, 255, 128, 255);

  writeln;
  writeln('=========================================================================');
  writeln('Press Enter to start recording 5 seconds worth of 44khz 16bit data');
  writeln('=========================================================================');
  readln;

  c.X := 1;
  c.Y := 23;
  GetConsoleMode(h, dw);
  SetConsoleMode(h, dw or ENABLE_PROCESSED_OUTPUT or ENABLE_PROCESSED_INPUT);
  FSOUND_Record_StartSample(samp1, FALSE);
    // it will record into this sample for 5 seconds then stop
  repeat
    Sleep(100);
    FlushConsoleInputBuffer(h);
    PeekConsoleInput(h, buf, 1, dw);
    SetConsoleCursorPosition(h1, c);
    str(FSOUND_Record_GetPosition(), s);
    position := ' Recording position = ' + s;
    WriteConsole(h1, pchar(position), length(position) - 1, dw, nil);
    position :=
      '                                                                                ';
  until ((FSOUND_Record_GetPosition() >= RECORDLEN) or
    (buf.Event.KeyEvent.bKeyDown));
  FSOUND_Record_Stop(); // it already stopped anyway
  writeln;
  writeln('=========================================================================');
  writeln('Press Enter to play back recorded data');
  writeln('=========================================================================');
  readln;

  channel := FSOUND_PlaySound(FSOUND_FREE, samp1);
  writeln('Playing back sound...');
  repeat
    Sleep(100);
    FlushConsoleInputBuffer(h);
    PeekConsoleInput(h, buf, 1, dw);
    SetConsoleCursorPosition(h1, c);
    str(FSOUND_GetCurrentPosition(channel), s);
    position := ' Playback position = ' + s;
    WriteConsole(h1, pchar(position), length(position) - 1, dw, nil);
    position :=
      '                                                                               ';
  until (not FSOUND_IsPlaying(channel));

  // ==========================================================================================
  // REALTIME FULL DUPLEX RECORD / PLAYBACK!
  // ==========================================================================================
  writeln;
  writeln('=========================================================================');
  writeln('Press Enter to do some full duplex realtime recording!');
  writeln('(with reverb for mmx users)');
  writeln('=========================================================================');
  readln;

  FSOUND_Sample_SetLoopMode(samp1, FSOUND_LOOP_NORMAL);
    // make it a looping sample
  FSOUND_Record_StartSample(samp1, TRUE);
    // start recording and make it loop also

  // let the record cursor move forward a little bit first before we try to play it
  // (the position jumps in blocks, so any non 0 value will mean 1 block has been recorded)
  while (FSOUND_Record_GetPosition() = 0) do
    Sleep(1);

  if ENABLEREVERB then
    SetupReverb();

  channel := FSOUND_PlaySound(FSOUND_FREE, samp1); // play the sound

  originalfreq := FSOUND_GetFrequency(channel);

  repeat
    playpos := FSOUND_GetCurrentPosition(channel);
    recordpos := FSOUND_Record_GetPosition();
    // NOTE : As the recording and playback frequencies arent guarranteed to be exactly in
    // sync, we have to adjust the playback frequency to keep the 2 cursors just enough
// apart not to overlap. (and sound corrupted)
// This code tries to keep it inside a reasonable size window just behind the record
// cursor. ie [........|play window|<-delay->|<-Record cursor.............]

// dont do this code if either of the cursors just wrapped
    if ((playpos > oldplaypos) and (recordpos > oldrecordpos)) then
    begin
      diff := playpos - recordpos;
      if (diff > -RECORD_DELAY_SAMPLES) then
        FSOUND_SetFrequency(channel, originalfreq - 1000) // slow it down
      else if (diff < -(RECORD_DELAY_SAMPLES * 2)) then
        FSOUND_SetFrequency(channel, originalfreq + 1000) // speed it up
      else
        FSOUND_SetFrequency(channel, originalfreq);
    end;
    oldplaypos := playpos;
    oldrecordpos := recordpos;

    // print some info and a VU meter (vu is smoothed)
    vuval := FSOUND_GetCurrentVU(channel) * 15.0;

    if (vuval > smoothedvu) then
      smoothedvu := vuval;
    smoothedvu := smoothedvu - VUSPEED;
    if (smoothedvu < 0) then
      smoothedvu := 0;

    vu := '                ';
    for i := 1 to round(smoothedvu) do
      vu[i] := '=';

    SetConsoleCursorPosition(h1, c);
    str(playpos: 7, splaypos);
    str(recordpos: 7, srecordpos);
    str(diff: 5, sdiff);
    str(abs(FSOUND_GetFrequency(channel) - originalfreq), sfreq);
    if sfreq = '0' then
      position := ' Play=' + splaypos + ' Rec=' + srecordpos + ' (gap=' + sdiff +
        ', freqchange=   ' + sfreq + ' hz)  VU:' + vu
    else
      position := ' Play=' + splaypos + ' Rec=' + srecordpos + ' (gap=' + sdiff +
        ', freqchange=' + sfreq + ' hz)  VU:' + vu;
    WriteConsole(h1, pchar(position), length(position) - 1, dw, nil);
    position :=
      '                                                                                ';
    FlushConsoleInputBuffer(h);
    Sleep(10);
    PeekConsoleInput(h, buf, 1, dw);
    key := buf.Event.KeyEvent.wVirtualKeyCode;
  until key = 27;
  FSOUND_StopSound(channel);
  FSOUND_Record_Stop();
  if ENABLEREVERB then
    CloseReverb();
  // ==========================================================================================
  // CLEANUP AND SHUTDOWN
  // ==========================================================================================
  FSOUND_Close();
end.

