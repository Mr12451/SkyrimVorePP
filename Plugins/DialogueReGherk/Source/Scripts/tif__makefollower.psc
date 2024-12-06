;/ Decompiled by Champollion V1.0.1
Source   : tif__makefollower.psc
Modified : 2024-06-26 19:53:23
Compiled : 2024-06-26 19:53:25
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__MakeFollower extends TopicInfo hidden

;-- Properties --------------------------------------
faction property PotentialFollowerFaction auto
favordialoguescript property pFDS auto
Bool property persuade auto
Bool property bribe auto
Bool property intimidate auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GotoState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	akSpeaker.AddToFaction(PotentialFollowerFaction)
	akSpeaker.AddToFaction(game.GetForm(369060) as faction)
	if akSpeaker.GetRelationshipRank(game.GetPlayer()) < 1
		akSpeaker.SetRelationshipRank(game.GetPlayer(), 2)
	endIf
	if persuade
		pFDS.persuade(akSpeaker)
	elseIf bribe
		pFDS.bribe(akSpeaker)
	elseIf intimidate
		pFDS.intimidate(akSpeaker)
	endIf
endFunction

; Skipped compiler generated GetState
