object MainForm: TMainForm
  Left = 56
  Top = 76
  ActiveControl = CheckButton
  BorderStyle = bsDialog
  Caption = 'MainForm'
  ClientHeight = 368
  ClientWidth = 490
  Color = clBtnFace
  Font.Charset = GB2312_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = '�ͣ� �Х����å�'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefaultPosOnly
  OnCloseQuery = FormCloseQuery
  PixelsPerInch = 96
  TextHeight = 12
  object NotYetCheckedLabel: TLabel
    Left = 96
    Top = 328
    Width = 51
    Height = 12
    Caption = 'δ�����å�'
    Visible = False
  end
  object CheckingLabel: TLabel
    Left = 152
    Top = 328
    Width = 83
    Height = 12
    Caption = '�����å���(%d%%)'
    Visible = False
  end
  object BrokenLabel: TLabel
    Left = 272
    Top = 328
    Width = 24
    Height = 12
    Caption = '�Ɠp'
    Visible = False
  end
  object NotBrokenLabel: TLabel
    Left = 240
    Top = 328
    Width = 24
    Height = 12
    Caption = '����'
    Visible = False
  end
  object MoreThanOneFileIsBrokenLabel: TLabel
    Left = 112
    Top = 344
    Width = 148
    Height = 12
    Caption = ' �Ɠp�����ե����뤬����ޤ� '
    Color = clRed
    Font.Charset = GB2312_CHARSET
    Font.Color = clWhite
    Font.Height = -12
    Font.Name = '�ͣ� �Х����å�'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    Visible = False
  end
  object CheckingButtonLabel: TLabel
    Left = 104
    Top = 352
    Width = 57
    Height = 12
    Caption = '�����å���...'
    Visible = False
  end
  object TheResultWasCopiedIntoClipboardLabel: TLabel
    Left = 304
    Top = 328
    Width = 187
    Height = 12
    Caption = '�Y���򥯥�åץܩ`�ɤ˥��ԩ`���ޤ���'
    Visible = False
  end
  object NoticeMemo: TMemo
    Left = 8
    Top = 8
    Width = 473
    Height = 57
    BorderStyle = bsNone
    Color = clBtnFace
    Font.Charset = GB2312_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = '�ͣ� �Х����å�'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 0
  end
  object ListView: TListView
    Left = 8
    Top = 72
    Width = 473
    Height = 249
    Columns = <
      item
        Caption = '�ե�������'
        Width = 180
      end
      item
        Caption = '״�B'
        Width = 80
      end>
    ColumnClick = False
    GridLines = True
    RowSelect = True
    ShowWorkAreas = True
    SortType = stText
    TabOrder = 1
    ViewStyle = vsReport
  end
  object CheckButton: TButton
    Left = 8
    Top = 336
    Width = 89
    Height = 25
    Caption = '�����å�(&V)'
    Default = True
    TabOrder = 2
    OnClick = CheckButtonClick
  end
  object CloseButton: TButton
    Left = 392
    Top = 336
    Width = 89
    Height = 25
    Cancel = True
    Caption = '�]����(&X)'
    TabOrder = 4
    OnClick = CloseButtonClick
  end
  object CopyResultButton: TButton
    Left = 272
    Top = 336
    Width = 107
    Height = 25
    Hint = '�Y���򥯥�åץܩ`�ɤ˥��ԩ`���ޤ�'
    Caption = '�Y���򥳥ԩ`(&C)'
    Enabled = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 3
    OnClick = CopyResultButtonClick
  end
end
