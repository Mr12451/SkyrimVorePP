;/ Decompiled by Champollion V1.0.1
Source   : TIF__EndoLocusConditional.psc
Modified : 2023-12-05 10:07:22
Compiled : 2023-12-05 10:07:27
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__EndoLocusConditional extends TopicInfo hidden

;-- Properties --------------------------------------
Bool property Prey auto
faction property DialogueEndoPred auto
faction property DialogueEndoPrey auto
spell property DevourmentDialogue_ForceEndo auto
Bool property Consented auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GetState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	; devourmentmanager manager = devourmentmanager.instance()
	if Prey == false

        if Consented
            SVPP_API.Consent(game.GetPlayer() as Actor, akSpeaker, true, false)
		endIf
        SVPP_API.Swallow(game.GetPlayer() as Actor, akSpeaker, -1.0, 1.0, false)
		;DevourmentDialogue_ForceEndo.Cast(game.GetPlayer() as ObjectReference, akSpeaker as ObjectReference)
	else
		if Consented
            SVPP_API.Consent(akSpeaker, game.GetPlayer() as Actor, true, false)
		endIf
        SVPP_API.Swallow(akSpeaker, game.GetPlayer() as Actor, -1.0, 1.0, false)
		; DevourmentDialogue_ForceEndo.Cast(akSpeaker as ObjectReference, game.GetPlayer() as ObjectReference)
	endIf
endFunction

; Skipped compiler generated GotoState
