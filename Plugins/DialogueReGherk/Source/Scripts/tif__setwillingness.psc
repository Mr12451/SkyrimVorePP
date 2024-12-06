;/ Decompiled by Champollion V1.0.1
Source   : TIF__SetWillingness.psc
Modified : 2023-08-01 09:38:33
Compiled : 2023-08-01 09:38:35
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__SetWillingness extends TopicInfo hidden

;-- Properties --------------------------------------
faction property DialogueEndoPred auto
favordialoguescript property pFDS auto
Bool property PlayerPred auto
Bool property Persuade auto
Bool property Intimidate auto
Bool property Bribe auto
Bool property Endo auto
faction property DialogueVorePred auto
Bool property Vore auto
faction property DialogueVorePrey auto
faction property DialogueEndoPrey auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GetState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	if PlayerPred == true
		if Endo == true
            SVPP_API.Consent(game.GetPlayer(), akSpeaker, true, false)
		else
            SVPP_API.Consent(game.GetPlayer(), akSpeaker, false, false)
		endIf
		if Vore == true
            SVPP_API.Consent(game.GetPlayer(), akSpeaker, true, true)
		else
            SVPP_API.Consent(game.GetPlayer(), akSpeaker, false, true)
		endIf
	else
		if Endo == true
            SVPP_API.Consent(akSpeaker, game.GetPlayer(), true, false)
		else
            SVPP_API.Consent(akSpeaker, game.GetPlayer(), false, false)
		endIf
		if Vore == true
            SVPP_API.Consent(akSpeaker, game.GetPlayer(), true, true)
		else
            SVPP_API.Consent(akSpeaker, game.GetPlayer(), false, true)
		endIf
	endIf
	if Persuade == true
		pFDS.Persuade(akSpeaker)
	elseIf Bribe == true
		pFDS.Bribe(akSpeaker)
	elseIf Intimidate == true
		pFDS.Intimidate(akSpeaker)
	endIf
endFunction

; Skipped compiler generated GotoState
