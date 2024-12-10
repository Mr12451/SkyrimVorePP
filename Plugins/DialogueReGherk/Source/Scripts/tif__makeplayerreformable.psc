;/ Decompiled by Champollion V1.0.1
Source   : TIF__MakePlayerReformable.psc
Modified : 2023-10-14 11:14:24
Compiled : 2023-10-14 11:14:29
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__MakePlayerReformable extends TopicInfo hidden

;-- Properties --------------------------------------
perk property DevourmentPerk_Phylactery auto
Bool property Endo auto
Bool property persuade auto
Bool property Vore auto
favordialoguescript property pFDS auto
Bool property bribe auto
Bool property intimidate auto
faction property DialogueEndoPred auto
faction property DialogueVorePred auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GetState

; Skipped compiler generated GotoState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	; game.GetPlayer().AddPerk(DevourmentPerk_Phylactery)
	; devourmentreformationquest.instance().AddReformationHost(akSpeakerRef as actor)
    SVPP_API.PlanReformation(akSpeaker, game.GetPlayer())

	if bribe
		pFDS.bribe(akSpeaker)
	elseIf intimidate
		pFDS.intimidate(akSpeaker)
	elseIf persuade
		pFDS.persuade(akSpeaker)
	endIf


	if Vore
        SVPP_API.Consent(akSpeaker, game.GetPlayer(), true, true)
	elseIf Endo
        SVPP_API.Consent(akSpeaker, game.GetPlayer(), false, true)
        SVPP_API.Consent(akSpeaker, game.GetPlayer(), true, false)
	else
        SVPP_API.Consent(akSpeaker, game.GetPlayer(), false, true)
        SVPP_API.Consent(akSpeaker, game.GetPlayer(), false, false)
	endIf
endFunction
