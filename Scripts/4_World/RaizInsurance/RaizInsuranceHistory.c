class RaizRestoreHistoryEntry
{
	string InsuranceID;
	string VehicleUID;

	string OwnerId;
	string OwnerName;

	string VehicleType;
	string InsuranceTier;

	int RestoreNumber;
	float RestoreGameTime;

	vector RestorePosition;

	bool RestoredByAdmin;

	string AdminId;
	string AdminName;

	void RaizRestoreHistoryEntry()
	{
		InsuranceID = "";
		VehicleUID = "";

		OwnerId = "";
		OwnerName = "";

		VehicleType = "";
		InsuranceTier = "";

		RestoreNumber = 0;
		RestoreGameTime = 0;

		RestorePosition = "0 0 0";

		RestoredByAdmin = false;

		AdminId = "";
		AdminName = "";
	}
};

class RaizInsuranceHistoryManager
{
	static ref array<ref RaizRestoreHistoryEntry> m_History;

	static const string DATA_FOLDER = "$profile:RaizInsuranceData/";
	static const string HISTORY_FILE = "$profile:RaizInsuranceData/RestoreHistory.json";

	static void Init()
	{
		if (!m_History)
		{
			m_History = new array<ref RaizRestoreHistoryEntry>;
		}

		MakeDirectory(DATA_FOLDER);
		Load();

		Print("[RaizInsuranceMod] Histórico de restaurações iniciado.");
	}

	static void Load()
	{
		if (FileExist(HISTORY_FILE))
		{
			JsonFileLoader<array<ref RaizRestoreHistoryEntry>>.JsonLoadFile(HISTORY_FILE, m_History);

			if (!m_History)
			{
				m_History = new array<ref RaizRestoreHistoryEntry>;
			}

			Print("[RaizInsuranceMod] Histórico carregado: " + m_History.Count().ToString());
		}
		else
		{
			Save();
			Print("[RaizInsuranceMod] Arquivo RestoreHistory.json criado.");
		}
	}

	static void Save()
	{
		if (!m_History)
		{
			m_History = new array<ref RaizRestoreHistoryEntry>;
		}

		JsonFileLoader<array<ref RaizRestoreHistoryEntry>>.JsonSaveFile(HISTORY_FILE, m_History);
	}

	static void AddPlayerRestoreLog(RaizInsuredVehicle data, vector restorePosition)
	{
		AddRestoreLog(data, restorePosition, false, "", "");
	}

	static void AddAdminRestoreLog(RaizInsuredVehicle data, vector restorePosition, PlayerIdentity adminIdentity)
	{
		string adminId = "";
		string adminName = "";

		if (adminIdentity)
		{
			adminId = adminIdentity.GetPlainId();
			adminName = adminIdentity.GetName();
		}

		AddRestoreLog(data, restorePosition, true, adminId, adminName);
	}

	static void AddRestoreLog(RaizInsuredVehicle data, vector restorePosition, bool restoredByAdmin, string adminId, string adminName)
	{
		if (!data)
			return;

		if (!m_History)
		{
			Init();
		}

		RaizRestoreHistoryEntry entry = new RaizRestoreHistoryEntry;

		entry.InsuranceID = data.InsuranceID;
		entry.VehicleUID = data.VehicleUID;

		entry.OwnerId = data.OwnerId;
		entry.OwnerName = data.OwnerName;

		entry.VehicleType = data.VehicleType;
		entry.InsuranceTier = data.InsuranceTier;

		entry.RestoreNumber = data.RestoreCount;
		entry.RestoreGameTime = GetGame().GetTime();

		entry.RestorePosition = restorePosition;

		entry.RestoredByAdmin = restoredByAdmin;

		entry.AdminId = adminId;
		entry.AdminName = adminName;

		m_History.Insert(entry);
		Save();

		Print("[RaizInsuranceMod] Histórico salvo | Seguro: " + data.InsuranceID + " | Veículo: " + data.VehicleType + " | Restauração: " + entry.RestoreNumber.ToString());
	}
};