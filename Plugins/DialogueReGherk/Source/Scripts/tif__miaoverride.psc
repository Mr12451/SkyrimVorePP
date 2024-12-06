;/ Decompiled by Champollion V1.0.1
Source   : TIF__MiaOverride.psc
Modified : 2022-04-07 11:47:26
Compiled : 2022-12-30 09:47:10
User     : gazho
Computer : GAZ
/;
scriptName TIF__MiaOverride extends TopicInfo hidden

;-- Properties --------------------------------------
Bool property Consented = false auto
Int property Override = -1 auto
Bool property NoEscape = false auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

function Fragment_0(ObjectReference akSpeakerRef)

	self.Override(akSpeakerRef as actor)
endFunction

; Skipped compiler generated GotoState

function Override(actor target)

	; devourmentmanager manager = devourmentmanager.instance()
	if Override >= 0
		faction StrangerFaction = game.GetFormFromFile(3328, "Devourment.esp") as faction
		target.SetFactionRank(StrangerFaction, Override)
	endIf

    if Consented
        SVPP_API.Consent(none, target, true, true)
    endIf
    if NoEscape
        SVPP_API.DisableEscape(target)
    endIf
endFunction

; Skipped compiler generated GetState

function Fragment_1(ObjectReference akSpeakerRef)

	self.Override(akSpeakerRef as actor)
endFunction
