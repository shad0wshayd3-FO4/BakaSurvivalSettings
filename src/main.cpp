namespace MCM
{
	class Settings
	{
	public:
		class General
		{
		public:
			inline static REX::INI::Bool bEnabled{ "General", "bEnabled", true };

			inline static REX::INI::Bool bDisableConsole{ "General", "bDisableConsole", false };
			inline static REX::INI::Bool bDisableFastTravel{ "General", "bDisableFastTravel", false };
			inline static REX::INI::Bool bDisableGodMode{ "General", "bDisableGodMode", false };
			inline static REX::INI::Bool bDisableSaveAuto{ "General", "bDisableSaveAuto", false };
			inline static REX::INI::Bool bDisableSaveSelf{ "General", "bDisableSaveSelf", false };
			inline static REX::INI::Bool bEnableAlchWeight{ "General", "bEnableAlchWeight", true };
			inline static REX::INI::Bool bEnableAmmoWeight{ "General", "bEnableAmmoWeight", true };
			inline static REX::INI::Bool bEnableSleepSave{ "General", "bEnableSleepSave", true };
			inline static REX::INI::Bool bLimitCompassEnemies{ "General", "bLimitCompassEnemies", true };
			inline static REX::INI::Bool bLimitCompassLocations{ "General", "bLimitCompassLocations", true };
			inline static REX::INI::Bool bLockSurvival{ "General", "bLockSurvival", false };
		};

		static void Update()
		{
			Register();

			const auto ini = REX::INI::SettingStore::GetSingleton();
			ini->Init(
				"Data/MCM/Config/BakaSurvivalSettings/settings.ini",
				"Data/MCM/Settings/BakaSurvivalSettings.ini");
			ini->Load();

			PostUpdate();
		}

	private:
		class EventHandler :
			public REX::Singleton<EventHandler>,
			public RE::BSTEventSink<RE::MenuOpenCloseEvent>
		{
		public:
			virtual RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent& a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_sink) override
			{
				if (a_event.menuName == "PauseMenu" && !a_event.opening)
				{
					MCM::Settings::Update();
				}

				return RE::BSEventNotifyControl::kContinue;
			}
		};

		static void NotifyOfPlayerDifficultySettingChanged(std::int32_t a_old, std::int32_t a_new)
		{
			using func_t = decltype(&NotifyOfPlayerDifficultySettingChanged);
			static REL::Relocation<func_t> func{ REL::ID(2232842) };
			return func(a_old, a_new);
		}

		static void PostUpdate()
		{
			if (auto PlayerCharacter = RE::PlayerCharacter::GetSingleton())
			{
				auto idx = std::to_underlying(PlayerCharacter->GetDifficultyLevel());
				NotifyOfPlayerDifficultySettingChanged(idx, idx);
			}
		}

		static void Register()
		{
			if (bRegistered)
			{
				return;
			}

			if (auto UI = RE::UI::GetSingleton())
			{
				UI->RegisterSink<RE::MenuOpenCloseEvent>(EventHandler::GetSingleton());
				bRegistered = true;
			}
		}

		inline static bool bRegistered{ false };
	};
}

namespace Hooks
{
	namespace detail
	{
		class hkGetDifficultyLevel
		{
		public:
			virtual bool UseSurvivalLogic() = 0;

			virtual RE::DifficultyLevel GetDifficultyLevel_Impl(RE::PlayerCharacter* a_this)
			{
				if (!MCM::Settings::General::bEnabled)
				{
					return a_this->GetDifficultyLevel();
				}

				if (UseSurvivalLogic())
				{
					return RE::DifficultyLevel::kTrueSurvival;
				}

				return RE::DifficultyLevel::kVeryEasy;
			}
		};

		class hkDisableGodMode :
			public hkGetDifficultyLevel
		{
		public:
			virtual bool UseSurvivalLogic() override
			{
				return MCM::Settings::General::bDisableGodMode;
			}
		};

