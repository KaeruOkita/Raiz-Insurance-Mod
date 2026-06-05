class RaizInsuranceAdminMenu extends UIScriptedMenu
{
	protected Widget m_Root;
	protected TextListboxWidget m_List;
	protected TextWidget m_DetailsText;

	protected ButtonWidget m_CloseButton;
	protected ButtonWidget m_RefreshButton;
	protected ButtonWidget m_DetailsButton;
	protected ButtonWidget m_RemoveButton;
	protected ButtonWidget m_ForceRestoreButton;
	protected ButtonWidget m_TeleportButton;

	protected ref array<string> m_AdminList;
	protected ref array<string> m_AdminDetails;

	override Widget Init()
	{
		m_Root = GetGame().GetWorkspace().CreateWidgets("RaizInsuranceMod/GUI/layouts/RaizInsuranceAdminMenu.layout");

		m_List = TextListboxWidget.Cast(m_Root.FindAnyWidget("AdminInsuranceList"));
		m_DetailsText = TextWidget.Cast(m_Root.FindAnyWidget("AdminDetailsText"));

		m_CloseButton = ButtonWidget.Cast(m_Root.FindAnyWidget("AdminCloseButton"));
		m_RefreshButton = ButtonWidget.Cast(m_Root.FindAnyWidget("AdminRefreshButton"));
		m_DetailsButton = ButtonWidget.Cast(m_Root.FindAnyWidget("AdminDetailsButton"));
		m_RemoveButton = ButtonWidget.Cast(m_Root.FindAnyWidget("AdminRemoveButton"));
		m_ForceRestoreButton = ButtonWidget.Cast(m_Root.FindAnyWidget("AdminForceRestoreButton"));
		m_TeleportButton = ButtonWidget.Cast(m_Root.FindAnyWidget("AdminTeleportButton"));

		m_AdminList = new array<string>;
		m_AdminDetails = new array<string>;

		GetGame().GetInput().ChangeGameFocus(1);
		GetGame().GetUIManager().ShowCursor(true);

		RequestAdminInsuranceList();

		return m_Root;
	}

	void RequestAdminInsuranceList()
	{
		Param1<int> data = new Param1<int>(0);
		GetRPCManager().SendRPC("RaizInsuranceMod", "RequestAdminInsuranceList", data, true);
	}

	void ReceiveAdminInsuranceList(array<string> list, array<string> details)
	{
		if (!m_List)
			return;

		m_AdminList.Clear();
		m_AdminDetails.Clear();

		m_List.ClearItems();

		if (!list || list.Count() == 0)
		{
			m_List.AddItem("Nenhum seguro encontrado.", NULL, 0);

			if (m_DetailsText)
				m_DetailsText.SetText("Nenhum veículo segurado no momento.");

			return;
		}

		for (int i = 0; i < list.Count(); i++)
		{
			m_AdminList.Insert(list.Get(i));
			m_List.AddItem(list.Get(i), NULL, 0);
		}

		if (details)
		{
			foreach (string detail : details)
			{
				m_AdminDetails.Insert(detail);
			}
		}

		if (m_DetailsText)
			m_DetailsText.SetText("Selecione um seguro na lista e clique em DETALHES.");
	}

	int GetSelectedIndex()
	{
		if (!m_List)
			return -1;

		return m_List.GetSelectedRow();
	}

	void ShowSelectedDetails()
	{
		if (!m_List || !m_DetailsText)
			return;

		int selectedIndex = GetSelectedIndex();

		if (selectedIndex < 0)
		{
			m_DetailsText.SetText("Nenhum seguro selecionado.");
			return;
		}

		if (selectedIndex >= m_AdminDetails.Count())
		{
			m_DetailsText.SetText("Detalhes não encontrados.");
			return;
		}

		m_DetailsText.SetText(m_AdminDetails.Get(selectedIndex));
	}

	void RemoveSelectedInsurance()
	{
		int selectedIndex = GetSelectedIndex();

		if (selectedIndex < 0)
		{
			if (m_DetailsText)
				m_DetailsText.SetText("Selecione um seguro para remover.");

			return;
		}

		Param1<int> data = new Param1<int>(selectedIndex);
		GetRPCManager().SendRPC("RaizInsuranceMod", "AdminRemoveInsurance", data, true);
	}

	void ForceRestoreSelectedInsurance()
	{
		int selectedIndex = GetSelectedIndex();

		if (selectedIndex < 0)
		{
			if (m_DetailsText)
				m_DetailsText.SetText("Selecione um seguro para restaurar.");

			return;
		}

		Param1<int> data = new Param1<int>(selectedIndex);
		GetRPCManager().SendRPC("RaizInsuranceMod", "AdminForceRestore", data, true);
	}

	void TeleportToSelectedInsurance()
	{
		int selectedIndex = GetSelectedIndex();

		if (selectedIndex < 0)
		{
			if (m_DetailsText)
				m_DetailsText.SetText("Selecione um seguro para teleportar.");

			return;
		}

		Param1<int> data = new Param1<int>(selectedIndex);
		GetRPCManager().SendRPC("RaizInsuranceMod", "AdminTeleportToVehicle", data, true);
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

		if (w == m_RefreshButton)
		{
			RequestAdminInsuranceList();
			return true;
		}

		if (w == m_DetailsButton)
		{
			ShowSelectedDetails();
			return true;
		}

		if (w == m_RemoveButton)
		{
			RemoveSelectedInsurance();
			return true;
		}

		if (w == m_ForceRestoreButton)
		{
			ForceRestoreSelectedInsurance();
			return true;
		}

		if (w == m_TeleportButton)
		{
			TeleportToSelectedInsurance();
			return true;
		}

		return super.OnClick(w, x, y, button);
	}
};