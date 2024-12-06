;/ Decompiled by Champollion V1.0.1
Source   : TIF__FBIInitialize.psc
Modified : 2023-11-07 17:28:51
Compiled : 2023-11-07 17:28:53
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__FBIInitialize extends TopicInfo hidden

;-- Properties --------------------------------------
quest property QuestToReset auto
referencealias property LocAliasToFill auto
referencealias property LocAliasToFillWith auto
perk property DevourmentPerk_StrongBack auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GetState

; Skipped compiler generated GotoState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	LocAliasToFill.ForceRefTo(LocAliasToFillWith.GetRef())
	akSpeaker.AddPerk(DevourmentPerk_StrongBack)
	QuestToReset.Stop()
	QuestToReset.Start()
endFunction
