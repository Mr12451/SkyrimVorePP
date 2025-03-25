#include "headers/ui.h"
#include "headers/nutils.h"
#include "headers/settings.h"
#include "headers/times.h"
#include "headers/util.h"
#include "headers/voredata.h"
#include "headers/voremain.h"
#include "headers/vutils.h"
#include "headers/dialogue.h"

namespace Vore::UI
{

	//
	//
	//
	//
	//
	//
	//
	//

	VoreMenu::VoreMenu()
	{
		auto scaleformManager = RE::BSScaleformManager::GetSingleton();

		inputContext = Context::kNone;
		depthPriority = 0;

		menuFlags.set(RE::UI_MENU_FLAGS::kAlwaysOpen);
		menuFlags.set(RE::UI_MENU_FLAGS::kRequiresUpdate);
		menuFlags.set(RE::UI_MENU_FLAGS::kAllowSaving);

		if (uiMovie) {
			uiMovie->SetMouseCursorCount(0);  // disable input
		}

		scaleformManager->LoadMovieEx(this, MENU_PATH, [](RE::GFxMovieDef* a_def) -> void {
			a_def->SetState(RE::GFxState::StateType::kLog, RE::make_gptr<Logger>().get());
			flog::debug("THE MENU IS REEEEEEEEEEEEEEEEEAL!!!!");
		});
	}

	RE::GPtr<VoreMenu> VoreMenu::GetVoreMenu()
	{
		auto ui = RE::UI::GetSingleton();
		return ui ? ui->GetMenu<VoreMenu>(VoreMenu::MENU_NAME) : nullptr;
	}

	void VoreMenu::SetText()
	{
		RE::GFxValue newText{ text };
		uiMovie->Invoke("Menu.setText", nullptr, &newText, 1);
	}

	static void WriteStats(std::string& text, RE::Actor* actr)
	{
		text += "\nHP ";
		text += std::to_string(static_cast<int>(AV::GetAV(actr, RE::ActorValue::kHealth)));
		text += "/";
		text += std::to_string(static_cast<int>(AV::GetMaxAV(actr, RE::ActorValue::kHealth)));
		text += " ST ";
		text += std::to_string(static_cast<int>(AV::GetAV(actr, RE::ActorValue::kStamina)));
		text += "/";
		text += std::to_string(static_cast<int>(AV::GetMaxAV(actr, RE::ActorValue::kStamina)));
		text += " M ";
		text += std::to_string(static_cast<int>(AV::GetAV(actr, RE::ActorValue::kMagicka)));
		text += "/";
		text += std::to_string(static_cast<int>(AV::GetMaxAV(actr, RE::ActorValue::kMagicka)));
	}

	static void WritePreyData(std::string& text, RE::FormID prey, bool countIndex, bool writeName)
	{
		if (VoreDataEntry* pyData = VoreData::IsValidGet(prey)) {
			if (writeName) {
				text += "\n";
				if (countIndex && VoreMenu::_charIndex == VoreMenu::_lastCharCount) {
					text += "--> ";
				}
				text += Name::GetName(pyData->get());
			}

			if (pyData->aIsChar && pyData->aAlive) {
				WriteStats(text, pyData->get()->As<RE::Actor>());
			}
			text += "\n";
			text += PlayerPrefs::GetLocStr(pyData->pyLocus);
			text += ", ";
			text += PlayerPrefs::GetTypeStr(pyData->pyDigestion);
			text += ", ";
			if (pyData->pyDigestion == VoreDataEntry::hLethal) {
				text += pyData->pyConsentLethal ? "Willing" : "Unwilling";
			} else {
				text += pyData->pyConsentEndo ? "Willing" : "Unwilling";
			}
			if (pyData->pySwallowProcess != 100) {
				text += "\nSwallow process: ";
				text += std::format("{:.2f}", pyData->pySwallowProcess);
			}
			if (!pyData->aAlive) {
				text += "\nDigestion: ";
				text += std::format("{:.2f}", pyData->pyDigestProgress);
			} else if (pyData->pyLocusMovement != VoreDataEntry::mStill) {
				text += "\nFULL TOUR";
			}

			if (countIndex) {
				VoreMenu::_lastCharCount++;
			}
		} else {
			flog::critical("Found an illigal prey {}", Name::GetName(prey));
			return;
		}
		
	}

