;/ Decompiled by Champollion V1.0.1
Source   : TIF__MakeTrainable.psc
Modified : 2024-07-02 10:52:21
Compiled : 2024-07-02 10:52:22
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__MakeTrainable extends TopicInfo hidden

;-- Properties --------------------------------------
faction property DialogueCurrentTrainee auto
faction property DialogueVorePred auto
faction property DialogueCurrentTrainer auto
Bool property PlayerPred auto
faction property DialogueVorePrey auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GotoState

; Skipped compiler generated GetState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	if PlayerPred
		SVPP_API.Consent(none, akSpeaker, false, true)
		SVPP_API.Consent(none, akSpeaker, false, false)
		akSpeaker.AddToFaction(DialogueCurrentTrainee)
	else
		SVPP_API.Consent(none, akSpeaker, false, true)
		SVPP_API.Consent(none, akSpeaker, false, false)
		akSpeaker.AddToFaction(DialogueCurrentTrainer)
	endIf
endFunction
