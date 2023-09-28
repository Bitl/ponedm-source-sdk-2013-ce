//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Player for HL2.
//
//=============================================================================//

#include "cbase.h"
#include "weapon_hl2mpbasehlmpcombatweapon.h"
#include "hl2mp_player.h"
#include "globalstate.h"
#include "game.h"
#include "gamerules.h"
#include "hl2mp_player_shared.h"
#include "predicted_viewmodel.h"
#include "in_buttons.h"
#include "hl2mp_gamerules.h"
#include "KeyValues.h"
#include "team.h"
#include "weapon_hl2mpbase.h"
#include "grenade_satchel.h"
#include "eventqueue.h"
#include "gamestats.h"

#include "engine/IEngineSound.h"
#include "SoundEmitterSystem/isoundemittersystembase.h"

#include "ilagcompensationmanager.h"
#include <sceneentity.h>
#include <particle_parse.h>
#include "bots/ponedm_bot.h"
#include "weapon_railgun.h"
#include "hl2_shareddefs.h"

int g_iLastCitizenModel = 0;
int g_iLastCombineModel = 0;

CBaseEntity	 *g_pLastCombineSpawn = NULL;
CBaseEntity	 *g_pLastRebelSpawn = NULL;
extern CBaseEntity				*g_pLastSpawn;

#ifdef PONEDM
ConVar sv_ponedm_updatecolors("sv_ponedm_updatecolors", "1", FCVAR_REPLICATED | FCVAR_NOTIFY, "Updates player color immediately.");
ConVar sv_ponedm_updateappearance("sv_ponedm_updateappearance", "1", FCVAR_REPLICATED | FCVAR_NOTIFY, "Updates player appearance immediately.");
ConVar sv_ponedm_damagescale_self("sv_ponedm_damagescale_self", "0.25", FCVAR_CHEAT | FCVAR_NOTIFY, "");
ConVar sv_ponedm_damageforcescale_self("sv_ponedm_damageforcescale_self", "4", FCVAR_CHEAT | FCVAR_NOTIFY, "");
ConVar sv_ponedm_randomizer_weaponcount("sv_ponedm_randomizer_weaponcount", "5", FCVAR_NOTIFY, "");
#endif

void CC_SettoSpectate(void)
{
	CHL2MP_Player* pPlayer = ToHL2MPPlayer(UTIL_GetCommandClient());

	if (pPlayer)
	{
		if (sv_ponedm_gamemode.GetInt() == 3)
		{
			ClientPrint(pPlayer, HUD_PRINTTALK, "You cannot change teams right now.");
			return;
		}

		if (pPlayer->GetNextTeamChangeTime() <= gpGlobals->curtime)
		{
			if (pPlayer->GetTeamNumber() != TEAM_SPECTATOR)
			{
				pPlayer->m_bEnterObserver = true;
				pPlayer->RemoveAllItems(true);
				pPlayer->State_Transition(STATE_OBSERVER_MODE);
				pPlayer->ChangeTeam(TEAM_SPECTATOR);
			}
			else
			{
				pPlayer->PickDefaultSpawnTeam();
				pPlayer->StopObserverMode();
				pPlayer->State_Transition(STATE_ACTIVE);
				pPlayer->Spawn();
			}
		}
		else
		{
			char szReturnString[128];
			Q_snprintf(szReturnString, sizeof(szReturnString), "Please wait %d more seconds before trying to switch teams again.\n", (int)(pPlayer->GetNextTeamChangeTime() - gpGlobals->curtime));

			ClientPrint(pPlayer, HUD_PRINTTALK, szReturnString);
		}
	}
}
static ConCommand dospectate("dospectate", CC_SettoSpectate, "");

#define HL2MP_COMMAND_MAX_RATE 0.3

void DropPrimedFragGrenade( CHL2MP_Player *pPlayer, CBaseCombatWeapon *pGrenade );

LINK_ENTITY_TO_CLASS( player, CHL2MP_Player );

LINK_ENTITY_TO_CLASS( info_player_combine, CPointEntity );
LINK_ENTITY_TO_CLASS( info_player_rebel, CPointEntity );
LINK_ENTITY_TO_CLASS(info_player_terrorist, CPointEntity);
LINK_ENTITY_TO_CLASS(info_player_counterterrorist, CPointEntity);
LINK_ENTITY_TO_CLASS(info_player_axis, CPointEntity);
LINK_ENTITY_TO_CLASS(info_player_allies, CPointEntity);
LINK_ENTITY_TO_CLASS(info_player_red, CPointEntity);
LINK_ENTITY_TO_CLASS(info_player_blue, CPointEntity);
LINK_ENTITY_TO_CLASS(info_player_teamspawn, CPointEntity);

IMPLEMENT_SERVERCLASS_ST(CHL2MP_Player, DT_HL2MP_Player)
	SendPropAngle( SENDINFO_VECTORELEM(m_angEyeAngles, 0), 11, SPROP_CHANGES_OFTEN ),
	SendPropAngle( SENDINFO_VECTORELEM(m_angEyeAngles, 1), 11, SPROP_CHANGES_OFTEN ),
	SendPropEHandle( SENDINFO( m_hRagdoll ) ),
	SendPropInt( SENDINFO( m_iSpawnInterpCounter), 4 ),
	
	SendPropExclude( "DT_BaseAnimating", "m_flPoseParameter" ),
	SendPropExclude( "DT_BaseFlex", "m_viewtarget" ),

#ifdef PONEDM
	SendPropVector(SENDINFO(m_vPrimaryColor)),
	SendPropVector(SENDINFO(m_vSecondaryColor)),
	SendPropVector(SENDINFO(m_vTertiaryColor)),
#endif

//	SendPropExclude( "DT_ServerAnimationData" , "m_flCycle" ),	
//	SendPropExclude( "DT_AnimTimeMustBeFirst" , "m_flAnimTime" ),
	
END_SEND_TABLE()

BEGIN_DATADESC( CHL2MP_Player )
END_DATADESC()

#ifndef PONEDM
const char *g_ppszRandomCitizenModels[] = 
{
	"models/humans/group03/male_01.mdl",
	"models/humans/group03/male_02.mdl",
	"models/humans/group03/female_01.mdl",
	"models/humans/group03/male_03.mdl",
	"models/humans/group03/female_02.mdl",
	"models/humans/group03/male_04.mdl",
	"models/humans/group03/female_03.mdl",
	"models/humans/group03/male_05.mdl",
	"models/humans/group03/female_04.mdl",
	"models/humans/group03/male_06.mdl",
	"models/humans/group03/female_06.mdl",
	"models/humans/group03/male_07.mdl",
	"models/humans/group03/female_07.mdl",
	"models/humans/group03/male_08.mdl",
	"models/humans/group03/male_09.mdl",
};

const char *g_ppszRandomCombineModels[] =
{
	"models/combine_soldier.mdl",
	"models/combine_soldier_prisonguard.mdl",
	"models/combine_super_soldier.mdl",
	"models/police.mdl",
};
#endif

#define TEAM_CHANGE_INTERVAL 5.0f

#define HL2MPPLAYER_PHYSDAMAGE_SCALE 4.0f

#pragma warning( disable : 4355 )

CHL2MP_Player::CHL2MP_Player() : m_PlayerAnimState( this )
{
	m_angEyeAngles.Init();

	m_iLastWeaponFireUsercmd = 0;

	m_flNextTeamChangeTime = 0.0f;
	m_flLastSpawn = 0.0f;

	m_iSpawnInterpCounter = 0;

    m_bEnterObserver = false;
	m_bReady = false;

#ifdef PONEDM
	m_vPrimaryColor.Init(1.0f, 1.0f, 1.0f);
	m_vSecondaryColor.Init(1.0f, 1.0f, 1.0f);
	m_vTertiaryColor.Init(1.0f, 1.0f, 1.0f);

	m_iUpperManeBodygroup = 0;
	m_iLowerManeBodygroup = 0;
	m_iTailBodygroup = 0;
	m_iHornBodygroup = 0;
	m_iWingsBodygroup = 0;
#endif

	BaseClass::ChangeTeam( 0 );
	
//	UseClientSideAnimation();
}

CHL2MP_Player::~CHL2MP_Player( void )
{

}

void CHL2MP_Player::UpdateOnRemove( void )
{
	if ( m_hRagdoll )
	{
		UTIL_RemoveImmediate( m_hRagdoll );
		m_hRagdoll = NULL;
	}

	BaseClass::UpdateOnRemove();
}

void CHL2MP_Player::Precache( void )
{
	BaseClass::Precache();

	PrecacheModel ( "sprites/glow01.vmt" );

#ifndef PONEDM
	//Precache Citizen models
	int nHeads = ARRAYSIZE( g_ppszRandomCitizenModels );
	int i;	

	for ( i = 0; i < nHeads; ++i )
	   	 PrecacheModel( g_ppszRandomCitizenModels[i] );

	//Precache Combine Models
	nHeads = ARRAYSIZE( g_ppszRandomCombineModels );

	for ( i = 0; i < nHeads; ++i )
	   	 PrecacheModel( g_ppszRandomCombineModels[i] );
#else
	PrecacheModel("models/ppm/player_default_base_new_pantoneshift.mdl");
	PrecacheModel("models/ppm/c_arms_pony.mdl");
	PrecacheModel("models/gibs/pgib_p3.mdl");
	PrecacheModel("models/gibs/pgib_p4.mdl");
#endif

	PrecacheFootStepSounds();

#ifndef PONEDM
	PrecacheScriptSound( "NPC_MetroPolice.Die" );
	PrecacheScriptSound( "NPC_CombineS.Die" );
	PrecacheScriptSound( "NPC_Citizen.die" );
#else
	PrecacheScriptSound("Pony.Die");
	PrecacheScriptSound("Gore.Headshot");
	PrecacheParticleSystem("conc_stars");
#endif
}

