;/ Decompiled by Champollion V1.0.1
Source   : TIF__SleepRun.psc
Modified : 2023-11-24 08:46:38
Compiled : 2023-11-24 08:46:44
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__SleepRun extends TopicInfo hidden

;-- Properties --------------------------------------
globalvariable property DevourmentDialogue_SurpriseDigestions auto
globalvariable property DevourmentDialog_PlayerIsDead auto
Bool property DigestionRisk auto
devourmentdialog property DD auto
imagespacemodifier property FadeToBlack auto
survival_needexhaustion property SleepQuest auto
Bool property DigestionCertainty auto
furniture property DevourmentPsuedoBedroll auto
imagespacemodifier property FadeFromBlack auto
spell property DevourmentDialogue_Sleeping auto
globalvariable property GameHour auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GetState

; Skipped compiler generated GotoState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	game.GetPlayer().AddSpell(DevourmentDialogue_Sleeping, false)
	if DigestionRisk
		if utility.randomint(1, 100) as Float <= DevourmentDialogue_SurpriseDigestions.GetValue()
			DevourmentDialog_PlayerIsDead.SetValue(1 as Float)
		endIf
	endIf
	if DigestionCertainty as Bool && DevourmentDialogue_SurpriseDigestions.GetValue() <= 1 as Float
		DevourmentDialog_PlayerIsDead.SetValue(1 as Float)
	endIf
	while akSpeaker.IsInDialogueWithPlayer()
		utility.wait(1 as Float)
	endWhile
	FadeToBlack.ApplyCrossFade(1.00000)
	utility.wait(4 as Float)
	ObjectReference sneepytime = game.GetPlayer().PlaceAtMe(DevourmentPsuedoBedroll as form, 1, false, false)
	sneepytime.Activate(game.GetPlayer() as ObjectReference, false)
	utility.wait(4 as Float)
	sneepytime.Delete()
	FadeFromBlack.ApplyCrossFade(1.00000)
	utility.wait(2 as Float)
	DD.DoDialog_PlayerAndApex()
endFunction