	void VoreMenu::Update()
	{
		NeedUpdate = false;
		if (_menuMode == kDefault || _menuMode > kInfo) {
			return;
		}

		if (!_exists) {
			if (_loadedOnce) {
				Show();
			} else {
				flog::critical("Cannot update menu - no menu found!");
				return;
			}
		}

		auto thisMenu = GetVoreMenu();
		if (!thisMenu) {
			flog::critical("Cannot update menu - no menu found!");
			return;
		}

		switch (_menuMode) {
		case (VoreMenuMode::kPredPrey):
			{
				_lastCharCount = 0;
				
				text = "";
				text += "Up [";
				text += KeyUtil::Interpreter::GetKeyName(VoreSettings::k_menu_1);
				text += "] Down [";
				text += KeyUtil::Interpreter::GetKeyName(VoreSettings::k_menu_3);
				text += "] Select [";
				text += KeyUtil::Interpreter::GetKeyName(VoreSettings::k_menu_2);
				text += "]\n\n";

				RE::FormID playerId = RE::PlayerCharacter::GetSingleton()->GetFormID();

				if (VoreData::IsPrey(playerId)) {
					VoreDataEntry& playerData = VoreData::Data[playerId];
					VoreDataEntry& predData = VoreData::Data[playerData.pred];
					text += "Pred: ";

					if (_charIndex == _lastCharCount) {
						text += "--> ";
					}

					text += Name::GetName(predData.get());
					text += "\nVore Type: ";
					text += PlayerPrefs::GetLocStr(playerData.pyLocus);
					text += ", ";
					text += PlayerPrefs::GetTypeStr(playerData.pyDigestion);
					text += ", ";
					if (playerData.pyDigestion == VoreDataEntry::hLethal) {
						text += playerData.pyConsentLethal ? "Willing" : "Unwilling";
					} else {
						text += playerData.pyConsentEndo ? "Willing" : "Unwilling";
					}
					if (!playerData.aAlive) {
						text += "\nDigestion: ";
						text += std::format("{:.2f}", playerData.pyDigestProgress);
					}

					std::vector<RE::FormID> otherPrey = FilterPrey(playerData.pred, playerData.pyLocus, false);

					_lastCharCount++;

					if (otherPrey.size() > 1) {
						text += "\nOther prey";
						for (RE::FormID& prey : otherPrey) {
							if (prey != playerId) {
								WritePreyData(text, prey, true, true);
							}
						}
						text += "\n";
					}
				}
				if ((VoreData::IsPred(playerId, true))) {
					VoreDataEntry& playerData = VoreData::Data[playerId];
					text += "Your prey:";
					for (auto& prey : playerData.prey) {
						WritePreyData(text, prey, true, true);
					}
					text += "\nIndigestion:";
					for (auto& lInd : playerData.pdIndigestion) {
						text += " ";
						text += std::to_string(static_cast<int>(lInd));
					}
					text += "\nAcid levels:";
					for (auto& lInd : playerData.pdAcid) {
						text += " ";
						text += std::to_string(static_cast<int>(lInd));
					}
				}

				thisMenu->SetText();
				break;
			}
		case (VoreMenuMode::kInfo):
			{
				_iMode = kNeutral;
				_iAllowReg = false;
				_iFullTour = false;
				if (!_infoTarget.get()) {
					_firstInfoUpdate = false;
					text = "No target (THIS SHOULD NEVER BE DISPLAYED!)";
					thisMenu->SetText();
					return;
				}
				RE::TESObjectREFR* target = _infoTarget.get().get();

				// no need to update characters who are not involved in vore
				// yes, their hp will not be update in real time, but why should it matter?
				if (!_firstInfoUpdate && !VoreData::IsValid(target->GetFormID())) {
					return;
				}
				_firstInfoUpdate = false;
				if (target->GetFormType() == RE::FormType::Reference) {
					text = "Object Info:\n\n";
					text += "Name: ";
					text += target->GetDisplayFullName();
					text += "\nSize: ";

					text += std::format("{:.4f}, {:.4f}", GetObjectSize(target), GetModelScale(target));

					thisMenu->SetText();
					return;
				} else if (target->GetFormType() != RE::FormType::ActorCharacter) {
					text = "Incorrect form type!";
					thisMenu->SetText();
					return;
				}

				text = "Character Info:\n\n";

				RE::Actor* actr = target->As<RE::Actor>();
				text += "Name: ";
				text += actr->GetDisplayFullName();
				text += "\nRace: ";
				text += actr->GetRace()->GetFullName();
				RE::FormID charId = actr->GetFormID();
				if (VoreCharStats* actorStats = VoreData::IsValidStatGet(charId)) {
					text += "\nPred Skill: ";
					text += std::format("{:.4f}, ",actorStats->predLevel);
					text += " ";
					text += std::format("{:.4f}, ", actorStats->predXp);
					text += " ";
					text += std::format("{:.4f}, ", actorStats->predThreshold);
					text += "\nPrey Skill: ";
					text += std::format("{:.4f}, ", actorStats->preyLevel);
					text += " ";
					text += std::format("{:.4f}, ", actorStats->preyXp);
					text += " ";
					text += std::format("{:.4f}, ", actorStats->preyThreshold);
				}

				if (VoreDataEntry* actorData = VoreData::IsValidGet(charId)) {
					text += "\nHas vore data: true";
					text += "\nAlive: ";
					text += actorData->aAlive ? "true" : "false";
					text += "\nSize: ";
					text += std::format("{:.4f}", actorData->aSize);

					if (VoreData::IsPrey(charId)) {
						WritePreyData(text, charId, false, false);
					} else {
						WriteStats(text, actr);
					}

					if (actorData->prey.size() > 0) {
						text += "\n\nPreys: ";
						for (auto& el : actorData->prey) {
							if (VoreDataEntry* pyData = VoreData::IsValidGet(el)) {
								text += Name::GetName(pyData->get());
								text += " ";
							} else {
								flog::critical("Found an illegal prey {}", Name::GetName(el));
							}
						}
						/*text += "\nTotal burden: ";
						text += std::format("{:.2f}", actorData.pdFullBurden + actorData.aWeight);*/
						if (VoreSettings::ui_show_struggle_sliders) {
							text += "\nStruggle sliders\n";
							for (uint8_t stSlId = 0; stSlId < 5; stSlId++) {
								text += std::format("{:.1f}, {:.1f}\n", actorData->pdStruggleSliders[stSlId], actorData->pdStruggleGoal[stSlId]);
							}
						}
						if (VoreSettings::ui_show_wg) {
							text += "\nStruggle sliders\n";
							text += std::format("WG {:.2f}, PERMA {:.2f}, SIZE {:.2f}\n", actorData->pdFat, actorData->pdFatgrowth, actorData->pdSizegrowth);
							text += std::format("{:.2f}, {:.2f}, {:.2f}, {:.2f}\n", actorData->pdGrowthLocus[0], actorData->pdGrowthLocus[1], actorData->pdGrowthLocus[2], actorData->pdGrowthLocus[3]);
						}
					}

					RE::FormID playerId = RE::PlayerCharacter::GetSingleton()->GetFormID();

					if (charId != playerId) {
						// char is player's prey
						if (actorData->pred == playerId) {
							if (actorData->aAlive) {
								text += "\nTalk [";
								text += KeyUtil::Interpreter::GetKeyName(VoreSettings::k_menu_1);
								text += "]\nSwitch Lethality [";
								text += KeyUtil::Interpreter::GetKeyName(VoreSettings::k_menu_2);
								text += "]";
							} else if (actorData->pyDigestProgress == 100.0) {
								text += "\nReform [";
								text += KeyUtil::Interpreter::GetKeyName(VoreSettings::k_menu_1);
							}
							if (Core::CanBeRegurgitated(actorData)) {
								text += "\nRelease [";
								text += KeyUtil::Interpreter::GetKeyName(VoreSettings::k_menu_3);
								_iAllowReg = true;
							}
							if (actorData->aAlive && actorData->pyLocus == lStomach && Core::CanMoveToLocus(playerId, charId, lBowel, lStomach)) {
								text += "]\nFull tour [";
								text += KeyUtil::Interpreter::GetKeyName(VoreSettings::k_sw_menu);
								_iFullTour = true;
							}
							text += "]";
							_iMode = kPrey;
						} else if (VoreData::IsValid(playerId)) {
							if (VoreData::Data[playerId].pred == charId) {
								text += "\nTalk [";
								text += KeyUtil::Interpreter::GetKeyName(VoreSettings::k_menu_1);
								text += "]\nStruggle/Stop [";
								text += KeyUtil::Interpreter::GetKeyName(VoreSettings::k_menu_2);
								text += "]";
								_iMode = kPred;
							} else if (VoreData::Data[playerId].pred == actorData->pred && actorData->pred) {
								if (actorData->aAlive) {
									text += "\nTalk [";
									text += KeyUtil::Interpreter::GetKeyName(VoreSettings::k_menu_1);
									text += "]\nTry To swallow [";
									text += KeyUtil::Interpreter::GetKeyName(VoreSettings::k_menu_2);
									text += "]";
								}
								_iMode = kSameLocus;
							}
						}
					}
					text += "\n";

				} else {
					text += "\nHas vore data: false";
					text += "\nAlive: ";
					text += actr->IsDead() ? "false" : "true";
					text += "\nSize: ";
					text += std::format("{:.4f}, {:.4f}", GetObjectSize(actr), GetModelScale(actr));

					WriteStats(text, actr);
					text += "\n";
				}

				//talk
				//release
				//digest
				//back
				//try to swallow

				text += "\nBack [";
				text += KeyUtil::Interpreter::GetKeyName(VoreSettings::k_i_menu);
				text += "]";

				thisMenu->SetText();
				break;
			}
		}
	}

