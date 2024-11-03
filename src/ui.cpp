#include "headers/ui.h"

namespace Vore
{
	namespace UI
	{
		VoreMenu::VoreMenu() {
			abort();
			flog::info("!!!!!!!!!!!!!!!!!!!!!!!!!!! ");


			auto scaleformManager = RE::BSScaleformManager::GetSingleton();

			inputContext = Context::kNone;
			depthPriority = 0;

			menuFlags.set(RE::UI_MENU_FLAGS::kAlwaysOpen);
			menuFlags.set(RE::UI_MENU_FLAGS::kRequiresUpdate);
			menuFlags.set(RE::UI_MENU_FLAGS::kAllowSaving);

			if (uiMovie) {
				uiMovie->SetMouseCursorCount(0);
			}

			scaleformManager->LoadMovieEx(this, MENU_PATH, [](RE::GFxMovieDef* a_def) -> void {
				a_def->SetState(RE::GFxState::StateType::kLog,
					RE::make_gptr<Logger>().get());
			});

			Show();
			SetMenuVisibilityMode(MenuVisibilityMode::kVisible);
			flog::info("loaded bar");

			/*RE::GFxValue initArray[6]{ _width, _height, _primaryColor, _secondaryColor, _flashColor, _percent };
			flog::info("{}", uiMovie->Invoke("main.initNumbers", nullptr, initArray, 6));
			
			RE::GFxValue stringArray[1]{ _fillDirection };
			uiMovie->Invoke("main.initStrings", nullptr, stringArray, 1);

			uiMovie->Invoke("main.initCommit", nullptr, nullptr, 0);*/
		}

		void VoreMenu::Register()
		{
			auto ui = RE::UI::GetSingleton();
			if (ui) {
				ui->Register(VoreMenu::MENU_NAME, Creator);
				flog::info("Registered {}"sv, VoreMenu::MENU_NAME);
			}
		}

		RE::GPtr<VoreMenu> VoreMenu::GetVoreMenu()
		{
			auto ui = RE::UI::GetSingleton();
			return ui ? ui->GetMenu<VoreMenu>(VoreMenu::MENU_NAME) : nullptr;
		}

		void VoreMenu::Show()
		{
			auto msgQ = RE::UIMessageQueue::GetSingleton();
			if (msgQ) {
				msgQ->AddMessage(VoreMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr);
			}
		}

