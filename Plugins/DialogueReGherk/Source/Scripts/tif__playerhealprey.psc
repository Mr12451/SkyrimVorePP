;/ Decompiled by Champollion V1.0.1
Source   : TIF__PlayerHealPrey.psc
Modified : 2024-07-11 18:40:47
Compiled : 2024-07-11 18:40:48
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__PlayerHealPrey extends TopicInfo hidden

;-- Properties --------------------------------------

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	game.GetPlayer().Damageav("Magicka", 45 as Float)
	akSpeaker.Restoreav("Health", (akSpeaker.GetBaseAV("Health") as Int / 3) as Float)
endFunction

; Skipped compiler generated GotoState

; Skipped compiler generated GetState
