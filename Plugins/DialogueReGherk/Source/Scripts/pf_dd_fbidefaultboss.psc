;/ Decompiled by Champollion V1.0.1
Source   : PF_DD_FBIDefaultBoss.psc
Modified : 2023-12-08 10:28:30
Compiled : 2023-12-08 10:28:31
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName PF_DD_FBIDefaultBoss extends Package hidden

;-- Properties --------------------------------------
quest property DevourmentDialogueFBI auto
referencealias property FBIBoss auto
referencealias property FBICenterMarker auto
referencealias property FBIChest auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GotoState

function Fragment_0(Actor akActor)

	DevourmentDialogueFBI.Stop()
	akActor.Modav("Speedmult", -50 as Float)
endFunction

; Skipped compiler generated GetState

function Fragment_1(Actor akActor)

	objectreference GoalRef
	debug.MessageBox("Package started!")
	akActor.Modav("Speedmult", 50 as Float)
	if FBIChest.GetRef()
		GoalRef = FBIChest.GetRef()
	elseIf FBIBoss.GetRef()
		GoalRef = FBIBoss.GetRef()
	elseIf FBICenterMarker.GetRef()
		GoalRef = FBICenterMarker.GetRef()
	else
		debug.MessageBox("No goal!")
		DevourmentDialogueFBI.Stop()
	endIf
	Bool Success = false
	Int TryAgain = 1
	while TryAgain == 1
		Success = akActor.PathToReference(GoalRef, 1 as Float)
		if Success == true
			
		elseIf akActor.GetCombatState() == 1
			while akActor.GetCombatState() == 1
				utility.Wait(3 as Float)
			endWhile
			Success = akActor.PathToReference(GoalRef, 1 as Float)
		endIf
		if Success == true
			TryAgain == 0
		else
			debug.Notification("Trying again.")
		endIf
	endWhile
	debug.MessageBox("Could not find path to goal!")
	DevourmentDialogueFBI.Stop()
endFunction
