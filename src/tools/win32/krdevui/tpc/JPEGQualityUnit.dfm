object JPEGQualityForm: TJPEGQualityForm
  Left = 214
  Top = 359
  BorderStyle = bsDialog
  Caption = 'JPEG ���ץ����'
  ClientHeight = 113
  ClientWidth = 382
  Color = clBtnFace
  Font.Charset = GB2312_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = '�ͣ� �Х����å�'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  PixelsPerInch = 96
  TextHeight = 12
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 71
    Height = 12
    Caption = '������ƥ�(&Q) :'
    FocusControl = TrackBar
  end
  object Label2: TLabel
    Left = 264
    Top = 32
    Width = 46
    Height = 12
    Caption = '���| : ��'
  end
  object Label3: TLabel
    Left = 264
    Top = 48
    Width = 97
    Height = 12
    Caption = '�ե����륵���� : ��'
  end
  object Label4: TLabel
    Left = 56
    Top = 32
    Width = 46
    Height = 12
    Caption = '���| : ��'
  end
  object Label5: TLabel
    Left = 8
    Top = 48
    Width = 97
    Height = 12
    Caption = '�ե����륵���� : С'
  end
  object QualityLabel: TLabel
    Left = 88
    Top = 8
    Width = 12
    Height = 12
    Caption = '50'
  end
  object TrackBar: TTrackBar
    Left = 112
    Top = 32
    Width = 145
    Height = 29
    Max = 100
    Min = 1
    Orientation = trHorizontal
    PageSize = 5
    Frequency = 1
    Position = 50
    SelEnd = 0
    SelStart = 0
    TabOrder = 0
    TickMarks = tmBottomRight
    TickStyle = tsNone
    OnChange = TrackBarChange
  end
  object RestoreDefaultButton: TButton
    Left = 16
    Top = 80
    Width = 123
    Height = 25
    Caption = '�ǥե���Ȥˑ���(&D)'
    TabOrder = 1
    OnClick = RestoreDefaultButtonClick
  end
  object OKButton: TButton
    Left = 200
    Top = 80
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 2
  end
  object CancelButton: TButton
    Left = 288
    Top = 80
    Width = 75
    Height = 25
    Cancel = True
    Caption = '����󥻥�'
    ModalResult = 2
    TabOrder = 3
  end
end
