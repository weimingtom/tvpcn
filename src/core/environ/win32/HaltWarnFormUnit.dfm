object TVPHaltWarnForm: TTVPHaltWarnForm
  Left = 53
  Top = 251
  BorderStyle = bsDialog
  ClientHeight = 125
  ClientWidth = 476
  Color = clBtnFace
  DefaultMonitor = dmDesktop
  Font.Charset = GB2312_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = '宋体'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 12
  object Label1: TLabel
    Left = 72
    Top = 16
    Width = 420
    Height = 12
    Caption = 
       '本程序现在因为停止了事件处理，或因为处在' +
       '待机状态'
  end
  object Label2: TLabel
    Left = 72
    Top = 32
    Width = 84
    Height = 12
    Caption = '而没有应答。'
  end
  object Label3: TLabel
    Left = 72
    Top = 48
    Width = 432
    Height = 12
    Caption = 
       '如果要中止本程序，请点击“强行中止”按钮，如果要继续运行，' +
       '请点击“继续”按钮。'
  end
  object Label4: TLabel
    Left = 72
    Top = 64
    Width = 120
    Height = 12
    Caption = ''
  end
  object IconPaintBox: TPaintBox
    Left = 16
    Top = 16
    Width = 49
    Height = 49
    OnPaint = IconPaintBoxPaint
  end
  object ExitButton: TButton
    Left = 141
    Top = 88
    Width = 91
    Height = 25
    Caption = '强行退出(&E)'
    TabOrder = 0
    OnClick = ExitButtonClick
  end
  object ContinueButton: TButton
    Left = 245
    Top = 88
    Width = 89
    Height = 25
    Cancel = True
    Caption = '继续(&C)'
    Default = True
    TabOrder = 1
    OnClick = ContinueButtonClick
  end
end
