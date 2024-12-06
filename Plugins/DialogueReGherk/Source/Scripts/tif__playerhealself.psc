;/ Decompiled by Champollion V1.0.1
Source   : TIF__PlayerHealSelf.psc
Modified : 2024-07-26 18:51:52
Compiled : 2024-07-26 18:51:53
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__PlayerHealSelf extends TopicInfo hidden

;-- Properties --------------------------------------

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	game.GetPlayer().Damageav("Magicka", 45 as Float)
	game.GetPlayer().Restoreav("Health", (game.GetPlayer().GetBaseAV("Health") as Int / 3) as Float)
endFunction

; Skipped compiler generated GotoState

; Skipped compiler generated GetState
