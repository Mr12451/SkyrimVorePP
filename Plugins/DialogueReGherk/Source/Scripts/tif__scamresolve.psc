;/ Decompiled by Champollion V1.0.1
Source   : TIF__ScamResolve.psc
Modified : 2024-01-27 09:19:16
Compiled : 2024-01-27 09:19:18
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__ScamResolve extends TopicInfo hidden

;-- Properties --------------------------------------
faction property DialogueSold auto
miscobject property Gold001 auto
faction property DialogueScamAgreed auto
Bool property Paying auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GotoState

; Skipped compiler generated GetState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	akSpeaker.RemoveFromFaction(DialogueSold)
	akSpeaker.RemoveFromFaction(DialogueScamAgreed)
	if Paying == true
		game.GetPlayer().RemoveItem(Gold001 as form, akSpeaker.GetLevel() * 25, false, akSpeaker as ObjectReference)
	else
		akSpeaker.SetRelationshipRank(game.GetPlayer(), akSpeaker.GetRelationshipRank(game.GetPlayer()) - 2)
	endIf
endFunction
