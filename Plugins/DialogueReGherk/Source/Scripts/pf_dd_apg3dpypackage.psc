;/ Decompiled by Champollion V1.0.1
Source   : PF_DD_APG3DPyPackage.psc
Modified : 2023-10-05 19:51:48
Compiled : 2023-10-05 19:51:49
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName PF_DD_APG3DPyPackage extends Package hidden

;-- Properties --------------------------------------
spell property DevourmentFullnessEffects auto
referencealias property DD_SelectedPrey auto
spell property DevourmentPower_Vore auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

function Fragment_0(Actor akActor)

	; DevourmentPower_Vore.Cast(akActor as objectreference, DD_SelectedPrey.GetRef())
    SVPP_API.Swallow(akActor, DD_SelectedPrey.GetRef() as objectreference, -1.0, 2.0, false)
	utility.wait(3 as Float)
	if akActor.HasSpell(DevourmentFullnessEffects as form)
		akActor.StopCombatAlarm()
	endIf
endFunction

; Skipped compiler generated GotoState

; Skipped compiler generated GetState
