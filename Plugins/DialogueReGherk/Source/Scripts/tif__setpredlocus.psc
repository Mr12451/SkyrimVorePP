;/ Decompiled by Champollion V1.0.1
Source   : TIF__SetPredLocus.psc
Modified : 2023-07-28 09:29:32
Compiled : 2023-07-28 09:29:32
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__SetPredLocus extends TopicInfo hidden

;-- Properties --------------------------------------
faction property DialogueLocusPrey auto
Int property Locus auto
Bool property Pred auto
faction property DialogueLocusPred auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	if Pred == true
		akSpeaker.SetFactionRank(DialogueLocusPrey, Locus)
	else
		akSpeaker.SetFactionRank(DialogueLocusPred, Locus)
	endIf
endFunction

; Skipped compiler generated GotoState

; Skipped compiler generated GetState
