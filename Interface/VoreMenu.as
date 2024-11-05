import gfx.io.GameDelegate;
import gfx.managers.FocusHandler;
import gfx.ui.InputDetails;
import gfx.ui.NavigationCode;
import Shared.GlobalFunc;
import Components.Meter;
import skyui.util.Tween;
import mx.utils.Delegate;

class VoreMenu extends MovieClip
{
	public var countText: TextField;
	
	function VoreMenu()
	{
		super();
		//setText("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	}
	
	function setText(aText: String): Void
	{
		countText.text = aText;
	}
		
}