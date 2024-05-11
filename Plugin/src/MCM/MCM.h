#pragma once

namespace MCM
{
	class Settings
	{
	public:
		class General
		{
		public:
			inline static bool bEnable{ true };
		};

		class Setting
		{
		public:
			inline static bool bAidWeight{ true };
			inline static bool bAmmoWeight{ true };
			inline static bool bConsole{ true };
			inline static bool bEnemyMarkers{ true };
			inline static bool bFastTravel{ true };
			inline static bool bLocationMarkers{ true };
			inline static bool bMenuSaving{ true };
			inline static bool bQuickSaveLoad{ true };
			inline static bool bSaveOnLevel{ true };
			inline static bool bSaveOnPip{ true };
			inline static bool bSaveOnSleep{ true };
			inline static bool bSaveOnTravel{ true };
			inline static bool bSaveOnWorkshop{ true };
			inline static bool bSurvivalLock{ true };
			inline static bool bToggleGodMode{ true };
		};

		static void Update()
		{
			if (m_FirstRun)
			{
				m_FirstRun = false;
			}

			m_ini_base.LoadFile("Data/MCM/Config/BakaSurvivalSettings/settings.ini");
			m_ini_user.LoadFile("Data/MCM/Settings/BakaSurvivalSettings.ini");

			GetModSettingBool("General", "bEnable", General::bEnable);

			GetModSettingBool("Setting", "bAidWeight", Setting::bAidWeight);
			GetModSettingBool("Setting", "bAmmoWeight", Setting::bAmmoWeight);
			GetModSettingBool("Setting", "bConsole", Setting::bConsole);
			GetModSettingBool("Setting", "bEnemyMarkers", Setting::bEnemyMarkers);
			GetModSettingBool("Setting", "bFastTravel", Setting::bFastTravel);
			GetModSettingBool("Setting", "bLocationMarkers", Setting::bLocationMarkers);
			GetModSettingBool("Setting", "bMenuSaving", Setting::bMenuSaving);
			GetModSettingBool("Setting", "bQuickSaveLoad", Setting::bQuickSaveLoad);
			GetModSettingBool("Setting", "bSaveOnLevel", Setting::bSaveOnLevel);
			GetModSettingBool("Setting", "bSaveOnPip", Setting::bSaveOnPip);
			GetModSettingBool("Setting", "bSaveOnSleep", Setting::bSaveOnSleep);
			GetModSettingBool("Setting", "bSaveOnTravel", Setting::bSaveOnTravel);
			GetModSettingBool("Setting", "bSaveOnWorkshop", Setting::bSaveOnWorkshop);
			GetModSettingBool("Setting", "bSurvivalLock", Setting::bSurvivalLock);
			GetModSettingBool("Setting", "bToggleGodMode", Setting::bToggleGodMode);

			m_ini_base.Reset();
			m_ini_user.Reset();
		}

		inline static bool m_FirstRun{ true };

	private:
		static void GetModSettingBool(const std::string& a_section, const std::string& a_setting, bool& a_value)
		{
			auto base = m_ini_base.GetBoolValue(a_section.c_str(), a_setting.c_str(), a_value);
			auto user = m_ini_user.GetBoolValue(a_section.c_str(), a_setting.c_str(), base);
			a_value = user;
		}

		inline static CSimpleIniA m_ini_base{ true };
		inline static CSimpleIniA m_ini_user{ true };
	};
}
