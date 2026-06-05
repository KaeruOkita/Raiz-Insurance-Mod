modded class CarScript
{
	protected string m_RaizInsuranceUID;

	void SetRaizInsuranceUID(string uid)
	{
		m_RaizInsuranceUID = uid;

		if (m_RaizInsuranceUID && m_RaizInsuranceUID != "")
		{
			RaizInsuranceVehicleRegistry.RegisterVehicle(m_RaizInsuranceUID, this);
		}

		SetSynchDirty();
	}

	string GetRaizInsuranceUID()
	{
		return m_RaizInsuranceUID;
	}

	bool HasRaizInsuranceUID()
	{
		if (m_RaizInsuranceUID && m_RaizInsuranceUID != "")
			return true;

		return false;
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);

		ctx.Write(m_RaizInsuranceUID);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		if (!ctx.Read(m_RaizInsuranceUID))
		{
			m_RaizInsuranceUID = "";
		}

		if (m_RaizInsuranceUID && m_RaizInsuranceUID != "")
		{
			RaizInsuranceVehicleRegistry.RegisterVehicle(m_RaizInsuranceUID, this);
		}

		return true;
	}
};