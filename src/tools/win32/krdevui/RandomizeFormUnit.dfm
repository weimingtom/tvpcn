object RandomizeForm: TRandomizeForm
  Left = 429
  Top = 26
  BorderStyle = bsDialog
  Caption = '����ȥ�ԩ`�򅧼���'
  ClientHeight = 182
  ClientWidth = 215
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
    Left = 64
    Top = 48
    Width = 87
    Height = 12
    Alignment = taCenter
    Caption = '�ޥ������`�����'
  end
  object Label2: TLabel
    Left = 44
    Top = 80
    Width = 126
    Height = 12
    Alignment = taCenter
    Caption = '���Ф餯�ޥ�����Ӥ�����'
  end
  object Label3: TLabel
    Left = 21
    Top = 96
    Width = 173
    Height = 12
    Alignment = taCenter
    Caption = '���Υ�����ɥ����ԄӵĤ��]���ޤ�'
  end
  object Label5: TLabel
    Left = 35
    Top = 64
    Width = 145
    Height = 12
    Alignment = taCenter
    Caption = '�Ǥ����˄Ӥ����Ƥ���������'
  end
  object Label7: TLabel
    Left = 27
    Top = 8
    Width = 161
    Height = 12
    Caption = '���Ǥ���ᤵ������Ҫ�Ȥʤ�ޤ���'
  end
  object CancelButton: TButton
    Left = 59
    Top = 144
    Width = 97
    Height = 25
    Cancel = True
    Caption = '����󥻥�(&C)'
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
