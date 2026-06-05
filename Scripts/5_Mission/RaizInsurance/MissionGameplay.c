modded class MissionGameplay
{
	protected ref RaizInsuranceMenu m_RaizInsuranceMenu;
	protected ref RaizInsuranceAdminMenu m_RaizInsuranceAdminMenu;

	override void OnInit()
	{
		super.OnInit();

		GetRPCManager().AddRPC("RaizInsuranceMod", "ReceiveInsuranceList", this, SingeplayerExecutionType.Client);
		GetRPCManager().AddRPC("RaizInsuranceMod", "ReceiveAdminAccess", this, SingeplayerExecutionType.Client);
		GetRPCManager().AddRPC("RaizInsuranceMod", "ReceiveAdminInsuranceList", this, SingeplayerExecutionType.Client);
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		if (GetGame().GetInput().LocalPress("UAOpenRaizInsuranceMenu", false))
		{
			OpenRaizInsuranceMenu();
		}

		if (GetGame().GetInput().LocalPress("UAOpenRaizInsuranceAdminMenu", false))
		{
			RequestAdminMenu();
		}
	}

	void OpenRaizInsuranceMenu()
	{
		if (GetGame().GetUIManager().GetMenu())
			return;

		UIScriptedMenu menu = GetGame().GetUIManager().EnterScriptedMenu(9876101, null);
		m_RaizInsuranceMenu = RaizInsuranceMenu.Cast(menu);
	}

	void RequestAdminMenu()
	{
		if (GetGame().GetUIManager().GetMenu())
			return;

		Param1<int> data = new Param1<int>(0);
		GetRPCManager().SendRPC("RaizInsuranceMod", "RequestAdminAccess", data, true);
	}

	void OpenRaizInsuranceAdminMenu()
	{
		if (GetGame().GetUIManager().GetMenu())
			return;

		UIScriptedMenu menu = GetGame().GetUIManager().EnterScriptedMenu(9876102, null);
		m_RaizInsuranceAdminMenu = RaizInsuranceAdminMenu.Cast(menu);
	}

	override UIScriptedMenu CreateScriptedMenu(int id)
	{
		UIScriptedMenu menu = super.CreateScriptedMenu(id);

		if (!menu && id == 9876101)
			menu = new RaizInsuranceMenu;

		if (!menu && id == 9876102)
			menu = new RaizInsuranceAdminMenu;

		return menu;
	}

	void ReceiveInsuranceList(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client)
			return;

		Param1<ref array<string>> data;

		if (!ctx.Read(data))
			return;

		if (!m_RaizInsuranceMenu)
			return;

		m_RaizInsuranceMenu.ReceiveInsuranceList(data.param1);
	}

	void ReceiveAdminAccess(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client)
			return;

		Param1<bool> data;

		if (!ctx.Read(data))
			return;

		if (data.param1)
			OpenRaizInsuranceAdminMenu();
		else
			GetGame().GetMission().OnEvent(ChatMessageEventTypeID, new ChatMessageEventParams(CCDirect, "", "Você não tem permissão para abrir o painel admin.", ""));
	}

	void ReceiveAdminInsuranceList(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client)
			return;

		Param2<ref array<string>, ref array<string>> data;

		if (!ctx.Read(data))
			return;

		if (!m_RaizInsuranceAdminMenu)
			return;

		m_RaizInsuranceAdminMenu.ReceiveAdminInsuranceList(data.param1, data.param2);
	}
};