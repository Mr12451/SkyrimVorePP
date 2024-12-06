;/ Decompiled by Champollion V1.0.1
Source   : tif__rdcrunch.psc
Modified : 2024-07-26 21:43:37
Compiled : 2024-07-26 21:43:43
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__RDCrunch extends TopicInfo hidden

;-- Properties --------------------------------------
devourmentmanager property Manager auto
Bool property PlayerPrey auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GetState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	if PlayerPrey == true
        game.GetPlayer().Damageav("Health", game.GetPlayer().GetBaseAV("Health") / 4.50000)
	else
		akSpeaker.Damageav("Health", (akSpeakerRef as actor).GetBaseAV("Health") / 4.50000)
	endIf
endFunction

; Skipped compiler generated GotoState