	void VoreMenu::WriteSwallowMenu()
	{
		text = "Player preferences:\n\n";

		text += "Current modes:\n\n";
		text += "Swallow: ";
		text += PlayerPrefs::GetLocStr(PlayerPrefs::voreLoc);
		text += " [";
		text += KeyUtil::Interpreter::GetKeyName(VoreSettings::k_menu_1);
		text += "]\nLethality: ";
		text += PlayerPrefs::GetTypeStr(PlayerPrefs::voreType);
		text += " [";
		text += KeyUtil::Interpreter::GetKeyName(VoreSettings::k_menu_2);
		text += "]\nRelease: ";
		text += PlayerPrefs::GetLocStr(PlayerPrefs::regLoc);
		text += " [";
		text += KeyUtil::Interpreter::GetKeyName(VoreSettings::k_menu_3);
		text += "]";

		SetText();
	}

	void VoreMenu::SetMenuMode(VoreMenuMode mode, bool ignoreInfoCrosshair)
	{
		if (mode == kNone) {
			return;
		}
		if (!_exists) {
			if (_loadedOnce) {
				Show();
			} else {
				_setModeAfterShow = mode;
				Show();
				return;
			}
		}
		auto thisMenu = GetVoreMenu();
		if (!thisMenu) {
			flog::critical("Cannot switch menu states - no menu found!");
			return;
		}
		if (!ignoreInfoCrosshair) {
			_infoTarget = {};
		}
		// return to default/predprey menu
		if (mode > VoreMenuMode::kPredPrey && _menuMode == mode) {
			SetMenuMode(VoreMenuMode::kDefault);
			return;
		}
		_menuMode = mode;
		switch (mode) {
		case VoreMenuMode::kDefault:
			{
				RE::FormID playerId = RE::PlayerCharacter::GetSingleton()->GetFormID();
				if (VoreData::IsPred(playerId, true) || VoreData::IsPrey(playerId)) {
					SetMenuMode(VoreMenuMode::kPredPrey);
				} else {
					SetMenuVisibilityMode(false);
				}
				break;
			}
		case VoreMenuMode::kPredPrey:
			SetMenuVisibilityMode(true);
			Update();
			break;
		case VoreMenuMode::kInfo:
			{
				// get target or player
				if (!ignoreInfoCrosshair) {
					RE::TESObjectREFR* target = Utils::GetCrosshairObject();
					if (!target) {
						target = RE::PlayerCharacter::GetSingleton();
					}
					_infoTarget = target->GetHandle();
				}
				_firstInfoUpdate = true;
				SetMenuVisibilityMode(true);
				Update();
				break;
			}
		case VoreMenuMode::kSwallow:
			SetMenuVisibilityMode(true);
			thisMenu->WriteSwallowMenu();
			break;
		default:
			flog::critical("Trying to switch to a non-existant menu mode!");
			SetMenuVisibilityMode(false);
			break;
		}
	}

