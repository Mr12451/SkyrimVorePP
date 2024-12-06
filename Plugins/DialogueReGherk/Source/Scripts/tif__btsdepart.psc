;/ Decompiled by Champollion V1.0.1
Source   : TIF__BTSDepart.psc
Modified : 2023-10-28 11:27:06
Compiled : 2023-10-28 11:27:11
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__BTSDepart extends TopicInfo hidden

;-- Properties --------------------------------------
Bool property persuade auto
Bool property intimidate auto
devourmentplayeralias property DPA auto
Bool property Paid auto
favordialoguescript property pFDS auto
faction property DialogueBTSArrivedFaction auto
referencealias property BTSTaxiAlias auto
Bool property bribe auto
globalvariable property DevourmentDialogueBTSPaid auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GetState

; Skipped compiler generated GotoState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	utility.wait(3 as Float)
	BTSTaxiAlias.ForceRefTo(akSpeaker as ObjectReference)
	if Paid
		DevourmentDialogueBTSPaid.SetValue(1 as Float)
	else
		DevourmentDialogueBTSPaid.SetValue(0 as Float)
	endIf
	if persuade
		pFDS.persuade(akSpeaker)
	elseIf bribe
		pFDS.bribe(akSpeaker)
	elseIf intimidate
		pFDS.intimidate(akSpeaker)
	endIf
    
    SVPP_API.SetPredControl(false)

	if akSpeaker.IsInFaction(DialogueBTSArrivedFaction)
		akSpeaker.RemoveFromFaction(DialogueBTSArrivedFaction)
		BTSTaxiAlias.Clear()
	endIf
endFunction
