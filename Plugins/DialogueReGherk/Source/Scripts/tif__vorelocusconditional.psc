;/ Decompiled by Champollion V1.0.1
Source   : TIF__VoreLocusConditional.psc
Modified : 2023-12-05 10:12:00
Compiled : 2023-12-05 10:12:05
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__VoreLocusConditional extends TopicInfo hidden

;-- Properties --------------------------------------
Bool property Prey auto
faction property DialogueVorePrey auto
faction property DialogueVorePred auto
Bool property Consented auto
spell property DevourmentDialogue_ForceVore auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GetState

function Fragment_0(ObjectReference akSpeakerRef)
    actor akSpeaker = akSpeakerRef as actor
	; devourmentmanager manager = devourmentmanager.instance()
	if Prey == false
        if Consented
            SVPP_API.Consent(game.GetPlayer() as Actor, akSpeaker, true, true)
		endIf
        SVPP_API.Swallow(game.GetPlayer() as Actor, akSpeaker, -1.0, 2.0, false)
		;DevourmentDialogue_ForceEndo.Cast(game.GetPlayer() as ObjectReference, akSpeaker as ObjectReference)
	else
        if Consented
            SVPP_API.Consent(akSpeaker, game.GetPlayer() as Actor, true, true)
		endIf
        SVPP_API.Swallow(akSpeaker, game.GetPlayer() as Actor, -1.0, 2.0, false)
		; DevourmentDialogue_ForceEndo.Cast(akSpeaker as ObjectReference, game.GetPlayer() as ObjectReference)
	endIf
endFunction

; Skipped compiler generated GotoState