	void VoreMenu::DoMenuAction(MenuAction action)
	{
		if (!_exists || _menuMode == kNone || _menuMode == kDefault) {
			return;
		}
		auto thisMenu = GetVoreMenu();
		if (!thisMenu) {
			flog::critical("Cannot switch menu states - no menu found!");
			return;
		}
		switch (_menuMode) {
		case VoreMenuMode::kPredPrey:
			{
				switch (action) {
				case (MenuAction::kMenuA1):
					_charIndex--;
					Update();
					break;
				case (MenuAction::kMenuA2):
					{
						if (_charIndex < 0) {
							_charIndex = 0;
						} else if (_charIndex >= _lastCharCount) {
							_charIndex = _lastCharCount - 1;
						}
						std::vector<RE::FormID> allCharsWithPlayer = {};

						RE::FormID playerId = RE::PlayerCharacter::GetSingleton()->GetFormID();
						if (VoreDataEntry* playerData = VoreData::IsValidGet(playerId)) {
							if (VoreData::IsPrey(playerId)) {
								allCharsWithPlayer.push_back(playerData->pred);
								std::vector<RE::FormID> otherPrey = FilterPrey(playerData->pred, playerData->pyLocus, false);
								if (otherPrey.size() > 1) {
									for (RE::FormID& prey : otherPrey) {
										if (prey != playerId) {
											allCharsWithPlayer.push_back(prey);
										}
									}
								}
							}
							if ((VoreData::IsPred(playerId, true))) {
								for (auto& prey : playerData->prey) {
									allCharsWithPlayer.push_back(prey);
								}
							}

							if (_charIndex < allCharsWithPlayer.size()) {
								_infoTarget = VoreData::Data[allCharsWithPlayer[_charIndex]].get()->GetHandle();
							}
							SetMenuMode(kInfo, true);

						} 
						return;
					}
				case (MenuAction::kMenuA3):
					_charIndex++;
					Update();
					break;
				}

				if (_charIndex < 0) {
					_charIndex = 0;
				} else if (_charIndex >= _lastCharCount) {
					_charIndex = _lastCharCount - 1;
				}
				break;
			}
		case VoreMenuMode::kInfo:
			{
				switch (_iMode) {
				case InfoMode::kPred:
					switch (action) {
					case (MenuAction::kMenuA1):
						Dialogue::TalkToA(_infoTarget.get().get()->As<RE::Actor>());
						SetMenuMode(kDefault);
						break;
					case (MenuAction::kMenuA2):
						{
							if (VoreDataEntry* playerData = VoreData::IsValidGet(RE::PlayerCharacter::GetSingleton()->GetFormID())) {
								if (playerData->pyConsentLethal) {
									playerData->SetConsent(false, true);
									playerData->SetConsent(false, false);
								} else {
									playerData->SetConsent(true, true);
									playerData->SetConsent(true, false);
								}
							}
							Update();
							break;
						}
					}
					break;
				case InfoMode::kPrey:
					switch (action) {
					case (MenuAction::kMenuA1):
						if (VoreDataEntry* preyData = VoreData::IsValidGet(_infoTarget.get().get()->GetFormID())) {
							if (preyData->aAlive) {
								Dialogue::TalkToA(preyData->get()->As<RE::Actor>());
								SetMenuMode(kDefault);
							} else if (preyData->pyDigestProgress == 100.0) {
								if (VoreDataEntry* playerData = VoreData::IsValidGet(RE::PlayerCharacter::GetSingleton()->GetFormID())) {
									Core::StartReformation(preyData, playerData);
								}
							}
						}
						break;
					case (MenuAction::kMenuA2):
						{
							VoreDataEntry* preyData = VoreData::IsValidGet(_infoTarget.get().get()->GetFormID());
							VoreDataEntry* playerData = VoreData::IsValidGet(RE::PlayerCharacter::GetSingleton()->GetFormID());
							if (preyData && playerData) {
								if (preyData->aAlive) {
									if (preyData->pyDigestion == VoreDataEntry::hLethal) {
										playerData->SetDigestionAsPred(preyData->pyLocus, VoreDataEntry::hSafe, false);
									} else {
										playerData->SetDigestionAsPred(preyData->pyLocus, VoreDataEntry::hLethal, false);
									}
									Update();
								}
							}
							break;
						}
					case (MenuAction::kMenuA3):
						if (_iAllowReg) {
							Core::Regurgitate(RE::PlayerCharacter::GetSingleton(), _infoTarget.get().get()->GetFormID(), Core::rNormal, false);
							SetMenuMode(kDefault);
						}
						break;
					case (MenuAction::kMenuA4):
						if (_iFullTour) {
							if (VoreDataEntry* preyData = VoreData::IsValidGet(_infoTarget.get().get()->GetFormID())) {
								Core::MoveToLocus(_infoTarget.get().get()->GetFormID(), lBowel);
								preyData->pyLocusMovement = VoreDataEntry::mDecrease;
								Update();
							}
						}
						break;
					}
					break;
				case InfoMode::kSameLocus:
					switch (action) {
					case (MenuAction::kMenuA1):
						if (VoreDataEntry* preyData = VoreData::IsValidGet(_infoTarget.get().get()->GetFormID())) {
							if (preyData->aAlive) {
								Dialogue::TalkToA(preyData->get()->As<RE::Actor>());
								SetMenuMode(kDefault);
							}
						}
						break;
					case (MenuAction::kMenuA2):
						if (VoreDataEntry* preyData = VoreData::IsValidGet(_infoTarget.get().get()->GetFormID())) {
							if (preyData->aAlive) {
								Core::Swallow(RE::PlayerCharacter::GetSingleton(), preyData->get(), PlayerPrefs::voreLoc, PlayerPrefs::voreType, false, true);
								SetMenuMode(kDefault);
							}
						}
						break;
					}
					break;
				}
				break;
			}
		case VoreMenuMode::kSwallow:
			{
				switch (action) {
				case (MenuAction::kMenuA1):
					{
						PlayerPrefs::voreLoc = PlayerPrefs::LIter(PlayerPrefs::voreLoc, false);
						thisMenu->WriteSwallowMenu();
						break;
					}
				case (MenuAction::kMenuA2):
					{
						if (PlayerPrefs::voreType == VoreDataEntry::hLethal) {
							PlayerPrefs::voreType = VoreDataEntry::hSafe;
						} else {
							PlayerPrefs::voreType = VoreDataEntry::hLethal;
						}
						thisMenu->WriteSwallowMenu();
						break;
					}
				case (MenuAction::kMenuA3):
					{
						PlayerPrefs::regLoc = PlayerPrefs::LIter(PlayerPrefs::regLoc, true);
						thisMenu->WriteSwallowMenu();
						break;
					}
				case (MenuAction::kMenuA4):
					{
						SetModelScale(RE::PlayerCharacter::GetSingleton(), 2.0f);
						break;
					}
				}
				break;
			}
		}
	}

