Attribute VB_Name = "Module2"
' **************************************************************
'
' Filename          dspfx.bas
' Module Prefix     dspfx
'
' Author            Saïd Bougribate aka KarLKoX
'                   http://www.multimania.com/xxkarlkoxxx
'                   http://www.starnetasso.fr.st
'                   XxKarLKoXxX@aol.com
'
' Description
'
' DLL Declarations
'
' All this code is under GNU GPL
'
' Revisions
' 09-06-01  KarLKoX
' Revisions
' 09-06-01 07:14 AM KarLKoX
' - Added : we can stop each effect
' - Added : Reverb and Preverb effects
' - Removed : Stupid code
' - Improved stability
'
' TODO
'
' **************************************************************

Public Declare Sub InitDSP Lib "DspFX.dll" Alias "_InitDSP@0" ()
Public Declare Sub CloseDSP Lib "DspFX.dll" Alias "_CloseDSP@0" ()
Public Declare Sub StartEcho Lib "DspFX.dll" Alias "_StartEcho@4" (ByVal NumEcho As Long)
Public Declare Sub StopEcho Lib "DspFX.dll" Alias "_StopEcho@0" ()
Public Declare Sub StartIIR Lib "DspFX.dll" Alias "_StartIIR@8" (ByVal cut As Single, ByVal resonance As Single)
Public Declare Sub StopIIR Lib "DspFX.dll" Alias "_StopIIR@0" ()
Public Declare Sub StartPreverb Lib "DspFX.dll" Alias "_StartPreverb@0" ()
Public Declare Sub StopPreverb Lib "DspFX.dll" Alias "_StopPreverb@0" ()
Public Declare Sub StartReverb Lib "DspFX.dll" Alias "_StartReverb@0" ()
Public Declare Sub StopReverb Lib "DspFX.dll" Alias "_StopReverb@0" ()
Public Declare Sub StopNoise Lib "DspFX.dll" Alias "_StopNoise@0" ()
Public Declare Sub StartNoise Lib "DspFX.dll" Alias "_StartNoise@0" ()
Public Declare Sub About Lib "DspFX.dll" Alias "_About@0" ()


