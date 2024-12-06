;/ Decompiled by Champollion V1.0.1
Source   : TIF__InstantDigest.psc
Modified : 2023-10-11 23:46:47
Compiled : 2023-10-11 23:46:52
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__InstantDigest extends TopicInfo hidden

;-- Properties --------------------------------------
devourmentmanager property Manager auto
Bool property PlayerPred auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GotoState

; Skipped compiler generated GetState

function Fragment_0(ObjectReference akSpeakerRef)

	actor akSpeaker = akSpeakerRef as actor
	if PlayerPred
		;Int Preydata = Manager.getpreydata(akSpeaker as ObjectReference)
		;Manager.FinishLiveDigestion(game.GetPlayer(), akSpeaker, Preydata)
        SVPP_API.KillPrey(akSpeaker)
	else
		;Int preydata = Manager.getpreydata(game.GetPlayer() as ObjectReference)
		;Manager.FinishLiveDigestion(akSpeaker, game.GetPlayer(), preydata)
        SVPP_API.KillPrey(game.GetPlayer() as actor)
	endIf
endFunction
