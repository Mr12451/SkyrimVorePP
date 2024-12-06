;/ Decompiled by Champollion V1.0.1
Source   : TIF__DDStartScene.psc
Modified : 2023-10-05 20:20:43
Compiled : 2023-10-05 20:20:43
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__DDStartScene extends TopicInfo hidden

;-- Properties --------------------------------------
favordialoguescript property pFDS auto
Bool property bribe auto
scene property SceneToStart auto
Bool property persuade auto
Bool property intimidate auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GetState

; Skipped compiler generated GotoState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	SceneToStart.Start()
	if bribe
		pFDS.bribe(akSpeaker)
	elseIf persuade
		pFDS.persuade(akSpeaker)
	elseIf intimidate
		pFDS.intimidate(akSpeaker)
	endIf
endFunction
