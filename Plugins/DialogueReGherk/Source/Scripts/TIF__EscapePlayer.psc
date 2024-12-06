ScriptName TIF__EscapePlayer extends DialogueVore hidden


bool property oralEscape = true auto


function Fragment_0(ObjectReference akSpeakerRef)
	Escape(Resolve(akSpeakerRef), Game.GetPlayer(), oralEscape)
endFunction


function Fragment_1(ObjectReference akSpeakerRef)
	Escape(Resolve(akSpeakerRef), Game.GetPlayer(), oralEscape)
endFunction
