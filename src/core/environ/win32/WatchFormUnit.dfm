object TVPWatchForm: TTVPWatchForm
  Left = 50
  Top = 127
  Width = 299
  Height = 223
  HorzScrollBar.Visible = False
  VertScrollBar.Visible = False
  Caption = '���ʽ����'
  Color = clBtnFace
  DefaultMonitor = dmDesktop
  Font.Charset = GB2312_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = '����'
  Font.Style = []
  OldCreateOrder = False
  PopupMenu = PopupMenu
  Position = poDefaultPosOnly
  OnClose = FormClose
  OnDestroy = FormDestroy
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 12
  object ToolBar: TToolBar
    Left = 0
    Top = 178
    Width = 89
    Height = 18
    Align = alNone
    Anchors = [akLeft, akBottom]
    ButtonHeight = 18
    ButtonWidth = 19
    Caption = 'ToolBar'
    EdgeBorders = []
    Flat = True
    Images = TVPMainForm.VerySmallIconImageList
    TabOrder = 0
    object UpdateButton: TToolButton
      Left = 0
      Top = 0
      Hint = '����'
      Caption = '����'
      ImageIndex = 4
      ParentShowHint = False
      ShowHint = True
      OnClick = UpdateButtonClick
    end
    object AutoUpdateButton: TToolButton
      Left = 19
      Top = 0
      Hint = '�Զ�����'
      Caption = '�Զ�����'
      ImageIndex = 5
      ParentShowHint = False
      ShowHint = True
      Style = tbsCheck
      OnClick = AutoUpdateButtonClick
    end
    object ToolButton3: TToolButton
      Left = 38
      Top = 0
      Width = 8
      Caption = 'ToolButton3'
      ImageIndex = 8
      Style = tbsSeparator
    end
    object NewExprButton: TToolButton
      Left = 46
      Top = 0
      Hint = '�±��ʽ'
      Caption = '�±��ʽ'
      ImageIndex = 6
      ParentShowHint = False
      ShowHint = True
      OnClick = NewExprButtonClick
    end
    object DeleteButton: TToolButton
      Left = 65
      Top = 0
      Hint = 'ɾ��'
      Caption = 'ɾ��'
      ImageIndex = 7
      ParentShowHint = False
      ShowHint = True
      OnClick = DeleteButtonClick
    end
  end
  object StatusBar: TStatusBar
    Left = 96
    Top = 177
    Width = 195
    Height = 19
    Align = alNone
    Anchors = [akLeft, akRight, akBottom]
    Panels = <>
    SimplePanel = False
  end
  object ListView: TListView
    Left = 0
    Top = 0
    Width = 289
    Height = 175
    Anchors = [akLeft, akTop, akRight, akBottom]
    Columns = <
      item
        Caption = 'ʽ'
        Width = 140
      end
      item
        Caption = '���'
        Width = 400
      end>
    ColumnClick = False
    MultiSelect = True
    TabOrder = 2
    ViewStyle = vsReport
    OnDblClick = ListViewDblClick
    OnEdited = ListViewEdited
    OnKeyPress = ListViewKeyPress
    OnMouseMove = ListViewMouseMove
  end
  object PopupMenu: TPopupMenu
    Images = TVPMainForm.SmallIconImageList
    OnPopup = PopupMenuPopup
    Left = 256
    Top = 24
    object NewExprMenuItem: TMenuItem
      Caption = '�±��ʽ(&N) ...'
      Hint = '�±��ʽ'
      ImageIndex = 27
      ShortCut = 45
      OnClick = NewExprButtonClick
    end
    object DeleteExprMenuItem: TMenuItem
      Caption = 'ɾ��(&X)'
      Hint = 'ɾ��'
      ImageIndex = 15
      ShortCut = 46
      OnClick = DeleteButtonClick
    end
    object EditExpressionMenuItem: TMenuItem
      Caption = '���ʽ�༭(&E)'
      OnClick = EditExpressionMenuItemClick
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object UpdateMenuItem: TMenuItem
      Caption = '����(&U)'
      ImageIndex = 25
      ShortCut = 116
      OnClick = UpdateButtonClick
    end
    object AutoUpdateMenuItem: TMenuItem
      Caption = '�Զ�����(&A)'
      ImageIndex = 26
      ShortCut = 16500
      OnClick = AutoUpdateMenuItemClick
    end
    object AutoUpdateIntervalMenuItem: TMenuItem
      Caption = '�Զ����¼��ʱ��(&I)'
      Hint = '�Զ����¼��ʱ��'
      object UIRealTimeMenuItem: TMenuItem
        Caption = 'ʵʱ(&R)'
        GroupIndex = 11
        Hint = 'ʵʱ'
        RadioItem = True
        Visible = False
        OnClick = UIRealTimeMenuItemClick
      end
      object UI0_2SecMenuItem: TMenuItem
        Tag = 200
        Caption = '0.2��(&A)'
        GroupIndex = 11
        Hint = '0.2��'
        RadioItem = True
        OnClick = UIRealTimeMenuItemClick
      end
      object UI0_5SecMenuItem: TMenuItem
        Tag = 500
        Caption = '0.5��(&B)'
        GroupIndex = 11
        Hint = '0.5��'
        RadioItem = True
        OnClick = UIRealTimeMenuItemClick
      end
      object N071: TMenuItem
        Tag = 700
        Caption = '0.7��(&C)'
        GroupIndex = 11
        Hint = '0.7��'
        RadioItem = True
        OnClick = UIRealTimeMenuItemClick
      end
      object UI1SecMenuItem: TMenuItem
        Tag = 1000
        Caption = '1��(&1)'
        Checked = True
        GroupIndex = 11
        Hint = '1��'
        RadioItem = True
        OnClick = UIRealTimeMenuItemClick
      end
      object N221: TMenuItem
        Tag = 2000
        Caption = '2��(&2)'
        GroupIndex = 11
        Hint = '2��'
        RadioItem = True
        OnClick = UIRealTimeMenuItemClick
      end
      object UI3SecMenuItem: TMenuItem
        Tag = 3000
        Caption = '3��(&3)'
        GroupIndex = 11
        Hint = '3��'
        RadioItem = True
        OnClick = UIRealTimeMenuItemClick
      end
      object UI5SecMenuItem: TMenuItem
        Tag = 5000
        Caption = '5��(&5)'
        GroupIndex = 11
        Hint = '5��'
        RadioItem = True
        OnClick = UIRealTimeMenuItemClick
      end
      object UI9SecMenuItem: TMenuItem
        Tag = 9000
        Caption = '9��(&9)'
        GroupIndex = 11
        Hint = '9��'
        RadioItem = True
        OnClick = UIRealTimeMenuItemClick
      end
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object ShowControllerMenuItem: TMenuItem
      Caption = '������(&Z)'
      Hint = '��ʾ������'
      ImageIndex = 28
      ShortCut = 8304
      OnClick = ShowControllerMenuItemClick
    end
    object ShowScriptEditorMenuItem: TMenuItem
      Caption = '�ű��༭��(&E)'
      Hint = '��ʾ�ű��༭��'
      ImageIndex = 2
      ShortCut = 8305
      OnClick = ShowScriptEditorMenuItemClick
    end
    object ShowWatchMenuItem: TMenuItem
      Caption = '���ʽ����(&W)'
      ShortCut = 8306
      Visible = False
      OnClick = ShowWatchMenuItemClick
    end
    object ShowConsoleMenuItem: TMenuItem
      Caption = '����̨(&D)'
      Hint = '��ʾ����̨'
      ImageIndex = 13
      ShortCut = 8307
      OnClick = ShowConsoleMenuItemClick
    end
    object N3: TMenuItem
      Caption = '-'
    end
    object ShowOnTopMenuItem: TMenuItem
      Caption = '������ǰ��ʾ(&F)'
      ImageIndex = 10
      OnClick = ShowOnTopMenuItemClick
    end
    object ShowAboutMenuItem: TMenuItem
      Caption = 'about ...'
      ShortCut = 16507
      Visible = False
      OnClick = ShowAboutMenuItemClick
    end
    object CopyImportantLogMenuItem: TMenuItem
      Caption = 'copy important log'
      ShortCut = 16506
      Visible = False
      OnClick = CopyImportantLogMenuItemClick
    end
  end
  object EliminateTimer: TTimer
    Enabled = False
    Interval = 100
    OnTimer = EliminateTimerTimer
    Left = 256
    Top = 56
  end
  object UpdateTimer: TTimer
    Enabled = False
    OnTimer = UpdateTimerTimer
    Left = 256
    Top = 88
  end
end
