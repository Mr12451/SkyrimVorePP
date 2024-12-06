;/ Decompiled by Champollion V1.0.1
Source   : TIF__DDSetScenePredPrey.psc
Modified : 2023-10-05 22:07:06
Compiled : 2023-10-05 22:07:07
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__DDSetScenePredPrey extends TopicInfo hidden

;-- Properties --------------------------------------
referencealias property AliasToSelect auto
referencealias property DDSelectedPrey auto
Bool property ThirdAsPred auto
referencealias property DDSelectedPred auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GetState

; Skipped compiler generated GotoState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	if ThirdAsPred == true
		DDSelectedPred.ForceRefTo(AliasToSelect.GetReference())
		DDSelectedPrey.ForceRefTo(akSpeaker as ObjectReference)
	elseIf ThirdAsPred == false
		DDSelectedPrey.ForceRefTo(AliasToSelect.GetReference())
		DDSelectedPred.ForceRefTo(akSpeaker as ObjectReference)
	endIf
endFunction
