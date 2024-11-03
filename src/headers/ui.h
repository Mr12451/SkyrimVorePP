#pragma once

namespace Scaleform
{

	class VoreMenu : public RE::IMenu
	{
	public:
		static constexpr const char* MENU_PATH = "VoreMenu";
		static constexpr const char* MENU_NAME = "VoreMenu";

		VoreMenu();

		static void Register();
		static void Show();
		static void Hide();
		static void ShowShow();
		static RE::GPtr<VoreMenu> GetVoreMenu();

		static RE::stl::owner<RE::IMenu*> Creator()
		{
			return new VoreMenu();
		}
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

	private:
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

}
