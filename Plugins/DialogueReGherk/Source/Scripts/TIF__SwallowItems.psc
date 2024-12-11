ScriptName TIF__SwallowItems extends TopicInfo hidden


Spell property EatThis auto


function Fragment_0(ObjectReference akSpeakerRef)
    actor akSpeaker = akSpeakerRef as actor
    if akSpeaker
        SVPP_API.InventoryVore(akSpeaker)
    endIf
endFunction
