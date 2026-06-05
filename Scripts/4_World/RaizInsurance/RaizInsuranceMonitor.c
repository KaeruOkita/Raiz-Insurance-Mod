class RaizInsuranceMonitor
{
	static void Start()
	{
		RaizInsuranceVehicleRegistry.Init();
		Print("[RaizInsuranceMod] Monitor iniciado.");
	}

	static void UpdateAll()
	{
		CheckVehicles();
	}

	static void CheckVehicles()
	{
		if (!RaizInsuranceManager.m_InsuredVehicles)
			return;

		foreach (RaizInsuredVehicle insured : RaizInsuranceManager.m_InsuredVehicles)
		{
			if (!insured)
				continue;

			if (insured.InsuranceExpired)
				continue;

			if (!insured.UnlimitedRestores)
			{
				if (insured.RestoreCount >= insured.MaxRestores)
				{
					if (RaizInsuranceConfigManager.ShouldExpireAfterMaxRestores())
					{
						insured.InsuranceExpired = true;
						insured.InsuranceState = "EXPIRED";

						Print("[RaizInsuranceMod] Seguro expirado por limite: " + insured.InsuranceID);
					}

					continue;
				}
			}

			CheckVehicleState(insured);
		}

		RaizInsuranceManager.Save();
	}

	static string CalculateVehicleState(float health)
	{
		if (health <= 0)
			return "DESTROYED";

		if (health <= 3000)
			return "CRITICAL";

		if (health <= 6000)
			return "DAMAGED";

		return "ACTIVE";
	}

	static void MarkDestroyed(RaizInsuredVehicle insured, string reason)
	{
		if (!insured)
			return;

		if (!insured.CanRestore)
		{
			insured.CanRestore = true;
			insured.InsuranceState = "DESTROYED";

			Print("[RaizInsuranceMod] Veículo marcado como destruído: " + insured.VehicleType + " | Seguro: " + insured.InsuranceID + " | Motivo: " + reason);
		}
	}

	static void UpdateLiveVehicleData(RaizInsuredVehicle insured, CarScript car)
	{
		if (!insured || !car)
			return;

		insured.VehiclePosition = car.GetPosition();
		insured.VehicleOrientation = car.GetOrientation();

		insured.VehicleHealth = car.GetHealth();
		insured.EngineHealth = car.GetHealth("Engine", "Health");

		insured.FuelQuantity = car.GetFluidFraction(CarFluid.FUEL);
		insured.OilQuantity = car.GetFluidFraction(CarFluid.OIL);
		insured.WaterQuantity = car.GetFluidFraction(CarFluid.COOLANT);

		insured.LastUpdateTime = GetGame().GetTime();
	}

	static void CheckVehicleState(RaizInsuredVehicle insured)
	{
		if (!insured)
			return;

		CarScript car = RaizInsuranceVehicleRegistry.GetVehicle(insured.VehicleUID);

		if (!car)
		{
			MarkDestroyed(insured, "veículo não encontrado no registry");
			return;
		}

		UpdateLiveVehicleData(insured, car);

		if (insured.VehicleHealth <= 0)
		{
			MarkDestroyed(insured, "vida do veículo zerada");
			return;
		}

		if (insured.EngineHealth <= 0)
		{
			MarkDestroyed(insured, "motor destruído");
			return;
		}

		string vehicleState = CalculateVehicleState(insured.VehicleHealth);
		string engineState = CalculateVehicleState(insured.EngineHealth);

		insured.CanRestore = false;

		if (vehicleState == "CRITICAL" || engineState == "CRITICAL")
		{
			insured.InsuranceState = "CRITICAL";
			return;
		}

		if (vehicleState == "DAMAGED" || engineState == "DAMAGED")
		{
			insured.InsuranceState = "DAMAGED";
			return;
		}

		insured.InsuranceState = "ACTIVE";
	}

	static void Stop()
	{
		Print("[RaizInsuranceMod] Monitor parado.");
	}
};