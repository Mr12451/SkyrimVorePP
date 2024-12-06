;/ Decompiled by Champollion V1.0.1
Source   : TIF__DDDismissFollower.psc
Modified : 2023-11-09 22:29:11
Compiled : 2023-11-09 22:29:12
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__DDDismissFollower extends TopicInfo hidden

;-- Properties --------------------------------------
quest property pDialogueFollower auto
faction property DismissedFollowerFaction auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	if !akSpeaker.IsInFaction(DismissedFollowerFaction)
		(pDialogueFollower as dialoguefollowerscript).DismissFollower(0, 0)
	endIf
endFunction

; Skipped compiler generated GotoState

; Skipped compiler generated GetState
