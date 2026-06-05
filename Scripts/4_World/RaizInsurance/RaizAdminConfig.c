class RaizAdminConfig
{
	ref array<string> Admins;

	void RaizAdminConfig()
	{
		Admins = new array<string>;
	}
};

class RaizAdminConfigManager
{
	static ref RaizAdminConfig m_Config;

	static const string DATA_FOLDER = "$profile:RaizInsuranceData/";
	static const string CONFIG_FILE = "$profile:RaizInsuranceData/AdminConfig.json";

	static void Init()
	{
		MakeDirectory(DATA_FOLDER);

		if (!m_Config)
		{
			m_Config = new RaizAdminConfig;
		}

		Load();
	}

	static void Load()
	{
		if (FileExist(CONFIG_FILE))
		{
			JsonFileLoader<RaizAdminConfig>.JsonLoadFile(CONFIG_FILE, m_Config);
			Print("[RaizInsuranceMod] AdminConfig carregado.");
		}
		else
		{
			m_Config.Admins.Insert("76561198108777229");
			Save();
			Print("[RaizInsuranceMod] AdminConfig criado.");
		}
	}

	static void Save()
	{
		JsonFileLoader<RaizAdminConfig>.JsonSaveFile(CONFIG_FILE, m_Config);
	}

	static bool IsAdmin(PlayerIdentity identity)
	{
		if (!identity || !m_Config || !m_Config.Admins)
			return false;

		string playerId = identity.GetPlainId();

		foreach (string adminId : m_Config.Admins)
		{
			if (adminId == playerId)
				return true;
		}

		return false;
	}
};