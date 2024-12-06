;/ Decompiled by Champollion V1.0.1
Source   : TIF__SetGlobalValue.psc
Modified : 2023-10-27 23:01:49
Compiled : 2023-10-27 23:01:51
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__SetGlobalValue extends TopicInfo hidden

;-- Properties --------------------------------------
globalvariable property GlobalToSet auto
Int property ValueToSet auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	GlobalToSet.SetValue(ValueToSet as Float)
endFunction

; Skipped compiler generated GetState

; Skipped compiler generated GotoState
