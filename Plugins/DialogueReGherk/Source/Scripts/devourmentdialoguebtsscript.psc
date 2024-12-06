;/ Decompiled by Champollion V1.0.1
Source   : DevourmentDialogueBTSScript.psc
Modified : 2023-10-19 10:58:23
Compiled : 2023-10-19 10:58:23
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName DevourmentDialogueBTSScript extends Quest

;-- Properties --------------------------------------
formlist[] property TheReachLocations auto
referencealias property BTSTargetAlias auto
globalvariable property DevourmentDialogueBTS_Type auto
formlist[] property HaafingarLocations auto
referencealias property BTSTaxiAlias auto
formlist[] property TheRiftLocations auto
formlist[] property HjaalmarchLocations auto
formlist[] property EastmarchLocations auto
formlist[] property FalkreathLocations auto
globalvariable property DevourmentDialogueBTS_Hold auto
formlist[] property ThePaleLocations auto
formlist[] property WinterholdLocations auto
formlist[] property WhiterunHoldLocations auto
formlist[] property SolstheimLocations auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GetState

function DecideBTSLocation(Int SelectedLoc)

	ObjectReference Target
	Int SelectedType = DevourmentDialogueBTS_Type.GetValue() as Int
	if DevourmentDialogueBTS_Hold.GetValue() == 0 as Float
		Target = EastmarchLocations[SelectedType].GetAt(SelectedLoc) as ObjectReference
	elseIf DevourmentDialogueBTS_Hold.GetValue() == 1 as Float
		Target = FalkreathLocations[SelectedType].GetAt(SelectedLoc) as ObjectReference
	elseIf DevourmentDialogueBTS_Hold.GetValue() == 2 as Float
		Target = HaafingarLocations[SelectedType].GetAt(SelectedLoc) as ObjectReference
	elseIf DevourmentDialogueBTS_Hold.GetValue() == 3 as Float
		Target = HjaalmarchLocations[SelectedType].GetAt(SelectedLoc) as ObjectReference
	elseIf DevourmentDialogueBTS_Hold.GetValue() == 4 as Float
		Target = SolstheimLocations[SelectedType].GetAt(SelectedLoc) as ObjectReference
	elseIf DevourmentDialogueBTS_Hold.GetValue() == 5 as Float
		Target = ThePaleLocations[SelectedType].GetAt(SelectedLoc) as ObjectReference
	elseIf DevourmentDialogueBTS_Hold.GetValue() == 6 as Float
		Target = TheReachLocations[SelectedType].GetAt(SelectedLoc) as ObjectReference
	elseIf DevourmentDialogueBTS_Hold.GetValue() == 7 as Float
		Target = TheRiftLocations[SelectedType].GetAt(SelectedLoc) as ObjectReference
	elseIf DevourmentDialogueBTS_Hold.GetValue() == 8 as Float
		Target = WhiterunHoldLocations[SelectedType].GetAt(SelectedLoc) as ObjectReference
	elseIf DevourmentDialogueBTS_Hold.GetValue() == 9 as Float
		Target = WinterholdLocations[SelectedType].GetAt(SelectedLoc) as ObjectReference
	endIf
	BTSTargetAlias.ForceRefTo(Target)
endFunction

; Skipped compiler generated GotoState
