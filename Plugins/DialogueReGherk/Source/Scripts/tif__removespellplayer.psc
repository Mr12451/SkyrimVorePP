;/ Decompiled by Champollion V1.0.1
Source   : TIF__RemoveSpellPlayer.psc
Modified : 2023-10-12 09:54:28
Compiled : 2023-10-12 09:54:28
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__RemoveSpellPlayer extends TopicInfo hidden

;-- Properties --------------------------------------
spell property SpellToRemove auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	game.GetPlayer().RemoveSpell(SpellToRemove)
endFunction

; Skipped compiler generated GetState

; Skipped compiler generated GotoState