void CHL2MP_Player::GiveItems(bool bGiveAll)
{
	EquipSuit();

	CUtlVector<string_t> m_weapons;

	KeyValues* pKV = new KeyValues("Weapons");
	if (pKV->LoadFromFile(filesystem, "scripts/weapons.txt", "GAME"))
	{
		FOR_EACH_VALUE(pKV, pSubData)
		{
			if (FStrEq(pSubData->GetString(), ""))
				continue;

			string_t iName = AllocPooledString(pSubData->GetString());
			if (m_weapons.Find(iName) == m_weapons.InvalidIndex())
				m_weapons[m_weapons.AddToTail()] = iName;
		}
	}

	pKV->deleteThis();

	if (m_weapons.Count() == 0)
		return;

	CUtlVector<string_t> m_ammo;

	KeyValues* pKV2 = new KeyValues("Ammo");
	if (pKV2->LoadFromFile(filesystem, "scripts/ammo.txt", "GAME"))
	{
		FOR_EACH_VALUE(pKV2, pSubData)
		{
			if (FStrEq(pSubData->GetString(), ""))
				continue;

			string_t iName = AllocPooledString(pSubData->GetString());
			if (m_ammo.Find(iName) == m_ammo.InvalidIndex())
				m_ammo[m_ammo.AddToTail()] = iName;
		}
	}

	pKV2->deleteThis();

	if (m_ammo.Count() == 0)
		return;

	if (bGiveAll)
	{
		//give us everything.
		int WeaponList = m_weapons.Count();
		for (int i = 0; i < WeaponList; ++i)
		{
			GiveNamedItem(STRING(m_weapons[i]));
		}

		int AmmoList = m_ammo.Count();
		for (int i = 0; i < AmmoList; ++i)
		{
			CBasePlayer::GiveAmmo(999, STRING(m_ammo[i]));
		}
	}
	else
	{
		//give us some weapons.
		int nWeapons = m_weapons.Count();
		int weaponCount = Clamp(sv_ponedm_randomizer_weaponcount.GetInt(), 1, nWeapons);
		for (int i = 0; i < weaponCount; ++i)
		{
			static int nameIndex = RandomInt(0, m_weapons.Count() - 1);
			string_t iszName = m_weapons[++nameIndex % m_weapons.Count()];
			GiveNamedItem(STRING(iszName));
		}

		//give us ammo for our new weapons.
		int WeaponList = m_weapons.Count();
		for (int i = 0; i < WeaponList; ++i)
		{
			CBaseCombatWeapon* pCheckWeapon = Weapon_OwnsThisType(STRING(m_weapons[i]));
			if (pCheckWeapon)
			{
				CBasePlayer::GiveAmmo(RandomInt(1, 999), pCheckWeapon->GetPrimaryAmmoType());
				CBasePlayer::GiveAmmo(RandomInt(1, 999), pCheckWeapon->GetSecondaryAmmoType());
			}
		}
	}

	m_weapons.RemoveAll();
}

bool CHL2MP_Player::IsAllowedToPickupWeapons(void)
{
	if (m_flLastSpawn < gpGlobals->curtime)
	{
		if ((!HL2MPRules()->IsTeamplay() && (sv_ponedm_gamemode.GetInt() == 3)) && GetTeamNumber() == TEAM_ZOMBIES)
		{
			return false;
		}

		if (sv_ponedm_gamemode.GetInt() == 2)
		{
			return false;
		}
	}

	return BaseClass::IsAllowedToPickupWeapons();
}

void CHL2MP_Player::GiveDefaultItems( void )
{
	EquipSuit();

	if (sv_ponedm_gamemode.GetInt() == 2)
	{
		CBasePlayer::GiveAmmo(255, "Railgun");
		GiveNamedItem("weapon_railgun");
	}
	else
	{
		if (GetTeamNumber() != TEAM_ZOMBIES)
		{
			CBasePlayer::GiveAmmo(255, "Pistol");
			CBasePlayer::GiveAmmo(100, "SMG1");
			CBasePlayer::GiveAmmo(2, "grenade");
			if ((!HL2MPRules()->IsTeamplay() && (sv_ponedm_gamemode.GetInt() == 3)))
			{
				CBasePlayer::GiveAmmo(18, "Buckshot");
			}
			else
			{
				CBasePlayer::GiveAmmo(6, "Buckshot");
			}
			CBasePlayer::GiveAmmo(6, "357");
			CBasePlayer::GiveAmmo(3, "smg1_grenade");

			GiveNamedItem("weapon_crowbar");
			GiveNamedItem("weapon_pistol");
			GiveNamedItem("weapon_smg1");
			if ((!HL2MPRules()->IsTeamplay() && (sv_ponedm_gamemode.GetInt() == 3)))
			{
				GiveNamedItem("weapon_shotgun");
			}
			GiveNamedItem("weapon_frag");
			GiveNamedItem("weapon_physcannon");
		}
	}

	GiveNamedItem("weapon_fists");

	const char *szDefaultWeaponName = engine->GetClientConVarValue( engine->IndexOfEdict( edict() ), "cl_defaultweapon" );

	CBaseCombatWeapon *pDefaultWeapon = Weapon_OwnsThisType( szDefaultWeaponName );

	if ( pDefaultWeapon )
	{
		Weapon_Switch( pDefaultWeapon );
	}
	else
	{
		if (((!HL2MPRules()->IsTeamplay() && (sv_ponedm_gamemode.GetInt() == 3)) && GetTeamNumber() == TEAM_ZOMBIES))
		{
			Weapon_Switch(Weapon_OwnsThisType("weapon_fists"));
			return;
		}

		if (sv_ponedm_gamemode.GetInt() == 2)
		{
			Weapon_Switch(Weapon_OwnsThisType("weapon_physcannon"));
		}
		else
		{
			Weapon_Switch(Weapon_OwnsThisType("weapon_railgun"));
		}
	}
}

