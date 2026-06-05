modded class MissionServer
{
	protected float m_RaizInsuranceMonitorTimer;

	override void OnInit()
	{
		super.OnInit();

		m_RaizInsuranceMonitorTimer = 0;

		RaizInsuranceConfigManager.Init();
		RaizVipConfigManager.Init();
		RaizInsuranceManager.Init();
		RaizAdminConfigManager.Init();

		GetRPCManager().AddRPC("RaizInsuranceMod", "RequestInsuranceList", this, SingeplayerExecutionType.Server);
		GetRPCManager().AddRPC("RaizInsuranceMod", "RestoreVehicle", this, SingeplayerExecutionType.Server);
		GetRPCManager().AddRPC("RaizInsuranceMod", "RequestAdminAccess", this, SingeplayerExecutionType.Server);
		GetRPCManager().AddRPC("RaizInsuranceMod", "RequestAdminInsuranceList", this, SingeplayerExecutionType.Server);
		GetRPCManager().AddRPC("RaizInsuranceMod", "AdminRemoveInsurance", this, SingeplayerExecutionType.Server);
		GetRPCManager().AddRPC("RaizInsuranceMod", "AdminForceRestore", this, SingeplayerExecutionType.Server);
		GetRPCManager().AddRPC("RaizInsuranceMod", "AdminTeleportToVehicle", this, SingeplayerExecutionType.Server);

		Print("[RaizInsuranceMod] MissionServer iniciado.");
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		m_RaizInsuranceMonitorTimer += timeslice;

		if (m_RaizInsuranceMonitorTimer >= 30.0)
		{
			m_RaizInsuranceMonitorTimer = 0;
			RaizInsuranceMonitor.UpdateAll();
		}
	}

	void RequestInsuranceList(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender)
			return;

		array<string> vehicles = new array<string>;
		RaizInsuranceManager.GetVehiclesForPlayer(sender, vehicles);

		Param1<ref array<string>> response = new Param1<ref array<string>>(vehicles);
		GetRPCManager().SendRPC("RaizInsuranceMod", "ReceiveInsuranceList", response, true, sender);
	}

	void RestoreVehicle(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender)
			return;

		Param1<int> data;

		if (!ctx.Read(data))
			return;

		PlayerBase player = RaizFindPlayerByIdentity(sender);

		if (!player)
			return;

		int playerListIndex = data.param1;
		int realIndex = RaizInsuranceManager.GetRealInsuranceIndex(sender, playerListIndex);

		if (realIndex < 0)
			return;

		RaizInsuranceRestore.RestoreVehicle(player, realIndex);
	}

	void RequestAdminAccess(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender)
			return;

		bool isAdmin = RaizAdminConfigManager.IsAdmin(sender);

		Param1<bool> response = new Param1<bool>(isAdmin);
		GetRPCManager().SendRPC("RaizInsuranceMod", "ReceiveAdminAccess", response, true, sender);

		Print("[RaizInsuranceMod] Admin access request: " + sender.GetName() + " | " + isAdmin.ToString());
	}

	void RequestAdminInsuranceList(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender)
			return;

		if (!RaizAdminConfigManager.IsAdmin(sender))
			return;

		array<string> list = new array<string>;
		array<string> details = new array<string>;

		RaizInsuranceManager.GetAdminVehicleList(list);
		RaizInsuranceManager.GetAdminVehicleDetails(details);

		Param2<ref array<string>, ref array<string>> response = new Param2<ref array<string>, ref array<string>>(list, details);
		GetRPCManager().SendRPC("RaizInsuranceMod", "ReceiveAdminInsuranceList", response, true, sender);
	}

	void AdminRemoveInsurance(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender)
			return;

		if (!RaizAdminConfigManager.IsAdmin(sender))
			return;

		Param1<int> data;

		if (!ctx.Read(data))
			return;

		int index = data.param1;

		if (index < 0 || index >= RaizInsuranceManager.m_InsuredVehicles.Count())
			return;

		RaizInsuredVehicle removed = RaizInsuranceManager.m_InsuredVehicles.Get(index);

		RaizInsuranceManager.m_InsuredVehicles.Remove(index);
		RaizInsuranceManager.Save();

		if (removed)
			Print("[RaizInsuranceMod] Admin removeu seguro: " + removed.InsuranceID + " | Admin: " + sender.GetName());

		RequestAdminInsuranceList(type, ctx, sender, target);
	}

	void AdminForceRestore(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender)
			return;

		if (!RaizAdminConfigManager.IsAdmin(sender))
			return;

		Param1<int> data;

		if (!ctx.Read(data))
			return;

		int index = data.param1;

		if (index < 0 || index >= RaizInsuranceManager.m_InsuredVehicles.Count())
			return;

		PlayerBase adminPlayer = RaizFindPlayerByIdentity(sender);

		if (!adminPlayer)
			return;

		RaizInsuranceRestore.AdminRestoreVehicle(adminPlayer, index);

		Print("[RaizInsuranceMod] Admin forçou restauração do seguro index: " + index.ToString() + " | Admin: " + sender.GetName());

		RequestAdminInsuranceList(type, ctx, sender, target);
	}

	void AdminTeleportToVehicle(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender)
			return;

		if (!RaizAdminConfigManager.IsAdmin(sender))
			return;

		Param1<int> data;

		if (!ctx.Read(data))
			return;

		int index = data.param1;

		if (index < 0 || index >= RaizInsuranceManager.m_InsuredVehicles.Count())
			return;

		PlayerBase adminPlayer = RaizFindPlayerByIdentity(sender);

		if (!adminPlayer)
			return;

		RaizInsuredVehicle insured = RaizInsuranceManager.m_InsuredVehicles.Get(index);

		if (!insured)
			return;

		vector tpPos = insured.VehiclePosition + "0 1.5 0";
		adminPlayer.SetPosition(tpPos);

		adminPlayer.MessageStatus("Teleportado até o veículo segurado.");
		Print("[RaizInsuranceMod] Admin teleportou até: " + insured.InsuranceID + " | " + insured.VehicleType);
	}

	PlayerBase RaizFindPlayerByIdentity(PlayerIdentity identity)
	{
		if (!identity)
			return null;

		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);

		foreach (Man man : players)
		{
			PlayerBase player = PlayerBase.Cast(man);

			if (!player || !player.GetIdentity())
				continue;

			if (player.GetIdentity().GetPlainId() == identity.GetPlainId())
				return player;
		}

		return null;
	}
};