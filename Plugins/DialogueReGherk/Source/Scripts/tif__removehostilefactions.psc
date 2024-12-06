;/ Decompiled by Champollion V1.0.1
Source   : tif__removehostilefactions.psc
Modified : 2024-01-24 13:34:42
Compiled : 2024-01-24 13:34:42
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__RemoveHostileFactions extends TopicInfo hidden

;-- Properties --------------------------------------
faction property NecromancerFaction auto
faction property PlayerFaction auto
faction property ForswornFaction auto
faction property dunYngvildFaction auto
faction property DunBlackreachFalmerServantFaction auto
faction property WarlockFaction auto
faction property VampireFaction auto
faction property BanditFaction auto
faction property DunMistwatchFaction auto
faction property WIPlayerEnemyFaction auto
faction property MS07BanditFaction auto
faction property DraugrFaction auto
faction property DwarvenAutomatonFaction auto
faction property DialogueFormerHostile auto
faction property SilverHandFaction auto
faction property VampireThrallFaction auto
faction property HagravenFaction auto
faction property ThalmorFaction auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GetState

; Skipped compiler generated GotoState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	akSpeaker.RemoveFromFaction(BanditFaction)
	akSpeaker.RemoveFromFaction(WarlockFaction)
	akSpeaker.RemoveFromFaction(VampireFaction)
	akSpeaker.RemoveFromFaction(ForswornFaction)
	akSpeaker.RemoveFromFaction(NecromancerFaction)
	akSpeaker.RemoveFromFaction(dunYngvildFaction)
	akSpeaker.RemoveFromFaction(WIPlayerEnemyFaction)
	akSpeaker.RemoveFromFaction(VampireThrallFaction)
	akSpeaker.RemoveFromFaction(HagravenFaction)
	akSpeaker.RemoveFromFaction(MS07BanditFaction)
	akSpeaker.RemoveFromFaction(DunMistwatchFaction)
	akSpeaker.RemoveFromFaction(ThalmorFaction)
	akSpeaker.RemoveFromFaction(DwarvenAutomatonFaction)
	akSpeaker.RemoveFromFaction(SilverHandFaction)
	akSpeaker.RemoveFromFaction(DunBlackreachFalmerServantFaction)
	akSpeaker.RemoveFromFaction(DraugrFaction)
	akSpeaker.RemoveFromAllFactions()
	akSpeaker.SetActorValue("Aggression", 1 as Float)
	akSpeaker.SetActorValue("Confidence", 2 as Float)
	akSpeaker.AddToFaction(DialogueFormerHostile)
endFunction
