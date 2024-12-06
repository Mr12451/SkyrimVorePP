;/ Decompiled by Champollion V1.0.1
Source   : SwallowCalculate.psc
Modified : 2023-11-22 10:25:04
Compiled : 2023-11-22 10:25:09
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName SwallowCalculate extends DevourmentAnimatedSpell
{This is the script that is called for all vore operations.
It calculates the odds of success, disables the player's controls
if they are the prey, applies the swallow sound and visual
effects, and adds the spell/items that prevent either of them from
being swallowed.}

;-- Properties --------------------------------------
devourmentplayeralias property playerAlias auto
message property Message_Trust auto
Bool property Scripted = false auto
Bool property Reversed = false auto
keyword property KeywordSurrender auto
faction property DialogueVorePrey auto
message property Message_SheathWeapon auto
message property Message_Capacity auto
spell property SwallowPreventSpell auto
sound[] property SwallowSounds auto
perk property SilentSwallow auto
effectshader property SwallowShader auto
faction property DialogueVorePred auto
keyword property BeingSwallowed auto

;-- Variables ---------------------------------------
Float updateInterval = 0.100000
Bool weaponDrawn
Float swallowDifficulty
Float d100Roll
Bool DEBUGGING = false
Int timer = 0
String PREFIX = "SwallowCalculate"
Int animCode
Bool deadPrey

;-- Functions ---------------------------------------

function OnEffectStart(Actor akTarget, Actor akCaster)

	if !(akTarget as Bool && akCaster as Bool)
		return 
	endIf
	if !Reversed
		Prey = akTarget
		Pred = akCaster
	else
		Pred = akTarget
		Prey = akCaster
	endIf

	if Locus < 0
		Locus = self.DecideLocus()
	endIf

    if Endo
        SVPP_API.Swallow(Pred, Prey, Locus as float, 1.0, false)
    else
        SVPP_API.Swallow(Pred, Prey, Locus as float, 2.0, false)
    endIf
    self.Dispel()
endFunction

Int function DecideLocus()

	if Pred == PlayerRef
		Int deflocus = SVPP_API.GetDefaultLocus() as Int
		return deflocus
	else
		return SVPP_API.GetRandomLocus(Pred) as Int
	endIf
endFunction
