Attribute VB_Name = "Module1"
Public stream1 As Long          'the stream we open
Public channel1 As Long         'the channel we play the stream in
Public dsp1 As Long

Public lp_reson As Single
Public lp_cutoff As Single

'Set this to false to disable effects
'Note, these effects cannot be used in Visual Basic,
'unless you put the fmod_dsp_lowpass.dll in your windows/system dir
'When you compile the program before starting it, there is no problem
Public Const USE_EFFECTS = True

'LowPass
Public Declare Function dsp_lowpass_create Lib "fmod_dsp_lowpass.dll" (ByVal stream As Long, ByVal priority As Long, ByVal nr As Long) As Long
Public Declare Function dsp_LowPassClose Lib "fmod_dsp_lowpass.dll" (ByVal nr As Long) As Long
Public Declare Function dsp_SetLowPassParams Lib "fmod_dsp_lowpass.dll" (ByVal cutoff As Single, ByVal reson As Single, ByVal nr As Long) As Long
'Some Explanation about the lowpass functions
'dsp_lowpass_create creates a lowpass dsp by calling FSOUND_DSP_Create
'this can't be done in VB for some reason (it crashes), so you have to use this function instead
'the parameter nr is the number. you can have 0 or 1
'so you can have two streams with different effect parameters

Public Function FormatTime(ByVal sec As Long, Optional ByVal FullWords As Boolean = False) As String
Dim s As Long
Dim m As Long
Dim h As Long

s = sec
m = 0
h = 0
If s >= 60 Then
    m = Int(s / 60)
    s = s - m * 60
End If
If m >= 60 Then
    h = Int(m / 60)
    m = m - h * 60
End If

If Not FullWords Then
    If h > 0 Then
        FormatTime = Format$(h, "00") & ":"
    End If
    FormatTime = FormatTime & Format$(m, "00") & ":" & Format$(s, "00")
Else
    If h > 0 Then
        FormatTime = h & " Hours, "
    End If
    If m > 0 Then
        FormatTime = FormatTime & m & " Minutes, "
    End If
    FormatTime = FormatTime & s & " Seconds"
End If
End Function

