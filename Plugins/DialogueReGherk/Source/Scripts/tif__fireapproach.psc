;/ Decompiled by Champollion V1.0.1
Source   : TIF__FireApproach.psc
Modified : 2024-07-01 09:14:07
Compiled : 2024-07-01 09:14:07
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__FireApproach extends TopicInfo hidden

;-- Properties --------------------------------------
scene property ApproachScene auto
referencealias property GreeterAlias auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GetState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	GreeterAlias.ForceRefTo(akSpeaker as ObjectReference)
	akSpeaker.EvaluatePackage()
endFunction

; Skipped compiler generated GotoState
