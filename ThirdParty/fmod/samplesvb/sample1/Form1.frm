VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Begin VB.Form Form1 
   BackColor       =   &H00C5D0D0&
   Caption         =   "FMod Streaming Example"
   ClientHeight    =   4305
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   6225
   Icon            =   "Form1.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   4305
   ScaleWidth      =   6225
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton Command4 
      Caption         =   "||"
      Height          =   375
      Left            =   1080
      TabIndex        =   15
      Top             =   960
      Width           =   495
   End
   Begin VB.PictureBox picLP_reson 
      Height          =   855
      Left            =   720
      ScaleHeight     =   795
      ScaleWidth      =   195
      TabIndex        =   14
      Top             =   3360
      Width           =   255
   End
   Begin VB.PictureBox picLP_cutoff 
      Height          =   855
      Left            =   360
      ScaleHeight     =   795
      ScaleWidth      =   195
      TabIndex        =   13
      Top             =   3360
      Width           =   255
   End
   Begin VB.PictureBox Picture5 
      BackColor       =   &H00E0E0E0&
      Height          =   855
      Left            =   1680
      ScaleHeight     =   795
      ScaleWidth      =   2955
      TabIndex        =   9
      Top             =   960
      Width           =   3015
      Begin VB.PictureBox picVU 
         BackColor       =   &H00404040&
         Height          =   615
         Left            =   2760
         ScaleHeight     =   37
         ScaleMode       =   3  'Pixel
         ScaleWidth      =   5
         TabIndex        =   11
         Top             =   120
         Width           =   135
      End
      Begin VB.Label lblProcessor 
         Alignment       =   2  'Center
         BackStyle       =   0  'Transparent
         Caption         =   "Processor Usage"
         Height          =   255
         Left            =   120
         TabIndex        =   12
         Top             =   480
         Width           =   2415
      End
      Begin VB.Label lblTime 
         Alignment       =   2  'Center
         BackStyle       =   0  'Transparent
         Caption         =   "Time Display"
         Height          =   255
         Left            =   120
         TabIndex        =   10
         Top             =   120
         Width           =   2295
      End
   End
   Begin VB.Timer Timer1 
      Interval        =   100
      Left            =   4920
      Top             =   1440
   End
   Begin VB.CheckBox Check1 
      BackColor       =   &H00C5D0D0&
      Caption         =   "Lowpass"
      Height          =   255
      Left            =   240
      TabIndex        =   8
      Top             =   3000
      Width           =   975
   End
   Begin VB.PictureBox Picture2 
      BackColor       =   &H00E0E0E0&
      Height          =   495
      Left            =   120
      ScaleHeight     =   435
      ScaleWidth      =   4515
      TabIndex        =   6
      Top             =   2400
      Width           =   4575
      Begin VB.Label Label1 
         BackStyle       =   0  'Transparent
         Caption         =   "Effects"
         Height          =   255
         Left            =   120
         TabIndex        =   7
         Top             =   120
         Width           =   4215
      End
   End
   Begin VB.PictureBox picStatus 
      BackColor       =   &H00404040&
      Height          =   255
      Left            =   360
      ScaleHeight     =   195
      ScaleWidth      =   4275
      TabIndex        =   5
      Top             =   1920
      Width           =   4335
   End
   Begin VB.CommandButton Command3 
      Caption         =   "Stop"
      Enabled         =   0   'False
      Height          =   375
      Left            =   360
      TabIndex        =   4
      Top             =   1440
      Width           =   1215
   End
   Begin VB.CommandButton Command2 
      BackColor       =   &H00C0C0C0&
      Caption         =   ">"
      Height          =   375
      Left            =   360
      TabIndex        =   3
      Top             =   960
      Width           =   615
   End
   Begin VB.PictureBox Picture1 
      BackColor       =   &H00E0E0E0&
      Height          =   495
      Left            =   120
      ScaleHeight     =   435
      ScaleWidth      =   5955
      TabIndex        =   1
      Top             =   240
      Width           =   6015
      Begin VB.Label lblFile 
         BackStyle       =   0  'Transparent
         Caption         =   "No File Opened"
         Height          =   255
         Left            =   120
         TabIndex        =   2
         Top             =   120
         Width           =   5655
      End
   End
   Begin VB.CommandButton Command1 
      BackColor       =   &H00C0C0C0&
      Caption         =   "Open File..."
      Height          =   375
      Left            =   4800
      TabIndex        =   0
      Top             =   960
      Width           =   1335
   End
   Begin MSComDlg.CommonDialog CommonDialog1 
      Left            =   5640
      Top             =   1440
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Check1_Click()
If USE_EFFECTS Then
    If Check1.Value = 1 Then
        FSOUND_DSP_SetActive dsp1, 1
        dsp_SetLowPassParams lp_cutoff, lp_reson, 0
    Else
        FSOUND_DSP_SetActive dsp1, 0
    End If
