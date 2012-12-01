object TVPPadForm: TTVPPadForm
  Left = 74
  Top = 278
  HorzScrollBar.Visible = False
  VertScrollBar.Visible = False
  BorderIcons = [biSystemMenu, biMaximize]
  Caption = 'Pad'
  ClientHeight = 316
  ClientWidth = 522
  Color = clBtnFace
  Constraints.MinHeight = 100
  Constraints.MinWidth = 70
  DefaultMonitor = dmDesktop
  Font.Charset = GB2312_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #23435#20307
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnDestroy = FormDestroy
  OnShow = FormShow
  DesignSize = (
    522
    316)
  PixelsPerInch = 96
  TextHeight = 12
  object StatusBar: TStatusBar
    Left = 32
    Top = 306
    Width = 498
    Height = 19
    Align = alNone
    Anchors = [akLeft, akRight, akBottom]
    Panels = <
      item
        Width = 80
      end
      item
        Width = 50
      end>
  end
  object ToolBar: TToolBar
    Left = 0
    Top = 307
    Width = 25
    Height = 18
    Align = alNone
    Anchors = [akLeft, akBottom]
    ButtonHeight = 18
    ButtonWidth = 19
    Caption = 'ToolBar'
    Images = TVPMainForm.VerySmallIconImageList
    ParentShowHint = False
    ShowHint = True
    TabOrder = 1
    object ExecuteButton: TToolButton
      Left = 0
      Top = 0
      Hint = #25191#34892
      Caption = 'Execute'
      ImageIndex = 0
      OnClick = ExecuteButtonClick
    end
  end
  object Memo: TRichEdit
    Left = 0
    Top = 0
    Width = 530
    Height = 305
    Anchors = [akLeft, akTop, akRight, akBottom]
    Color = clNavy
    Font.Charset = GB2312_CHARSET
    Font.Color = clWhite
    Font.Height = -12
    Font.Name = #23435#20307
    Font.Style = []
    HideScrollBars = False
    MaxLength = 10000000
    ParentFont = False
    PlainText = True
    PopupMenu = MemoPopupMenu
    ScrollBars = ssBoth
    TabOrder = 2
    WantTabs = True
    WordWrap = False
    OnKeyDown = MemoKeyDown
    OnMouseDown = MemoMouseDown
    OnMouseUp = MemoMouseUp
    OnSelectionChange = MemoSelectionChange
  end
  object MemoPopupMenu: TPopupMenu
    Images = TVPMainForm.SmallIconImageList
    OnPopup = MemoPopupMenuPopup
    Left = 24
    Top = 24
    object CutMenuItem: TMenuItem
      Caption = #21098#20999'(&T)'
      Hint = #21098#20999
      ImageIndex = 5
      ShortCut = 16472
      OnClick = CutMenuItemClick
    end
    object CopyMenuItem: TMenuItem
      Caption = #22797#21046'(&C)'
      Hint = #22797#21046
      ImageIndex = 6
      ShortCut = 16451
      OnClick = CopyMenuItemClick
    end
    object PasteMenuItem: TMenuItem
      Caption = #31896#36148'(&P)'
      Hint = #31896#36148
      ImageIndex = 7
      ShortCut = 16470
      OnClick = PasteMenuItemClick
    end
    object UndoMenuItem: TMenuItem
      Caption = #25764#38144'(&U)'
      ImageIndex = 3
      ShortCut = 16474
      OnClick = UndoMenuItemClick
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object ExecuteMenuItem: TMenuItem
      Caption = #25191#34892'(&G)'
      Hint = #25191#34892#33050#26412
      ImageIndex = 1
      ShortCut = 16397
      OnClick = ExecuteButtonClick
    end
    object N3: TMenuItem
      Caption = '-'
    end
    object SaveMenuItem: TMenuItem
      Caption = #20445#23384'(&S) ...'
      ImageIndex = 9
      ShortCut = 16467
      OnClick = SaveMenuItemClick
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object ShowControllerMenuItem: TMenuItem
      Caption = #25511#21046#22120'(&Z)'
      Hint = #26174#31034#25511#21046#22120
      ImageIndex = 28
      ShortCut = 8304
      OnClick = ShowControllerMenuItemClick
    end
    object ShowScriptEditorMenuItem: TMenuItem
      Caption = #33050#26412#32534#36753#22120'(&E)'
      ShortCut = 8305
      Visible = False
      OnClick = ShowScriptEditorMenuItemClick
    end
    object ShowWatchMenuItem: TMenuItem
      Caption = #34920#36798#24335#30417#35270'(&W)'
      Hint = #26174#31034#34920#36798#24335#30417#35270#31383#21475
      ImageIndex = 14
      ShortCut = 8306
      OnClick = ShowWatchMenuItemClick
    end
    object ShowConsoleMenuItem: TMenuItem
      Caption = #25511#21046#21488'(&D)'
      Hint = #26174#31034#25511#21046#21488
      ImageIndex = 13
      ShortCut = 8307
      OnClick = ShowConsoleMenuItemClick
    end
    object N4: TMenuItem
      Caption = '-'
    end
    object ShowOnTopMenuItem: TMenuItem
      Caption = #24635#22312#26368#21069#38754#26174#31034'(&F)'
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
  object SaveDialog: TSaveDialog
    DefaultExt = '.tjs'
    Filter = 'TJS2 '#33050#26412'(*.tjs)|*.tjs|'#25152#26377#25991#20214' (*.*)|*.*'
    Options = [ofHideReadOnly, ofPathMustExist, ofEnableSizing]
    Left = 56
    Top = 24
  end
end
