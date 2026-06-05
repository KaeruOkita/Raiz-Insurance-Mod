class RaizInsuranceMenu extends UIScriptedMenu
{
	protected Widget m_Root;
	protected TextListboxWidget m_List;
	protected TextWidget m_StatusText;
	protected ButtonWidget m_CloseButton;
	protected ButtonWidget m_RestoreButton;
	protected ButtonWidget m_RefreshButton;

	override Widget Init()
	{
		m_Root = GetGame().GetWorkspace().CreateWidgets("RaizInsuranceMod/GUI/layouts/RaizInsuranceMenu.layout");

		m_List = TextListboxWidget.Cast(m_Root.FindAnyWidget("InsuranceList"));
		m_StatusText = TextWidget.Cast(m_Root.FindAnyWidget("StatusText"));
		m_CloseButton = ButtonWidget.Cast(m_Root.FindAnyWidget("CloseButton"));
		m_RestoreButton = ButtonWidget.Cast(m_Root.FindAnyWidget("RestoreButton"));
		m_RefreshButton = ButtonWidget.Cast(m_Root.FindAnyWidget("RefreshButton"));

		GetGame().GetInput().ChangeGameFocus(1);
		GetGame().GetUIManager().ShowCursor(true);

		SetStatus("Carregando veículos segurados...");
		RequestInsuranceList();

		return m_Root;
	}

	void SetStatus(string message)
	{
		if (m_StatusText)
		{
			m_StatusText.SetText(message);
		}
	}

	void RequestInsuranceList()
	{
		SetStatus("Atualizando lista de seguros...");

		Param1<int> data = new Param1<int>(0);
		GetRPCManager().SendRPC("RaizInsuranceMod", "RequestInsuranceList", data, true);
	}

	void ReceiveInsuranceList(array<string> vehicles)
	{
		if (!m_List)
			return;

		m_List.ClearItems();

		if (!vehicles || vehicles.Count() == 0)
		{
			m_List.AddItem("Nenhum veículo segurado encontrado.", NULL, 0);
			SetStatus("Nenhum veículo segurado encontrado.");
			return;
		}

		foreach (string vehicleName : vehicles)
		{
			m_List.AddItem(vehicleName, NULL, 0);
		}

		SetStatus("Selecione um veículo. Restauração disponível apenas quando aparecer Restaurar: SIM.");
	}

	bool SelectedVehicleCanRestore()
	{
		if (!m_List)
			return false;

		int selectedIndex = m_List.GetSelectedRow();

		if (selectedIndex < 0)
			return false;

		string selectedText;
		m_List.GetItemText(selectedIndex, 0, selectedText);

		if (selectedText.Contains("Restaurar: SIM"))
			return true;

		return false;
	}

	void RestoreSelectedVehicle()
	{
		if (!m_List)
			return;

		int selectedIndex = m_List.GetSelectedRow();

		if (selectedIndex < 0)
		{
			SetStatus("Selecione um veículo antes de restaurar.");
			return;
		}

		if (!SelectedVehicleCanRestore())
		{
			SetStatus("Este veículo ainda não está liberado para restauração.");
			return;
		}

		Param1<int> data = new Param1<int>(selectedIndex);
		GetRPCManager().SendRPC("RaizInsuranceMod", "RestoreVehicle", data, true);

		SetStatus("Solicitação de restauração enviada ao servidor.");
		Close();
	}

	override void OnHide()
	{
		super.OnHide();

		GetGame().GetInput().ResetGameFocus();
		GetGame().GetUIManager().ShowCursor(false);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_CloseButton)
		{
			Close();
			return true;
		}

		if (w == m_RestoreButton)
		{
			RestoreSelectedVehicle();
			return true;
		}

		if (w == m_RefreshButton)
		{
			RequestInsuranceList();
			return true;
		}

		return super.OnClick(w, x, y, button);
	}
};