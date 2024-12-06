;/ Decompiled by Champollion V1.0.1
Source   : TIF__WimpOut.psc
Modified : 2024-07-26 21:16:47
Compiled : 2024-07-26 21:16:58
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__WimpOut extends DialogueVore hidden

;-- Properties --------------------------------------
faction property DialogueTraineeQuit auto
faction property DialogueEndoPrey auto
faction property DialogueCurrentTrainee auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	if applyToPred == true
		akSpeaker.RemoveFromFaction(DialogueCurrentTrainee)
		akSpeaker.AddToFaction(DialogueTraineeQuit)
		game.GetPlayer().ModAV("Health", game.GetPlayer().GetAV("Health") / 50 as Float)

        SVPP_API.Consent(akSpeaker, game.GetPlayer(), true, true)

		self.Escape(akSpeaker, game.GetPlayer(), true)
	else
		akSpeaker.RemoveFromFaction(DialogueCurrentTrainee)
		akSpeaker.AddToFaction(DialogueTraineeQuit)
		akSpeaker.ModAV("Health", akSpeaker.GetAV("Health") / 50 as Float)

        SVPP_API.Consent(game.GetPlayer(), akSpeaker, true, true)

		self.Escape(game.GetPlayer(), akSpeaker, true)
	endIf
endFunction

; Skipped compiler generated GetState

; Skipped compiler generated GotoState
