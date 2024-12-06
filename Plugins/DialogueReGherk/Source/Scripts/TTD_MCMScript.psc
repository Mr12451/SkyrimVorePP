;/ Decompiled by Champollion V1.0.1
Source   : TTD_MCMScript.psc
Modified : 2024-07-30 21:16:44
Compiled : 2024-07-30 21:16:45
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TTD_MCMScript extends SKI_ConfigBase

;-- Properties --------------------------------------
globalvariable property TTDialogue_ApproachFrequency auto
globalvariable property TTDialogue_Reformation auto
globalvariable property TTDialogueApproach_Female auto
globalvariable property TTDialogueApproach_PlayerPred auto
globalvariable property TTDialogueApproach_Male auto
globalvariable property TTDialogue_TrainingHealth auto
globalvariable property TTDialogue_IFAdjustment auto
globalvariable property TTDialogue_IdleFrequency auto
globalvariable property TTDialogue_SurpriseEndo auto
globalvariable property TTDialogueApproach_PlayerPrey auto
globalvariable property TTDialogue_SurpriseDigestions auto
globalvariable property TTDialogue_Approach auto

;-- Variables ---------------------------------------
Int TTDAPPd
Int TTDAF
Bool TTDAPPyState = true
Bool TTDRState = true
Bool TTDAPPdState = true
Float TTDSDState = 99.0000
Int TTDIFA
Int TTDIF
Float TTDSEState = 99.0000
Int TTDSD
Bool TTDAState = true
Int TTDAPPy
Int TTDR
Float TTDIFAValue = 99.0000
Int TTDA
Float TTDAFValue = 99.0000
Int TTDSE
Float TTDIFValue = 10.0000

;-- Functions ---------------------------------------

; Skipped compiler generated GetState

function OnPageReset(String page)

	if page == ""
		self.SetCursorFillMode(self.TOP_TO_BOTTOM)
		self.AddHeaderOption("Dialogue Function Toggles", 0)
		TTDA = self.AddToggleOption("Approach", TTDAState, 0)
		TTDAF = self.AddSliderOption("Approach Frequency", TTDAFValue, "", 0)
		TTDIF = self.AddSliderOption("Idle Dialogue Frequency", TTDIFValue, "", 0)
		TTDIFA = self.AddSliderOption("Idle Dialogue Frequency. Again", TTDIFAValue, "", 0)
		TTDR = self.AddToggleOption("Reformation Dialogue", TTDRState, 0)
		TTDSD = self.AddSliderOption("Surprise Digestions", TTDSDState, "", 0)
		TTDSE = self.AddSliderOption("Surprise Endo", TTDSEState, "", 0)
		TTDAPPd = self.AddToggleOption("NPC Approach Player Pred", TTDAPPdState, 0)
		TTDAPPy = self.AddToggleOption("NPC Approach Player Prey", TTDAPPyState, 0)
	endIf
endFunction

function OnOptionSliderAccept(Int option, Float value)

	if option == TTDAF
		TTDAFValue = value
		self.SetSliderOptionValue(TTDAF, TTDAFValue, "", false)
		TTDialogue_ApproachFrequency.SetValue((value as Int) as Float)
	elseIf option == TTDIF
		TTDIFValue = value
		self.SetSliderOptionValue(TTDIF, TTDIFValue, "", false)
		TTDialogue_IdleFrequency.SetValue((value as Int) as Float)
	elseIf option == TTDIFA
		TTDIFAValue = value
		self.SetSliderOptionValue(TTDIFA, TTDIFAValue, "", false)
		TTDialogue_IFAdjustment.SetValue((value as Int) as Float)
	elseIf option == TTDSD
		TTDSDState = value
		self.SetSliderOptionValue(TTDSD, TTDSDState, "", false)
		TTDialogue_SurpriseDigestions.SetValue((value as Int) as Float)
	elseIf option == TTDSE
		TTDSEState = value
		self.SetSliderOptionValue(TTDSE, TTDSEState, "", false)
		TTDialogue_SurpriseEndo.SetValue((value as Int) as Float)
	endIf
