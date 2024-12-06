scriptName TIF__ChangeWhitelist extends TopicInfo hidden

DevourmentManager Property Manager Auto
bool property add auto

function Fragment_0(ObjectReference akSpeakerRef)
	faction PredatorWhitelistFaction = game.GetFormFromFile(3490, "Devourment.esp") as faction
	if add
		(akSpeakerRef as Actor).AddToFaction(PredatorWhitelistFaction)
	else
		(akSpeakerRef as Actor).RemoveFromFaction(PredatorWhitelistFaction)
	endIf
endFunction