void CHL2MP_Player::PickDefaultSpawnTeam( void )
{
	if ( GetTeamNumber() == TEAM_UNASSIGNED || GetTeamNumber() == TEAM_SPECTATOR)
	{
		if ( !HL2MPRules()->IsTeamplay() )
		{
			if (sv_ponedm_gamemode.GetInt() == 3)
			{
				CTeam* pCombine = g_Teams[TEAM_ZOMBIES];
				CTeam* pRebels = g_Teams[TEAM_UNASSIGNED];

				if (pCombine == NULL || pRebels == NULL)
				{
					int team = random->RandomInt(0, 1);
					ChangeTeam((team == 1 ? TEAM_ZOMBIES : TEAM_UNASSIGNED));
				}
				else
				{
					if (pCombine->GetNumPlayers() > pRebels->GetNumPlayers())
					{
						ChangeTeam(TEAM_UNASSIGNED);
					}
					else if (pCombine->GetNumPlayers() < pRebels->GetNumPlayers())
					{
						ChangeTeam(TEAM_ZOMBIES);
					}
					else
					{
						int team = random->RandomInt(0, 1);
						ChangeTeam((team == 1 ? TEAM_ZOMBIES : TEAM_UNASSIGNED));
					}
				}
			}
			else
			{
				ChangeTeam(TEAM_UNASSIGNED);
			}
		}
		else
		{
			CTeam *pCombine = g_Teams[TEAM_BLUE];
			CTeam *pRebels = g_Teams[TEAM_RED];

			if ( pCombine == NULL || pRebels == NULL )
			{
				ChangeTeam( random->RandomInt( TEAM_BLUE, TEAM_RED ) );
			}
			else
			{
				if ( pCombine->GetNumPlayers() > pRebels->GetNumPlayers() )
				{
					ChangeTeam( TEAM_RED );
				}
				else if ( pCombine->GetNumPlayers() < pRebels->GetNumPlayers() )
				{
					ChangeTeam( TEAM_BLUE );
				}
				else
				{
					ChangeTeam( random->RandomInt( TEAM_BLUE, TEAM_RED ) );
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Called the first time the player's created
//-----------------------------------------------------------------------------
void CHL2MP_Player::InitialSpawn(void)
{
	BaseClass::InitialSpawn();
	m_flLastSpawn = gpGlobals->curtime + 1.5f;
}

//-----------------------------------------------------------------------------
// Purpose: Sets HL2 specific defaults.
//-----------------------------------------------------------------------------
void CHL2MP_Player::Spawn(void)
{
	m_flNextTeamChangeTime = 0.0f;

	PickDefaultSpawnTeam();

	BaseClass::Spawn();
	
	if (!IsObserver())
	{
		pl.deadflag = false;
		RemoveSolidFlags( FSOLID_NOT_SOLID );

		RemoveEffects( EF_NODRAW );
		
		if (sv_ponedm_gamemode.GetInt() == 1)
		{
			GiveItems();
		}
		else
		{
			GiveDefaultItems();
		}
	}

	SetNumAnimOverlays( 3 );
	ResetAnimation();

#ifdef PONEDM
	UpdatePlayerColor();
	UpdatePlayerAppearance();
	GetViewModel(1)->SetModel("models/ppm/c_arms_pony.mdl");
	// Gore
	m_iGoreHead = 0;
	m_iGoreFrontLeftLeg = 0;
	m_iGoreFrontRightLeg = 0;
	m_iGoreRearLeftLeg = 0;
	m_iGoreRearRightLeg = 0;
	SetContextThink(&CHL2MP_Player::CustomizationUpdateThink, gpGlobals->curtime, "CustomizationUpdateThink");

	if (((!HL2MPRules()->IsTeamplay() && (sv_ponedm_gamemode.GetInt() == 3)) && GetTeamNumber() == TEAM_ZOMBIES))
	{
		m_nSkin = 1;
	}
#endif

	m_nRenderFX = kRenderNormal;

	m_Local.m_iHideHUD = 0;
	
	AddFlag(FL_ONGROUND); // set the player on the ground at the start of the round.

	m_impactEnergyScale = HL2MPPLAYER_PHYSDAMAGE_SCALE;

	if ( HL2MPRules()->IsIntermission() )
	{
		AddFlag( FL_FROZEN );
	}
	else
	{
		RemoveFlag( FL_FROZEN );
	}

	m_iSpawnInterpCounter = (m_iSpawnInterpCounter + 1) % 8;

	m_Local.m_bDucked = false;

	SetPlayerUnderwater(false);

	m_bReady = false;

	if (GetBotController()) {
		GetBotController()->Spawn();

		//zombie bots will be more agressive.
		if (((!HL2MPRules()->IsTeamplay() && (sv_ponedm_gamemode.GetInt() == 3)) && GetTeamNumber() == TEAM_ZOMBIES))
		{
			CBotProfile* profile = GetBotController()->GetProfile();

			profile->SetAggression(200.0f);
			profile->SetMemoryDuration(20.0f);
			profile->SetReactionDelay(0.0f);
			profile->SetAlertDuration(10.0f);
			profile->SetAimSpeed(AIM_SPEED_VERYFAST, AIM_SPEED_VERYFAST);
			profile->SetAttackDelay(0.0f);
		}
	}

	m_flLastSpawn = gpGlobals->curtime + 0.5f;
}

#ifdef PONEDM
void CHL2MP_Player::UpdatePlayerColor(void)
{
	// bots have their own system so dont do this

	Vector vecNewColor;
	switch (GetTeamNumber())
	{
	case TEAM_RED:
		vecNewColor.x = 255.0f / 255.0f;
		vecNewColor.y = 63.0f / 255.0f;
		vecNewColor.z = 63.0f / 255.0f;
		m_vPrimaryColor = vecNewColor;
		break;
	case TEAM_BLUE:
		vecNewColor.x = 93.3f / 255.0f;
		vecNewColor.y = 166.2f / 255.0f;
		vecNewColor.z = 225.4f / 255.0f;
		m_vPrimaryColor = vecNewColor;
		break;
	case TEAM_ZOMBIES:
		vecNewColor.x = 93.0f / 255.0f;
		vecNewColor.y = 180.0f / 255.0f;
		vecNewColor.z = 117.0f / 255.0f;
		m_vPrimaryColor = vecNewColor;
		break;
	case TEAM_UNASSIGNED:
	default:
		if (!IsFakeClient())
		{
			vecNewColor.x = V_atoi(engine->GetClientConVarValue(entindex(), "cl_ponedm_primarycolor_r")) / 255.0f;
			vecNewColor.y = V_atoi(engine->GetClientConVarValue(entindex(), "cl_ponedm_primarycolor_g")) / 255.0f;
			vecNewColor.z = V_atoi(engine->GetClientConVarValue(entindex(), "cl_ponedm_primarycolor_b")) / 255.0f;
			m_vPrimaryColor = vecNewColor;
		}
		break;
	}

	if (!IsFakeClient())
	{
		Vector vecNewSecondaryColor;

		vecNewSecondaryColor.x = V_atoi(engine->GetClientConVarValue(entindex(), "cl_ponedm_secondarycolor_r")) / 255.0f;
		vecNewSecondaryColor.y = V_atoi(engine->GetClientConVarValue(entindex(), "cl_ponedm_secondarycolor_g")) / 255.0f;
		vecNewSecondaryColor.z = V_atoi(engine->GetClientConVarValue(entindex(), "cl_ponedm_secondarycolor_b")) / 255.0f;

		m_vSecondaryColor = vecNewSecondaryColor;

		Vector vecNewTertiaryColor;

		vecNewTertiaryColor.x = V_atoi(engine->GetClientConVarValue(entindex(), "cl_ponedm_tertiarycolor_r")) / 255.0f;
		vecNewTertiaryColor.y = V_atoi(engine->GetClientConVarValue(entindex(), "cl_ponedm_tertiarycolor_g")) / 255.0f;
		vecNewTertiaryColor.z = V_atoi(engine->GetClientConVarValue(entindex(), "cl_ponedm_tertiarycolor_b")) / 255.0f;

		m_vTertiaryColor = vecNewTertiaryColor;
	}
}

void CHL2MP_Player::UpdatePlayerAppearance()
{
	if (!IsFakeClient())
	{
		m_iUpperManeBodygroup = V_atoi(engine->GetClientConVarValue(entindex(), "cl_ponedm_uppermane"));
		m_iLowerManeBodygroup = V_atoi(engine->GetClientConVarValue(entindex(), "cl_ponedm_lowermane"));
		m_iTailBodygroup = V_atoi(engine->GetClientConVarValue(entindex(), "cl_ponedm_tail"));
		m_iHornBodygroup = V_atoi(engine->GetClientConVarValue(entindex(), "cl_ponedm_horn"));
		m_iWingsBodygroup = V_atoi(engine->GetClientConVarValue(entindex(), "cl_ponedm_wings"));
		SetBodygroup(PONEDM_UPPERMANE_BODYGROUP, m_iUpperManeBodygroup);
		SetBodygroup(PONEDM_LOWERMANE_BODYGROUP, m_iLowerManeBodygroup);
		SetBodygroup(PONEDM_TAIL_BODYGROUP, m_iTailBodygroup);
		SetBodygroup(PONEDM_HORN_BODYGROUP, m_iHornBodygroup);
		SetBodygroup(PONEDM_WINGS_BODYGROUP, m_iWingsBodygroup);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHL2MP_Player::CustomizationUpdateThink()
{
	//no rainbow seizures allowed.
	if (sv_ponedm_updatecolors.GetBool())
		UpdatePlayerColor();

	if (sv_ponedm_updateappearance.GetBool())
		UpdatePlayerAppearance();

	SetContextThink(&CHL2MP_Player::CustomizationUpdateThink, gpGlobals->curtime, "CustomizationUpdateThink");
}
#endif

void CHL2MP_Player::PickupObject( CBaseEntity *pObject, bool bLimitMassAndSize )
{
	
}

void CHL2MP_Player::ResetAnimation( void )
{
	if ( IsAlive() )
	{
		SetSequence ( -1 );
		SetActivity( ACT_INVALID );

		if (!GetAbsVelocity().x && !GetAbsVelocity().y)
			SetAnimation( PLAYER_IDLE );
		else if ((GetAbsVelocity().x || GetAbsVelocity().y) && ( GetFlags() & FL_ONGROUND ))
			SetAnimation( PLAYER_WALK );
		else if (GetWaterLevel() > 1)
			SetAnimation( PLAYER_WALK );
	}
}


bool CHL2MP_Player::Weapon_Switch( CBaseCombatWeapon *pWeapon, int viewmodelindex )
{
	bool bRet = BaseClass::Weapon_Switch( pWeapon, viewmodelindex );

	if ( bRet == true )
	{
		ResetAnimation();
	}

	return bRet;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHL2MP_Player::TraceAttack(const CTakeDamageInfo& info, const Vector& vecDir, trace_t* ptr, CDmgAccumulator* pAccumulator)
{
	if (m_takedamage != DAMAGE_YES)
		return;

	// Save this bone for the ragdoll.
	m_nForceBone = ptr->physicsbone;

	CTakeDamageInfo info_modified = info;

	m_iGoreHead = 0;
	m_iGoreFrontLeftLeg = 0;
	m_iGoreFrontRightLeg = 0;
	m_iGoreRearLeftLeg = 0;
	m_iGoreRearRightLeg = 0;

	switch (ptr->hitgroup)
	{
	case HITGROUP_HEAD:
		m_iGoreHead = 3;
		break;
	case HITGROUP_LEFTARM:
		m_iGoreFrontLeftLeg = 3;
		break;
	case HITGROUP_RIGHTARM:
		m_iGoreFrontRightLeg = 3;
		break;
	case HITGROUP_LEFTLEG:
		m_iGoreRearLeftLeg = 3;
		break;
	case HITGROUP_RIGHTLEG:
		m_iGoreRearRightLeg = 3;
		break;
	default:
		break;
	}

	AddMultiDamage(info_modified, this);

	BaseClass::TraceAttack(info, vecDir, ptr, pAccumulator);
}

void CHL2MP_Player::PreThink( void )
{
	QAngle vOldAngles = GetLocalAngles();
	QAngle vTempAngles = GetLocalAngles();

	vTempAngles = EyeAngles();

	if ( vTempAngles[PITCH] > 180.0f )
	{
		vTempAngles[PITCH] -= 360.0f;
	}

	SetLocalAngles( vTempAngles );

	BaseClass::PreThink();
	State_PreThink();

	//Reset bullet force accumulator, only lasts one frame
	m_vecTotalBulletForce = vec3_origin;
	SetLocalAngles( vOldAngles );
}

void CHL2MP_Player::PostThink( void )
{
	BaseClass::PostThink();
	
	if ( GetFlags() & FL_DUCKING )
	{
		SetCollisionBounds( VEC_CROUCH_TRACE_MIN, VEC_CROUCH_TRACE_MAX );
	}

	m_PlayerAnimState.Update();

	// Store the eye angles pitch so the client can compute its animation state correctly.
	m_angEyeAngles = EyeAngles();

	QAngle angles = GetLocalAngles();
	angles[PITCH] = 0;
	SetLocalAngles( angles );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHL2MP_Player::DismemberRandomLimbs(void)
{
	CUniformRandomStream rand;

	rand.SetSeed((int)gpGlobals->curtime);

	if (m_iGoreHead < 3)
	{
		int iGoreHead = rand.RandomInt(0, 3);

		if (m_iGoreHead < iGoreHead)
			m_iGoreHead = iGoreHead;
	}

	if (m_iGoreFrontLeftLeg < 3)
	{
		int iGoreFrontLeftLeg = rand.RandomInt(0, 3);

		if (m_iGoreFrontLeftLeg < iGoreFrontLeftLeg)
			m_iGoreFrontLeftLeg = iGoreFrontLeftLeg;
	}

	if (m_iGoreFrontRightLeg < 3)
	{
		int iGoreFrontRightLeg = rand.RandomInt(0, 3);

		if (m_iGoreFrontRightLeg < iGoreFrontRightLeg)
			m_iGoreFrontRightLeg = iGoreFrontRightLeg;
	}

	if (m_iGoreRearLeftLeg < 3)
	{
		int iGoreRearLeftLeg = rand.RandomInt(0, 3);

		if (m_iGoreRearLeftLeg < iGoreRearLeftLeg)
			m_iGoreRearLeftLeg = iGoreRearLeftLeg;
	}

	if (m_iGoreRearRightLeg < 3)
	{
		int iGoreRearRightLeg = rand.RandomInt(0, 3);

		if (m_iGoreRearRightLeg < iGoreRearRightLeg)
			m_iGoreRearRightLeg = iGoreRearRightLeg;
	}
}

void CHL2MP_Player::PlayerDeathThink()
{
	if( !IsObserver() )
	{
		BaseClass::PlayerDeathThink();
	}
}

void CHL2MP_Player::FireBullets(const FireBulletsInfo_t &info)
{
	// Move other players back to history positions based on local player's lag
	lagcompensation->StartLagCompensation(this, this->GetCurrentCommand());

	FireBulletsInfo_t modinfo = info;

	CWeaponHL2MPBase* pWeapon = dynamic_cast<CWeaponHL2MPBase*>(GetActiveWeapon());

	if (pWeapon)
	{
		modinfo.m_iPlayerDamage = modinfo.m_flDamage = pWeapon->GetHL2MPWpnData().m_iPlayerDamage;
	}

	NoteWeaponFired();

	BaseClass::FireBullets(modinfo);

	// Move other players back to history positions based on local player's lag
	lagcompensation->FinishLagCompensation(this);
}

void CHL2MP_Player::NoteWeaponFired( void )
{
	Assert( m_pCurrentCommand );
	if( m_pCurrentCommand )
	{
		m_iLastWeaponFireUsercmd = m_pCurrentCommand->command_number;
	}
}

extern ConVar sv_maxunlag;

bool CHL2MP_Player::WantsLagCompensationOnEntity( const CBasePlayer *pPlayer, const CUserCmd *pCmd, const CBitVec<MAX_EDICTS> *pEntityTransmitBits ) const
{
	// No need to lag compensate at all if we're not attacking in this command and
	// we haven't attacked recently.
	if ( !( pCmd->buttons & IN_ATTACK ) && (pCmd->command_number - m_iLastWeaponFireUsercmd > 5) )
		return false;

	// If this entity hasn't been transmitted to us and acked, then don't bother lag compensating it.
	if ( pEntityTransmitBits && !pEntityTransmitBits->Get( pPlayer->entindex() ) )
		return false;

	const Vector &vMyOrigin = GetAbsOrigin();
	const Vector &vHisOrigin = pPlayer->GetAbsOrigin();

	// get max distance player could have moved within max lag compensation time, 
	// multiply by 1.5 to to avoid "dead zones"  (sqrt(2) would be the exact value)
	float maxDistance = 1.5 * pPlayer->MaxSpeed() * sv_maxunlag.GetFloat();

	// If the player is within this distance, lag compensate them in case they're running past us.
	if ( vHisOrigin.DistTo( vMyOrigin ) < maxDistance )
		return true;

	// If their origin is not within a 45 degree cone in front of us, no need to lag compensate.
	Vector vForward;
	AngleVectors( pCmd->viewangles, &vForward );
	
	Vector vDiff = vHisOrigin - vMyOrigin;
	VectorNormalize( vDiff );

	float flCosAngle = 0.707107f;	// 45 degree angle
	if ( vForward.Dot( vDiff ) < flCosAngle )
		return false;

	return true;
}

Activity CHL2MP_Player::TranslateTeamActivity( Activity ActToTranslate )
{
	if ( ActToTranslate == ACT_RUN )
		 return ACT_RUN_AIM_AGITATED;

	if ( ActToTranslate == ACT_IDLE )
		 return ACT_IDLE_AIM_AGITATED;

	if ( ActToTranslate == ACT_WALK )
		 return ACT_WALK_AIM_AGITATED;

	return ActToTranslate;
}

extern ConVar hl2_normspeed;

// Set the activity based on an event or current state
void CHL2MP_Player::SetAnimation( PLAYER_ANIM playerAnim )
{
	int animDesired;

	float speed;

	speed = GetAbsVelocity().Length2D();

	
	// bool bRunning = true;

	//Revisit!
/*	if ( ( m_nButtons & ( IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT ) ) )
	{
		if ( speed > 1.0f && speed < hl2_normspeed.GetFloat() - 20.0f )
		{
			bRunning = false;
		}
	}*/

	if ( GetFlags() & ( FL_FROZEN | FL_ATCONTROLS ) )
	{
		speed = 0;
		playerAnim = PLAYER_IDLE;
	}

	Activity idealActivity = ACT_HL2MP_RUN;

	// This could stand to be redone. Why is playerAnim abstracted from activity? (sjb)
	if ( playerAnim == PLAYER_JUMP )
	{
		idealActivity = ACT_HL2MP_JUMP;
	}
	else if ( playerAnim == PLAYER_DIE )
	{
		if ( m_lifeState == LIFE_ALIVE )
		{
			return;
		}
	}
	else if ( playerAnim == PLAYER_ATTACK1 )
	{
		if ( GetActivity( ) == ACT_HOVER	|| 
			 GetActivity( ) == ACT_SWIM		||
			 GetActivity( ) == ACT_HOP		||
			 GetActivity( ) == ACT_LEAP		||
			 GetActivity( ) == ACT_DIESIMPLE )
		{
			idealActivity = GetActivity( );
		}
		else
		{
			idealActivity = ACT_HL2MP_GESTURE_RANGE_ATTACK;
		}
	}
	else if ( playerAnim == PLAYER_RELOAD )
	{
		idealActivity = ACT_HL2MP_GESTURE_RELOAD;
	}
	else if ( playerAnim == PLAYER_IDLE || playerAnim == PLAYER_WALK )
	{
		if ( !( GetFlags() & FL_ONGROUND ) && GetActivity( ) == ACT_HL2MP_JUMP )	// Still jumping
		{
			idealActivity = GetActivity( );
		}
		/*
		else if ( GetWaterLevel() > 1 )
		{
			if ( speed == 0 )
				idealActivity = ACT_HOVER;
			else
				idealActivity = ACT_SWIM;
		}
		*/
		else
		{
			if ( GetFlags() & FL_DUCKING )
			{
				if ( speed > 0 )
				{
					idealActivity = ACT_HL2MP_WALK_CROUCH;
				}
				else
				{
					idealActivity = ACT_HL2MP_IDLE_CROUCH;
				}
			}
			else
			{
				if ( speed > 0 )
				{
					/*
					if ( bRunning == false )
					{
						idealActivity = ACT_WALK;
					}
					else
					*/
					{
						idealActivity = ACT_HL2MP_RUN;
					}
				}
				else
				{
					idealActivity = ACT_HL2MP_IDLE;
				}
			}
		}

		idealActivity = TranslateTeamActivity( idealActivity );
	}
	
	if ( idealActivity == ACT_HL2MP_GESTURE_RANGE_ATTACK )
	{
		RestartGesture( Weapon_TranslateActivity( idealActivity ) );

		// FIXME: this seems a bit wacked
		Weapon_SetActivity( Weapon_TranslateActivity( ACT_RANGE_ATTACK1 ), 0 );

		return;
	}
	else if ( idealActivity == ACT_HL2MP_GESTURE_RELOAD )
	{
		RestartGesture( Weapon_TranslateActivity( idealActivity ) );
		return;
	}
	else
	{
		SetActivity( idealActivity );

		animDesired = SelectWeightedSequence( Weapon_TranslateActivity ( idealActivity ) );

		if (animDesired == -1)
		{
			animDesired = SelectWeightedSequence( idealActivity );

			if ( animDesired == -1 )
			{
				animDesired = 0;
			}
		}
	
		// Already using the desired animation?
		if ( GetSequence() == animDesired )
			return;

		m_flPlaybackRate = 1.0;
		ResetSequence( animDesired );
		SetCycle( 0 );
		return;
	}

	// Already using the desired animation?
	if ( GetSequence() == animDesired )
		return;

	//Msg( "Set animation to %d\n", animDesired );
	// Reset to first frame of desired animation
	ResetSequence( animDesired );
	SetCycle( 0 );
}


extern int	gEvilImpulse101;
//-----------------------------------------------------------------------------
// Purpose: Player reacts to bumping a weapon. 
// Input  : pWeapon - the weapon that the player bumped into.
// Output : Returns true if player picked up the weapon
//-----------------------------------------------------------------------------
bool CHL2MP_Player::BumpWeapon( CBaseCombatWeapon *pWeapon )
{
	CBaseCombatCharacter *pOwner = pWeapon->GetOwner();

	// Can I have this weapon type?
	if ( !IsAllowedToPickupWeapons() )
		return false;

	if ( pOwner || !Weapon_CanUse( pWeapon ) || !g_pGameRules->CanHavePlayerItem( this, pWeapon ) )
	{
		if ( gEvilImpulse101 )
		{
			UTIL_Remove( pWeapon );
		}
		return false;
	}

	// Don't let the player fetch weapons through walls (use MASK_SOLID so that you can't pickup through windows)
	if( !pWeapon->FVisible( this, MASK_SOLID ) && !(GetFlags() & FL_NOTARGET) )
	{
		return false;
	}

	bool bOwnsWeaponAlready = !!Weapon_OwnsThisType( pWeapon->GetClassname(), pWeapon->GetSubType());

	if ( bOwnsWeaponAlready == true ) 
	{
		//If we have room for the ammo, then "take" the weapon too.
		 if ( Weapon_EquipAmmoOnly( pWeapon ) )
		 {
			 pWeapon->CheckRespawn();

			 UTIL_Remove( pWeapon );
			 return true;
		 }
		 else
		 {
			 return false;
		 }
	}

	pWeapon->CheckRespawn();
	Weapon_Equip( pWeapon );

	return true;
}

void CHL2MP_Player::ChangeTeam( int iTeam )
{
	if (((sv_ponedm_gamemode.GetInt() == 3) && (GetTeamNumber() == TEAM_ZOMBIES)))
	{
		ClientPrint(this, HUD_PRINTTALK, "You cannot change teams right now.");
		return;
	}

	if (GetNextTeamChangeTime() >= gpGlobals->curtime)
	{
		char szReturnString[128];
		Q_snprintf( szReturnString, sizeof( szReturnString ), "Please wait %d more seconds before trying to switch teams again.\n", (int)(GetNextTeamChangeTime() - gpGlobals->curtime) );

		ClientPrint( this, HUD_PRINTTALK, szReturnString );
		return;
	}

	bool bKill = false;

	if ((sv_ponedm_gamemode.GetInt() != 3))
	{
		if (HL2MPRules()->IsTeamplay() != true && iTeam != TEAM_SPECTATOR)
		{
			iTeam = TEAM_UNASSIGNED;
		}
	}

	if ( HL2MPRules()->IsTeamplay() == true )
	{
		if ( iTeam != GetTeamNumber() && GetTeamNumber() != TEAM_UNASSIGNED )
		{
			bKill = true;
		}
	}

	BaseClass::ChangeTeam( iTeam );

	SetModel("models/ppm/player_default_base_new_pantoneshift.mdl");

	if ((sv_ponedm_gamemode.GetInt() != 3) || ((sv_ponedm_gamemode.GetInt() == 3) && (iTeam != TEAM_ZOMBIES)))
	{
		m_flNextTeamChangeTime = gpGlobals->curtime + TEAM_CHANGE_INTERVAL;
	}

	if ( iTeam == TEAM_SPECTATOR )
	{
		RemoveAllItems( true );

		State_Transition( STATE_OBSERVER_MODE );
	}

	if ( bKill == true )
	{
		CommitSuicide();
	}
}

bool CHL2MP_Player::HandleCommand_JoinTeam( int team )
{
	if ( !GetGlobalTeam( team ) || team == 0 )
	{
		Warning( "HandleCommand_JoinTeam( %d ) - invalid team index.\n", team );
		return false;
	}

	if ((!HL2MPRules()->IsTeamplay() && (sv_ponedm_gamemode.GetInt() == 3)))
	{
		Warning("HandleCommand_JoinTeam( %d ) - no zeds allowed\n", team);
		return false;
	}

	if ( team == TEAM_SPECTATOR )
	{
		// Prevent this is the cvar is set
		if ( !mp_allowspectators.GetInt() )
		{
			ClientPrint( this, HUD_PRINTCENTER, "#Cannot_Be_Spectator" );
			return false;
		}

		if ( GetTeamNumber() != TEAM_UNASSIGNED && !IsDead() )
		{
			m_fNextSuicideTime = gpGlobals->curtime;	// allow the suicide to work

			CommitSuicide();

			// add 1 to frags to balance out the 1 subtracted for killing yourself
			IncrementFragCount( 1 );
		}

		ChangeTeam( TEAM_SPECTATOR );

		return true;
	}
	else
	{
		StopObserverMode();
		State_Transition(STATE_ACTIVE);
	}

	// Switch their actual team...
	ChangeTeam( team );

	return true;
}

bool CHL2MP_Player::ClientCommand( const CCommand &args )
{
	if ( FStrEq( args[0], "spectate" ) )
	{
		if ( ShouldRunRateLimitedCommand( args ) )
		{
			// instantly join spectators
			HandleCommand_JoinTeam( TEAM_SPECTATOR );	
		}
		return true;
	}
	else if ( FStrEq( args[0], "jointeam" ) ) 
	{
		if ( args.ArgC() < 2 )
		{
			Warning( "Player sent bad jointeam syntax\n" );
		}

		if ( ShouldRunRateLimitedCommand( args ) )
		{
			int iTeam = atoi( args[1] );
			HandleCommand_JoinTeam( iTeam );
		}
		return true;
	}
	else if ( FStrEq( args[0], "joingame" ) )
	{
		return true;
	}

	return BaseClass::ClientCommand( args );
}

void CHL2MP_Player::CheatImpulseCommands( int iImpulse )
{
	switch ( iImpulse )
	{
		case 101:
			{
				if( sv_cheats->GetBool() )
				{
					GiveItems(true);
				}
			}
			break;

		default:
			BaseClass::CheatImpulseCommands( iImpulse );
	}
}

bool CHL2MP_Player::ShouldRunRateLimitedCommand( const CCommand &args )
{
	int i = m_RateLimitLastCommandTimes.Find( args[0] );
	if ( i == m_RateLimitLastCommandTimes.InvalidIndex() )
	{
		m_RateLimitLastCommandTimes.Insert( args[0], gpGlobals->curtime );
		return true;
	}
	else if ( (gpGlobals->curtime - m_RateLimitLastCommandTimes[i]) < HL2MP_COMMAND_MAX_RATE )
	{
		// Too fast.
		return false;
	}
	else
	{
		m_RateLimitLastCommandTimes[i] = gpGlobals->curtime;
		return true;
	}
}

void CHL2MP_Player::CreateViewModel( int index /*=0*/ )
{
	Assert( index >= 0 && index < MAX_VIEWMODELS );

	if ( GetViewModel( index ) )
		return;

	CPredictedViewModel *vm = ( CPredictedViewModel * )CreateEntityByName( "predicted_viewmodel" );
	if ( vm )
	{
		vm->SetAbsOrigin( GetAbsOrigin() );
		vm->SetOwner( this );
		vm->SetIndex( index );
		DispatchSpawn( vm );
		vm->FollowEntity( this, false );
		m_hViewModel.Set( index, vm );
	}
}

bool CHL2MP_Player::BecomeRagdollOnClient( const Vector &force )
{
	return true;
}

// -------------------------------------------------------------------------------- //
// Ragdoll entities.
// -------------------------------------------------------------------------------- //

class CHL2MPRagdoll : public CBaseAnimatingOverlay
{
public:
	DECLARE_CLASS( CHL2MPRagdoll, CBaseAnimatingOverlay );
	DECLARE_SERVERCLASS();

	// Transmit ragdolls to everyone.
	virtual int UpdateTransmitState()
	{
		return SetTransmitState( FL_EDICT_ALWAYS );
	}

public:
	// In case the client has the player entity, we transmit the player index.
	// In case the client doesn't have it, we transmit the player's model index, origin, and angles
	// so they can create a ragdoll in the right place.
	CNetworkHandle( CBaseEntity, m_hPlayer );	// networked entity handle 
	CNetworkVector( m_vecRagdollVelocity );
	CNetworkVector( m_vecRagdollOrigin );
#ifdef PONEDM
	CNetworkVar(int, m_iPlayerIndex);
	CNetworkVar(int, m_iUpperManeBodygroup);
	CNetworkVar(int, m_iLowerManeBodygroup);
	CNetworkVar(int, m_iTailBodygroup);
	CNetworkVar(int, m_iHornBodygroup);
	CNetworkVar(int, m_iWingsBodygroup);
	CNetworkVar(unsigned short, m_iGoreHead);
	CNetworkVar(unsigned short, m_iGoreFrontLeftLeg);
	CNetworkVar(unsigned short, m_iGoreFrontRightLeg);
	CNetworkVar(unsigned short, m_iGoreRearLeftLeg);
	CNetworkVar(unsigned short, m_iGoreRearRightLeg);
#endif
};

LINK_ENTITY_TO_CLASS( hl2mp_ragdoll, CHL2MPRagdoll );

IMPLEMENT_SERVERCLASS_ST_NOBASE( CHL2MPRagdoll, DT_HL2MPRagdoll )
	SendPropVector( SENDINFO(m_vecRagdollOrigin), -1,  SPROP_COORD ),
#ifdef PONEDM
	SendPropInt(SENDINFO(m_iPlayerIndex), 7, SPROP_UNSIGNED),
	SendPropInt(SENDINFO(m_iGoreHead), 2, SPROP_UNSIGNED),
	SendPropInt(SENDINFO(m_iGoreFrontLeftLeg), 2, SPROP_UNSIGNED),
	SendPropInt(SENDINFO(m_iGoreFrontRightLeg), 2, SPROP_UNSIGNED),
	SendPropInt(SENDINFO(m_iGoreRearLeftLeg), 2, SPROP_UNSIGNED),
	SendPropInt(SENDINFO(m_iGoreRearRightLeg), 2, SPROP_UNSIGNED),
	SendPropInt(SENDINFO(m_iUpperManeBodygroup), -1, SPROP_UNSIGNED),
	SendPropInt(SENDINFO(m_iLowerManeBodygroup), -1, SPROP_UNSIGNED),
	SendPropInt(SENDINFO(m_iTailBodygroup), -1, SPROP_UNSIGNED),
	SendPropInt(SENDINFO(m_iHornBodygroup), -1, SPROP_UNSIGNED),
	SendPropInt(SENDINFO(m_iWingsBodygroup), -1, SPROP_UNSIGNED),
#endif
	SendPropEHandle( SENDINFO( m_hPlayer ) ),
	SendPropModelIndex( SENDINFO( m_nModelIndex ) ),
	SendPropInt		( SENDINFO(m_nForceBone), 8, 0 ),
	SendPropVector	( SENDINFO(m_vecForce), -1, SPROP_NOSCALE ),
	SendPropVector( SENDINFO( m_vecRagdollVelocity ) )
END_SEND_TABLE()


void CHL2MP_Player::CreateRagdollEntity( void )
{
	if ( m_hRagdoll )
	{
		UTIL_RemoveImmediate( m_hRagdoll );
		m_hRagdoll = NULL;
	}

	// If we already have a ragdoll, don't make another one.
	CHL2MPRagdoll *pRagdoll = dynamic_cast< CHL2MPRagdoll* >( m_hRagdoll.Get() );
	
	if ( !pRagdoll )
	{
		// create a new one
		pRagdoll = dynamic_cast< CHL2MPRagdoll* >( CreateEntityByName( "hl2mp_ragdoll" ) );
	}

	if ( pRagdoll )
	{
		pRagdoll->m_hPlayer = this;
		pRagdoll->m_vecRagdollOrigin = GetAbsOrigin();
		pRagdoll->m_vecRagdollVelocity = GetAbsVelocity();
		pRagdoll->m_nModelIndex = m_nModelIndex;
		pRagdoll->m_nForceBone = m_nForceBone;
#ifdef PONEDM
		pRagdoll->m_iPlayerIndex.Set(entindex());
		pRagdoll->m_iGoreHead = m_iGoreHead;
		pRagdoll->m_iGoreFrontLeftLeg = m_iGoreFrontLeftLeg;
		pRagdoll->m_iGoreFrontRightLeg = m_iGoreFrontRightLeg;
		pRagdoll->m_iGoreRearLeftLeg = m_iGoreRearLeftLeg;
		pRagdoll->m_iGoreRearRightLeg = m_iGoreRearRightLeg;
		pRagdoll->m_iUpperManeBodygroup = m_iUpperManeBodygroup;
		pRagdoll->m_iLowerManeBodygroup = m_iLowerManeBodygroup;
		pRagdoll->m_iTailBodygroup = m_iTailBodygroup;
		pRagdoll->m_iHornBodygroup = m_iHornBodygroup;
		pRagdoll->m_iWingsBodygroup = m_iWingsBodygroup;
#endif
		pRagdoll->m_vecForce = m_vecTotalBulletForce;
		pRagdoll->SetAbsOrigin( GetAbsOrigin() );
	}

	// ragdolls will be removed on round restart automatically
	m_hRagdoll = pRagdoll;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int CHL2MP_Player::FlashlightIsOn( void )
{
	return IsEffectActive( EF_DIMLIGHT );
}

extern ConVar flashlight;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHL2MP_Player::FlashlightTurnOn( void )
{
	if( flashlight.GetInt() > 0 && IsAlive() )
	{
		AddEffects( EF_DIMLIGHT );
		EmitSound( "HL2Player.FlashlightOn" );
	}
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHL2MP_Player::FlashlightTurnOff( void )
{
	RemoveEffects( EF_DIMLIGHT );
	
	if( IsAlive() )
	{
		EmitSound( "HL2Player.FlashlightOff" );
	}
}

void CHL2MP_Player::Weapon_Drop( CBaseCombatWeapon *pWeapon, const Vector *pvecTarget, const Vector *pVelocity )
{
	//Drop a grenade if it's primed.
	if ( GetActiveWeapon() )
	{
		CBaseCombatWeapon *pGrenade = Weapon_OwnsThisType("weapon_frag");

		if ( GetActiveWeapon() == pGrenade )
		{
			if ( ( m_nButtons & IN_ATTACK ) || (m_nButtons & IN_ATTACK2) )
			{
				DropPrimedFragGrenade( this, pGrenade );
				return;
			}
		}
	}

	BaseClass::Weapon_Drop( pWeapon, pvecTarget, pVelocity );
}


void CHL2MP_Player::DetonateTripmines( void )
{
	CBaseEntity *pEntity = NULL;

	while ((pEntity = gEntList.FindEntityByClassname( pEntity, "npc_satchel" )) != NULL)
	{
		CSatchelCharge *pSatchel = dynamic_cast<CSatchelCharge *>(pEntity);
		if (pSatchel->m_bIsLive && pSatchel->GetThrower() == this )
		{
			g_EventQueue.AddEvent( pSatchel, "Explode", 0.20, this, this );
		}
	}

	// Play sound for pressing the detonator
	EmitSound( "Weapon_SLAM.SatchelDetonate" );
}

void CHL2MP_Player::Event_Killed( const CTakeDamageInfo &info )
{
	//update damage info with our accumulated physics force
	CTakeDamageInfo subinfo = info;
	subinfo.SetDamageForce( m_vecTotalBulletForce );

	SetNumAnimOverlays( 0 );

	/*if (info.GetDamageType() & DMG_FALL) // fall damage
	{
		// begone legs!
		if (m_iGoreFrontRightLeg < 3)
			m_iGoreFrontRightLeg = 3;
		if (m_iGoreFrontLeftLeg < 3)
			m_iGoreFrontLeftLeg = 3;
		if (m_iGoreRearRightLeg < 3)
			m_iGoreRearRightLeg = 3;
		if (m_iGoreRearLeftLeg < 3)
			m_iGoreRearLeftLeg = 3;
	}*/

	if (sv_ponedm_gamemode.GetInt() == 2 || (info.GetDamageType() & DMG_BLAST || info.GetDamageType() & DMG_NERVEGAS)) // explosives or sawblade
		DismemberRandomLimbs();

	// Note: since we're dead, it won't draw us on the client, but we don't set EF_NODRAW
	// because we still want to transmit to the clients in our PVS.
	CreateRagdollEntity();

	DetonateTripmines();

	BaseClass::Event_Killed( subinfo );

	if ( info.GetDamageType() & DMG_DISSOLVE )
	{
		if ( m_hRagdoll )
		{
			m_hRagdoll->GetBaseAnimating()->Dissolve( NULL, gpGlobals->curtime, false, ENTITY_DISSOLVE_NORMAL );
		}
	}

	CBaseEntity *pAttacker = info.GetAttacker();

	if ( pAttacker )
	{
		int iScoreToAdd = 1;

		if ( pAttacker == this )
		{
			iScoreToAdd = -1;
		}

		GetGlobalTeam( pAttacker->GetTeamNumber() )->AddScore( iScoreToAdd );
	}

	FlashlightTurnOff();

	m_lifeState = LIFE_DEAD;

	RemoveEffects( EF_NODRAW );	// still draw player body
	StopZooming();

	if (GetBotController()) {
		GetBotController()->OnDeath(info);
	}
}

void DeliverDamageForce(CHL2MP_Player *pPlayer, float damage)
{
	Vector forward, up;
	AngleVectors(pPlayer->GetLocalAngles(), &forward, NULL, &up);
	forward = forward * 100 * damage;
	up = up * 100 * damage;

	pPlayer->VelocityPunch(-forward);
	pPlayer->VelocityPunch(up);
}

int CHL2MP_Player::OnTakeDamage( const CTakeDamageInfo &inputInfo )
{
	CTakeDamageInfo info = inputInfo;

	//return here if the player is in the respawn grace period vs. slams.
	if ( gpGlobals->curtime < m_flSlamProtectTime &&  (info.GetDamageType() == DMG_BLAST ) )
		return 0;

	m_vecTotalBulletForce += info.GetDamageForce();

	// if this is our own rocket, scale down the damage
	if (info.GetAttacker() == this)
	{
		float flDamage = info.GetDamage() * sv_ponedm_damagescale_self.GetFloat();
		info.SetDamage(flDamage);

		DeliverDamageForce(this, sv_ponedm_damageforcescale_self.GetFloat());
	}
	
	gamestats->Event_PlayerDamage( this, info);

	if (GetBotController()) {
		GetBotController()->OnTakeDamage(info);
	}

	return BaseClass::OnTakeDamage(info);
}

void CHL2MP_Player::DeathSound( const CTakeDamageInfo &info )
{
	if ( m_hRagdoll && m_hRagdoll->GetBaseAnimating()->IsDissolving() )
		 return;

	char szStepSound[128];
#ifndef PONEDM
	Q_snprintf( szStepSound, sizeof( szStepSound ), "%s.Die", GetPlayerModelSoundPrefix() );
#else
	Q_snprintf(szStepSound, sizeof(szStepSound), "Pony.Die");
#endif

	const char *pModelName = STRING( GetModelName() );

	CSoundParameters params;
	if ( GetParametersForSound( szStepSound, params, pModelName ) == false )
		return;

	Vector vecOrigin = GetAbsOrigin();
	
	CRecipientFilter filter;
	filter.AddRecipientsByPAS( vecOrigin );

	EmitSound_t ep;
	ep.m_nChannel = params.channel;
	ep.m_pSoundName = params.soundname;
	ep.m_flVolume = params.volume;
	ep.m_SoundLevel = params.soundlevel;
	ep.m_nFlags = 0;
	ep.m_nPitch = params.pitch;
	ep.m_pOrigin = &vecOrigin;

	EmitSound( filter, entindex(), ep );
}

CBaseEntity* CHL2MP_Player::EntSelectSpawnPoint(void)
{
	CBaseEntity* pSpot = NULL;
	CBaseEntity* pLastSpawnPoint = g_pLastSpawn;
	//edict_t		*player = edict();
	const char* pSpawnpointName = "info_player_deathmatch";

	static const char* Team1Spawns[] = {
		"info_player_rebel",
		"info_player_terrorist",
		"info_player_axis",
		"info_player_red",
		"info_player_teamspawn"
	};
	static const char* Team2Spawns[] = {
		"info_player_combine",
		"info_player_counterterrorist",
		"info_player_allies",
		"info_player_blue",
		"info_player_teamspawn"
	};
	static const char* NonTeamSpawns[] = {
		"info_player_deathmatch",
		"info_player_start",
	};

	if (HL2MPRules()->IsTeamplay() == true)
	{
		if (GetTeamNumber() == TEAM_BLUE)
		{
			int teamBlue = ARRAYSIZE(Team2Spawns);
			for (int i = 0; i < teamBlue; ++i)
			{
				CBaseEntity* pSpawn2 = gEntList.FindEntityByClassname(NULL, Team2Spawns[i]);
				if (pSpawn2 != NULL)
				{
					if (FStrEq(pSpawn2->GetClassname(), "info_player_teamspawn"))
					{
						//TF2 uses different team numbers for teams.
						if (pSpawn2->GetTeamNumber() == TEAM_RED)
						{
							pSpawnpointName = Team2Spawns[i];
							pLastSpawnPoint = g_pLastCombineSpawn;
						}
					}
					else
					{
						pSpawnpointName = Team2Spawns[i];
						pLastSpawnPoint = g_pLastCombineSpawn;
					}
				}
			}
		}
		else if (GetTeamNumber() == TEAM_RED)
		{
			int teamRed = ARRAYSIZE(Team1Spawns);
			for (int i = 0; i < teamRed; ++i)
			{
				CBaseEntity* pSpawn1 = gEntList.FindEntityByClassname(NULL, Team1Spawns[i]);
				if (pSpawn1 != NULL)
				{
					if (FStrEq(pSpawn1->GetClassname(), "info_player_teamspawn"))
					{
						//TF2 uses different team numbers for teams.
						if (pSpawn1->GetTeamNumber() == TEAM_BLUE)
						{
							pSpawnpointName = Team1Spawns[i];
							pLastSpawnPoint = g_pLastRebelSpawn;
						}
					}
					else
					{
						pSpawnpointName = Team1Spawns[i];
						pLastSpawnPoint = g_pLastRebelSpawn;
					}
				}
			}
		}
	}
	else if (sv_ponedm_gamemode.GetInt() == 3)
	{
		if (((!HL2MPRules()->IsTeamplay() && (sv_ponedm_gamemode.GetInt() == 3)) && GetTeamNumber() == TEAM_ZOMBIES))
		{
			int teamBlue = ARRAYSIZE(Team2Spawns);
			for (int i = 0; i < teamBlue; ++i)
			{
				CBaseEntity* pSpawn2 = gEntList.FindEntityByClassname(NULL, Team2Spawns[i]);
				if (pSpawn2 != NULL)
				{
					if (FStrEq(pSpawn2->GetClassname(), "info_player_teamspawn"))
					{
						//TF2 uses different team numbers for teams.
						if (pSpawn2->GetTeamNumber() == TEAM_RED)
						{
							pSpawnpointName = Team2Spawns[i];
							pLastSpawnPoint = g_pLastCombineSpawn;
						}
					}
					else
					{
						pSpawnpointName = Team2Spawns[i];
						pLastSpawnPoint = g_pLastCombineSpawn;
					}
				}
			}
		}
		else if (GetTeamNumber() == TEAM_UNASSIGNED)
		{
			int teamRed = ARRAYSIZE(Team1Spawns);
			for (int i = 0; i < teamRed; ++i)
			{
				CBaseEntity* pSpawn1 = gEntList.FindEntityByClassname(NULL, Team1Spawns[i]);
				if (pSpawn1 != NULL)
				{
					if (FStrEq(pSpawn1->GetClassname(), "info_player_teamspawn"))
					{
						//TF2 uses different team numbers for teams.
						if (pSpawn1->GetTeamNumber() == TEAM_BLUE)
						{
							pSpawnpointName = Team1Spawns[i];
							pLastSpawnPoint = g_pLastRebelSpawn;
						}
					}
					else
					{
						pSpawnpointName = Team1Spawns[i];
						pLastSpawnPoint = g_pLastRebelSpawn;
					}
				}
			}
		}
	}
	else
	{
		int teamSpawn = RandomInt(0, 1);

		if (teamSpawn == 1)
		{
			int teamBlue = ARRAYSIZE(Team2Spawns);
			for (int i = 0; i < teamBlue; ++i)
			{
				if (gEntList.FindEntityByClassname(NULL, Team2Spawns[i]) != NULL)
				{
					pSpawnpointName = Team2Spawns[i];
				}
			}
		}
		else
		{
			int teamRed = ARRAYSIZE(Team1Spawns);
			for (int i = 0; i < teamRed; ++i)
			{
				if (gEntList.FindEntityByClassname(NULL, Team1Spawns[i]) != NULL)
				{
					pSpawnpointName = Team1Spawns[i];
				}
			}
		}

		pLastSpawnPoint = g_pLastSpawn;
	}

	if (gEntList.FindEntityByClassname(NULL, pSpawnpointName) == NULL)
	{
		int nonTeamSpawns = ARRAYSIZE(NonTeamSpawns);
		for (int i = 0; i < nonTeamSpawns; ++i)
		{
			if (gEntList.FindEntityByClassname(NULL, NonTeamSpawns[i]) != NULL)
			{
				pSpawnpointName = NonTeamSpawns[i];
				pLastSpawnPoint = g_pLastSpawn;
			}
		}
	}

	pSpot = pLastSpawnPoint;
	// Randomize the start spot
	for (int i = random->RandomInt(1, 5); i > 0; i--)
		pSpot = gEntList.FindEntityByClassname(pSpot, pSpawnpointName);
	if (!pSpot)  // skip over the null point
		pSpot = gEntList.FindEntityByClassname(pSpot, pSpawnpointName);

	CBaseEntity* pFirstSpot = pSpot;

	do
	{
		if (pSpot)
		{
			// check if pSpot is valid
			if (g_pGameRules->IsSpawnPointValid(pSpot, this))
			{
				if (pSpot->GetLocalOrigin() == vec3_origin)
				{
					pSpot = gEntList.FindEntityByClassname(pSpot, pSpawnpointName);
					continue;
				}

				// if so, go to pSpot
				goto ReturnSpot;
			}
		}
		// increment pSpot
		pSpot = gEntList.FindEntityByClassname(pSpot, pSpawnpointName);
	} while (pSpot != pFirstSpot); // loop if we're not back to the start

	// we haven't found a place to spawn yet,  so kill any guy at the first spawn point and spawn there
	/*if ( pSpot )
	{
		CBaseEntity *ent = NULL;
		for ( CEntitySphereQuery sphere( pSpot->GetAbsOrigin(), 128 ); (ent = sphere.GetCurrentEntity()) != NULL; sphere.NextEntity() )
		{
			// if ent is a client, kill em (unless they are ourselves)
			if ( ent->IsPlayer() && !(ent->edict() == player) )
				ent->TakeDamage( CTakeDamageInfo( GetContainingEntity(INDEXENT(0)), GetContainingEntity(INDEXENT(0)), 300, DMG_GENERIC ) );
		}
		goto ReturnSpot;
	}*/

	if (!pSpot)
	{
		pSpot = gEntList.FindEntityByClassname(pSpot, "info_player_start");

		if (pSpot)
			goto ReturnSpot;
	}

ReturnSpot:

	if (HL2MPRules()->IsTeamplay() == true)
	{
		if (GetTeamNumber() == TEAM_BLUE)
		{
			g_pLastCombineSpawn = pSpot;
		}
		else if (GetTeamNumber() == TEAM_RED)
		{
			g_pLastRebelSpawn = pSpot;
		}
	}
	else
	{
		if (((!HL2MPRules()->IsTeamplay() && (sv_ponedm_gamemode.GetInt() == 3)) && GetTeamNumber() == TEAM_ZOMBIES))
		{
			g_pLastCombineSpawn = pSpot;
		}
		else if (GetTeamNumber() == TEAM_UNASSIGNED)
		{
			g_pLastRebelSpawn = pSpot;
		}
	}

	g_pLastSpawn = pSpot;

	m_flSlamProtectTime = gpGlobals->curtime + 0.5;

	return pSpot;
}

CON_COMMAND( timeleft, "prints the time remaining in the match" )
{
	CHL2MP_Player *pPlayer = ToHL2MPPlayer( UTIL_GetCommandClient() );

	int iTimeRemaining = (int)HL2MPRules()->GetMapRemainingTime();
    
	if ( iTimeRemaining == 0 )
	{
		if ( pPlayer )
		{
			ClientPrint( pPlayer, HUD_PRINTTALK, "This game has no timelimit." );
		}
		else
		{
			Msg( "* No Time Limit *\n" );
		}
	}
	else
	{
		int iMinutes, iSeconds;
		iMinutes = iTimeRemaining / 60;
		iSeconds = iTimeRemaining % 60;

		char minutes[8];
		char seconds[8];

		Q_snprintf( minutes, sizeof(minutes), "%d", iMinutes );
		Q_snprintf( seconds, sizeof(seconds), "%2.2d", iSeconds );

		if ( pPlayer )
		{
			ClientPrint( pPlayer, HUD_PRINTTALK, "Time left in map: %s1:%s2", minutes, seconds );
		}
		else
		{
			Msg( "Time Remaining:  %s:%s\n", minutes, seconds );
		}
	}	
}

CON_COMMAND(showposition, "shows the player's position for mapadds.")
{
	CHL2MP_Player* pPlayer = ToHL2MPPlayer(UTIL_GetCommandClient());
	Vector vecOrigin = pPlayer->GetAbsOrigin() + Vector(0, 0, 10);
	Msg("Player position XYZ Coords: X: %f Y: %f Z: %f\n", vecOrigin.x, vecOrigin.y, vecOrigin.z);
}

void CHL2MP_Player::Reset()
{	
	ResetDeathCount();
	ResetFragCount();
}

bool CHL2MP_Player::IsReady()
{
	return m_bReady;
}

void CHL2MP_Player::SetReady( bool bReady )
{
	m_bReady = bReady;
}

void CHL2MP_Player::CheckChatText( char *p, int bufsize )
{
	//Look for escape sequences and replace

	char *buf = new char[bufsize];
	int pos = 0;

	// Parse say text for escape sequences
	for ( char *pSrc = p; pSrc != NULL && *pSrc != 0 && pos < bufsize-1; pSrc++ )
	{
		// copy each char across
		buf[pos] = *pSrc;
		pos++;
	}

	buf[pos] = '\0';

	// copy buf back into p
	Q_strncpy( p, buf, bufsize );

	delete[] buf;	

	const char *pReadyCheck = p;

	HL2MPRules()->CheckChatForReadySignal( this, pReadyCheck );
}

void CHL2MP_Player::State_Transition( HL2MPPlayerState newState )
{
	State_Leave();
	State_Enter( newState );
}


void CHL2MP_Player::State_Enter( HL2MPPlayerState newState )
{
	m_iPlayerState = newState;
	m_pCurStateInfo = State_LookupInfo( newState );

	// Initialize the new state.
	if ( m_pCurStateInfo && m_pCurStateInfo->pfnEnterState )
		(this->*m_pCurStateInfo->pfnEnterState)();
}


void CHL2MP_Player::State_Leave()
{
	if ( m_pCurStateInfo && m_pCurStateInfo->pfnLeaveState )
	{
		(this->*m_pCurStateInfo->pfnLeaveState)();
	}
}


void CHL2MP_Player::State_PreThink()
{
	if ( m_pCurStateInfo && m_pCurStateInfo->pfnPreThink )
	{
		(this->*m_pCurStateInfo->pfnPreThink)();
	}
}


CHL2MPPlayerStateInfo *CHL2MP_Player::State_LookupInfo( HL2MPPlayerState state )
{
	// This table MUST match the 
	static CHL2MPPlayerStateInfo playerStateInfos[] =
	{
		{ STATE_ACTIVE,			"STATE_ACTIVE",			&CHL2MP_Player::State_Enter_ACTIVE, NULL, &CHL2MP_Player::State_PreThink_ACTIVE },
		{ STATE_OBSERVER_MODE,	"STATE_OBSERVER_MODE",	&CHL2MP_Player::State_Enter_OBSERVER_MODE,	NULL, &CHL2MP_Player::State_PreThink_OBSERVER_MODE }
	};

	for ( int i=0; i < ARRAYSIZE( playerStateInfos ); i++ )
	{
		if ( playerStateInfos[i].m_iPlayerState == state )
			return &playerStateInfos[i];
	}

	return NULL;
}

bool CHL2MP_Player::StartObserverMode(int mode)
{
	//we only want to go into observer mode if the player asked to, not on a death timeout
	if ( m_bEnterObserver == true )
	{
		VPhysicsDestroyObject();
		return BaseClass::StartObserverMode( mode );
	}
	return false;
}

void CHL2MP_Player::StopObserverMode()
{
	m_bEnterObserver = false;
	BaseClass::StopObserverMode();
}

void CHL2MP_Player::State_Enter_OBSERVER_MODE()
{
	int observerMode = m_iObserverLastMode;
	if ( IsNetClient() )
	{
		const char *pIdealMode = engine->GetClientConVarValue( engine->IndexOfEdict( edict() ), "cl_spec_mode" );
		if ( pIdealMode )
		{
			observerMode = atoi( pIdealMode );
			if ( observerMode <= OBS_MODE_FIXED || observerMode > OBS_MODE_ROAMING )
			{
				observerMode = m_iObserverLastMode;
			}
		}
	}
	m_bEnterObserver = true;
	StartObserverMode( observerMode );
}

void CHL2MP_Player::State_PreThink_OBSERVER_MODE()
{
	// Make sure nobody has changed any of our state.
	//	Assert( GetMoveType() == MOVETYPE_FLY );
	Assert( m_takedamage == DAMAGE_NO );
	Assert( IsSolidFlagSet( FSOLID_NOT_SOLID ) );
	//	Assert( IsEffectActive( EF_NODRAW ) );

	// Must be dead.
	Assert( m_lifeState == LIFE_DEAD );
	Assert( pl.deadflag );
}


void CHL2MP_Player::State_Enter_ACTIVE()
{
	SetMoveType( MOVETYPE_WALK );
	
	// md 8/15/07 - They'll get set back to solid when they actually respawn. If we set them solid now and mp_forcerespawn
	// is false, then they'll be spectating but blocking live players from moving.
	// RemoveSolidFlags( FSOLID_NOT_SOLID );
	
	m_Local.m_iHideHUD = 0;
}


void CHL2MP_Player::State_PreThink_ACTIVE()
{
	//we don't really need to do anything here. 
	//This state_prethink structure came over from CS:S and was doing an assert check that fails the way hl2dm handles death
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CHL2MP_Player::CanHearAndReadChatFrom( CBasePlayer *pPlayer )
{
	// can always hear the console unless we're ignoring all chat
	if ( !pPlayer )
		return false;

	return true;
}

//================================================================================
//================================================================================
void CHL2MP_Player::SetBotController(IBot* pBot)
{
	if (m_pBotController) {
		delete m_pBotController;
		m_pBotController = NULL;
	}

	m_pBotController = pBot;
}

//================================================================================
//================================================================================
void CHL2MP_Player::SetUpBot()
{
	CreateSenses();
	//SetBotController(new CBot(this));
	SetBotController(new CPoneDM_Bot(this));
}

//================================================================================
//================================================================================
void CHL2MP_Player::CreateSenses()
{
	m_pSenses = new CAI_Senses;
	m_pSenses->SetOuter(this);
}

//================================================================================
//================================================================================
void CHL2MP_Player::SetDistLook(float flDistLook)
{
	if (GetSenses()) {
		GetSenses()->SetDistLook(flDistLook);
	}
}

//================================================================================
//================================================================================
int CHL2MP_Player::GetSoundInterests()
{
	return SOUND_DANGER | SOUND_COMBAT | SOUND_PLAYER | SOUND_CARCASS | SOUND_MEAT | SOUND_GARBAGE;
}

//================================================================================
//================================================================================
int CHL2MP_Player::GetSoundPriority(CSound* pSound)
{
	if (pSound->IsSoundType(SOUND_COMBAT)) {
		return SOUND_PRIORITY_HIGH;
	}

	if (pSound->IsSoundType(SOUND_DANGER)) {
		if (pSound->IsSoundType(SOUND_CONTEXT_FROM_SNIPER | SOUND_CONTEXT_EXPLOSION)) {
			return SOUND_PRIORITY_HIGHEST;
		}
		else if (pSound->IsSoundType(SOUND_CONTEXT_GUNFIRE | SOUND_BULLET_IMPACT)) {
			return SOUND_PRIORITY_VERY_HIGH;
		}

		return SOUND_PRIORITY_HIGH;
	}

	if (pSound->IsSoundType(SOUND_CARCASS | SOUND_MEAT | SOUND_GARBAGE)) {
		return SOUND_PRIORITY_VERY_LOW;
	}

	return SOUND_PRIORITY_NORMAL;
}

//================================================================================
//================================================================================
bool CHL2MP_Player::QueryHearSound(CSound* pSound)
{
	CBaseEntity* pOwner = pSound->m_hOwner.Get();

	if (pOwner == this)
		return false;

	if (pSound->IsSoundType(SOUND_PLAYER) && !pOwner) {
		return false;
	}

	if (pSound->IsSoundType(SOUND_CONTEXT_ALLIES_ONLY)) {
		if (Classify() != CLASS_PLAYER_ALLY && Classify() != CLASS_PLAYER_ALLY_VITAL) {
			return false;
		}
	}

	if (pOwner) {
		// Solo escuchemos sonidos provocados por nuestros aliados si son de combate.
		if (TheGameRules->PlayerRelationship(this, pOwner) == GR_ALLY) {
			if (pSound->IsSoundType(SOUND_COMBAT) && !pSound->IsSoundType(SOUND_CONTEXT_GUNFIRE)) {
				return true;
			}

			return false;
		}
	}

	if (ShouldIgnoreSound(pSound)) {
		return false;
	}

	return true;
}

//================================================================================
//================================================================================
bool CHL2MP_Player::QuerySeeEntity(CBaseEntity* pEntity, bool bOnlyHateOrFear)
{
	if (bOnlyHateOrFear) {
		if (HL2MPRules()->PlayerRelationship(this, pEntity) == GR_NOTTEAMMATE)
			return true;

		Disposition_t disposition = IRelationType(pEntity);
		return (disposition == D_HT || disposition == D_FR);
	}

	return true;
}

//================================================================================
//================================================================================
void CHL2MP_Player::OnLooked(int iDistance)
{
	if (GetBotController()) {
		GetBotController()->OnLooked(iDistance);
	}
}

//================================================================================
//================================================================================
void CHL2MP_Player::OnListened()
{
	if (GetBotController()) {
		GetBotController()->OnListened();
	}
}

//================================================================================
//================================================================================
CSound* CHL2MP_Player::GetLoudestSoundOfType(int iType)
{
	return CSoundEnt::GetLoudestSoundOfType(iType, EarPosition());
}

//================================================================================
// Devuelve si podemos ver el origen del sonido
//================================================================================
bool CHL2MP_Player::SoundIsVisible(CSound* pSound)
{
	return (FVisible(pSound->GetSoundReactOrigin()) && IsInFieldOfView(pSound->GetSoundReactOrigin()));
}

//================================================================================
//================================================================================
CSound* CHL2MP_Player::GetBestSound(int validTypes)
{
	CSound* pResult = GetSenses()->GetClosestSound(false, validTypes);

	if (pResult == NULL) {
		DevMsg("NULL Return from GetBestSound\n");
	}

	return pResult;
}

//================================================================================
//================================================================================
CSound* CHL2MP_Player::GetBestScent()
{
	CSound* pResult = GetSenses()->GetClosestSound(true);

	if (pResult == NULL) {
		DevMsg("NULL Return from GetBestScent\n");
	}

	return pResult;
}
