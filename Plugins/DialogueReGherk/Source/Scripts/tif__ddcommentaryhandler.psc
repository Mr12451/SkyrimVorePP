;/ Decompiled by Champollion V1.0.1
Source   : TIF__DDCommentaryHandler.psc
Modified : 2024-07-12 17:16:29
Compiled : 2024-07-12 17:16:30
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__DDCommentaryHandler extends TopicInfo hidden

;-- Properties --------------------------------------
spell property DevourmentStatus_SwallowedByPlayer auto
faction property DialogueTraineeWon auto
faction property DialogueAtePlayerBeforeWilling auto
faction property DialogueTraineeQuit auto
faction property DialogueTrainerWon auto
faction property DialogueTrainerQuit auto
faction property DialogueReformedPlayerBefore auto
faction property DialogueTrainerFailed auto
faction property DialogueTraineeFailed auto
faction property DialogueEatenWillingBefore auto
spell property DevourmentStatus_ReformedThePlayer auto
spell property DevourmentStatus_ReformedByPlayer auto
faction property DialogueCurrentTrainee auto
spell property DLC2BlackBookCompanionsSafeSpell auto
faction property DialogueCurrentTrainer auto
faction property DialogueEndoPrey auto
faction property DialogueReformedUnwillingBefore auto
faction property DialogueVorePred auto
faction property DialogueReformedWillingBefore auto
faction property DialogueVorePrey auto
spell property DevourmentStatus_DigestedThePlayer auto
spell property DevourmentStatus_SwallowedThePlayer auto
faction property DialogueAtePlayerBeforeUnwilling auto
faction property DialogueEatenUnwillingBefore auto
faction property DialogueEndoPred auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GetState

; Skipped compiler generated GotoState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	if akSpeaker.HasSpell(DevourmentStatus_DigestedThePlayer as form)
		if akSpeaker.HasSpell(DevourmentStatus_ReformedThePlayer as form)
			akSpeaker.AddToFaction(DialogueReformedPlayerBefore)
			akSpeaker.AddToFaction(DialogueAtePlayerBeforeWilling)
		elseIf akSpeaker.IsInFaction(DialogueVorePred) || akSpeaker.IsInFaction(DialogueEndoPred) || akSpeaker.IsInFaction(DialogueCurrentTrainer)
			akSpeaker.AddToFaction(DialogueAtePlayerBeforeWilling)
		else
			akSpeaker.AddToFaction(DialogueAtePlayerBeforeUnwilling)
		endIf
		if akSpeaker.IsInFaction(DialogueCurrentTrainer)
			akSpeaker.RemoveFromFaction(DialogueCurrentTrainer)
			akSpeaker.AddToFaction(DialogueTrainerFailed)
			game.GetPlayer().ModAv("Health", game.GetPlayer().GetAV("Health") / 25 as Float)
		endIf
	endIf
	if akSpeaker.HasSpell(DevourmentStatus_SwallowedThePlayer as form)
		if akSpeaker.IsInFaction(DialogueVorePred) || akSpeaker.IsInFaction(DialogueEndoPred) || akSpeaker.IsInFaction(DialogueCurrentTrainer)
			akSpeaker.AddToFaction(DialogueAtePlayerBeforeWilling)
		else
			akSpeaker.AddToFaction(DialogueAtePlayerBeforeUnwilling)
		endIf
		if akSpeaker.IsInFaction(DialogueCurrentTrainer) && !akSpeaker.HasSpell(DevourmentStatus_DigestedThePlayer as form)
			akSpeaker.AddToFaction(DialogueTrainerWon)
			akSpeaker.RemoveFromFaction(DialogueCurrentTrainer)
			game.GetPlayer().ModAv("Health", game.GetPlayer().GetAV("Health") / 16 as Float)
		endIf
	endIf
	if akSpeaker.HasSpell(DevourmentStatus_SwallowedByPlayer as form)
		if akSpeaker.IsInFaction(DialogueVorePrey) || akSpeaker.IsInFaction(DialogueEndoPrey) || akSpeaker.IsInFaction(DialogueCurrentTrainee)
			akSpeaker.AddToFaction(DialogueEatenWillingBefore)
		else
			akSpeaker.SetRelationshipRank(game.GetPlayer(), akSpeaker.getrelationshiprank(game.GetPlayer()) - 1)
			akSpeaker.AddToFaction(DialogueEatenUnwillingBefore)
		endIf
		if akSpeaker.IsInFaction(DialogueCurrentTrainee) && !akSpeaker.HasSpell(DevourmentStatus_ReformedByPlayer as form)
			akSpeaker.AddToFaction(DialogueTraineeWon)
			akSpeaker.RemoveFromFaction(DialogueCurrentTrainee)
			akSpeaker.ModAv("Health", game.GetPlayer().GetAV("Health") / 16 as Float)
		elseIf akSpeaker.IsInFaction(DialogueCurrentTrainee)
			akSpeaker.AddToFaction(DialogueTraineeFailed)
			akSpeaker.RemoveFromFaction(DialogueCurrentTrainee)
			akSpeaker.ModAv("Health", game.GetPlayer().GetAV("Health") / 25 as Float)
		endIf
	endIf
	if akSpeaker.HasSpell(DevourmentStatus_ReformedByPlayer as form) || akSpeaker.HasSpell(DLC2BlackBookCompanionsSafeSpell as form)
		if !akSpeaker.HasSpell(DevourmentStatus_SwallowedByPlayer as form) || akSpeaker.IsInFaction(DialogueVorePrey)
			akSpeaker.AddToFaction(DialogueReformedWillingBefore)
		else
			akSpeaker.SetRelationshipRank(game.GetPlayer(), akSpeaker.getrelationshiprank(game.GetPlayer()) - 1)
			akSpeaker.AddToFaction(DialogueReformedUnwillingBefore)
		endIf
		if akSpeaker.IsInFaction(DialogueCurrentTrainee) && !akSpeaker.HasSpell(DevourmentStatus_ReformedByPlayer as form)
			akSpeaker.AddToFaction(DialogueTraineeWon)
			akSpeaker.RemoveFromFaction(DialogueCurrentTrainee)
			akSpeaker.ModAv("Health", game.GetPlayer().GetAV("Health") / 16 as Float)
		elseIf akSpeaker.IsInFaction(DialogueCurrentTrainee)
			akSpeaker.AddToFaction(DialogueTraineeFailed)
			akSpeaker.RemoveFromFaction(DialogueCurrentTrainee)
			akSpeaker.ModAv("Health", game.GetPlayer().GetAV("Health") / 25 as Float)
		endIf
	endIf
	akSpeaker.RemoveSpell(DevourmentStatus_DigestedThePlayer)
	akSpeaker.RemoveSpell(DevourmentStatus_ReformedByPlayer)
	akSpeaker.RemoveSpell(DevourmentStatus_ReformedThePlayer)
	akSpeaker.RemoveSpell(DevourmentStatus_SwallowedByPlayer)
	akSpeaker.RemoveSpell(DevourmentStatus_SwallowedThePlayer)
	akSpeaker.RemoveSpell(DLC2BlackBookCompanionsSafeSpell)
endFunction
