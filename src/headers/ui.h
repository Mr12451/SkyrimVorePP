#pragma once

//some code taken from oxyMeter2 and TrueHUD source
namespace Vore
{
	namespace UI
	{
		class VoreMenu : public RE::IMenu
		{
		public:
			static constexpr const char* MENU_PATH = "VoreMenu";
			static constexpr const char* MENU_NAME = "VoreMenu";
			VoreMenu();
			static void Register();
			static RE::GPtr<VoreMenu> GetVoreMenu();
			static void Show();
			static void Hide();
			static void Update();
			static void ApplyLayout(RE::GPtr<RE::IMenu> voreMeter);
			static void ApplyColour(RE::GPtr<RE::IMenu> voreMeter);

			static RE::stl::owner<RE::IMenu*> Creator() { 
				flog::info("????????????????????????? ");
				return new VoreMenu(); 
			}

			void AdvanceMovie(float a_interval, std::uint32_t a_currentTime) override;
			RE::UI_MESSAGE_RESULTS ProcessMessage(RE::UIMessage& a_message) override;

			enum class MenuVisibilityMode : uint8_t
			{
				kHidden,
				kVisible
			};

			MenuVisibilityMode _menuVisibilityMode = MenuVisibilityMode::kVisible;
			static inline bool want_visible{ false };

			static void SetMenuVisibilityMode(MenuVisibilityMode a_mode);

			bool IsOpen() const;
			void OnOpen();
			void OnClose();

			static constexpr std::string_view Name();

		private:
			static inline bool holding_breath{ false };
			static inline bool drowning{ false };
			bool _bIsOpen = false;

		private:
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

					flog::info("{}"sv, buf.data());
				}
			};

		};

		/*class VoreWidget
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
		};*/
	}
}