#pragma once

namespace Vore
{
	class Dialogue
	{
	private:
		static void SetupBeforeDialogue();
		static void ClearAfterDialogue();
		enum DialogueState : uint8_t
		{
			kNone,
			kWaiting,
			kDialogue,
			kFail
		};
		static inline bool plugin_loaded = false;
		static inline std::atomic<DialogueState> _running{ kNone };
		static inline RE::ActorHandle _handlePlayer;
		static inline RE::ActorHandle _handleNPC;

		static inline RE::TESQuest* _dialogueQuest = nullptr;

		static inline RE::SpellItem* s_fullness = nullptr;
		static inline RE::SpellItem* s_notThere = nullptr;

		static inline RE::SpellItem* s_soundStrF = nullptr;
		static inline RE::SpellItem* s_soundStrM = nullptr;
		static inline RE::SpellItem* s_soundDig = nullptr;
		//static inline RE::SpellItem* s_soundDig = nullptr;

		static inline RE::SpellItem* s_statusSwalP = nullptr;
		static inline RE::SpellItem* s_statusSwalByP = nullptr;
		static inline RE::SpellItem* s_statusSwalFol = nullptr;
		static inline RE::SpellItem* s_statusRefP = nullptr;
		static inline RE::SpellItem* s_statusRefByP = nullptr;
		static inline RE::SpellItem* s_statusDigP = nullptr;

		static inline RE::TESGlobal* g_playerDead = nullptr;

		static inline RE::TESGlobal* g_lethal = nullptr;
		static inline RE::TESGlobal* g_locus = nullptr;

	public:
		static void OnSwallow_Pred(RE::Actor* pred);
		static void OnSwallow_Prey(RE::Actor* pred, RE::TESObjectREFR* prey);

		static void Clear_Pred(RE::Actor* pred);
		static void Clear_Prey(RE::Actor* pred, RE::TESObjectREFR* prey, bool forceClear);
		static void SetupForReform(RE::Actor* prey);
		static void OnDigestionChange(RE::Actor* pred);
		static void PlayerDied();

		static void TalkToA(RE::Actor* target);
		static void CheckPlugin();
	};
}