		class hkDisableSaveAuto :
			public hkGetDifficultyLevel
		{
		public:
			virtual bool UseSurvivalLogic() override
			{
				return MCM::Settings::General::bDisableSaveAuto;
			}
		};

		class hkLockSurvival :
			public hkGetDifficultyLevel
		{
		public:
			virtual bool UseSurvivalLogic() override
			{
				return MCM::Settings::General::bLockSurvival;
			}
		};

		static bool GodMode()
		{
			static REL::Relocation<bool*> singleton{ REL::ID(2698645) };
			return *singleton;
		}

		static bool ImmortalMode()
		{
			static REL::Relocation<bool*> singleton{ REL::ID(2698646) };
			return *singleton;
		}
	}

	class hkDisableConsole :
		public detail::hkGetDifficultyLevel,
		public REX::Singleton<hkDisableConsole>
	{
	private:
		virtual bool UseSurvivalLogic() override
		{
			return MCM::Settings::General::bDisableConsole;
		}

		static RE::DifficultyLevel GetDifficultyLevel(RE::PlayerCharacter* a_this)
		{
			return GetSingleton()->GetDifficultyLevel_Impl(a_this);
		}

		inline static REL::Hook _GetDifficultyLevel0{ REL::ID(2249425), 0x1AA, GetDifficultyLevel };  // MenuOpenHandler::HandleEvent
	};

	class hkDisableFastTravel :
		public detail::hkGetDifficultyLevel,
		public REX::Singleton<hkDisableFastTravel>
	{
	private:
		virtual bool UseSurvivalLogic() override
		{
			return MCM::Settings::General::bDisableFastTravel;
		}

		static RE::DifficultyLevel GetDifficultyLevel(RE::PlayerCharacter* a_this)
		{
			return GetSingleton()->GetDifficultyLevel_Impl(a_this);
		}

		inline static REL::Hook _GetDifficultyLevel0{ REL::ID(2224179), 0x34C, GetDifficultyLevel };  // PipboyMenu::PipboyMenu
		inline static REL::Hook _GetDifficultyLevel1{ REL::ID(2224206), 0x014, GetDifficultyLevel };  // nsPipboyMenu::CheckHardcoreFastTravel
	};

	class hkDisableGodMode0 :
		public detail::hkDisableGodMode,
		public REX::Singleton<hkDisableGodMode0>
	{
	public:
		static void Install()
		{
			static REL::Relocation target{ REL::ID(2232986) };
			target.replace_func(0x3F, IsGodMode);
	}

	private:
		static bool IsGodMode()
		{
			if (!detail::GodMode())
				return false;
			if (!RE::PlayerCharacter::GetSingleton())
				return true;
			if (!GetSingleton()->UseSurvivalLogic())
				return true;
			return false;
		}
	};

	class hkDisableGodMode1 :
		public detail::hkDisableGodMode,
		public REX::Singleton<hkDisableGodMode1>
	{
	public:
		static void Install()
		{
			static REL::Relocation target{ REL::ID(2232988) };
			target.replace_func(0x3F, IsImmortal);
		}

	private:
		static bool IsImmortal()
		{
			if (!detail::ImmortalMode())
				return false;
			if (!RE::PlayerCharacter::GetSingleton())
				return true;
			if (!GetSingleton()->UseSurvivalLogic())
				return true;
			return false;
		}
	};

	class hkDisableGodMode2 :
		public detail::hkDisableGodMode,
		public REX::Singleton<hkDisableGodMode2>
	{
	private:
		static bool IsInvulnerable(RE::PlayerCharacter* a_this)
		{
			if (detail::GodMode() && !GetSingleton()->UseSurvivalLogic())
			{
				return true;
			}

			if (a_this->GetGhost() || ((a_this->queuedTargetLoc.angle.x - RE::AITimer::fTimer()) > 0.0f))
			{
				return true;
			}

			return false;
		}

		inline static REL::HookVFT _IsInvulnerable{ RE::PlayerCharacter::VTABLE[0], 0x4, IsInvulnerable };
	};