	void VoreMenu::Register()
	{
		auto ui = RE::UI::GetSingleton();
		if (ui) {
			ui->Register(MENU_NAME, Creator);
			flog::info("VoreMenu Registered.");
		}
	}

	// tells skyrim to actually create the menu
	void VoreMenu::Show()
	{
		auto msgQ = RE::UIMessageQueue::GetSingleton();
		if (msgQ) {
			msgQ->AddMessage(VoreMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr);
		}
	}

	// tells skyrim to hide the menu
	// unused by the mod
	void VoreMenu::Hide()
	{
		auto msgQ = RE::UIMessageQueue::GetSingleton();
		if (msgQ) {
			msgQ->AddMessage(VoreMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
		}
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

	bool VoreMenu::IsOpen()
	{
		return _exists;
	}

	void VoreMenu::OnOpen()
	{
		// Hide menu upon it's creation
		_exists = true;
		flog::info("Menu opened");
		if (_setModeAfterShow != kNone) {
			SetMenuMode(_setModeAfterShow);
			_setModeAfterShow = kNone;
		} else if (!_loadedOnce) {
			VoreMenu::SetMenuVisibilityMode(false);
		}
		_loadedOnce = true;
	}

	void VoreMenu::OnClose()
	{
		_exists = false;
		flog::info("Menu closed");
	}

	void VoreMenu::SetMenuVisibilityMode(bool a_mode)
	{
		auto menu = GetVoreMenu();
		if (menu) {
			RE::GPtr<RE::GFxMovieView> _view = menu->uiMovie;

			if (_view) {
				if (a_mode != menu->_visible) {
					_view->SetVisible(a_mode);
					menu->_visible = a_mode;
				}
			}
		}
	}
}