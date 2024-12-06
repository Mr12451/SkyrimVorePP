;/ Decompiled by Champollion V1.0.1
Source   : DialogueVore.psc
Modified : 2023-10-11 23:58:11
Compiled : 2023-10-11 23:58:18
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName DialogueVore extends TopicInfo hidden

;-- Properties --------------------------------------
Bool property NoEscape = false auto
{If set to true, the prey will not be able to struggle or escape.}
Int property override = -1 auto
Bool property Consented = false auto
{If set to true, the prey will not be able to struggle or escape; and non-consensual dialogue will be blocked.}
Bool property kissoff = false auto
{The pred and prey will share a kiss before swallowing (requires Sexlab).}
Float property damage = 0.000000 auto
{The prey will take sufficient damage to reduce their health to this amount.}
Bool property applyToPred = false auto
{Indicates that diplomacy and influence options apply to the pred instead of the prey.}
Bool property intimidate = false auto
{Indicates that the prey (or the pred if ApplyToPred is set) had to pass an Intimidate check.}
Bool property bribe = false auto
{Indicates that the prey (or the pred if ApplyToPred is set) had to pass a Bribe check.}
Bool property strip = false auto
{Strip the clothing/armor of the prey before swallowing.}
faction property DialogueVorePred auto
faction property DialogueVorePrey auto
Bool property persuade = false auto
{Indicates that the prey (or the pred if ApplyToPred is set) had to pass a Persuade check.}

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

function Escape(Actor pred, Actor prey, Bool oralEscape)

	; devourmentmanager manager = devourmentmanager.instance()
	; manager.forceEscape(prey as form)
    SVPP_API.Regurgitate(pred, prey)
    ; --
	if applyToPred
		if intimidate
			self.intimidate(pred)
		elseIf persuade
			self.persuade(pred)
		elseIf bribe
			self.bribe(pred)
		endIf
	elseIf intimidate
		self.intimidate(prey)
	elseIf persuade
		self.persuade(prey)
	elseIf bribe
		self.bribe(prey)
	endIf
endFunction

function Process(Actor pred, Actor prey, Bool endo)

	if override >= 0
		faction StrangerFaction = game.GetFormFromFile(3328, "Devourment.esp") as faction
		pred.SetFactionRank(StrangerFaction, override)
	endIf
	if strip
		SVPP_API.Strip(prey)
	endIf
	if kissoff
        ; Placeholder for kisses
		self.Swallow(pred, prey, endo)
	else
		self.Swallow(pred, prey, endo)
	endIf
endFunction

function SwitchToVore(ObjectReference prey)

	if !(prey as Actor).IsDead()
		if Consented
			SVPP_API.Consent(none, prey, true, true)
		endIf
		if NoEscape
			SVPP_API.DisableEscape(prey)
		endIf
		SVPP_API.SwitchTo(prey, 2.0)
	endIf
endFunction


function SwitchToEndo(Actor prey)

	if !prey.IsDead()
		if Consented
			SVPP_API.Consent(none, prey, true, false)
		endIf
		if NoEscape
			SVPP_API.DisableEscape(prey)
		endIf
		SVPP_API.SwitchTo(prey, 1.0)
	endIf
endFunction

function DisableEscape(Actor prey)

	SVPP_API.DisableEscape(prey)
	if Consented
        SVPP_API.Consent(none, prey, true, true)
	endIf
endFunction

function intimidate(Actor pTarget)

	(quest.GetQuest("DialogueFavorGeneric") as favordialoguescript).intimidate(pTarget)
endFunction

Actor function Resolve(ObjectReference target)

    if target as Actor
		return target as Actor
	else
		return none
	endIf
endFunction

function Swallow(Actor pred, Actor prey, Bool endo)

	if applyToPred
		if intimidate
			self.intimidate(pred)
		elseIf persuade
			self.persuade(pred)
		elseIf bribe
			self.bribe(pred)
		endIf
	elseIf intimidate
		self.intimidate(prey)
	elseIf persuade
		self.persuade(prey)
	elseIf bribe
		self.bribe(prey)
	endIf

    if endo
        if Consented
            SVPP_API.Consent(pred, prey, true, false)
        endIf
        SVPP_API.Swallow(pred, prey, -1.0, 1.0, false)
    else
        if Consented
            SVPP_API.Consent(pred, prey, true, true)
        endIf
        SVPP_API.Swallow(pred, prey, -1.0, 2.0, false)
    endIf

	if damage > 0.000000
		Float health = prey.getActorValue("health")
		if damage < health
			prey.damageActorValue("health", health - damage)
		elseIf health > 11.0000
			prey.damageActorValue("health", health - 10.0000)
		endIf
	endIf
	if NoEscape
        SVPP_API.DisableEscape(prey)
    endIf
endFunction

function bribe(Actor pTarget)
{We need a custom version of Bribe, because the default bribe amount is WAY too low.}

	Actor playerRef = game.GetPlayer()
	favordialoguescript generic = quest.GetQuest("DialogueFavorGeneric") as favordialoguescript
	generic.SkillUseMultiplier = generic.SpeechSkillMult.value
	generic.SkillUseBribe = generic.SkillUseMultiplier * playerRef.getActorValue("Speechcraft")
	; debug.trace(self as String + "Current Skill uses given: " + generic.SkillUseBribe as String + " times the Skill Use Multiplier", 0)
	Int bribeAmount = pTarget.GetBribeAmount()
	if bribeAmount <= playerRef.GetGoldAmount()
		playerRef.RemoveItem(generic.Gold as form, bribeAmount, false, none)
		pTarget.AddItem(generic.Gold as form, bribeAmount, false)
		pTarget.SetBribed(true)
		game.AdvanceSkill("Speechcraft", generic.SkillUseBribe)
		game.IncrementStat("Bribes", 1)
		if game.QueryStat("Persuasions") as Bool && game.QueryStat("Intimidations") as Bool
			game.AddAchievement(28)
		endIf
	endIf
endFunction

function LethalDisable(Actor pred, Actor prey)

	;devourmentmanager manager = devourmentmanager.instance()
	if !prey.IsDead()
		if applyToPred
			if intimidate
				self.intimidate(pred)
			elseIf persuade
				self.persuade(pred)
			elseIf bribe
				self.bribe(pred)
			endIf
		elseIf prey
			if intimidate
				self.intimidate(prey)
			elseIf persuade
				self.persuade(prey)
			elseIf bribe
				self.bribe(prey)
			endIf
		endIf
		if prey as Bool && !prey.IsDead()
			SVPP_API.DisableEscape(prey)
            if Consented
                SVPP_API.Consent(pred, prey, true, true)
            endIf
            SVPP_API.SwitchTo(prey, 2.0)
			;manager.switchLethal(prey as form, true)
		else
            if pred as Bool
                SVPP_API.SwitchPredTo(pred, 2.0, true)
            endIf
		endIf
	endIf
endFunction

; Skipped compiler generated GotoState

function persuade(Actor pTarget)

	(quest.GetQuest("DialogueFavorGeneric") as favordialoguescript).persuade(pTarget)
endFunction