End If
End Sub

Private Sub Command1_Click()

CommonDialog1.CancelError = True
CommonDialog1.Filter = "Streaming Sound Files (*.mp3;*.wav;*.ogg;*.mid)|*.mp3;*.wav;*.ogg;*.mid"
On Error GoTo errh
CommonDialog1.ShowOpen

'Stop the song that was playing
If channel1 <> 0 Then
    FSOUND_Stream_Stop stream1
    channel1 = 0
End If
'Free DSP
If dsp1 <> 0 Then
    FSOUND_DSP_Free dsp1
    If USE_EFFECTS Then
        dsp_LowPassClose 0
    End If
End If
'Close any opened file
If stream1 <> 0 Then
    FSOUND_Stream_Close stream1
    stream1 = 0
End If

'Open the new stream
stream1 = FSOUND_Stream_OpenFile(CommonDialog1.filename, FSOUND_2D, 0)
If stream1 = 0 Then
    MsgBox "Error while opening file" & vbCrLf & FSOUND_GetErrorString(FSOUND_GetError)
    lblFile = "No File Opened"
Else
    lblFile = CommonDialog1.filename
End If

'Create the effect
If USE_EFFECTS Then
    dsp1 = dsp_lowpass_create(stream1, 1, 0)
End If

Exit Sub
errh:

End Sub

Private Sub Command2_Click()
'If it was already playing, stop it first
If channel1 <> 0 Then
    Command3_Click
End If

'When a file was opened, play it
If stream1 <> 0 Then
    channel1 = FSOUND_Stream_Play(FSOUND_FREE, stream1)
End If

If channel1 <> 0 Then
    Command3.Enabled = True
End If
End Sub

Private Sub Command3_Click()
If channel1 <> 0 Then
    FSOUND_Stream_Stop stream1
    channel1 = 0
End If
End Sub

Private Sub Command4_Click()
FSOUND_SetPaused channel1, Not CBool(FSOUND_GetPaused(channel1))

End Sub

Private Sub Form_Load()
'First, we init FMOD
If FSOUND_Init(44100, 32, 0) = 0 Then
    'Error
    MsgBox "An error occured initializing fmod" & vbCrLf & FSOUND_GetErrorString(FSOUND_GetError)
    End
End If

'FMod initialized successfully!

lp_cutoff = 5000
lp_reson = 1
End Sub

Private Sub Form_Unload(Cancel As Integer)
'Stop the song that was playing
If channel1 <> 0 Then
    FSOUND_Stream_Stop stream1
    channel1 = 0
End If
'Close any opened file
If stream1 <> 0 Then
    FSOUND_Stream_Close stream1
    stream1 = 0
End If

'Make sure you close FMOD on exiting
'(If you forget this, Visual Basic will crash upon exiting the app in debug mode)
FSOUND_Close
End
End Sub

Private Sub pic_LP_reson_Click()

End Sub

Private Sub picLP_reson_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
picLP_reson_MouseMove Button, Shift, X, Y
End Sub