	class hkDisableSaveAuto0 :
		public detail::hkDisableSaveAuto,
		public REX::Singleton<hkDisableSaveAuto0>
	{
	private:
		static RE::DifficultyLevel GetDifficultyLevel(RE::PlayerCharacter* a_this)
		{
			return GetSingleton()->GetDifficultyLevel_Impl(a_this);
		}

		inline static REL::Hook _GetDifficultyLevel0{ REL::ID(2223294), 0x004E, GetDifficultyLevel };  // LevelUpMenu::unk (dtor copy?)
		inline static REL::Hook _GetDifficultyLevel1{ REL::ID(2223327), 0x0057, GetDifficultyLevel };  // LevelUpMenu::~LevelUpMenu
		inline static REL::Hook _GetDifficultyLevel2{ REL::ID(2224974), 0x013E, GetDifficultyLevel };  // WorkshopMenu::~WorkshopMenu
		inline static REL::Hook _GetDifficultyLevel3{ REL::ID(2225457), 0x01A4, GetDifficultyLevel };  // PipboyManager::OnPipboyCloseAnim
		inline static REL::Hook _GetDifficultyLevel4{ REL::ID(2232905), 0x06A1, GetDifficultyLevel };  // PlayerCharacter::HandlePositionPlayerRequest
	};

	class hkDisableSaveAuto1 :
		public detail::hkDisableSaveAuto,
		public REX::Singleton<hkDisableSaveAuto1>
	{
	private:
		static void RequestQueueDoorAutosave(RE::PlayerCharacter* a_this)
		{
			auto DifficultyLevel = GetSingleton()->GetDifficultyLevel_Impl(a_this);
			switch (DifficultyLevel)
			{
			case RE::DifficultyLevel::kTrueSurvival:
				break;
			default:
				a_this->doorAutosaveQueued = true;
				break;
			}
		}

		inline static REL::Hook _RequestQueueDoorAutosave0{ REL::ID(2198697), 0x1B4, RequestQueueDoorAutosave };  // TESObjectDOOR::DoorTeleportPlayerArrivalCallback
	};

	class hkDisableSaveAuto2 :
		public detail::hkDisableSaveAuto,
		public REX::Singleton<hkDisableSaveAuto2>
	{
	private:
		static RE::DifficultyLevel GetMenuDifficultyLevel(RE::StartMenuBase* a_this)
		{
			if (auto PlayerCharacter = RE::PlayerCharacter::GetSingleton())
			{
				_GetMenuDifficultyLevel0(a_this);
				return GetSingleton()->GetDifficultyLevel_Impl(PlayerCharacter);
			}

			return _GetMenuDifficultyLevel0(a_this);
		}

		inline static REL::Hook _GetMenuDifficultyLevel0{ REL::ID(4483089), 0x3A, GetMenuDifficultyLevel };  // StartMenuBase::SendGameplayOptions
	};

	class hkDisableSaveAuto3 :
		public detail::hkDisableSaveAuto,
		public REX::Singleton<hkDisableSaveAuto3>
	{
	private:
		static bool SetMember(void* a_this, void* a_data, const char* a_name, Scaleform::GFx::Value& a_value, bool a_isObj)
		{
			if (auto UI = RE::UI::GetSingleton())
			{
				if (auto MainMenu = UI->GetMenu<RE::MainMenu>())
				{
					a_value = std::to_underlying(MainMenu->GetMenuDifficultyLevel());
				}
			}

			return _SetMember0(a_this, a_data, a_name, a_value, a_isObj);
		}

		inline static REL::Hook _SetMember0{ REL::ID(4483089), 0xDB, SetMember };  // StartMenuBase::SendGameplayOptions
	};

