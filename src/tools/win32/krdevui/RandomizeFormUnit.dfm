object RandomizeForm: TRandomizeForm
  Left = 429
  Top = 26
  BorderStyle = bsDialog
  Caption = 'エントロピ�`を�Ъ�嶄'
  ClientHeight = 182
  ClientWidth = 215
  Color = clBtnFace
  Font.Charset = GB2312_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'ＭＳ Ｐゴシック'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  PixelsPerInch = 96
  TextHeight = 12
  object Label1: TLabel
    Left = 64
    Top = 48
    Width = 87
    Height = 12
    Alignment = taCenter
    Caption = 'マウスカ�`ソルを'
  end
  object Label2: TLabel
    Left = 44
    Top = 80
    Width = 126
    Height = 12
    Alignment = taCenter
    Caption = 'しばらくマウスを�咾�すと'
  end
  object Label3: TLabel
    Left = 21
    Top = 96
    Width = 173
    Height = 12
    Alignment = taCenter
    Caption = 'このウィンドウは徭�啜弔哩]じます'
  end
  object Label5: TLabel
    Left = 35
    Top = 64
    Width = 145
    Height = 12
    Alignment = taCenter
    Caption = 'でたらめに�咾�してください。'
  end
  object Label7: TLabel
    Left = 27
    Top = 8
    Width = 161
    Height = 12
    Caption = '仝でたらめさ々が駅勣となります。'
  end
  object CancelButton: TButton
    Left = 59
    Top = 144
    Width = 97
    Height = 25
    Cancel = True
    Caption = 'キャンセル(&C)'
    ModalResult = 2
    TabOrder = 0
  end
  object Timer: TTimer
    Interval = 50
    OnTimer = TimerTimer
    Left = 168
    Top = 144
  end
end
