VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "mscomctl.ocx"
Begin VB.Form Form1 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Vb Dsp "
   ClientHeight    =   2910
   ClientLeft      =   45
   ClientTop       =   375
   ClientWidth     =   4650
   ClipControls    =   0   'False
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   2910
   ScaleWidth      =   4650
   StartUpPosition =   3  'Windows Default
   Begin VB.CheckBox Check5 
      Caption         =   "Robotique"
      Height          =   285
      Left            =   3150
      TabIndex        =   12
      Top             =   1860
      Width           =   1425
   End
   Begin VB.CheckBox Check4 
      Caption         =   "LowPass"
      Height          =   285
      Left            =   3150
      TabIndex        =   11
      Top             =   300
      Width           =   1425
   End
   Begin VB.CheckBox Check3 
      Caption         =   "Echo"
      Height          =   285
      Left            =   3150
      TabIndex        =   10
      Top             =   690
      Width           =   1425
   End
   Begin VB.CheckBox Check2 
      Caption         =   "Reverb"
      Height          =   285
      Left            =   3150
      TabIndex        =   9
      Top             =   1080
      Width           =   1425
   End
   Begin VB.CheckBox Check1 
      Caption         =   "Preverb"
      Height          =   285
      Left            =   3150
      TabIndex        =   8
      Top             =   1470
      Width           =   1425
   End
   Begin VB.CommandButton Command2 
      Caption         =   "Play"
      Height          =   375
      Left            =   150
      TabIndex        =   7
      Top             =   2130
      Width           =   1455
   End
   Begin MSComctlLib.Slider Slider2 
      Height          =   1395
      Left            =   900
      TabIndex        =   2
      Top             =   300
      Width           =   615
      _ExtentX        =   1085
      _ExtentY        =   2461
      _Version        =   393216
      Enabled         =   0   'False
      Orientation     =   1
      Min             =   2
      SelStart        =   2
      Value           =   2
   End
   Begin MSComctlLib.Slider Slider1 
      Height          =   1395
      Left            =   240
      TabIndex        =   1
      Top             =   300
      Width           =   615
      _ExtentX        =   1085
      _ExtentY        =   2461
      _Version        =   393216
      Enabled         =   0   'False
      Orientation     =   1
      Min             =   200
      Max             =   5000
      SelStart        =   200
      TickFrequency   =   10
      Value           =   200
   End
   Begin VB.CommandButton Command1 
      Caption         =   "About"
      Height          =   375
      Left            =   1590
      TabIndex        =   0
      Top             =   2130
      Width           =   1455
   End
   Begin MSComctlLib.Slider Slider3 
      Height          =   1395
      Left            =   2340
      TabIndex        =   5
      Top             =   300
      Width           =   615
      _ExtentX        =   1085
      _ExtentY        =   2461
      _Version        =   393216
      Enabled         =   0   'False
      Orientation     =   1
      Min             =   500
      Max             =   22050
      SelStart        =   500
      TickFrequency   =   500
      Value           =   500
   End
   Begin VB.Label Label3 
      Caption         =   "NumEcho"
      Height          =   255
      Left            =   2280
      TabIndex        =   6
      Top             =   1740
      Width           =   765
   End
   Begin VB.Label Label2 
      Caption         =   "resonance"
      Height          =   255
      Left            =   870
      TabIndex        =   4
      Top             =   1740
      Width           =   765
   End
   Begin VB.Label Label1 
      Caption         =   "CuttOff"
      Height          =   255
      Left            =   150
      TabIndex        =   3
      Top             =   1740
      Width           =   705
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' **************************************************************
'
' Filename          form1.frm
' Module Prefix     form1
'
' Author            Saïd Bougribate aka KarLKoX
'                   http://www.multimania.com/xxkarlkoxxx
'                   http://www.starnetasso.fr.st
'                   XxKarLKoXxX@aol.com
'
' Description
'
' Sample providing acces for DSP chain with Fmod
'
' All this code is under GNU GPL
'
' Revisions
' 09-06-01 01:00 AM KarLKoX
' Revisions
' 09-06-01 07:14 AM KarLKoX
' - Added : we can stop each effect
' - Added : Reverb and Preverb effects
' - Removed : Stupid code
' - Improved stability
'
' TODO
'
' Note : i think this sample is self explanatory without comments :)
' **************************************************************

