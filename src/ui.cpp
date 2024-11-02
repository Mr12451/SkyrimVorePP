#include "headers/ui.h"

namespace Vore
{
	namespace UI
	{
		void VoreMenu::Register()
		{
			auto ui = RE::UI::GetSingleton();
			if (ui) {
				ui->Register(MENU_NAME, Creator);
				flog::info("Registered {}"sv, MENU_NAME);
			}
		}

		void VoreBar::Initialize() {

		}
		void VoreBar::Dispose()
		{
		}
		void VoreMenu::MakePreyBar(RE::ObjectRefHandle a_actorHandle)
		{
			std::shared_ptr<PreyBar> widget = std::make_shared<PreyBar>(_view, a_actorHandle);
			RE::GFxValue arg;
			arg.SetNumber(widget->_widgetID);
			_view->Invoke("_root.SkyrimVorePP.AddBossInfoBarWidget", &widget->_object, &arg, 1);
		}

	}
}