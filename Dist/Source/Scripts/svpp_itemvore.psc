scriptName SVPP_ItemVore extends ActiveMagicEffect


actor property PlayerRef auto

ObjectReference sc
Actor pred


function OnMenuClose(String menuName)

	SVPP_API.CommitSC(pred, sc)
	self.UnregisterForAllMenus()
	self.Dispel()
endFunction

function OnItemAdded(form baseItem, Int itemCount, objectreference itemReference, objectreference source)

	if source == PlayerRef
		if itemReference
			pred.RemoveItem(itemReference, itemCount, false, sc)
		else
			pred.RemoveItem(baseItem, itemCount, false, sc)
		endIf
	endIf
endFunction


function OnEffectStart(actor akTarget, actor akCaster)

	sc = SVPP_API.MakeSC()
    pred = akTarget
	self.RegisterForMenu("GiftMenu")
endFunction