endFunction

function OnOptionHighlight(Int option)

	if option == TTDA
		self.SetInfoText("Controls whether sexlab style 'Approaches' in general are enabled.")
	elseIf option == TTDAF
		self.SetInfoText("Controls the probability every 23.5 seconds that an npc may attempt to Approach.")
	elseIf option == TTDIF
		self.SetInfoText("Controls the frequency of idle dialogue. Higher number = more frequent.")
	elseIf option == TTDIFA
		self.SetInfoText("Also controls frequency of idle dialogue. Lower number = less frequent.")
	elseIf option == TTDR
		self.SetInfoText("Controls whether dialogue referencing reformation is enabled. ill explode ur computer if u turn this off.")
	elseIf option == TTDSD
		self.SetInfoText("Controls the percent chance that Surprise Digestions may be valid to occur. This affects many things; from approach, to sleep, to idle dialogue.")
	elseIf option == TTDSE
		self.SetInfoText("Controls the percent chance that Surprise Endo may be valid to occur. Mostly affects approach and feeding dialogue.")
	elseIf option == TTDAPPd
		self.SetInfoText("Controls whether player pred approach scenarios can occur.")
	elseIf option == TTDAPPy
		self.SetInfoText("Controls whether player prey approach scenarious can occur.")
	endIf
endFunction

function OnOptionSelect(Int option)

	if option == TTDA
		TTDAState = !TTDAState
		self.SetToggleOptionValue(TTDA, TTDAState, 0 as Bool)
		TTDialogue_Approach.SetValue(TTDAState as Float)
	elseIf option == TTDR
		TTDRState = !TTDRState
		self.SetToggleOptionValue(TTDR, TTDRState, 0 as Bool)
		TTDialogue_Reformation.SetValue(TTDR as Float)
	elseIf option == TTDAPPd
		TTDAPPdState = !TTDAPPdState
		self.SetToggleOptionValue(TTDAPPd, TTDAPPdState, 0 as Bool)
		TTDialogueApproach_PlayerPred.SetValue(TTDAPPd as Float)
	elseIf option == TTDAPPy
		TTDAPPyState = !TTDAPPyState
		self.SetToggleOptionValue(TTDAPPy, TTDAPPyState, 0 as Bool)
		TTDialogueApproach_PlayerPrey.SetValue(TTDAPPy as Float)
	endIf
endFunction

; Skipped compiler generated GotoState

function OnOptionSliderOpen(Int option)

	if option == TTDAF
		self.SetSliderDialogStartValue(TTDAFValue)
		self.SetSliderDialogDefaultValue(99 as Float)
		self.SetSliderDialogRange(0.000000, 100.000)
		self.SetSliderDialogInterval(1.00000)
	elseIf option == TTDIF
		self.SetSliderDialogStartValue(TTDIFValue)
		self.SetSliderDialogDefaultValue(10 as Float)
		self.SetSliderDialogRange(0.000000, 100.000)
		self.SetSliderDialogInterval(1.00000)
	elseIf option == TTDIFA
		self.SetSliderDialogStartValue(TTDIFAValue)
		self.SetSliderDialogDefaultValue(99 as Float)
		self.SetSliderDialogRange(0.000000, 100.000)
		self.SetSliderDialogInterval(1.00000)
	elseIf option == TTDSD
		self.SetSliderDialogStartValue(TTDSDState)
		self.SetSliderDialogDefaultValue(99 as Float)
		self.SetSliderDialogRange(0.000000, 100.000)
		self.SetSliderDialogInterval(1.00000)
	elseIf option == TTDSE
		self.SetSliderDialogStartValue(TTDSEState)
		self.SetSliderDialogDefaultValue(99 as Float)
		self.SetSliderDialogRange(0.000000, 100.000)
		self.SetSliderDialogInterval(1.00000)
	endIf
endFunction
