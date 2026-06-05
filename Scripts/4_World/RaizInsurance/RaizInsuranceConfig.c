class RaizInsuranceConfig
{
	int NextInsuranceID;
	int NextVehicleUID;

	int BasicMaxRestores;
	int PlusMaxRestores;
	int VIPMaxRestores;

	int DefaultMaxRestores;
	float DefaultCooldownHours;

	bool ExpireAfterMaxRestores;

	ref array<string> VIPPlayers;

	void RaizInsuranceConfig()
	{
		NextInsuranceID = 1;
		NextVehicleUID = 1;

		BasicMaxRestores = 1;
		PlusMaxRestores = 3;
		VIPMaxRestores = 999999;

		DefaultMaxRestores = 1;
		DefaultCooldownHours = 24.0;

		ExpireAfterMaxRestores = true;

		VIPPlayers = new array<string>;
		VIPPlayers.Insert("76561198000000000");
	}
};

class RaizInsuranceConfigManager
{
	static ref RaizInsuranceConfig m_Config;

	static const string DATA_FOLDER = "$profile:RaizInsuranceData/";
	static const string CONFIG_FILE = "$profile:RaizInsuranceData/InsuranceConfig.json";

	static void Init()
	{
		MakeDirectory(DATA_FOLDER);

		if (!m_Config)
		{
			m_Config = new RaizInsuranceConfig;
		}

		Load();
	}

	static void Load()
	{
		if (FileExist(CONFIG_FILE))
		{
			JsonFileLoader<RaizInsuranceConfig>.JsonLoadFile(CONFIG_FILE, m_Config);

			if (!m_Config.VIPPlayers)
			{
				m_Config.VIPPlayers = new array<string>;
			}

			if (m_Config.NextInsuranceID <= 0)
			{
				m_Config.NextInsuranceID = 1;
			}

			if (m_Config.NextVehicleUID <= 0)
			{
				m_Config.NextVehicleUID = 1;
			}

			Save();

			Print("[RaizInsuranceMod] InsuranceConfig carregado.");
		}
		else
		{
			Save();
			Print("[RaizInsuranceMod] InsuranceConfig criado.");
		}
	}

	static void Save()
	{
		JsonFileLoader<RaizInsuranceConfig>.JsonSaveFile(CONFIG_FILE, m_Config);
	}

	static string GenerateInsuranceID()
	{
		if (!m_Config)
		{
			Init();
		}

		int idNumber = m_Config.NextInsuranceID;
		m_Config.NextInsuranceID++;

		Save();

		string numberText = idNumber.ToString();

		while (numberText.Length() < 7)
		{
			numberText = "0" + numberText;
		}

		return "RZINS_" + numberText;
	}

	static string GenerateVehicleUID()
	{
		if (!m_Config)
		{
			Init();
		}

		int idNumber = m_Config.NextVehicleUID;
		m_Config.NextVehicleUID++;

		Save();

		string numberText = idNumber.ToString();

		while (numberText.Length() < 7)
		{
			numberText = "0" + numberText;
		}

		return "RZVEH_" + numberText;
	}

	static int GetDefaultMaxRestores()
	{
		if (!m_Config)
		{
			Init();
		}

		return m_Config.DefaultMaxRestores;
	}

	static int GetMaxRestoresByTier(string tier)
	{
		if (!m_Config)
		{
			Init();
		}

		if (tier == "BASIC")
		{
			return m_Config.BasicMaxRestores;
		}

		if (tier == "PLUS")
		{
			return m_Config.PlusMaxRestores;
		}

		if (tier == "VIP")
		{
			return m_Config.VIPMaxRestores;
		}

		return m_Config.DefaultMaxRestores;
	}

	static float GetDefaultCooldownHours()
	{
		if (!m_Config)
		{
			Init();
		}

		return m_Config.DefaultCooldownHours;
	}

	static bool ShouldExpireAfterMaxRestores()
	{
		if (!m_Config)
		{
			Init();
		}

		return m_Config.ExpireAfterMaxRestores;
	}

	static bool IsVIPPlayer(string steamId)
	{
		if (!m_Config)
		{
			Init();
		}

		if (!m_Config.VIPPlayers)
		{
			return false;
		}

		foreach (string allowedId : m_Config.VIPPlayers)
		{
			if (allowedId == steamId)
			{
				return true;
			}
		}

		return false;
	}
};