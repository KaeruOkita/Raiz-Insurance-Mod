class RaizVehicleAttachmentData
{
	int SlotId;
	string SlotName;
	string ItemType;
	float Health;

	void RaizVehicleAttachmentData()
	{
		SlotId = -1;
		SlotName = "";
		ItemType = "";
		Health = 0;
	}
};

class RaizInsuredVehicle
{
	string InsuranceID;

	string VehicleUID;

	string OwnerId;
	string OwnerName;

	string VehicleType;

	vector VehiclePosition;
	vector VehicleOrientation;

	float VehicleHealth;
	float EngineHealth;

	float FuelQuantity;
	float OilQuantity;
	float WaterQuantity;

	float DefaultVehicleHealth;
	float DefaultEngineHealth;

	float DefaultFuelQuantity;
	float DefaultOilQuantity;
	float DefaultWaterQuantity;

	float RegisterTime;
	float LastUpdateTime;
	float LastRestoreTime;

	int MaxRestores;
	int RestoreCount;

	float CooldownHours;
	float NextRestoreAvailableTime;

	bool InsuranceExpired;

	string InsuranceState;
	bool CanRestore;

	string InsuranceTier;
	bool UnlimitedRestores;

	ref array<ref RaizVehicleAttachmentData> Attachments;

	void RaizInsuredVehicle()
	{
		InsuranceID = "";
		VehicleUID = "";

		OwnerId = "";
		OwnerName = "";

		VehicleType = "";

		VehiclePosition = "0 0 0";
		VehicleOrientation = "0 0 0";

		VehicleHealth = 0;
		EngineHealth = 0;

		FuelQuantity = 0;
		OilQuantity = 0;
		WaterQuantity = 0;

		DefaultVehicleHealth = 0;
		DefaultEngineHealth = 0;

		DefaultFuelQuantity = 0;
		DefaultOilQuantity = 0;
		DefaultWaterQuantity = 0;

		RegisterTime = 0;
		LastUpdateTime = 0;
		LastRestoreTime = 0;

		MaxRestores = 3;
		RestoreCount = 0;

		CooldownHours = 24.0;
		NextRestoreAvailableTime = 0;

		InsuranceExpired = false;

		InsuranceState = "ACTIVE";
		CanRestore = false;

		InsuranceTier = "BASIC";
		UnlimitedRestores = false;

		Attachments = new array<ref RaizVehicleAttachmentData>;
	}
};