Private Sub picLP_reson_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)
Dim prcnt As Single
If Button = vbLeftButton Then
    prcnt = 1 - (Y / picLP_reson.ScaleHeight)
    lp_reson = prcnt * 9.8 + 0.2
    If lp_reson < 0.2 Then lp_reson = 0.2
    If lp_reson > 10 Then lp_reson = 10
    dsp_SetLowPassParams lp_cutoff, lp_reson, 0
End If
End Sub

Private Sub picLP_cutoff_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
picLP_cutoff_MouseMove Button, Shift, X, Y
End Sub

Private Sub picLP_cutoff_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)
Dim prcnt As Single
If Button = vbLeftButton Then
    prcnt = 1 - (Y / picLP_cutoff.ScaleHeight)
    lp_cutoff = prcnt * 4800 + 200
    If lp_cutoff < 200 Then lp_cutoff = 200
    If lp_cutoff > 5000 Then lp_cutoff = 5000
    dsp_SetLowPassParams lp_cutoff, lp_reson, 0
End If
End Sub

Private Sub picStatus_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
If channel1 > 0 Then
    FSOUND_Stream_SetPosition stream1, (X / picStatus.ScaleWidth) * FSOUND_Stream_GetLength(stream1)
End If
End Sub

Private Sub Timer1_Timer()
Dim prcnt As Double
'We use this lousy timer to update the information
'For really cool applications you should be working with
'constant loops and stuff...
'For real performance apps you also shouldn't use
'Line and stuff, but hey, this is just an example huh

If channel1 <> 0 Then
    If FSOUND_Stream_GetPosition(stream1) = FSOUND_Stream_GetLength(stream1) Then
        'the file is finished, stop it
        FSOUND_Stream_Stop stream1
        channel1 = 0
    End If
    'Show the position
    prcnt = FSOUND_Stream_GetPosition(stream1) / FSOUND_Stream_GetLength(stream1)
    picStatus.Line (0, 0)-(picStatus.ScaleWidth, picStatus.ScaleHeight), RGB(64, 64, 64), BF
    picStatus.Line (0, 0)-(prcnt * picStatus.ScaleWidth, picStatus.ScaleHeight), RGB(255, 0, 0), BF
    
    lblTime = FormatTime(FSOUND_Stream_GetTime(stream1) / 1000) & " / " & FormatTime(FSOUND_Stream_GetLengthMs(stream1) / 1000)
    
    'Show the VU (lousy VU, certainly with this lousy timer)
    prcnt = FSOUND_GetCurrentVU(channel1)
    picVU.Line (0, 0)-(picVU.ScaleWidth, picVU.ScaleHeight), RGB(128, 64, 64), BF
    picVU.Line (0, picVU.ScaleHeight)-(picVU.ScaleWidth, picVU.ScaleHeight - prcnt * picVU.ScaleHeight), RGB(255, 0, 0), BF
    
    'Show the lowpass settings
    prcnt = (lp_cutoff - 200) / 4800
    picLP_cutoff.Line (0, 0)-(picLP_cutoff.ScaleWidth, picLP_cutoff.ScaleHeight), RGB(128, 64, 64), BF
    picLP_cutoff.Line (0, picLP_cutoff.ScaleHeight)-(picLP_cutoff.ScaleWidth, picLP_cutoff.ScaleHeight - prcnt * picLP_cutoff.ScaleHeight), RGB(255, 0, 0), BF

    prcnt = (lp_reson - 0.2) / 9.8
    picLP_reson.Line (0, 0)-(picLP_reson.ScaleWidth, picLP_reson.ScaleHeight), RGB(128, 64, 64), BF
    picLP_reson.Line (0, picLP_reson.ScaleHeight)-(picLP_reson.ScaleWidth, picLP_reson.ScaleHeight - prcnt * picLP_reson.ScaleHeight), RGB(255, 0, 0), BF

End If

'Show Processor time
lblProcessor = Format(FSOUND_GetCPUUsage, "00.00") & "%"
End Sub
