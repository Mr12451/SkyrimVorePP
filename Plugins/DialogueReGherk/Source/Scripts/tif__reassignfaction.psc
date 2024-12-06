;/ Decompiled by Champollion V1.0.1
Source   : TIF__ReassignFaction.psc
Modified : 2024-02-02 09:21:32
Compiled : 2024-02-02 09:21:35
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__ReassignFaction extends TopicInfo hidden

;-- Properties --------------------------------------
weapon property FollowerHuntingBow auto
referencealias property DDSelectedPrey auto
quest property pDialogueFollower auto
faction property FactionToAdd auto
faction property pDismissedFollower auto
sethirelingrehire property HirelingRehireScript auto
ammo property FollowerIronArrow auto
faction property factionToRemove auto
faction property pCurrentHireling auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GetState

; Skipped compiler generated GotoState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	akSpeaker.RemoveFromFaction(factionToRemove)
	akSpeaker.AddToFaction(FactionToAdd)
	quest nwsFF = game.GetFormFromFile(17231, "nwsFollowerFramework.esp") as quest
	(nwsFF as nwsfollowercontrollerscript).RemoveFollower(akSpeaker, 0, 0)
endFunction
