class RaizInsuranceVehicleRegistry
{
	static ref map<string, CarScript> m_Vehicles;

	static void Init()
	{
		if (!m_Vehicles)
		{
			m_Vehicles = new map<string, CarScript>();
		}

		Print("[RaizInsuranceMod] Vehicle Registry iniciado.");
	}

	static void RegisterVehicle(string vehicleUID, CarScript car)
	{
		if (vehicleUID == "")
			return;

		if (!car)
			return;

		if (!m_Vehicles)
			Init();

		if (m_Vehicles.Contains(vehicleUID))
			m_Vehicles.Remove(vehicleUID);

		m_Vehicles.Insert(vehicleUID, car);

		Print("[RaizInsuranceMod] Vehicle Registry registrado: " + vehicleUID + " | " + car.GetType());
	}

	static void UnregisterVehicle(string vehicleUID)
	{
		if (vehicleUID == "")
			return;

		if (!m_Vehicles)
			return;

		if (m_Vehicles.Contains(vehicleUID))
		{
			m_Vehicles.Remove(vehicleUID);
			Print("[RaizInsuranceMod] Vehicle Registry removido: " + vehicleUID);
		}
	}

	static CarScript GetVehicle(string vehicleUID)
	{
		if (vehicleUID == "")
			return null;

		if (!m_Vehicles)
			Init();

		if (!m_Vehicles.Contains(vehicleUID))
			return null;

		CarScript car = m_Vehicles.Get(vehicleUID);

		if (!car)
		{
			m_Vehicles.Remove(vehicleUID);
			return null;
		}

		return car;
	}
};