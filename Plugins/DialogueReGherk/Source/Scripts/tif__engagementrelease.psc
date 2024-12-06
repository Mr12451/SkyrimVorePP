;/ Decompiled by Champollion V1.0.1
Source   : TIF__EngagementRelease.psc
Modified : 2023-08-15 17:45:50
Compiled : 2023-08-15 17:45:57
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__EngagementRelease extends DialogueVore hidden

;-- Properties --------------------------------------
Bool property PlayerPred auto
faction property MarriageAskedFaction auto
quest property RelationshipMarriage auto
referencealias property LoveInterest auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GetState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	if PlayerPred == true
		self.Escape(game.GetPlayer(), self.Resolve(akSpeakerRef), true)
	else
		self.Escape(self.Resolve(akSpeakerRef), game.GetPlayer(), true)
	endIf
	akSpeaker.AddtoFaction(MarriageAskedFaction)
	LoveInterest.ForceRefTo(akSpeaker as ObjectReference)
	RelationshipMarriage.SetStage(20)
endFunction

; Skipped compiler generated GotoState
