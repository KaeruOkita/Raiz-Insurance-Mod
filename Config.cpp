class CfgPatches
{
	class RaizInsuranceMod
	{
		units[]=
		{
			"Raiz_RegisterTicket",
			"Raiz_RegisterTicket_Basic",
			"Raiz_RegisterTicket_Plus",
			"Raiz_RegisterTicket_VIP"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Scripts",
			"DZ_Characters",
			"DZ_Gear_Camping",
			"DZ_Structures_Military"
		};
	};
};

class CfgMods
{
	class RaizInsuranceMod
	{
		dir="RaizInsuranceMod";
		picture="RaizInsuranceMod\GUI\textures\raiz_logo.paa";
		action="";
		hideName=0;
		hidePicture=0;
		name="Raiz Insurance Mod";
		credits="";
		author="Kaeru";
		authorID="";
		version="1.1";
		extra=0;
		type="mod";

		inputs="RaizInsuranceMod\inputs\inputs.xml";

		dependencies[]=
		{
			"Game",
			"World",
			"Mission"
		};

		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"RaizInsuranceMod/Scripts/3_Game"
				};
			};

			class worldScriptModule
			{
				value="";
				files[]=
				{
					"RaizInsuranceMod/Scripts/4_World"
				};
			};

			class missionScriptModule
			{
				value="";
				files[]=
				{
					"RaizInsuranceMod/Scripts/5_Mission"
				};
			};
		};
	};
};

class CfgVehicles
{
	class Paper;

	class Raiz_RegisterTicket_Basic: Paper
	{
		scope=2;
		displayName="Ticket de Seguro Básico";
		descriptionShort="Use este ticket em um veículo para registrar seguro básico com 1 restauração.";
		model="\dz\gear\consumables\Paper.p3d";
		weight=1;
		itemSize[]={1,1};
		rotationFlags=17;
		hiddenSelectionsTextures[]=
		{
			"RaizInsuranceMod\GUI\textures\raiz_logo.paa"
		};
	};

	class Raiz_RegisterTicket_Plus: Paper
	{
		scope=2;
		displayName="Ticket de Seguro Plus";
		descriptionShort="Use este ticket em um veículo para registrar seguro Plus com 3 restaurações.";
		model="\dz\gear\consumables\Paper.p3d";
		weight=1;
		itemSize[]={1,1};
		rotationFlags=17;
		hiddenSelectionsTextures[]=
		{
			"RaizInsuranceMod\GUI\textures\raiz_logo.paa"
		};
	};

	class Raiz_RegisterTicket_VIP: Paper
	{
		scope=2;
		displayName="Ticket de Seguro VIP";
		descriptionShort="Use este ticket em um veículo para registrar seguro VIP com restaurações ilimitadas. Requer SteamID autorizada.";
		model="\dz\gear\consumables\Paper.p3d";
		weight=1;
		itemSize[]={1,1};
		rotationFlags=17;
		hiddenSelectionsTextures[]=
		{
			"RaizInsuranceMod\GUI\textures\raiz_logo.paa"
		};
	};

	class Raiz_RegisterTicket: Raiz_RegisterTicket_Basic
	{
		scope=2;
		displayName="Ticket de Seguro";
		descriptionShort="Ticket antigo mantido por compatibilidade. Funciona como seguro básico com 1 restauração.";
	};
};