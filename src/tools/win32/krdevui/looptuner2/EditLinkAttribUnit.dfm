object EditLinkAttribFrame: TEditLinkAttribFrame
  Left = 0
  Top = 0
  Width = 419
  Height = 30
  HorzScrollBar.Visible = False
  VertScrollBar.Visible = False
  TabOrder = 0
  object Label1: TLabel
    Left = 152
    Top = 8
    Width = 67
    Height = 12
    Caption = '桑のフラグが'
  end
  object EnableConditionCheckBox: TCheckBox
    Left = 8
    Top = 6
    Width = 81
    Height = 17
    Caption = '訳周原き'
    TabOrder = 0
    OnClick = EnableConditionCheckBoxClick
    OnKeyDown = ConditionComboBoxKeyDown
  end
  object CondVarComboBox: TComboBox
    Left = 104
    Top = 4
    Width = 41
    Height = 20
    Style = csDropDownList
    DropDownCount = 16
    ItemHeight = 12
    TabOrder = 1
    OnChange = CondVarComboBoxChange
    OnKeyDown = ConditionComboBoxKeyDown
    Items.Strings = (
      '0'
      '1'
      '2'
      '3'
      '4'
      '5'
      '6'
      '7'
      '8'
      '9'
      '10'
      '11'
      '12'
      '13'
      '14'
      '15')
  end
  object CondRefValueEdit: TEdit
    Left = 232
    Top = 4
    Width = 33
    Height = 20
    MaxLength = 4
    TabOrder = 2
    Text = '0'
    OnChange = CondRefValueEditChange
    OnExit = CondRefValueEditExit
    OnKeyDown = ConditionComboBoxKeyDown
    OnKeyPress = CondRefValueEditKeyPress
  end
  object ConditionComboBox: TComboBox
    Left = 280
    Top = 4
    Width = 121
    Height = 20
    Style = csDropDownList
    ItemHeight = 12
    TabOrder = 3
    OnChange = ConditionComboBoxChange
    OnKeyDown = ConditionComboBoxKeyDown
    Items.Strings = (
      'と揖じとき'
      'でないとき'
      'より寄きいとき'
      '參貧のとき'
      'より弌さいとき'
      '參和のとき')
  end
end
