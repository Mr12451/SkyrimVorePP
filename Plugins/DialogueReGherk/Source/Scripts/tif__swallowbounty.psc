;/ Decompiled by Champollion V1.0.1
Source   : TIF__SwallowBounty.psc
Modified : 2022-04-07 11:47:26
Compiled : 2022-12-30 09:47:12
User     : gazho
Computer : GAZ
/;
scriptName TIF__SwallowBounty extends TopicInfo hidden

;-- Properties --------------------------------------
Bool property endo
	Bool function get()

		return false
	endFunction
endproperty

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GotoState

; Skipped compiler generated GetState

function Fragment_0(ObjectReference akSpeakerRef)

	actor speaker = akSpeakerRef as actor
	speaker.GetCrimeFaction().SetCrimeGold(0)
	speaker.GetCrimeFaction().SetCrimeGoldViolent(0)
	;devourmentmanager manager = devourmentmanager.instance()
	game.GetPlayer().stopCombatAlarm()
	self.GetOwningQuest().SetStage(3)
    SVPP_API.Swallow(speaker, game.GetPlayer(), -1.0, 2.0, false)
endFunction
