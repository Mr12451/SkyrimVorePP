;/ Decompiled by Champollion V1.0.1
Source   : PF_DD_BTSTravelPackage.psc
Modified : 2023-10-31 09:41:45
Compiled : 2023-10-31 09:41:46
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName PF_DD_BTSTravelPackage extends Package hidden

;-- Properties --------------------------------------
faction property DialogueBTSArrivedFaction auto
perk property DevourmentPerk_StrongBack auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

function Fragment_0(Actor akActor)

	akActor.AddToFaction(DialogueBTSArrivedFaction)
	akActor.RemovePerk(DevourmentPerk_StrongBack)
endFunction

function Fragment_1(Actor akActor)

	akActor.AddPerk(DevourmentPerk_StrongBack)
endFunction

; Skipped compiler generated GetState

; Skipped compiler generated GotoState
