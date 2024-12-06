;/ Decompiled by Champollion V1.0.1
Source   : DevourmentDialog.psc
Modified : 2022-11-07 20:06:33
Compiled : 2022-12-30 09:46:46
User     : gazho
Computer : GAZ
/;
scriptName DevourmentDialog extends Quest conditional
{None of this is in use yet.}

;-- Properties --------------------------------------
keyword property ActorTypeAnimal auto
globalvariable property Consented auto conditional
globalvariable property VoreDialog auto
devourmentmanager property Manager auto
actor property PlayerRef auto
referencealias property DialogPreyAlias auto
referencealias property DialogPredAlias auto
globalvariable property Lethal auto conditional
Bool property Activated = false auto conditional
globalvariable property NoEscape auto conditional
globalvariable property Locus auto conditional
devourmenttalker property TheTalker auto

;-- Variables ---------------------------------------
String PREFIX = "DevourmentDialog"

;-- Functions ---------------------------------------

DevourmentDialog function instance() global

	return Quest.GetQuest("DevourmentDialog") as DevourmentDialog
endFunction

function DoDialog_PlayerAndApex()
{Starts dialog with the player and their apex predator.}
    SVPP_API.PredDialogue()
endFunction

function DoDialog_PlayerAndPrey(actor prey, Bool useTalkActivator)
{Starts dialog with the player and their prey.}
    SVPP_API.PreyDialogue()
endFunction
