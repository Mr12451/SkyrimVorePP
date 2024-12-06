;/ Decompiled by Champollion V1.0.1
Source   : TIF__BTSSetLocation.psc
Modified : 2023-10-19 11:06:34
Compiled : 2023-10-19 11:06:35
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__BTSSetLocation extends TopicInfo hidden

;-- Properties --------------------------------------
formlist[] property EastmarchLocations auto
globalvariable property DevourmentDialogueBTS_Type auto
formlist[] property HjaalmarchLocations auto
referencealias property BTSTargetAlias auto
formlist[] property WinterholdLocations auto
Int property SelectedLoc auto
referencealias property BTSTaxiAlias auto
formlist[] property TheRiftLocations auto
formlist[] property WhiterunHoldLocations auto
globalvariable property DevourmentDialogueBTS_Hold auto
formlist[] property HaafingarLocations auto
formlist[] property ThePaleLocations auto
formlist[] property TheReachLocations auto
formlist[] property FalkreathLocations auto
formlist[] property SolstheimLocations auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GotoState

; Skipped compiler generated GetState

function Fragment_0(ObjectReference akSpeakerRef)

	ObjectReference Target
	actor akSpeaker = akSpeakerRef as actor
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
