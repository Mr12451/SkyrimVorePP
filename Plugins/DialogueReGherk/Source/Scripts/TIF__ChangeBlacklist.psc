scriptName TIF__ChangeBlacklist extends TopicInfo hidden

DevourmentManager Property Manager Auto
bool property add auto

function Fragment_0(ObjectReference akSpeakerRef)
    faction PredatorBlacklistFaction = game.GetFormFromFile(3489, "Devourment.esp") as faction
	if add
		(akSpeakerRef as Actor).AddToFaction(PredatorBlacklistFaction)
	else
		(akSpeakerRef as Actor).RemoveFromFaction(PredatorBlacklistFaction)
	endIf
endFunction
