object TVPWatchForm: TTVPWatchForm
  Left = 50
  Top = 127
  Width = 299
  Height = 223
  HorzScrollBar.Visible = False
  VertScrollBar.Visible = False
  Caption = '表达式监视'
  Color = clBtnFace
  DefaultMonitor = dmDesktop
  Font.Charset = GB2312_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = '宋体'
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
      Hint = '更新'
      Caption = '更新'
      ImageIndex = 4
      ParentShowHint = False
      ShowHint = True
      OnClick = UpdateButtonClick
    end
    object AutoUpdateButton: TToolButton
      Left = 19
      Top = 0
      Hint = '自动更新'
      Caption = '自动更新'
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
      Hint = '新表达式'
      Caption = '新表达式'
      ImageIndex = 6
      ParentShowHint = False
      ShowHint = True
      OnClick = NewExprButtonClick
    end
    object DeleteButton: TToolButton
      Left = 65
      Top = 0
      Hint = '删除'
      Caption = '删除'
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
        Caption = '式'
        Width = 140
      end
      item
        Caption = '结果'
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
      Caption = '新表达式(&N) ...'
      Hint = '新表达式'
      ImageIndex = 27
      ShortCut = 45
      OnClick = NewExprButtonClick
    end
    object DeleteExprMenuItem: TMenuItem
      Caption = '删除(&X)'
      Hint = '删除'
      ImageIndex = 15
      ShortCut = 46
      OnClick = DeleteButtonClick
    end
    object EditExpressionMenuItem: TMenuItem
      Caption = '表达式编辑(&E)'
      OnClick = EditExpressionMenuItemClick
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object UpdateMenuItem: TMenuItem
      Caption = '更新(&U)'
      ImageIndex = 25
      ShortCut = 116
      OnClick = UpdateButtonClick
    end
    object AutoUpdateMenuItem: TMenuItem
      Caption = '自动更新(&A)'
      ImageIndex = 26
      ShortCut = 16500
      OnClick = AutoUpdateMenuItemClick
    end
    object AutoUpdateIntervalMenuItem: TMenuItem
      Caption = '自动更新间隔时间(&I)'
      Hint = '自动更新间隔时间'
      object UIRealTimeMenuItem: TMenuItem
        Caption = '实时(&R)'
        GroupIndex = 11
        Hint = '实时'
        RadioItem = True
        Visible = False
        OnClick = UIRealTimeMenuItemClick
      end
      object UI0_2SecMenuItem: TMenuItem
        Tag = 200
        Caption = '0.2秒(&A)'
        GroupIndex = 11
        Hint = '0.2秒'
        RadioItem = True
        OnClick = UIRealTimeMenuItemClick
      end
      object UI0_5SecMenuItem: TMenuItem
        Tag = 500
        Caption = '0.5秒(&B)'
        GroupIndex = 11
        Hint = '0.5秒'
        RadioItem = True
        OnClick = UIRealTimeMenuItemClick
      end
      object N071: TMenuItem
        Tag = 700
        Caption = '0.7秒(&C)'
        GroupIndex = 11
        Hint = '0.7秒'
        RadioItem = True
        OnClick = UIRealTimeMenuItemClick
      end
      object UI1SecMenuItem: TMenuItem
        Tag = 1000
        Caption = '1秒(&1)'
        Checked = True
        GroupIndex = 11
        Hint = '1秒'
        RadioItem = True
        OnClick = UIRealTimeMenuItemClick
      end
      object N221: TMenuItem
        Tag = 2000
        Caption = '2秒(&2)'
        GroupIndex = 11
        Hint = '2秒'
        RadioItem = True
        OnClick = UIRealTimeMenuItemClick
      end
      object UI3SecMenuItem: TMenuItem
        Tag = 3000
        Caption = '3秒(&3)'
        GroupIndex = 11
        Hint = '3秒'
        RadioItem = True
        OnClick = UIRealTimeMenuItemClick
      end
      object UI5SecMenuItem: TMenuItem
        Tag = 5000
        Caption = '5秒(&5)'
        GroupIndex = 11
        Hint = '5秒'
        RadioItem = True
        OnClick = UIRealTimeMenuItemClick
      end
      object UI9SecMenuItem: TMenuItem
        Tag = 9000
        Caption = '9秒(&9)'
        GroupIndex = 11
        Hint = '9秒'
        RadioItem = True
        OnClick = UIRealTimeMenuItemClick
      end
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object ShowControllerMenuItem: TMenuItem
      Caption = '控制器(&Z)'
      Hint = '显示控制器'
      ImageIndex = 28
      ShortCut = 8304
      OnClick = ShowControllerMenuItemClick
    end
    object ShowScriptEditorMenuItem: TMenuItem
      Caption = '脚本编辑器(&E)'
      Hint = '显示脚本编辑器'
      ImageIndex = 2
      ShortCut = 8305
      OnClick = ShowScriptEditorMenuItemClick
    end
    object ShowWatchMenuItem: TMenuItem
      Caption = '表达式监视(&W)'
      ShortCut = 8306
      Visible = False
      OnClick = ShowWatchMenuItemClick
    end
    object ShowConsoleMenuItem: TMenuItem
      Caption = '控制台(&D)'
      Hint = '显示控制台'
      ImageIndex = 13
      ShortCut = 8307
      OnClick = ShowConsoleMenuItemClick
    end
    object N3: TMenuItem
      Caption = '-'
    end
    object ShowOnTopMenuItem: TMenuItem
      Caption = '总在最前显示(&F)'
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
