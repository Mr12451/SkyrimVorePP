;/ Decompiled by Champollion V1.0.1
Source   : TIF__SpeechCheckRelationshipBump.psc
Modified : 2023-10-09 17:45:43
Compiled : 2023-10-09 17:45:44
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__SpeechCheckRelationshipBump extends TopicInfo hidden

;-- Properties --------------------------------------
Bool property persuade auto
Bool property bribe auto
Bool property intimidate auto
favordialoguescript property pFDS auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GotoState

; Skipped compiler generated GetState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	if persuade
		pFDS.persuade(akSpeaker)
	elseIf bribe
		pFDS.bribe(akSpeaker)
	elseIf intimidate
		pFDS.intimidate(akSpeaker)
	endIf
	akSpeaker.SetRelationshipRank(game.GetPlayer(), akSpeaker.GetRelationshipRank(game.GetPlayer()) + 1)
endFunction
