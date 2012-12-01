object ConfMainFrame: TConfMainFrame
  Left = 0
  Top = 0
  Width = 585
  Height = 328
  TabOrder = 0
  TabStop = True
  object IconGroupBox: TGroupBox
    Left = 0
    Top = 265
    Width = 585
    Height = 63
    Align = alBottom
    Caption = #22270#26631'(&I)'
    TabOrder = 2
    object ChangeIconCheck: TCheckBox
      Left = 8
      Top = 32
      Width = 121
      Height = 17
      Caption = #20462#25913#22270#26631
      TabOrder = 0
    end
    object RefIconButton: TButton
      Left = 168
      Top = 29
      Width = 75
      Height = 20
      Caption = #27983#35272' ...'
      TabOrder = 1
      OnClick = RefIconButtonClick
    end
    object IconPanel: TPanel
      Left = 272
      Top = 16
      Width = 41
      Height = 41
      BevelInner = bvRaised
      BevelOuter = bvLowered
      TabOrder = 2
      object IconImage: TImage
        Left = 2
        Top = 2
        Width = 37
        Height = 37
        Align = alClient
        Center = True
        Transparent = True
      end
    end
  end
  object OptionsGroupBox: TGroupBox
    Left = 0
    Top = 0
    Width = 585
    Height = 206
    Align = alClient
    Caption = #36873#39033'(&O)'
    Enabled = False
    TabOrder = 0
    DesignSize = (
      585
      206)
    object Label1: TLabel
      Left = 8
      Top = 16
      Width = 70
      Height = 13
      Caption = #36873#39033#21517#31216'(&N) :'
      FocusControl = OptionsTreeView
    end
    object Label2: TLabel
      Left = 363
      Top = 16
      Width = 57
      Height = 13
      Anchors = [akTop, akRight]
      Caption = #36873#39033#20540'(&V) :'
    end
    object OptionsReadFailedLabel: TLabel
      Left = 120
      Top = 16
      Width = 96
      Height = 13
      Caption = #35835#21462#36873#39033#20449#24687#22833#36133
      Visible = False
    end
    object ReadingOptionsLabel: TLabel
      Left = 232
      Top = 16
      Width = 99
      Height = 13
      Caption = #36873#39033#20449#24687#35835#21462#20013' ...'
      Visible = False
    end
    object Label3: TLabel
      Left = 360
      Top = 88
      Width = 55
      Height = 13
      Anchors = [akTop, akRight]
      Caption = #36873#39033#35828#26126' :'
    end
    object OptionsTreeView: TTreeView
      Left = 8
      Top = 32
      Width = 348
      Height = 159
      Anchors = [akLeft, akTop, akRight, akBottom]
      HideSelection = False
      Indent = 19
      ReadOnly = True
      RowSelect = True
      ShowRoot = False
      TabOrder = 0
      OnChange = OptionsTreeViewChange
    end
    object OptionValueComboBox: TComboBox
      Left = 363
      Top = 32
      Width = 209
      Height = 21
      Style = csDropDownList
      Anchors = [akTop, akRight]
      Enabled = False
      ItemHeight = 13
      TabOrder = 1
      OnChange = OptionValueComboBoxChange
    end
    object OptionDescMemo: TMemo
      Left = 363
      Top = 104
      Width = 209
      Height = 87
      Anchors = [akTop, akRight, akBottom]
      BorderStyle = bsNone
      Color = clBtnFace
      ReadOnly = True
      ScrollBars = ssVertical
      TabOrder = 2
    end
    object RestoreDefaultButton: TButton
      Left = 363
      Top = 56
      Width = 118
      Height = 20
      Anchors = [akTop, akRight]
      Caption = #22238#21040#40664#35748#20540'(&D)'
      Enabled = False
      TabOrder = 3
      OnClick = RestoreDefaultButtonClick
    end
    object InvisibleCheckBox: TCheckBox
      Left = 508
      Top = 56
      Width = 65
      Height = 17
      Anchors = [akTop, akRight]
      Caption = #19981#21487#35265
      Enabled = False
      TabOrder = 4
      OnClick = InvisibleCheckBoxClick
    end
    object OptionValueEdit: TEdit
      Left = 363
      Top = 72
      Width = 209
      Height = 21
      Anchors = [akTop, akRight]
      TabOrder = 5
      Visible = False
      OnChange = OptionValueEditChange
    end
  end
  object ReleaseOptionGroupBox: TGroupBox
    Left = 0
    Top = 215
    Width = 585
    Height = 41
    Align = alBottom
    Caption = #21457#24067#36873#39033'(&R)'
    TabOrder = 1
    object DisableMessageMapCheckBox: TCheckBox
      Left = 8
      Top = 16
      Width = 345
      Height = 17
      Caption = #19981#35835#21462#20449#24687#26144#23556#25991#20214'[msgmap.tjs](&M)'
      TabOrder = 0
    end
    object ForceDataXP3CheckBox: TCheckBox
      Left = 368
      Top = 16
      Width = 201
      Height = 17
      Caption = #21482#20174'XP3'#25991#20214#21253#25191#34892'(&3)'
      TabOrder = 1
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 206
    Width = 585
    Height = 9
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 3
  end
  object Panel2: TPanel
    Left = 0
    Top = 256
    Width = 585
    Height = 9
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 4
  end
  object OpenPictureDialog: TOpenPictureDialog
    Filter = #22270#26631' (*.ico)|*.ico'
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Title = #25351#23450#22270#26631
    Left = 88
    Top = 232
  end
end
