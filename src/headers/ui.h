#pragma once

namespace Vore::UI
{

	enum VoreMenuMode : uint8_t
	{
		kDefault, // hidden. When player is involed in vore, instead switches to PredPrey
		kPredPrey, // display player's pred, prey, and other characters in the same locus. Can select one of the characters
		kInfo, // shows the character's info, depending on the context
		kSwallow, // configurate player's swallow and regurgitation preferences

		kNone
	};

	enum MenuAction : uint8_t
	{
		kMenuA1,
		kMenuA2,
		kMenuA3,
		kMenuA4
	};

	class VoreMenu : public RE::IMenu
	{
	public:
		static constexpr std::string_view MENU_PATH{ "VoreMenu" };
		static constexpr std::string_view MENU_NAME{ "VoreMenu" };

		VoreMenu();

		static void Register();
		static void Show();
		static void Hide();
		static RE::GPtr<VoreMenu> GetVoreMenu();

		void SetText();
		static void Update();
		static void SetMenuMode(VoreMenuMode mode, bool ignoreInfoCrosshair = false);
		static void DoMenuAction(MenuAction action);

		static RE::stl::owner<RE::IMenu*> Creator()
		{
			return new VoreMenu();
		}
		RE::UI_MESSAGE_RESULTS ProcessMessage(RE::UIMessage& a_message) override;



		static void SetMenuVisibilityMode(bool a_mode);
		static bool IsOpen();
		void OnOpen();
		void OnClose();

		static inline bool NeedUpdate{ false };

		static inline int _lastCharCount { 0 };
		static inline int _charIndex { 0 };
		static inline VoreMenuMode _menuMode{ VoreMenuMode::kNone };
		static inline VoreMenuMode _setModeAfterShow{ kNone };
		static inline RE::ObjectRefHandle _infoTarget;
	private:

		enum InfoMode : uint8_t
		{
			kNeutral,
			kPred,
			kPrey,
			kSameLocus
		};

		static inline InfoMode _iMode = kNeutral;
		static inline bool _iAllowReg = false;
		static inline bool _iFullTour = false;
		bool _visible = true;
		static inline bool _exists { false };
		static inline bool _loadedOnce { false };
		static inline bool _firstInfoUpdate { false };
		static inline std::string text{ "" };


		void WriteSwallowMenu();

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

}