		void VoreMenu::Hide()
		{
			auto msgQ = RE::UIMessageQueue::GetSingleton();
			if (msgQ) {
				msgQ->AddMessage(VoreMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
			}
		}

		void VoreMenu::Update()
		{
			/*RE::GPtr<RE::IMenu> oxygenMeter = RE::UI::GetSingleton()->GetMenu(oxygenMenu::MENU_NAME);
			if (!oxygenMeter || !oxygenMeter->uiMovie)
				return;

			static bool fadeWhenDrowning{ Settings::GetSingleton()->fadeWhenDrowning };
			static float flashWhenBelow{ Settings::GetSingleton()->flashWhenBelow };
			auto fillPct = detail::get_player_breath_pct();

			if (fillPct) {
				RE::GFxValue MenuEnabled;
				oxygenMeter->uiMovie->GetVariable(&MenuEnabled, "main.MenuEnabled");

				if (!MenuEnabled.GetBool()) {
					ApplyLayout(oxygenMeter);
					ApplyColour(oxygenMeter);
					oxygenMeter->uiMovie->SetVariable("main.MenuEnabled", true);
				}

				SetMenuVisibilityMode(MenuVisibilityMode::kVisible);

				const RE::GFxValue currentBreathAmount = *fillPct;

				if (!holding_breath) {
					holding_breath = true;
				}

				if (drowning && fadeWhenDrowning) {
					oxygenMeter->uiMovie->Invoke("main.doFadeOut", nullptr, &currentBreathAmount, 1);
					return;
				}

				if (fillPct <= flashWhenBelow) {
					oxygenMeter->uiMovie->Invoke("main.doFlash", nullptr, nullptr, 0);
				}

				oxygenMeter->uiMovie->Invoke("main.doShow", nullptr, nullptr, 0);
				oxygenMeter->uiMovie->Invoke("main.setMeterPercent", nullptr, &currentBreathAmount, 1);

				if (*fillPct == 0.0) {
					drowning = true;
				}

			} else {
				static std::chrono::time_point<std::chrono::steady_clock> start;
				static bool fading{ false };

				if (holding_breath || drowning) {
					holding_breath = false;
					drowning = false;
					const RE::GFxValue refill = 100;
					oxygenMeter->uiMovie->Invoke("main.updateMeterPercent", nullptr, &refill, 1);
					oxygenMeter->uiMovie->Invoke("main.doFadeOut", nullptr, nullptr, 0);
					start = std::chrono::steady_clock::now();
					fading = true;
				}

				if (fading && ((std::chrono::steady_clock::now() - start) > 1s || !want_visible)) {
					SetMenuVisibilityMode(MenuVisibilityMode::kHidden);
					fading = false;
				}
			}*/
		}

		// apply location, rotations and scale settings to menu.
		void VoreMenu::ApplyLayout(RE::GPtr<RE::IMenu> voreMeter)
		{
			/*if (!voreMeter || !voreMeter->uiMovie)
				return;

			auto def = voreMeter->uiMovie->GetMovieDef();

			const RE::GFxValue widget_xpos = (Settings::GetSingleton()->widget_xpos / 100.0) * def->GetWidth();
			const RE::GFxValue widget_ypos = (Settings::GetSingleton()->widget_ypos / 100.0) * def->GetHeight();
			const RE::GFxValue widget_rotation = Settings::GetSingleton()->widget_rotation;
			const RE::GFxValue widget_xscale = Settings::GetSingleton()->widget_xscale;
			const RE::GFxValue widget_yscale = Settings::GetSingleton()->widget_yscale;
			RE::GFxValue posArray[5]{ widget_xpos, widget_ypos, widget_rotation, widget_xscale, widget_yscale };
			oxygenMeter->uiMovie->Invoke("main.setLocation", nullptr, posArray, 5);*/
		}

		void VoreMenu::ApplyColour(RE::GPtr<RE::IMenu> voreMeter)
		{
			/*if (!oxygenMeter || !oxygenMeter->uiMovie)
				return;

			const RE::GFxValue bar_colour = Settings::GetSingleton()->widget_colour;
			const RE::GFxValue flash_colour = Settings::GetSingleton()->widget_colour;
			RE::GFxValue colourArray[2]{ bar_colour, flash_colour };
			oxygenMeter->uiMovie->Invoke("main.setBarAndFlashColor", nullptr, colourArray, 2);*/
		}

		// Every time a new frame of the menu is rendered call the update function.
		void VoreMenu::AdvanceMovie(float a_interval, std::uint32_t a_currentTime)
		{
			VoreMenu::Update();
			RE::IMenu::AdvanceMovie(a_interval, a_currentTime);
		}

		RE::UI_MESSAGE_RESULTS VoreMenu::ProcessMessage(RE::UIMessage& a_message)
		{
			using Type = RE::UI_MESSAGE_TYPE;

			switch (*a_message.type) {
			case Type::kShow:
				OnOpen();
				return RE::IMenu::ProcessMessage(a_message);
			case Type::kHide:
				OnClose();
				return RE::IMenu::ProcessMessage(a_message);
			default:
				return RE::IMenu::ProcessMessage(a_message);
			}
		}

		bool VoreMenu::IsOpen() const
		{
			return _bIsOpen;
		}

		void VoreMenu::OnOpen()
		{
			// The advance movie process will handle showing the meter when appropriate
			VoreMenu::SetMenuVisibilityMode(VoreMenu::MenuVisibilityMode::kHidden);
			_bIsOpen = true;
		}

		void VoreMenu::OnClose()
		{
			want_visible = false;
			_bIsOpen = false;
		}

		void VoreMenu::SetMenuVisibilityMode(MenuVisibilityMode a_mode)
		{
			auto menu = GetVoreMenu();
			if (menu) {
				RE::GPtr<RE::GFxMovieView> _view = menu->uiMovie;

				if (_view) {
					switch (a_mode) {
					case MenuVisibilityMode::kHidden:
						if (menu->_menuVisibilityMode == MenuVisibilityMode::kVisible) {
							_view->SetVisible(false);
							menu->_menuVisibilityMode = a_mode;
						}
						break;

					case MenuVisibilityMode::kVisible:
						if (menu->_menuVisibilityMode == MenuVisibilityMode::kHidden && want_visible) {
							_view->SetVisible(true);
							menu->_menuVisibilityMode = a_mode;
						}
						break;
					}
				}
			}
		}

		constexpr std::string_view VoreMenu::Name() { return VoreMenu::MENU_NAME; }

		/*void VoreBar::Initialize() {

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
		}*/

	}
}