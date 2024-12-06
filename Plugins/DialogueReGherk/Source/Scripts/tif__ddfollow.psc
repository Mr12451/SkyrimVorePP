;/ Decompiled by Champollion V1.0.1
Source   : TIF__DDFollow.psc
Modified : 2023-11-09 22:36:32
Compiled : 2023-11-09 22:36:32
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__DDFollow extends TopicInfo hidden

;-- Properties --------------------------------------
quest property pDialogueFollower auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	(pDialogueFollower as dialoguefollowerscript).SetFollower(akSpeaker as ObjectReference)
endFunction

; Skipped compiler generated GetState

; Skipped compiler generated GotoState
