;/ Decompiled by Champollion V1.0.1
Source   : PF_DDAP_GuardEatPrisonerPack_Script.psc
Modified : 2024-01-26 08:26:45
Compiled : 2024-01-26 08:26:46
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName PF_DDAP_GuardEatPrisonerPack_Script extends Package hidden

;-- Properties --------------------------------------
quest property pDialogueFollower auto
faction property DismissedFollowerFaction auto
spell property DevourmentDialogue_ForceVore auto
referencealias property DDSelectedPrey auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GetState

; Skipped compiler generated GotoState

function Fragment_0(Actor akActor)

	;DevourmentDialogue_ForceVore.Cast(akActor as objectreference, (DDSelectedPrey.GetRef() as Actor) as objectreference)

    SVPP_API.Swallow(akActor, (DDSelectedPrey.GetRef() as Actor) as objectreference, -1.0, 2.0, false)
	akActor.GetCrimeFaction().SetCrimeGold(0)
	akActor.GetCrimeFaction().SetCrimeGoldViolent(0)
endFunction
