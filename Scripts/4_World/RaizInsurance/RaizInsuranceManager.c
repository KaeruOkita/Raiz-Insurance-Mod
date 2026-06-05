class RaizInsuranceManager
{
	static ref array<ref RaizInsuredVehicle> m_InsuredVehicles;

	static const string DATA_FOLDER = "$profile:RaizInsuranceData/";
	static const string DATA_FILE = "$profile:RaizInsuranceData/insured_vehicles.json";

	static void Init()
	{
		if (!m_InsuredVehicles)
			m_InsuredVehicles = new array<ref RaizInsuredVehicle>;

		MakeDirectory(DATA_FOLDER);
		Load();

		Print("[RaizInsuranceMod] Sistema iniciado.");
	}

	static float GetVehicleEngineHealth(CarScript car)
	{
		if (!car)
			return 0;

		float engineHealth = car.GetHealth("Engine", "Health");

		if (engineHealth < 0)
			engineHealth = 0;

		return engineHealth;
	}

	static string GetTicketTier(ItemBase ticket)
	{
		if (!ticket)
			return "BASIC";

		if (ticket.IsKindOf("Raiz_RegisterTicket_VIP"))
			return "VIP";

		if (ticket.IsKindOf("Raiz_RegisterTicket_Plus"))
			return "PLUS";

		return "BASIC";
	}

	static bool IsUnlimitedTier(string tier)
	{
		if (tier == "VIP")
			return true;

		return false;
	}

	static string GetRestoreLimitText(RaizInsuredVehicle data)
	{
		if (!data)
			return "0";

		if (data.UnlimitedRestores)
			return "ILIMITADO";

		return data.RestoreCount.ToString() + " / " + data.MaxRestores.ToString();
	}

	static bool IsVehicleAlreadyInsured(PlayerBase player, CarScript car)
	{
		if (!player || !car)
			return false;

		PlayerIdentity identity = player.GetIdentity();

		if (!identity)
			return false;

		string playerId = identity.GetPlainId();

		if (car.HasRaizInsuranceUID())
		{
			foreach (RaizInsuredVehicle insuredByUID : m_InsuredVehicles)
			{
				if (!insuredByUID)
					continue;

				if (insuredByUID.InsuranceExpired)
					continue;

				if (insuredByUID.VehicleUID == car.GetRaizInsuranceUID())
					return true;
			}
		}

		string vehicleType = car.GetType();
		vector vehiclePos = car.GetPosition();

		foreach (RaizInsuredVehicle data : m_InsuredVehicles)
		{
			if (!data)
				continue;

			if (data.InsuranceExpired)
				continue;

			if (data.OwnerId == playerId && data.VehicleType == vehicleType)
			{
				float distance = vector.Distance(data.VehiclePosition, vehiclePos);

				if (distance < 5.0)
					return true;
			}
		}

		return false;
	}

	static bool RegisterVehicle(PlayerBase player, CarScript car, ItemBase ticket)
	{
		if (!player || !car || !ticket)
			return false;

		if (IsVehicleAlreadyInsured(player, car))
		{
			player.MessageStatus("Este veículo já possui seguro ativo.");
			Print("[RaizInsuranceMod] Este veículo já possui seguro ativo.");
			return false;
		}

		PlayerIdentity identity = player.GetIdentity();

		if (!identity)
			return false;

		string playerId = identity.GetPlainId();
		string tier = GetTicketTier(ticket);

		if (tier == "VIP" && !RaizVipConfigManager.IsVIPPlayer(playerId))
		{
			player.MessageStatus("Você não possui permissão para usar o Ticket VIP.");
			Print("[RaizInsuranceMod] Player tentou usar ticket VIP sem permissão: " + playerId);
			return false;
		}

		RaizInsuredVehicle data = new RaizInsuredVehicle;

		data.InsuranceID = RaizInsuranceConfigManager.GenerateInsuranceID();
		data.VehicleUID = RaizInsuranceConfigManager.GenerateVehicleUID();

		car.SetRaizInsuranceUID(data.VehicleUID);

		data.OwnerId = playerId;
		data.OwnerName = identity.GetName();

		data.VehicleType = car.GetType();

		data.VehiclePosition = car.GetPosition();
		data.VehicleOrientation = car.GetOrientation();

		data.VehicleHealth = car.GetHealth();
		data.EngineHealth = GetVehicleEngineHealth(car);

		data.FuelQuantity = car.GetFluidFraction(CarFluid.FUEL);
		data.OilQuantity = car.GetFluidFraction(CarFluid.OIL);
		data.WaterQuantity = car.GetFluidFraction(CarFluid.COOLANT);

		data.DefaultVehicleHealth = data.VehicleHealth;
		data.DefaultEngineHealth = data.EngineHealth;

		data.DefaultFuelQuantity = data.FuelQuantity;
		data.DefaultOilQuantity = data.OilQuantity;
		data.DefaultWaterQuantity = data.WaterQuantity;

		data.RegisterTime = GetGame().GetTime();
		data.LastUpdateTime = GetGame().GetTime();
		data.LastRestoreTime = 0;

		data.InsuranceTier = tier;
		data.UnlimitedRestores = IsUnlimitedTier(tier);

		data.MaxRestores = RaizInsuranceConfigManager.GetMaxRestoresByTier(tier);
		data.RestoreCount = 0;

		data.CooldownHours = RaizInsuranceConfigManager.GetDefaultCooldownHours();
		data.NextRestoreAvailableTime = 0;

		data.InsuranceExpired = false;
		data.InsuranceState = "ACTIVE";
		data.CanRestore = false;

		SaveVehicleAttachments(car, data);

		m_InsuredVehicles.Insert(data);

		Save();

		player.MessageStatus("Veículo registrado no seguro. Plano: " + data.InsuranceTier + " | ID: " + data.InsuranceID);

		Print("[RaizInsuranceMod] Seguro salvo: " + data.InsuranceID + " | VehicleUID: " + data.VehicleUID + " | Plano: " + data.InsuranceTier + " | Dono: " + data.OwnerName + " | Veículo: " + data.VehicleType);

		return true;
	}

	static bool IsStandardVehicleAttachment(EntityAI attachment)
	{
		if (!attachment)
			return false;

		if (attachment.IsKindOf("CarWheel"))
			return true;

		if (attachment.IsKindOf("CarBattery"))
			return true;

		if (attachment.IsKindOf("TruckBattery"))
			return true;

		if (attachment.IsKindOf("SparkPlug"))
			return true;

		if (attachment.IsKindOf("GlowPlug"))
			return true;

		if (attachment.IsKindOf("CarRadiator"))
			return true;

		string itemType = attachment.GetType();

		if (itemType.Contains("Wheel"))
			return true;

		if (itemType.Contains("Battery"))
			return true;

		if (itemType.Contains("Radiator"))
			return true;

		if (itemType.Contains("SparkPlug"))
			return true;

		if (itemType.Contains("GlowPlug"))
			return true;

		if (itemType.Contains("Door"))
			return true;

		if (itemType.Contains("Hood"))
			return true;

		if (itemType.Contains("Trunk"))
			return true;

		if (itemType.Contains("Headlight"))
			return true;

		if (itemType.Contains("Light"))
			return true;

		return false;
	}

	static void SaveVehicleAttachments(CarScript car, RaizInsuredVehicle data)
	{
		if (!car || !data)
			return;

		data.Attachments.Clear();

		int attachmentCount = car.GetInventory().AttachmentCount();

		for (int i = 0; i < attachmentCount; i++)
		{
			EntityAI attachment = car.GetInventory().GetAttachmentFromIndex(i);

			if (!attachment)
				continue;

			if (!IsStandardVehicleAttachment(attachment))
			{
				Print("[RaizInsuranceMod] Attachment ignorado no seguro: " + attachment.GetType());
				continue;
			}

			RaizVehicleAttachmentData attachmentData = new RaizVehicleAttachmentData;

			attachmentData.ItemType = attachment.GetType();
			attachmentData.Health = attachment.GetHealth();
			attachmentData.SlotName = "";

			data.Attachments.Insert(attachmentData);

			Print("[RaizInsuranceMod] Attachment salvo: " + attachmentData.ItemType + " | Health: " + attachmentData.Health.ToString());
		}

		Print("[RaizInsuranceMod] Attachments padrão salvos: " + data.Attachments.Count().ToString());
	}

	static string GetRestoreText(RaizInsuredVehicle data)
	{
		if (!data)
			return "NÃO";

		if (data.InsuranceExpired)
			return "EXPIRADO";

		if (!data.CanRestore)
			return "NÃO";

		if (!data.UnlimitedRestores && data.RestoreCount >= data.MaxRestores)
			return "LIMITE";

		float currentTime = GetGame().GetTime();

		if (data.NextRestoreAvailableTime > currentTime)
			return "COOLDOWN";

		return "SIM";
	}

	static void GetVehiclesForPlayer(PlayerIdentity identity, out array<string> result)
	{
		if (!result)
			result = new array<string>;

		result.Clear();

		if (!identity)
			return;

		string playerId = identity.GetPlainId();

		foreach (RaizInsuredVehicle data : m_InsuredVehicles)
		{
			if (!data)
				continue;

			if (data.OwnerId == playerId)
			{
				string restoreText = GetRestoreText(data);

				string line = data.InsuranceID + " | Plano: " + data.InsuranceTier + " | " + data.VehicleType + " | Estado: " + data.InsuranceState + " | Usos: " + GetRestoreLimitText(data) + " | Restaurar: " + restoreText;

				result.Insert(line);
			}
		}
	}

	static void GetAdminVehicleList(out array<string> result)
	{
		if (!result)
			result = new array<string>;

		result.Clear();

		foreach (RaizInsuredVehicle data : m_InsuredVehicles)
		{
			if (!data)
				continue;

			string restoreText = GetRestoreText(data);

			string line = data.InsuranceID + " | " + data.OwnerName + " | " + data.VehicleType + " | " + data.InsuranceState + " | " + restoreText;

			result.Insert(line);
		}
	}

	static void GetAdminVehicleDetails(out array<string> result)
	{
		if (!result)
			result = new array<string>;

		result.Clear();

		foreach (RaizInsuredVehicle data : m_InsuredVehicles)
		{
			if (!data)
				continue;

			string restoreText = GetRestoreText(data);

			string details = "";
			details += "ID do Seguro: " + data.InsuranceID + "\n";
			details += "VehicleUID: " + data.VehicleUID + "\n\n";

			details += "Dono: " + data.OwnerName + "\n";
			details += "SteamID: " + data.OwnerId + "\n\n";

			details += "Plano: " + data.InsuranceTier + "\n";
			details += "Restaurações ilimitadas: " + data.UnlimitedRestores.ToString() + "\n\n";

			details += "Veículo: " + data.VehicleType + "\n";
			details += "Estado: " + data.InsuranceState + "\n";
			details += "Pode restaurar: " + restoreText + "\n";
			details += "Seguro expirado: " + data.InsuranceExpired.ToString() + "\n\n";

			details += "Vida do carro atual: " + data.VehicleHealth.ToString() + "\n";
			details += "Vida do motor atual: " + data.EngineHealth.ToString() + "\n\n";

			details += "Vida padrão do carro: " + data.DefaultVehicleHealth.ToString() + "\n";
			details += "Vida padrão do motor: " + data.DefaultEngineHealth.ToString() + "\n\n";

			details += "Combustível atual: " + data.FuelQuantity.ToString() + "\n";
			details += "Óleo atual: " + data.OilQuantity.ToString() + "\n";
			details += "Água/Radiador atual: " + data.WaterQuantity.ToString() + "\n\n";

			details += "Posição atual: " + data.VehiclePosition.ToString() + "\n";
			details += "Orientação atual: " + data.VehicleOrientation.ToString() + "\n\n";

			details += "Restaurações usadas: " + GetRestoreLimitText(data) + "\n";
			details += "Cooldown em horas: " + data.CooldownHours.ToString() + "\n";
			details += "Próxima restauração disponível em: " + data.NextRestoreAvailableTime.ToString() + "\n\n";

			details += "Peças salvas: " + data.Attachments.Count().ToString();

			result.Insert(details);
		}
	}

	static int GetRealInsuranceIndex(PlayerIdentity identity, int playerListIndex)
	{
		if (!identity)
			return -1;

		string playerId = identity.GetPlainId();
		int visibleIndex = 0;

		for (int i = 0; i < m_InsuredVehicles.Count(); i++)
		{
			RaizInsuredVehicle data = m_InsuredVehicles.Get(i);

			if (!data)
				continue;

			if (data.OwnerId == playerId)
			{
				if (visibleIndex == playerListIndex)
					return i;

				visibleIndex++;
			}
		}

		return -1;
	}

	static void Save()
	{
		JsonFileLoader<array<ref RaizInsuredVehicle>>.JsonSaveFile(DATA_FILE, m_InsuredVehicles);
	}

	static void Load()
	{
		if (FileExist(DATA_FILE))
		{
			JsonFileLoader<array<ref RaizInsuredVehicle>>.JsonLoadFile(DATA_FILE, m_InsuredVehicles);

			foreach (RaizInsuredVehicle data : m_InsuredVehicles)
			{
				if (!data)
					continue;

				if (data.InsuranceTier == "")
					data.InsuranceTier = "BASIC";

				if (data.InsuranceTier == "VIP")
					data.UnlimitedRestores = true;

				if (data.VehicleUID == "")
					data.VehicleUID = RaizInsuranceConfigManager.GenerateVehicleUID();
			}

			Save();

			Print("[RaizInsuranceMod] Seguros carregados: " + m_InsuredVehicles.Count());
		}
		else
		{
			Save();
			Print("[RaizInsuranceMod] Arquivo de seguros criado.");
		}
	}
};