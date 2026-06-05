class RaizInsuranceRestore
{
	static bool CanRestoreVehicle(RaizInsuredVehicle data)
	{
		if (!data)
			return false;

		if (data.InsuranceExpired)
			return false;

		if (!data.CanRestore)
			return false;

		if (!data.UnlimitedRestores)
		{
			if (data.RestoreCount >= data.MaxRestores)
				return false;
		}

		float currentTime = GetGame().GetTime();

		if (data.NextRestoreAvailableTime > currentTime)
			return false;

		return true;
	}

	static string GetRestoreStatusText(RaizInsuredVehicle data)
	{
		if (!data)
			return "INVÁLIDO";

		if (data.InsuranceExpired)
			return "EXPIRADO";

		if (!data.CanRestore)
			return "NÃO DISPONÍVEL";

		if (!data.UnlimitedRestores)
		{
			if (data.RestoreCount >= data.MaxRestores)
				return "LIMITE ATINGIDO";
		}

		float currentTime = GetGame().GetTime();

		if (data.NextRestoreAvailableTime > currentTime)
			return "EM COOLDOWN";

		return "DISPONÍVEL";
	}

	static string GetRestoreCountText(RaizInsuredVehicle data)
	{
		if (!data)
			return "0";

		if (data.UnlimitedRestores)
			return "ILIMITADO";

		return data.RestoreCount.ToString() + "/" + data.MaxRestores.ToString();
	}

	static vector GetPlayerLookSpawnPosition(PlayerBase player)
	{
		if (!player)
			return "0 0 0";

		vector playerPos = player.GetPosition();
		vector direction = player.GetDirection();

		vector spawnPos = playerPos + (direction * 6.0);

		float surfaceY = GetGame().SurfaceY(spawnPos[0], spawnPos[2]);
		spawnPos[1] = surfaceY + 0.25;

		return spawnPos;
	}

	static void ApplyVehicleFluids(CarScript car, RaizInsuredVehicle data)
	{
		if (!car || !data)
			return;

		float fuelCapacity = car.GetFluidCapacity(CarFluid.FUEL);
		float oilCapacity = car.GetFluidCapacity(CarFluid.OIL);
		float waterCapacity = car.GetFluidCapacity(CarFluid.COOLANT);

		if (fuelCapacity > 0)
			car.Fill(CarFluid.FUEL, fuelCapacity * data.DefaultFuelQuantity);

		if (oilCapacity > 0)
			car.Fill(CarFluid.OIL, oilCapacity * data.DefaultOilQuantity);

		if (waterCapacity > 0)
			car.Fill(CarFluid.COOLANT, waterCapacity * data.DefaultWaterQuantity);
	}

	static void ApplyVehicleHealth(CarScript car, RaizInsuredVehicle data)
	{
		if (!car || !data)
			return;

		if (data.DefaultVehicleHealth > 0)
			car.SetHealth("", "", data.DefaultVehicleHealth);

		if (data.DefaultEngineHealth > 0)
			car.SetHealth("Engine", "Health", data.DefaultEngineHealth);
	}

	static void RestoreVehicleAttachments(CarScript car, RaizInsuredVehicle data)
	{
		if (!car || !data || !data.Attachments)
			return;

		foreach (RaizVehicleAttachmentData attachmentData : data.Attachments)
		{
			if (!attachmentData)
				continue;

			if (attachmentData.ItemType == "")
				continue;

			EntityAI attachment = car.GetInventory().CreateAttachment(attachmentData.ItemType);

			if (!attachment)
			{
				Print("[RaizInsuranceMod] Falha ao restaurar peça: " + attachmentData.ItemType);
				continue;
			}

			if (attachmentData.Health > 0)
				attachment.SetHealth("", "", attachmentData.Health);

			Print("[RaizInsuranceMod] Peça restaurada: " + attachmentData.ItemType);
		}
	}

	static void UpdateInsuranceToNewVehicle(RaizInsuredVehicle data, CarScript car)
	{
		if (!data || !car)
			return;

		data.VehiclePosition = car.GetPosition();
		data.VehicleOrientation = car.GetOrientation();

		data.VehicleHealth = car.GetHealth();
		data.EngineHealth = car.GetHealth("Engine", "Health");

		data.FuelQuantity = car.GetFluidFraction(CarFluid.FUEL);
		data.OilQuantity = car.GetFluidFraction(CarFluid.OIL);
		data.WaterQuantity = car.GetFluidFraction(CarFluid.COOLANT);

		data.LastUpdateTime = GetGame().GetTime();
		data.CanRestore = false;

		if (!data.InsuranceExpired)
			data.InsuranceState = "ACTIVE";

		if (data.VehicleUID != "")
		{
			car.SetRaizInsuranceUID(data.VehicleUID);
			RaizInsuranceVehicleRegistry.RegisterVehicle(data.VehicleUID, car);
		}
	}

	static CarScript SpawnVehicle(PlayerBase player, RaizInsuredVehicle data)
	{
		if (!player || !data)
			return null;

		vector spawnPos = GetPlayerLookSpawnPosition(player);
		vector spawnOri = player.GetOrientation();

		Object obj = GetGame().CreateObject(data.VehicleType, spawnPos, false, true, true);

		if (!obj)
		{
			Print("[RaizInsuranceMod] Falha ao criar veículo: " + data.VehicleType);
			return null;
		}

		CarScript car = CarScript.Cast(obj);

		if (!car)
		{
			GetGame().ObjectDelete(obj);
			Print("[RaizInsuranceMod] Objeto criado não é CarScript: " + data.VehicleType);
			return null;
		}

		car.SetPosition(spawnPos);
		car.SetOrientation(spawnOri);

		if (data.VehicleUID != "")
			car.SetRaizInsuranceUID(data.VehicleUID);

		ApplyVehicleHealth(car, data);
		ApplyVehicleFluids(car, data);
		RestoreVehicleAttachments(car, data);

		car.SetSynchDirty();

		UpdateInsuranceToNewVehicle(data, car);

		return car;
	}

	static void RegisterRestore(RaizInsuredVehicle data)
	{
		if (!data)
			return;

		data.RestoreCount++;
		data.LastRestoreTime = GetGame().GetTime();

		if (data.UnlimitedRestores)
		{
			data.NextRestoreAvailableTime = 0;
		}
		else
		{
			float cooldownMs = data.CooldownHours * 60.0 * 60.0 * 1000.0;
			data.NextRestoreAvailableTime = GetGame().GetTime() + cooldownMs;
		}

		data.CanRestore = false;

		if (data.InsuranceExpired)
		{
			RaizInsuranceManager.Save();
			return;
		}

		if (!data.UnlimitedRestores)
		{
			if (data.RestoreCount >= data.MaxRestores)
			{
				if (RaizInsuranceConfigManager.ShouldExpireAfterMaxRestores())
				{
					data.InsuranceExpired = true;
					data.InsuranceState = "EXPIRED";

					Print("[RaizInsuranceMod] Seguro expirado por limite: " + data.InsuranceID);
				}
				else
				{
					data.InsuranceState = "ACTIVE";
				}
			}
			else
			{
				data.InsuranceState = "ACTIVE";
			}
		}
		else
		{
			data.InsuranceExpired = false;
			data.InsuranceState = "ACTIVE";
		}

		RaizInsuranceManager.Save();
	}

	static bool RestoreVehicle(PlayerBase player, int insuranceIndex)
	{
		if (!player)
			return false;

		if (insuranceIndex < 0 || insuranceIndex >= RaizInsuranceManager.m_InsuredVehicles.Count())
			return false;

		RaizInsuredVehicle data = RaizInsuranceManager.m_InsuredVehicles.Get(insuranceIndex);

		if (!data)
			return false;

		if (!CanRestoreVehicle(data))
		{
			player.MessageStatus("Este seguro não está disponível para restauração. Status: " + GetRestoreStatusText(data));
			Print("[RaizInsuranceMod] Restauração bloqueada: " + data.InsuranceID + " | Status: " + GetRestoreStatusText(data));
			return false;
		}

		PlayerIdentity identity = player.GetIdentity();

		if (!identity)
			return false;

		if (data.OwnerId != identity.GetPlainId())
		{
			player.MessageStatus("Este seguro não pertence a você.");
			Print("[RaizInsuranceMod] Tentativa de restaurar seguro de outro jogador. Player: " + identity.GetPlainId() + " | Seguro: " + data.OwnerId);
			return false;
		}

		CarScript restoredCar = SpawnVehicle(player, data);

		if (!restoredCar)
		{
			player.MessageStatus("Falha ao restaurar veículo.");
			return false;
		}

		RegisterRestore(data);
		UpdateInsuranceToNewVehicle(data, restoredCar);

		player.MessageStatus("Veículo restaurado com sucesso. Usos: " + GetRestoreCountText(data));

		Print("[RaizInsuranceMod] Veículo restaurado: " + data.VehicleType + " | Seguro: " + data.InsuranceID + " | Dono: " + data.OwnerName + " | Plano: " + data.InsuranceTier + " | Usos: " + GetRestoreCountText(data));

		return true;
	}

	static bool AdminRestoreVehicle(PlayerBase adminPlayer, int insuranceIndex)
	{
		if (!adminPlayer)
			return false;

		if (insuranceIndex < 0 || insuranceIndex >= RaizInsuranceManager.m_InsuredVehicles.Count())
			return false;

		RaizInsuredVehicle data = RaizInsuranceManager.m_InsuredVehicles.Get(insuranceIndex);

		if (!data)
			return false;

		if (data.InsuranceExpired)
		{
			adminPlayer.MessageStatus("Este seguro está expirado.");
			Print("[RaizInsuranceMod] Admin tentou restaurar seguro expirado: " + data.InsuranceID);
			return false;
		}

		CarScript restoredCar = SpawnVehicle(adminPlayer, data);

		if (!restoredCar)
		{
			adminPlayer.MessageStatus("Falha ao restaurar veículo pelo painel admin.");
			return false;
		}

		RegisterRestore(data);
		UpdateInsuranceToNewVehicle(data, restoredCar);

		adminPlayer.MessageStatus("Admin restaurou veículo com sucesso. Seguro: " + data.InsuranceID);

		Print("[RaizInsuranceMod] Admin restaurou veículo: " + data.VehicleType + " | Seguro: " + data.InsuranceID + " | Dono: " + data.OwnerName + " | Plano: " + data.InsuranceTier + " | Usos: " + GetRestoreCountText(data));

		return true;
	}
};