object ConfSettingsForm: TConfSettingsForm
  Left = 56
  Top = 447
  Width = 607
  Height = 406
  Caption = '�O��'
  Color = clBtnFace
  Constraints.MinHeight = 389
  Constraints.MinWidth = 607
  Font.Charset = GB2312_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = '�ͣ� �Х����å�'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefaultPosOnly
  PixelsPerInch = 96
  TextHeight = 12
  object SpecifiedFileIsNotKrKrLabel: TLabel
    Left = 8
    Top = 328
    Width = 324
    Height = 12
    Caption = 'ָ�����줿�ե�����ϼ��Ｊ��g�п��ܥե�����ǤϤ���ޤ���'
    Visible = False
  end
  object IconChangeWarnLabel: TLabel
    Left = 8
    Top = 337
    Width = 407
    Height = 12
    Anchors = [akLeft, akBottom]
    Caption = 
      '�� һ�ȥ�������Ή�����Ф��Ȅe�Υ�������ˉ���Ǥ��ʤ��ʤ����' +
      '�Ԥ�����ޤ�'
  end
  object FileWroteLabel: TLabel
    Left = 8
    Top = 336
    Width = 98
    Height = 12
    Caption = '�O���򱣴椷�ޤ���'
    Visible = False
  end
  object ProgramMayRunningLabel: TLabel
    Left = 8
    Top = 344
    Width = 322
    Height = 12
    Caption = '�ե�������_���ޤ���(�ץ���ब�g���Фο����Ԥ�����ޤ�)'
    Visible = False
  end
  object OKButton: TButton
    Left = 425
    Top = 344
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'OK'
    Default = True
    TabOrder = 0
    OnClick = OKButtonClick
  end
  object CancelButton: TButton
    Left = 505
    Top = 344
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Cancel = True
    Caption = '����󥻥�'
    ModalResult = 2
    TabOrder = 1
  end
  inline ConfMainFrame: TConfMainFrame
    Left = 8
    Top = 8
    Width = 578
    Height = 323
    Anchors = [akLeft, akTop, akRight, akBottom]
    TabOrder = 2
    inherited IconGroupBox: TGroupBox
      Top = 260
      Width = 578
      TabOrder = 1
    end
    inherited OptionsGroupBox: TGroupBox
      Width = 578
      Height = 201
      inherited Label2: TLabel
        Left = 360
      end
      inherited OptionsTreeView: TTreeView
        TabOrder = 2
      end
      inherited OptionValueComboBox: TComboBox
        Left = 360
      end
      inherited OptionDescMemo: TMemo
        TabOrder = 0
      end
      inherited RestoreDefaultButton: TButton
        Width = 121
      end
    end
    inherited ReleaseOptionGroupBox: TGroupBox
      Top = 210
      Width = 578
      TabOrder = 2
    end
    inherited Panel1: TPanel
      Top = 201
      Width = 578
    end
    inherited Panel2: TPanel
      Top = 251
      Width = 578
    end
  end
  object OpenDialog: TOpenDialog
    DefaultExt = 'exe'
    Filter = '�g�п��ܥե�����(*.exe)|*.exe|���٤ƤΥե�����(*.*)|*.*'
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofNoReadOnlyReturn, ofEnableSizing]
    Title = '���Ｊ�� �g�п��ܥե�������x�k'
    Left = 384
    Top = 328
  end
end
