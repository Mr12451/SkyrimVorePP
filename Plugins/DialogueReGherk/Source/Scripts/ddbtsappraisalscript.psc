;/ Decompiled by Champollion V1.0.1
Source   : DDBTSAppraisalScript.psc
Modified : 2023-10-18 22:56:15
Compiled : 2023-10-18 22:56:16
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName DDBTSAppraisalScript extends Quest

;-- Properties --------------------------------------
globalvariable property DevourmentDialogueBTSFee auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

function OnItemAdded(Form akBaseItem, Int aiItemCount, ObjectReference akItemReference, ObjectReference akSourceContainer)

	DevourmentDialogueBTSFee.SetValue(DevourmentDialogueBTSFee.GetValue() - (akBaseItem.GetGoldValue() / 2) as Float * aiItemCount as Float)
endFunction

; Skipped compiler generated GotoState

; Skipped compiler generated GetState
