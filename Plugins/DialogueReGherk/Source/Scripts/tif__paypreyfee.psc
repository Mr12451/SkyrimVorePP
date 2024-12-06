;/ Decompiled by Champollion V1.0.1
Source   : TIF__PayPreyFee.psc
Modified : 2024-01-19 09:21:45
Compiled : 2024-01-19 09:21:46
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__PayPreyFee extends TopicInfo hidden

;-- Properties --------------------------------------
referencealias property DDSelectedPrey auto
miscobject property Gold auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GotoState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	game.GetPlayer().additem(Gold as form, (DDSelectedPrey.GetRef() as actor).GetLevel() * 50, false)
endFunction

; Skipped compiler generated GetState
