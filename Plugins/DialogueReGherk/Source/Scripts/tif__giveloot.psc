;/ Decompiled by Champollion V1.0.1
Source   : TIF__GiveLoot.psc
Modified : 2023-10-09 22:19:11
Compiled : 2023-10-09 22:19:11
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__GiveLoot extends TopicInfo hidden

;-- Properties --------------------------------------
leveleditem property LootBanditChestBossBase auto
Bool property persuade auto
favordialoguescript property pFDS auto
Bool property intimidate auto
Bool property bribe auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GotoState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	akSpeaker.additem(LootBanditChestBossBase as form, 1, true)
	if persuade
		pFDS.persuade(akSpeaker)
	elseIf bribe
		pFDS.bribe(akSpeaker)
	elseIf intimidate
		pFDS.intimidate(akSpeaker)
	endIf
	utility.wait(2 as Float)
	akSpeaker.OpenInventory(true)
endFunction

; Skipped compiler generated GetState
