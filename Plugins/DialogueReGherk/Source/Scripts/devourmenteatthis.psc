;/ Decompiled by Champollion V1.0.1
Source   : DevourmentEatThis.psc
Modified : 2024-07-31 22:31:48
Compiled : 2024-07-31 22:31:55
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName DevourmentEatThis extends ActiveMagicEffect

;-- Properties --------------------------------------
actor property PlayerRef auto
formlist property DDFeedingPotions auto
keyword property DevourmentSkull auto
formlist property TTDSkulls auto
devourmentmanager property Manager auto
formlist property DDFeedingIngredients auto
globalvariable property DevourmentDialogueStuffingType auto
container property BolusContainer auto
formlist property DDFeedingFood auto

;-- Variables ---------------------------------------
DevourmentBolus bolus
actor actualPred
String PREFIX = "DevourmentEatThis"
Bool DEBUGGING = false
actor receiverPred

;-- Functions ---------------------------------------

function OnItemRemoved(form akBaseItem, Int aiItemCount, objectreference akItemReference, objectreference akDestContainer)

	if akBaseItem.HasKeyword(DevourmentSkull) == true
		actor reformee = (akItemReference as devourmentskullobject).GetRevivee()
		Manager.registerreformation(actualPred, reformee, 0)
		akItemReference.delete()
	endIf
endFunction

; Skipped compiler generated GetState

; Skipped compiler generated GotoState

function OnMenuClose(String menuName)

	if bolus.GetNumItems() > 0
		if DEBUGGING
			logging.Log1(PREFIX, "OnMenuClose", "Bolus Size: " + bolus.GetNumItems() as String)
		endIf
		Manager.RegisterDigestion(actualPred as form, bolus as form, false, 0)
		Manager.PlayVoreAnimation_Item(actualPred, bolus as objectreference, 0, true)
	else
		if DEBUGGING
			logging.Log1(PREFIX, "OnMenuClose", "Bolus empty, deleting.")
		endIf
		bolus.delete()
	endIf
	if receiverPred == Manager.FakePlayer
		Manager.FakePlayer.MoveTo(Manager.HerStomach, 0.000000, 0.000000, 0.000000, true)
	endIf
	self.UnregisterForAllMenus()
	self.Dispel()
endFunction

function OnItemAdded(form baseItem, Int itemCount, objectreference itemReference, objectreference source)

	if DEBUGGING
		logging.Log6(PREFIX, "OnItemAdded", logging.Namer(baseItem, false), itemCount as String, logging.Namer(itemReference as form, false), logging.Namer(source as form, false), logging.Namer(actualPred as form, false), logging.Namer(receiverPred as form, false))
	endIf
	if source == PlayerRef as objectreference
		if itemReference
			if TTDSkulls.HasForm(baseItem)
				itemReference.MoveTo(source, 0.000000, 0.000000, 0.000000, true)
			endIf
			receiverPred.RemoveItem(itemReference as form, itemCount, false, bolus as objectreference)
			Manager.PlayVoreAnimation_Item(actualPred, itemReference, 0, true)
		else
			receiverPred.RemoveItem(baseItem, itemCount, false, bolus as objectreference)
		endIf
	endIf
endFunction

function OnEffectFinish(actor akTarget, actor akCaster)

	; Empty function
endFunction

function OnEffectStart(actor akTarget, actor akCaster)

	if !logging.assertNotNone(PREFIX, "OnEffectStart", "akTarget", akTarget as form)
		self.Dispel()
		return 
	endIf
	bolus = Manager.FakePlayer.placeAtMe(BolusContainer as form, 1, false, false) as DevourmentBolus
	receiverPred = akTarget
	if akTarget == Manager.FakePlayer
		actualPred = PlayerRef
	else
		actualPred = akTarget
	endIf
	if DEBUGGING
		logging.Log2(PREFIX, "OnEffectStart", logging.Namer(receiverPred as form, false), logging.Namer(actualPred as form, false))
	endIf
	bolus.Initialize(logging.Namer(PlayerRef as form, true) + "'s stash", PlayerRef, actualPred, actualPred != PlayerRef)
	self.RegisterForMenu("GiftMenu")
	if DevourmentDialogueStuffingType.GetValue() == 1 as Float
		receiverPred.ShowGiftMenu(true, DDFeedingFood, true, false)
	elseIf DevourmentDialogueStuffingType.GetValue() == 2 as Float
		receiverPred.ShowGiftMenu(true, DDFeedingIngredients, true, false)
	elseIf DevourmentDialogueStuffingType.GetValue() == 3 as Float
		receiverPred.ShowGiftMenu(true, DDFeedingPotions, true, false)
	else
		receiverPred.ShowGiftMenu(true, none, true, false)
	endIf
endFunction
