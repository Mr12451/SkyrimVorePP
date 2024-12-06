;/ Decompiled by Champollion V1.0.1
Source   : PF_DD_APG3DPdPackage.psc
Modified : 2023-11-22 10:04:25
Compiled : 2023-11-22 10:04:25
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName PF_DD_APG3DPdPackage extends Package hidden

;-- Properties --------------------------------------
faction property DialogueVorePrey auto
spell property DevourmentPower_Vore auto
spell property DevourmentFullnessEffects auto
referencealias property DD_SelectedPred auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

function Fragment_0(Actor akActor)

	akActor.AddToFaction(DialogueVorePrey)
    SVPP_API.Swallow(DD_SelectedPred.GetRef() as Actor, akActor, -1.0, 2.0, false)
	; DevourmentPower_Vore.Cast(DD_SelectedPred.GetRef(), akActor as objectreference)
	utility.wait(2 as Float)
	akActor.StopCombatAlarm()
	(DD_SelectedPred.GetRef() as Actor).StopCombatAlarm()
endFunction

; Skipped compiler generated GetState

; Skipped compiler generated GotoState
