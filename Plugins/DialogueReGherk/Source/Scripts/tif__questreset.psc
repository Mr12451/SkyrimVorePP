;/ Decompiled by Champollion V1.0.1
Source   : TIF__QuestReset.psc
Modified : 2023-10-28 11:09:26
Compiled : 2023-10-28 11:09:28
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__QuestReset extends TopicInfo hidden

;-- Properties --------------------------------------
quest property QuestToReset auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GotoState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	QuestToReset.Stop()
	QuestToReset.Start()
endFunction

; Skipped compiler generated GetState
