#pragma once

namespace MCM
{
	class Settings
	{
	public:
		class General
		{
		public:
			inline static REX::INI::Bool bEnable{ "Genreal", "bEnable", true };
		};

		class Setting
		{
		public:
			inline static REX::INI::Bool bAidWeight{ "Setting", "bAidWeight", true };
			inline static REX::INI::Bool bAmmoWeight{ "Setting", "bAmmoWeight", true };
			inline static REX::INI::Bool bConsole{ "Setting", "bConsole", true };
			inline static REX::INI::Bool bEnemyMarkers{ "Setting", "bEnemyMarkers", true };
			inline static REX::INI::Bool bFastTravel{ "Setting", "bFastTravel", true };
			inline static REX::INI::Bool bLocationMarkers{ "Setting", "bLocationMarkers", true };
			inline static REX::INI::Bool bMenuSaving{ "Setting", "bMenuSaving", true };
			inline static REX::INI::Bool bQuickSaveLoad{ "Setting", "bQuickSaveLoad", true };
			inline static REX::INI::Bool bSaveOnLevel{ "Setting", "bSaveOnLevel", true };
			inline static REX::INI::Bool bSaveOnPip{ "Setting", "bSaveOnPip", true };
			inline static REX::INI::Bool bSaveOnSleep{ "Setting", "bSaveOnSleep", true };
			inline static REX::INI::Bool bSaveOnTravel{ "Setting", "bSaveOnTravel", true };
			inline static REX::INI::Bool bSaveOnWorkshop{ "Setting", "bSaveOnWorkshop", true };
			inline static REX::INI::Bool bSurvivalLock{ "Setting", "bSurvivalLock", true };
			inline static REX::INI::Bool bToggleGodMode{ "Setting", "bToggleGodMode", true };
		};

		static void Update()
		{
			const auto ini = REX::INI::SettingStore::GetSingleton();
			ini->Init(
				"Data/MCM/Config/BakaSurvivalSettings/settings.ini",
				"Data/MCM/Settings/BakaSurvivalSettings.ini");
			ini->Load();
		}
	};
}
