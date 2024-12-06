Scriptname DevourmentAnimatedSpell extends ActiveMagicEffect
{
AUTHOR: Gaz
PURPOSE: Provides functions for deciding and executing vore animations.
Intended to be extended by spells where needed. Does not handle FX beyond animation.
TODO: Node Scaling systems.
}

DevourmentManager Property Manager Auto
Actor Property PlayerRef Auto
Int property Locus = -1 Auto
Bool Property Endo = false Auto
Bool Property PreferlongAnimation = False Auto
Bool Property ProtectActors = True Auto
Actor Property Prey Auto
Actor Property Pred Auto
FormList Property DevourmentPairedIdles Auto

