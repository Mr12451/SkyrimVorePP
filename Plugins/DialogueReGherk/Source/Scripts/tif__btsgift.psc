;/ Decompiled by Champollion V1.0.1
Source   : TIF__BTSGift.psc
Modified : 2023-10-18 23:11:08
Compiled : 2023-10-18 23:11:09
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__BTSGift extends TopicInfo hidden

;-- Properties --------------------------------------
globalvariable property DevourmentDialogueBTSFee auto
referencealias property BTSAppraiserAlias auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GetState

; Skipped compiler generated GotoState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	DevourmentDialogueBTSFee.SetValue(akSpeaker.GetBribeamount() as Float)
	BTSAppraiserAlias.ForceRefTo(akSpeaker as ObjectReference)
	akSpeaker.ShowGiftMenu(true, none, false, true)
endFunction