	class hkDisableSaveAuto4 :
		public detail::hkDisableSaveAuto,
		public REX::Singleton<hkDisableSaveAuto4>
	{
	public:
		static void Install()
		{
			static REL::Relocation target{ REL::ID(2232890) };
			static constexpr auto  TARGET_ADDR{ 0x122 };
			static constexpr auto  TARGET_RETN{ 0x163 };
			static constexpr auto  TARGET_FILL{ TARGET_RETN - TARGET_ADDR };
			target.write_fill<TARGET_ADDR>(REL::NOP, TARGET_FILL);
		}

	private:
		static void SendWaitStopEvent(bool a_interrupted)
		{
			_SendWaitStopEvent0(a_interrupted);
			if (bSaveOnWait->GetBinary())
			{
				if (GetSingleton()->UseSurvivalLogic())
				{
					return;
				}

				if (auto BGSSaveLoadManager = RE::BGSSaveLoadManager::GetSingleton())
				{
					BGSSaveLoadManager->QueueSaveLoadTask(RE::BGSSaveLoadManager::QUEUED_TASK::kAutoSave);
				}
			}
		}

		inline static REL::Relocation<RE::SettingT<RE::INISettingCollection>*> bSaveOnWait{ REL::ID(122528) };
		inline static REL::Hook _SendWaitStopEvent0{ REL::ID(2232890), 0x11D, SendWaitStopEvent };  // PlayerCharacter::WakeUp
	};

	class hkDisableSaveSelf :
		public detail::hkGetDifficultyLevel,
		public REX::Singleton<hkDisableSaveSelf>
	{
	private:
		virtual bool UseSurvivalLogic() override
		{
			return MCM::Settings::General::bDisableSaveSelf;
		}

		static RE::DifficultyLevel GetDifficultyLevel(RE::PlayerCharacter* a_this)
		{
			return GetSingleton()->GetDifficultyLevel_Impl(a_this);
		}

		inline static REL::Hook _GetDifficultyLevel0{ REL::ID(2223964), 0x00C6, GetDifficultyLevel };  // PauseMenu::InitMainList
		inline static REL::Hook _GetDifficultyLevel1{ REL::ID(2223965), 0x004D, GetDifficultyLevel };  // PauseMenu::CheckIfSaveLoadPossible
		inline static REL::Hook _GetDifficultyLevel2{ REL::ID(2228040), 0x1469, GetDifficultyLevel };  // BGSSaveLoadManager::DoLoadGame
		inline static REL::Hook _GetDifficultyLevel3{ REL::ID(2249427), 0x006C, GetDifficultyLevel };  // QuickSaveLoadHandler::HandleEvent
	};

	class hkEnableAlchWeight :
		public detail::hkGetDifficultyLevel,
		public REX::Singleton<hkEnableAlchWeight>
	{
	private:
		virtual bool UseSurvivalLogic() override
		{
			return MCM::Settings::General::bEnableAlchWeight;
		}

		static RE::DifficultyLevel GetDifficultyLevel(RE::PlayerCharacter* a_this)
		{
			return GetSingleton()->GetDifficultyLevel_Impl(a_this);
		}

		inline static REL::Hook _GetDifficultyLevel0{ REL::ID(2193446), 0x8E, GetDifficultyLevel };  // TESWeightForm::GetFormWeight
	};

	class hkEnableAmmoWeight :
		public detail::hkGetDifficultyLevel,
		public REX::Singleton<hkEnableAmmoWeight>
	{
	private:
		virtual bool UseSurvivalLogic() override
		{
			return MCM::Settings::General::bEnableAmmoWeight;
		}

		static RE::DifficultyLevel GetDifficultyLevel(RE::PlayerCharacter* a_this)
		{
			return GetSingleton()->GetDifficultyLevel_Impl(a_this);
		}

		inline static REL::Hook _GetDifficultyLevel0{ REL::ID(2193446), 0x110, GetDifficultyLevel };  // TESWeightForm::GetFormWeight
	};

