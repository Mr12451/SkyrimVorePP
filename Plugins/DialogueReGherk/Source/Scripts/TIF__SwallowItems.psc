ScriptName TIF__SwallowItems extends TopicInfo hidden


Spell property EatThis auto


function Fragment_0(ObjectReference akSpeakerRef)
	EatThis.cast(Game.GetPlayer(), akSpeakerRef as Actor)
endFunction