Dim STRM As Long                                  ' we have a stream ...
Dim HMUSIC As Long                                ' or a music ?
Dim chan As Long                                  ' Our channel pointer
Private Const SONG = "Test.mp3"
Private Const outputfreq = 44100

Private Sub Check1_Click()
If Check1 Then
    Call StartPreverb
Else
    Call StopPreverb
End If
End Sub

Private Sub Check2_Click()
If Check2 Then
    Call StartReverb
Else
    Call StopReverb
End If
End Sub

Private Sub Check3_Click()
If Check3 Then
    Call StartEcho(Slider3.Value)
    Slider3.Enabled = True
Else
    Call StopEcho
    Slider3.Enabled = False
End If

End Sub

Private Sub Check4_Click()
If Check4 Then
    Call StartIIR(CSng(Slider1.Value), CSng(Slider2.Value))
    Slider1.Enabled = True
    Slider2.Enabled = True
Else
    Call StopIIR
    Slider1.Enabled = False
    Slider2.Enabled = False
End If
End Sub

Private Sub Check5_Click()
If Check5 Then
    Call StartNoise
Else
    Call StopNoise
End If
End Sub

Private Sub Command1_Click()
Call About
End Sub
' Return TRUE if the file exist
Private Function Exists(filename As String) As Boolean
      
    On Local Error GoTo ExErr
    
    Exists = False
    If filename$ <> "" Then
        If Dir$(filename$) <> "" Then Exists = True
    End If
    Exit Function

ExErr:
End Function
' Free Sound
Public Sub CloseSound()
' we stop and close the Stream handle
If STRM Then
Call FSOUND_Stream_Stop(STRM)
Call FSOUND_Stream_Close(STRM)
End If
' close Fmod
FSOUND_Close
End Sub
' Setup and initialise sound
Public Function InitSound() As Boolean

Call FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND)
Call FSOUND_SetDriver(0)  'default
Call FSOUND_SetMixer(FSOUND_MIXER_QUALITY_AUTODETECT)
InitSound = FSOUND_Init(outputfreq, 32, FSOUND_INIT_GLOBALFOCUS) > 0

End Function
Public Sub Play()
If Exists(App.Path & "\" & SONG) = False Then
MsgBox "Put a song and rename it to Test.mp3 to this current directory.", vbInformation
Exit Sub
End If

STRM = FSOUND_Stream_OpenFile(App.Path & "\" & SONG, FSOUND_NORMAL Or FSOUND_2D, 0)
    If STRM Then
        chan = FSOUND_Stream_Play(FSOUND_FREE, STRM)
        Call FSOUND_SetVolume(chan, 50) ' i use headphones :)
        Call FSOUND_SetPan(chan, FSOUND_STEREOPAN)
    End If
End Sub

Private Sub Command2_Click()
If STRM Then
    Call FSOUND_Stream_Stop(STRM)
    Call FSOUND_Stream_Close(STRM)
    STRM = 0
End If

Call Play
End Sub

Private Sub Command3_Click()
Call StartNoise
End Sub

Private Sub Form_Load()
Call InitSound
Call InitDSP
End Sub

Private Sub Form_Unload(Cancel As Integer)
If STRM Then
    Call CloseDSP
End If
Call CloseSound
Unload Me
Set Form1 = Nothing ' force VB to really free memory
End
End Sub


Private Sub Slider1_Scroll()
Call StartIIR(CSng(Slider1.Value), CSng(Slider2.Value))
End Sub


Private Sub Slider2_Scroll()
Call StartIIR(CSng(Slider1.Value), CSng(Slider2.Value))
End Sub

Private Sub Slider3_scroll()
Call StartEcho(CSng(Slider3.Value))
End Sub
