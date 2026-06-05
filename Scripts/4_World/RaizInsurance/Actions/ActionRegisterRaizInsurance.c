class ActionRegisterRaizInsuranceCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(3.0);
	}
};

class ActionRegisterRaizInsurance : ActionContinuousBase
{
	void ActionRegisterRaizInsurance()
	{
		m_CallbackClass = ActionRegisterRaizInsuranceCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
	}

	override string GetText()
	{
		return "Registrar Seguro";
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTCursor;
	}

	bool IsInsuranceTicket(ItemBase item)
	{
		if (!item)
		{
			return false;
		}

		if (item.IsKindOf("Raiz_RegisterTicket"))
		{
			return true;
		}

		if (item.IsKindOf("Raiz_RegisterTicket_Basic"))
		{
			return true;
		}

		if (item.IsKindOf("Raiz_RegisterTicket_Plus"))
		{
			return true;
		}

		if (item.IsKindOf("Raiz_RegisterTicket_VIP"))
		{
			return true;
		}

		return false;
	}

	CarScript GetCarFromTarget(Object targetObject)
	{
		if (!targetObject)
		{
			return null;
		}

		CarScript car = CarScript.Cast(targetObject);

		if (car)
		{
			return car;
		}

		EntityAI entity = EntityAI.Cast(targetObject);

		while (entity)
		{
			car = CarScript.Cast(entity);

			if (car)
			{
				return car;
			}

			entity = entity.GetHierarchyParent();
		}

		return null;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player || !target || !item)
		{
			return false;
		}

		if (!IsInsuranceTicket(item))
		{
			return false;
		}

		Object targetObject = target.GetObject();

		if (!targetObject)
		{
			return false;
		}

		CarScript car = GetCarFromTarget(targetObject);

		if (!car)
		{
			return false;
		}

		return true;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		if (!action_data)
		{
			return;
		}

		PlayerBase player = action_data.m_Player;
		ItemBase ticket = ItemBase.Cast(action_data.m_MainItem);

		if (!player || !ticket)
		{
			return;
		}

		if (!action_data.m_Target)
		{
			return;
		}

		Object targetObject = action_data.m_Target.GetObject();

		if (!targetObject)
		{
			return;
		}

		CarScript car = GetCarFromTarget(targetObject);

		if (!car)
		{
			player.MessageStatus("Este alvo não é um veículo válido.");
			Print("[RaizInsuranceMod] Alvo não é veículo: " + targetObject.GetType());
			return;
		}

		bool registered = RaizInsuranceManager.RegisterVehicle(player, car, ticket);

		if (registered)
		{
			ticket.Delete();
			player.MessageStatus("Veículo registrado no seguro com sucesso.");
			Print("[RaizInsuranceMod] Veículo registrado com sucesso: " + car.GetType());
		}
		else
		{
			player.MessageStatus("Este veículo já possui seguro, você não tem permissão para este ticket ou ocorreu um erro.");
			Print("[RaizInsuranceMod] Falha ao registrar veículo.");
		}
	}
};