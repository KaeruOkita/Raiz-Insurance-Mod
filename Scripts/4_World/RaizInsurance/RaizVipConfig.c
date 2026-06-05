class RaizVipConfig
{
	ref array<string> VipSteamIds;

	void RaizVipConfig()
	{
		VipSteamIds = new array<string>;
		VipSteamIds.Insert("76561198108777229");
	}
};

class RaizVipConfigManager
{
	static ref RaizVipConfig m_Config;

	static const string DATA_FOLDER = "$profile:RaizInsuranceData/";
	static const string CONFIG_FILE = "$profile:RaizInsuranceData/VipConfig.json";

	static void Init()
	{
		MakeDirectory(DATA_FOLDER);

		if (!m_Config)
		{
			m_Config = new RaizVipConfig;
		}

		Load();
	}

	static void Load()
	{
		if (FileExist(CONFIG_FILE))
		{
			JsonFileLoader<RaizVipConfig>.JsonLoadFile(CONFIG_FILE, m_Config);

			if (!m_Config.VipSteamIds)
			{
				m_Config.VipSteamIds = new array<string>;
			}

			Save();

			Print("[RaizInsuranceMod] VipConfig carregado.");
		}
		else
		{
			Save();
			Print("[RaizInsuranceMod] VipConfig criado.");
		}
	}

	static void Save()
	{
		JsonFileLoader<RaizVipConfig>.JsonSaveFile(CONFIG_FILE, m_Config);
	}

	static bool IsVIPPlayer(string steamId)
	{
		if (!m_Config)
		{
			Init();
		}

		if (!steamId || steamId == "")
			return false;

		if (!m_Config.VipSteamIds)
			return false;

		foreach (string vipId : m_Config.VipSteamIds)
		{
			if (vipId == steamId)
				return true;
		}

		return false;
	}
};