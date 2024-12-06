;/ Decompiled by Champollion V1.0.1
Source   : TIF__RubBelly.psc
Modified : 2023-08-02 09:15:53
Compiled : 2023-08-02 09:15:54
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__RubBelly extends TopicInfo hidden

;-- Properties --------------------------------------
Bool property PlayerPred auto
idle property IdleWarmHandsStanding auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GotoState

; Skipped compiler generated GetState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	akSpeaker.PathToReference(game.GetPlayer() as ObjectReference, 0.500000)
	if PlayerPred
		akSpeaker.PlayIdle(IdleWarmHandsStanding)
	else
		game.GetPlayer().PlayIdle(IdleWarmHandsStanding)
	endIf
endFunction
