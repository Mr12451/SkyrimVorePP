;/ Decompiled by Champollion V1.0.1
Source   : TIF__EscapePrey.psc
Modified : 2023-10-10 22:04:22
Compiled : 2023-10-10 22:04:27
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName TIF__EscapePrey extends TopicInfo hidden

;-- Properties --------------------------------------
Bool property persuade = false auto
favordialoguescript property Generic auto
Bool property intimidate = false auto
Bool property bribe = false auto
Bool property oralEscape = true auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

function escape(actor target)

	if Generic
		if persuade
			Generic.persuade(target)
		elseIf bribe
			Generic.bribe(target)
		elseIf intimidate
			Generic.intimidate(target)
		endIf
	endIf
	; devourmentmanager manager = devourmentmanager.instance()
	; form[] stomach = manager.GetStomachArray(target)
	; if !stomach || !stomach.length || !stomach[0]
	; 	return 
	; endIf
	; Int preyIndex = stomach.length
	; while preyIndex
	; 	preyIndex -= 1
	; 	actor prey = stomach[preyIndex] as actor
	; 	if prey as Bool && !prey.isdead()
	; 		manager.ForceEscape(prey as form)
	; 		return 
	; 	endIf
	; endWhile
    SVPP_API.RegurgitateAll(target, 7.0)
endFunction

function Fragment_1(ObjectReference akSpeakerRef)

	self.escape(akSpeakerRef as actor)
endFunction

; Skipped compiler generated GetState

; Skipped compiler generated GotoState

function Fragment_0(ObjectReference akSpeakerRef)

	self.escape(akSpeakerRef as actor)
endFunction
