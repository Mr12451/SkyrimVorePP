;/ Decompiled by Champollion V1.0.1
Source   : TIF__ClearAlias.psc
Modified : 2024-07-01 10:03:13
Compiled : 2024-07-01 10:03:14
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__ClearAlias extends TopicInfo hidden

;-- Properties --------------------------------------
referencealias property AliasToClear auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GetState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	AliasToClear.Clear()
endFunction

; Skipped compiler generated GotoState
