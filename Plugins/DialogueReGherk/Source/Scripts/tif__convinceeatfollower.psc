;/ Decompiled by Champollion V1.0.1
Source   : TIF__ConvinceEatFollower.psc
Modified : 2024-01-25 10:15:54
Compiled : 2024-01-25 10:15:55
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__ConvinceEatFollower extends TopicInfo hidden

;-- Properties --------------------------------------
quest property DevourmentDialogueAliasPicker auto
favordialoguescript property pFDS auto
scene property SceneToStart auto
Bool property intimidate auto
Bool property bribe auto
Bool property persuade auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	if bribe == true
		DevourmentDialogueAliasPicker.SetStage(5)
	elseIf persuade == true
		pFDS.persuade(akSpeaker)
	elseIf intimidate
		pFDS.intimidate(akSpeaker)
	endIf
	SceneToStart.Start()
endFunction

; Skipped compiler generated GotoState

; Skipped compiler generated GetState
