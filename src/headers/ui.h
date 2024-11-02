#pragma once

namespace Vore
{
	namespace UI
	{
		class VoreMenu : RE::IMenu
		{
		private:
			constexpr static std::string_view FILE_NAME{ "SkyrimVorePP" };
			constexpr static std::string_view MENU_NAME{ "SkyrimVorePP" };
			constexpr static std::int8_t SORT_PRIORITY{ 0 };
			RE::GPtr<RE::GFxMovieView> _view;

			class Logger : public RE::GFxLog
			{
			public:
				void LogMessageVarg(LogMessageType, const char* a_fmt, std::va_list a_argList) override
				{
					std::string fmt(a_fmt ? a_fmt : "");
					while (!fmt.empty() && fmt.back() == '\n') {
						fmt.pop_back();
					}

					std::va_list args;
					va_copy(args, a_argList);
					std::vector<char> buf(static_cast<std::size_t>(std::vsnprintf(0, 0, fmt.c_str(), a_argList) + 1));
					std::vsnprintf(buf.data(), buf.size(), fmt.c_str(), args);
					va_end(args);

					flog::info("{}: {}"sv, VoreMenu::MenuName(), buf.data());
				}
			};

		public:
			static constexpr std::string_view MenuName() noexcept { return MENU_NAME; }
			static constexpr std::int8_t SortPriority() noexcept { return SORT_PRIORITY; }

			VoreMenu()
			{
				auto menu = static_cast<RE::IMenu*>(this);
				menu->depthPriority = SortPriority();
				auto scaleformManager = RE::BSScaleformManager::GetSingleton();

				[[maybe_unused]] const auto success =
					scaleformManager->LoadMovieEx(menu, FILE_NAME, [](RE::GFxMovieDef* a_def) -> void {
						a_def->SetState(
							RE::GFxState::StateType::kLog,
							RE::make_gptr<Logger>().get());
					});

				assert(success);
				if (menu && menu->uiMovie) {
					auto def = menu->uiMovie->GetMovieDef();
					if (def) {
						def->SetState(
							RE::GFxState::StateType::kLog,
							RE::make_gptr<Logger>().get());
					}
				}

				menuFlags.set(RE::UI_MENU_FLAGS::kAlwaysOpen);
				menuFlags.set(RE::UI_MENU_FLAGS::kRequiresUpdate);
				menuFlags.set(RE::UI_MENU_FLAGS::kAllowSaving);

				menu->inputContext = Context::kNone;

				_view = menu->uiMovie;
				_view->SetMouseCursorCount(0);  // disable input
			}
			VoreMenu(const VoreMenu&) = default;
			VoreMenu(VoreMenu&&) = default;
			~VoreMenu() = default;
			static RE::stl::owner<RE::IMenu*> Creator() { return new VoreMenu(); }
			void PostCreate()
			{
				RE::IMenu::PostCreate();
			}

			RE::UI_MESSAGE_RESULTS ProcessMessage(RE::UIMessage& a_message) override
			{
				using Type = RE::UI_MESSAGE_TYPE;

				switch (*a_message.type) {
				case Type::kShow:
					return RE::IMenu::ProcessMessage(a_message);
				case Type::kHide:
					return RE::IMenu::ProcessMessage(a_message);
				default:
					return RE::IMenu::ProcessMessage(a_message);
				}
			}
			static void Register();

			void MakePreyBar(RE::ObjectRefHandle a_actorHandle);
		};

		class VoreWidget
		{
		public:
			RE::GPtr<RE::GFxMovieView> _view;
			RE::GFxValue _object;
			uint32_t _widgetID;
			float _depth = 0;

			VoreWidget() = default;
			virtual ~VoreWidget() = default;

			VoreWidget(RE::GPtr<RE::GFxMovieView> a_view) :
				_view(a_view),
				_widgetID(0)
			{}

			VoreWidget(uint32_t a_widgetID) :
				_widgetID(a_widgetID)
			{}

			VoreWidget(RE::GPtr<RE::GFxMovieView> a_view, uint32_t a_widgetID) :
				_view(a_view),
				_widgetID(a_widgetID)
			{}

			virtual void Initialize() = 0;
			virtual void Dispose() = 0;
			
		};

		class VoreBar : public VoreWidget
		{
		public:
			RE::ObjectRefHandle _refHandle;
			VoreBar(RE::GPtr<RE::GFxMovieView> a_view, RE::ObjectRefHandle a_refHandle) :
				VoreWidget(a_view, a_refHandle.native_handle()),
				_refHandle(a_refHandle)
			{}

			virtual void Initialize() override;
			virtual void Dispose() override;
		};

		class PreyBar : public VoreBar
		{
		public:
			PreyBar(RE::GPtr<RE::GFxMovieView> a_view, RE::ObjectRefHandle a_refHandle) :
				VoreBar(a_view, a_refHandle)
			{
				
			}
		};
	}
}