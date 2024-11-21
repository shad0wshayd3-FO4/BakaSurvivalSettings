#include "MCM/MCM.h"

class Hooks
{
public:
	static void Install()
	{
		if (!MCM::Settings::General::bEnable.GetValue())
		{
			return;
		}

		if (!MCM::Settings::Setting::bAidWeight.GetValue())
		{
			hkcmpEAX<1321341, 0x97>::Install();  // TESWeightForm::GetFormWeight
		}

		if (!MCM::Settings::Setting::bAmmoWeight.GetValue())
		{
			hkcmpEAX<1321341, 0x121>::Install();  // TESWeightForm::GetFormWeight
		}

		if (!MCM::Settings::Setting::bConsole.GetValue())
		{
			hkcmpEAX<927099, 0x20F>::Install();  // MenuOpenHandler::HandleEvent
		}

		if (!MCM::Settings::Setting::bEnemyMarkers.GetValue())
		{
			hkcmpEAX<1475119, 0x23>::Install();  // HUDMarkerUtils::GetHostileEnemyMaxDistance
		}

		if (!MCM::Settings::Setting::bFastTravel.GetValue())
		{
			hkcmpEAX<712982, 0x323>::Install();  // PipboyMenu::PipboyMenu
			hkcmpEAX<1327120, 0x18>::Install();  // nsPipboyMenu::CheckHardcoreFastTravel
		}

		if (!MCM::Settings::Setting::bLocationMarkers.GetValue())
		{
			hkcmpEAX<1301956, 0x10>::Install();  // HUDMarkerUtils::GetLocationMaxDistance
			hkcmpEAX<1153736, 0xA7>::Install();  // CalculateCompassMarkersFunctor::UpdateLocationMarkers
		}

		if (!MCM::Settings::Setting::bMenuSaving.GetValue())
		{
			hkcmpEAX<1330449, 0xC6>::Install();   // PauseMenu::InitMainList
			hkcmpEAX<425422, 0x4C>::Install();    // PauseMenu::CheckIfSaveLoadPossible
			hkcmpEAX<540706, 0x6FE>::Install();   // BGSSaveLoadManager::DoLoadGame
			hkcmpEBX<1103363, 0x81A>::Install();  // StartMenuBase::SendGameplayOptions
		}

		if (!MCM::Settings::Setting::bQuickSaveLoad.GetValue())
		{
			hkcmpEAX<1470086, 0x71>::Install();  // QuickSaveLoadHandler::HandleEvent
		}

		if (!MCM::Settings::Setting::bSaveOnLevel.GetValue())
		{
			hkcmpEAX<1158548, 0x53>::Install();  // LevelUpMenu::~LevelUpMenu
		}

		if (!MCM::Settings::Setting::bSaveOnPip.GetValue())
		{
			hkcmpEAX<1231000, 0x18F>::Install();  // PipboyManager::OnPipboyCloseAnim
		}

		if (!MCM::Settings::Setting::bSaveOnSleep.GetValue())
		{
			hkcmpEAX<1551767, 0xCC>::Install();   // PlayerCharacter::WakeUp
			hkcmpEAX<1551767, 0x14B>::Install();  // PlayerCharacter::WakeUp
		}

		if (!MCM::Settings::Setting::bSaveOnTravel.GetValue())
		{
			hkcmpEAX<146861, 0x67D>::Install();  // PlayerCharacter::HandlePositionPlayerRequest
			hkcmpEAX<374033, 0x2B>::Install();   // PlayerCharacter::RequestQueueDoorAutosave
		}

		if (!MCM::Settings::Setting::bSaveOnWorkshop.GetValue())
		{
			hkcmpEAX<98443, 0x198>::Install();  // WorkshopMenu::~WorkshopMenu
		}

		if (!MCM::Settings::Setting::bSurvivalLock.GetValue())
		{
			// PauseMenu::CheckIfSaveLoadPossible
			static REL::Relocation<std::uintptr_t> target{ REL::ID(425422), 0x14D };
			REL::safe_fill(target.address(), REL::NOP, 0x05);

			hkmovEDX<402595, 0x28>::Install();  // ExitSurvivalModeCallback::operator()
		}

		if (!MCM::Settings::Setting::bToggleGodMode.GetValue())
		{
			hkcmpEAX<1032309, 0x35>::Install();  // PlayerCharacter::IsGodMode
			hkcmpEAX<500346, 0x35>::Install();   // PlayerCharacter::IsImmortal
			hkcmpEAX<1111932, 0x3E>::Install();  // PlayerCharacter::IsInvulnerable
			hkcmpEAX<1240293, 0x7A>::Install();  // PlayerCharacter::KillImpl
		}
	}

private:
	template <std::uint64_t ID, std::ptrdiff_t OFF>
	class hkcmpEAX
	{
	public:
		static void Install()
		{
			static REL::Relocation<std::uintptr_t> target{ REL::ID(ID), OFF };
			REL::safe_fill(target.address(), REL::NOP, 0x03);

			auto code = cmpEAX();
			assert(code.getSize() <= 0x03);
			REL::safe_write(target.address(), code.getCode(), code.getSize());
		}

	private:
		struct cmpEAX : Xbyak::CodeGenerator
		{
			cmpEAX()
			{
				cmp(eax, 7);
			}
		};
	};

	template <std::uint64_t ID, std::ptrdiff_t OFF>
	class hkcmpEBX
	{
	public:
		static void Install()
		{
			static REL::Relocation<std::uintptr_t> target{ REL::ID(ID), OFF };
			REL::safe_fill(target.address(), REL::NOP, 0x03);

			auto code = cmpEBX();
			assert(code.getSize() <= 0x03);
			REL::safe_write(target.address(), code.getCode(), code.getSize());
		}

	private:
		struct cmpEBX : Xbyak::CodeGenerator
		{
			cmpEBX()
			{
				cmp(ebx, 7);
			}
		};
	};

	template <std::uint64_t ID, std::ptrdiff_t OFF>
	class hkmovEDX
	{
	public:
		static void Install()
		{
			static REL::Relocation<std::uintptr_t> target{ REL::ID(ID), OFF };
			REL::safe_fill(target.address(), REL::NOP, 0x05);

			auto code = movEDX();
			assert(code.getSize() <= 0x05);
			REL::safe_write(target.address(), code.getCode(), code.getSize());
		}

	private:
		struct movEDX : Xbyak::CodeGenerator
		{
			movEDX()
			{
				mov(edx, 0);
			}
		};
	};
};

namespace
{
	void MessageCallback(F4SE::MessagingInterface::Message* a_msg)
	{
		switch (a_msg->type)
		{
		case F4SE::MessagingInterface::kPostLoad:
		{
			MCM::Settings::Update();
			Hooks::Install();
			break;
		}
		default:
			break;
		}
	}
}

F4SEPluginLoad(const F4SE::LoadInterface* a_F4SE)
{
	F4SE::Init(a_F4SE);
	F4SE::GetMessagingInterface()->RegisterListener(MessageCallback);

	return true;
}