	class hkEnableSleepSave :
		public detail::hkGetDifficultyLevel,
		public REX::Singleton<hkEnableSleepSave>
	{
	private:
		virtual bool UseSurvivalLogic() override
		{
			return MCM::Settings::General::bEnableSleepSave;
		}

		static void QueueSaveLoadTask(RE::BGSSaveLoadManager* a_this, RE::BGSSaveLoadManager::QUEUED_TASK a_task)
		{
			if (GetSingleton()->UseSurvivalLogic())
			{
				_QueueSaveLoadTask0(a_this, a_task);
			}
		}

		inline static REL::Hook _QueueSaveLoadTask0{ REL::ID(2232890), 0xDD, QueueSaveLoadTask };  // PlayerCharacter::WakeUp
	};

	class hkLimitCompassEnemies :
		public detail::hkGetDifficultyLevel,
		public REX::Singleton<hkLimitCompassEnemies>
	{
	private:
		virtual bool UseSurvivalLogic() override
		{
			return MCM::Settings::General::bLimitCompassEnemies;
		}

		static RE::DifficultyLevel GetDifficultyLevel(RE::PlayerCharacter* a_this)
		{
			return GetSingleton()->GetDifficultyLevel_Impl(a_this);
		}

		inline static REL::Hook _GetDifficultyLevel0{ REL::ID(2220612), 0x1B, GetDifficultyLevel };  // HUDMarkerUtils::GetHostileEnemyMaxDistance
	};

	class hkLimitCompassLocations :
		public detail::hkGetDifficultyLevel,
		public REX::Singleton<hkLimitCompassLocations>
	{
	private:
		virtual bool UseSurvivalLogic() override
		{
			return MCM::Settings::General::bLimitCompassLocations;
		}

		static RE::DifficultyLevel GetDifficultyLevel(RE::PlayerCharacter* a_this)
		{
			return GetSingleton()->GetDifficultyLevel_Impl(a_this);
		}

		inline static REL::Hook _GetDifficultyLevel0{ REL::ID(2220611), 0x0B, GetDifficultyLevel };  // HUDMarkerUtils::GetLocationMaxDistance
		inline static REL::Hook _GetDifficultyLevel1{ REL::ID(2220617), 0x86, GetDifficultyLevel };  // CalculateCompassMarkersFunctor::UpdateLocationMarkers
	};

	class hkLockSurvival0 :
		public detail::hkLockSurvival,
		public REX::Singleton<hkLockSurvival0>
	{
	private:
		static bool QueryStat(const RE::BSFixedString& a_name, std::int32_t& a_value)
		{
			if (GetSingleton()->UseSurvivalLogic())
			{
				return _QueryStat0(a_name, a_value);
			}

			return false;
		}

		inline static REL::Hook _QueryStat0{ REL::ID(2223965), 0x153, QueryStat };  // PauseMenu::CheckIfSaveLoadPossible
	};

	class hkLockSurvival1 :
		public detail::hkLockSurvival,
		public REX::Singleton<hkLockSurvival1>
	{
	private:
		static std::uint64_t SetVal(const RE::BSFixedString& a_name, std::int32_t a_value)
		{
			if (GetSingleton()->UseSurvivalLogic())
			{
				return _SetVal0(a_name, a_value);
			}

			return _SetVal0(a_name, 0);
		}

		inline static REL::Hook _SetVal0{ REL::ID(2223987), 0x35, SetVal };  // ExitSurvivalModeCallback::operator()
	};

	static void Install()
	{
		hkDisableSaveAuto4::Install();
	}
}

namespace
{
	void MessageCallback(F4SE::MessagingInterface::Message* a_msg)
	{
		switch (a_msg->type)
		{
		case F4SE::MessagingInterface::kPostLoad:
			Hooks::Install();
			break;
		case F4SE::MessagingInterface::kGameDataReady:
			if (static_cast<bool>(a_msg->data))
				MCM::Settings::Update();
			break;
		default:
			break;
		}
	}
}

F4SE_PLUGIN_LOAD(const F4SE::LoadInterface* a_f4se)
{
	F4SE::Init(a_f4se, { .trampoline = true, .trampolineSize = 256 });
	F4SE::GetMessagingInterface()->RegisterListener(MessageCallback);
	return true;
}
