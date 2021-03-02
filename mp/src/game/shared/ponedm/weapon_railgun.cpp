//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "in_buttons.h"
#include "beam_shared.h"

#ifdef CLIENT_DLL
	#include "c_hl2mp_player.h"
#else
	#include "hl2mp_player.h"
	#include "ammodef.h"
#endif

#include "weapon_hl2mpbasehlmpcombatweapon.h"

#define GAUSS_BEAM_SPRITE "sprites/laserbeam.vmt"
#define RAIL_RECHARGE_TIME 0.13f

#ifdef CLIENT_DLL
#define CWeaponRailgun C_WeaponRailgun
#endif

//-----------------------------------------------------------------------------
// CWeaponRailgun
//-----------------------------------------------------------------------------

class CWeaponRailgun : public CBaseHL2MPCombatWeapon
{
	DECLARE_CLASS( CWeaponRailgun, CBaseHL2MPCombatWeapon );
public:

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
#endif

	CWeaponRailgun( void );

	void	Equip(CBaseCombatCharacter* pOwner);
	bool	Deploy(void);
	bool	Holster(CBaseCombatWeapon* pSwitchingTo);
	void	ItemPostFrame(void);
	void	ItemBusyFrame(void);
	void	PrimaryAttack( void );
	void	SecondaryAttack(void);
	void	Fire(void);
	void	RechargeAmmo(void);
	void	DrawBeam(const Vector& startPos, const Vector& endPos);
	virtual const Vector& GetBulletSpread(void)
	{
		static Vector cone = VECTOR_CONE_1DEGREES;
		return cone;
	}
	float GetFireRate(void)
	{
		return 1.0f;
	}

private:
	void	CheckZoomToggle(void);
	void	ToggleZoom(void);

private:
	CBeam* m_pBeam;
	CNetworkVar(float, m_flNextCharge);
	CNetworkVar(bool, m_bInZoom);
	
	CWeaponRailgun( const CWeaponRailgun & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponRailgun, DT_WeaponRailgun )

BEGIN_NETWORK_TABLE( CWeaponRailgun, DT_WeaponRailgun )
#ifdef CLIENT_DLL
	RecvPropBool(RECVINFO(m_bInZoom)),
	RecvPropFloat(RECVINFO(m_flNextCharge)),
#else
	SendPropBool(SENDINFO(m_bInZoom)),
	SendPropFloat(SENDINFO(m_flNextCharge)),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA( CWeaponRailgun )
	DEFINE_PRED_FIELD(m_bInZoom, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE),
	DEFINE_PRED_FIELD_TOL(m_flNextCharge, FIELD_FLOAT, FTYPEDESC_INSENDTABLE, TD_MSECTOLERANCE),
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS( weapon_railgun, CWeaponRailgun );
PRECACHE_WEAPON_REGISTER( weapon_railgun );


#ifndef CLIENT_DLL
acttable_t CWeaponRailgun::m_acttable[] = 
{
	{ ACT_HL2MP_IDLE,					ACT_HL2MP_IDLE_CROSSBOW,					false },
	{ ACT_HL2MP_RUN,					ACT_HL2MP_RUN_CROSSBOW,						false },
	{ ACT_HL2MP_IDLE_CROUCH,			ACT_HL2MP_IDLE_CROUCH_CROSSBOW,				false },
	{ ACT_HL2MP_WALK_CROUCH,			ACT_HL2MP_WALK_CROUCH_CROSSBOW,				false },
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK,	ACT_HL2MP_GESTURE_RANGE_ATTACK_CROSSBOW,	false },
	{ ACT_HL2MP_GESTURE_RELOAD,			ACT_HL2MP_GESTURE_RELOAD_CROSSBOW,			false },
	{ ACT_HL2MP_JUMP,					ACT_HL2MP_JUMP_CROSSBOW,					false },
};

IMPLEMENT_ACTTABLE( CWeaponRailgun );

#endif

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeaponRailgun::CWeaponRailgun( void )
{
	m_bReloadsSingly	= false;
	m_bFiresUnderwater	= false;
	m_flNextCharge = 0;
	m_bInZoom = false;
}

void CWeaponRailgun::Equip(CBaseCombatCharacter* pOwner)
{
#ifndef CLIENT_DLL
	CBasePlayer* pPlayer = ToBasePlayer(pOwner);

	if (!pPlayer)
		return BaseClass::Equip(pOwner);

	if (!(pPlayer->m_Local.m_iHideHUD & HIDEHUD_CROSSHAIR))
	{
		pPlayer->ShowCrosshair(false);
	}
#endif

	return BaseClass::Equip(pOwner);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CWeaponRailgun::Deploy(void)
{
#ifndef CLIENT_DLL
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (!pPlayer)
		return BaseClass::Deploy();

	if (!(pPlayer->m_Local.m_iHideHUD & HIDEHUD_CROSSHAIR))
	{
		pPlayer->ShowCrosshair(false);
	}
#endif

	return BaseClass::Deploy();
}
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CWeaponRailgun::Holster(CBaseCombatWeapon* pSwitchingTo)
{
#ifndef CLIENT_DLL
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (!pPlayer)
		return BaseClass::Holster(pSwitchingTo);

	if (pPlayer->m_Local.m_iHideHUD & HIDEHUD_CROSSHAIR)
	{
		pPlayer->ShowCrosshair(true);
	}

	if (pPlayer->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
	{
		pPlayer->GiveAmmo(1, m_iPrimaryAmmoType, true);
	}
#endif

	return BaseClass::Holster(pSwitchingTo);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponRailgun::CheckZoomToggle(void)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (!pPlayer)
		return;

	if (pPlayer->m_afButtonPressed & IN_ATTACK2)
	{
		ToggleZoom();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponRailgun::ItemBusyFrame(void)
{
	// Allow zoom toggling even when we're reloading
	CheckZoomToggle();
}

void CWeaponRailgun::ItemPostFrame(void)
{
	// Allow zoom toggling
	CheckZoomToggle();

	CBasePlayer* pOwner = ToBasePlayer(GetOwner());
	if (!pOwner)
		return;

	if (pOwner->GetAmmoCount(m_iPrimaryAmmoType) < GetDefaultClip1())
	{
		if (((pOwner->m_nButtons & IN_ATTACK) == false) && ((pOwner->m_nButtons & IN_ATTACK2) == false))
		{
			RechargeAmmo();
		}
	}

	UpdateAutoFire();

	//Track the duration of the fire
	//FIXME: Check for IN_ATTACK2 as well?
	//FIXME: What if we're calling ItemBusyFrame?
	m_fFireDuration = (pOwner->m_nButtons & IN_ATTACK) ? (m_fFireDuration + gpGlobals->frametime) : 0.0f;

	bool bFired = false;

	if (!bFired && (pOwner->m_nButtons & IN_ATTACK) && (m_flNextPrimaryAttack <= gpGlobals->curtime))
	{
		//NOTENOTE: There is a bug with this code with regards to the way machine guns catch the leading edge trigger
			//			on the player hitting the attack key.  It relies on the gun catching that case in the same frame.
			//			However, because the player can also be doing a secondary attack, the edge trigger may be missed.
			//			We really need to hold onto the edge trigger and only clear the condition when the gun has fired its
			//			first shot.  Right now that's too much of an architecture change -- jdw

			// If the firing button was just pressed, or the alt-fire just released, reset the firing time
		if ((pOwner->m_afButtonPressed & IN_ATTACK) || (pOwner->m_afButtonReleased & IN_ATTACK2))
		{
			m_flNextPrimaryAttack = gpGlobals->curtime;
		}

		if (!(pOwner->GetAmmoCount(m_iPrimaryAmmoType) <= 0))
		{
			PrimaryAttack();
		}

		if (AutoFiresFullClip())
		{
			m_bFiringWholeClip = true;
	}

#ifdef CLIENT_DLL
		pOwner->SetFiredWeapon(true);
#endif
}

	// -----------------------
	//  No buttons down
	// -----------------------
	if (!((pOwner->m_nButtons & IN_ATTACK) || (pOwner->m_nButtons & IN_ATTACK2) || (CanReload() && pOwner->m_nButtons & IN_RELOAD)))
	{
		// no fire buttons down or reloading
		if (!ReloadOrSwitchWeapons() && (m_bInReload == false))
		{
			WeaponIdle();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponRailgun::ToggleZoom(void)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer == NULL)
		return;

#ifndef CLIENT_DLL

	if (m_bInZoom)
	{
		if (pPlayer->SetFOV(this, 0, 0.2f))
		{
			m_bInZoom = false;
		}
	}
	else
	{
		if (pPlayer->SetFOV(this, 20, 0.1f))
		{
			m_bInZoom = true;
		}
	}
#endif
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponRailgun::Fire( void )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	
	if ( pOwner == NULL )
		return;

	CBaseViewModel* vm = pOwner->GetViewModel();

	if (vm == NULL)
		return;

	Vector	startPos = pOwner->Weapon_ShootPosition();
	Vector	aimDir;

	Vector vecUp, vecRight, vecForward;

	pOwner->EyeVectors(&aimDir);

	Vector	endPos	= startPos + ( aimDir * MAX_TRACE_LENGTH );
	
	//Shoot a shot straight out
	trace_t	tr;
	UTIL_TraceLine( startPos, endPos, MASK_SHOT, pOwner, COLLISION_GROUP_NONE, &tr );
	
#ifndef CLIENT_DLL
	ClearMultiDamage();

	CBaseEntity *pHit = tr.m_pEnt;
	
	CTakeDamageInfo dmgInfo( this, pOwner, GetHL2MPWpnData().m_iPlayerDamage, DMG_SHOCK );

	if ( pHit != NULL )
	{
		CalculateBulletDamageForce( &dmgInfo, m_iPrimaryAmmoType, aimDir, tr.endpos );
		pHit->DispatchTraceAttack( dmgInfo, aimDir, &tr );
	}
#endif
	
	float hitAngle = -DotProduct(tr.plane.normal, aimDir);

	Vector vReflection;

	vReflection = 2.0 * tr.plane.normal * hitAngle + aimDir;

	startPos = tr.endpos;
	endPos = startPos + (vReflection * MAX_TRACE_LENGTH);

	//Kick up an effect
	if (!(tr.surface.flags & SURF_SKY))
	{
		UTIL_ImpactTrace(&tr, m_iPrimaryAmmoType, "ImpactJeep");

		//Do a gauss explosion
		CPVSFilter filter(tr.endpos);
		te->GaussExplosion(filter, 0.0f, tr.endpos, tr.plane.normal, 0);
	}

	//Draw beam to reflection point
	DrawBeam(tr.startpos, tr.endpos);
	
#ifndef CLIENT_DLL
	ApplyMultiDamage();

	// Register a muzzleflash for the AI
	pOwner->SetMuzzleFlashTime( gpGlobals->curtime + 0.5 );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponRailgun::DrawBeam(const Vector& startPos, const Vector& endPos)
{
#ifndef CLIENT_DLL

	CBasePlayer* pOwner = ToBasePlayer(GetOwner());

	if (pOwner == NULL)
		return;

	UTIL_Tracer(startPos, endPos, 0, TRACER_DONT_USE_ATTACHMENT, 6500, false, "GaussTracer");

	//Draw the main beam shaft
	m_pBeam = CBeam::BeamCreate(GAUSS_BEAM_SPRITE, 2.0f);

	CBaseViewModel* vm = pOwner->GetViewModel();

	if (vm == NULL)
		return;

	m_pBeam->SetStartPos(startPos);
	m_pBeam->PointEntInit(endPos, this);
	m_pBeam->SetEndAttachment(LookupAttachment("muzzle"));
	m_pBeam->SetColor(196, 47+random->RandomInt(-16, 16), 250);
	m_pBeam->SetScrollRate(25.6);
	m_pBeam->SetBrightness(255);
	m_pBeam->RelinkBeam();
	m_pBeam->LiveForTime(0.1f);
#endif
}

void CWeaponRailgun::RechargeAmmo(void)
{
	//if (m_flNextPrimaryAttack >= gpGlobals->curtime)
		//return;

	// Time to recharge yet?
	if (m_flNextCharge >= gpGlobals->curtime)
		return;

	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer == NULL)
		return;

#ifndef CLIENT_DLL
	pPlayer->GiveAmmo(1, m_iPrimaryAmmoType, true);
#endif // ! CLIENT_DLL

	m_flNextCharge = gpGlobals->curtime + RAIL_RECHARGE_TIME;

	if ((pPlayer->GetAmmoCount(m_iPrimaryAmmoType) % 25) == 0)
	{
		WeaponSound(SPECIAL1);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponRailgun::PrimaryAttack(void)
{
	if (!(m_flNextPrimaryAttack <= gpGlobals->curtime))
		return;

	CBasePlayer* pOwner = ToBasePlayer(GetOwner());

	if (pOwner == NULL)
		return;

	if (pOwner->GetAmmoCount(m_iPrimaryAmmoType) < 25)
	{
		SendWeaponAnim(ACT_VM_DRYFIRE);
		WeaponSound(EMPTY);
		m_flNextPrimaryAttack = gpGlobals->curtime + 0.5f;
		return;
	}

	WeaponSound(SINGLE);

	SendWeaponAnim(ACT_VM_PRIMARYATTACK);

	pOwner->DoMuzzleFlash();
	pOwner->ViewPunch(QAngle(-4, random->RandomFloat(-2, 2), 0));
	
	pOwner->RemoveAmmo(25, m_iPrimaryAmmoType);

	m_flNextPrimaryAttack = gpGlobals->curtime + GetFireRate();

	Fire();

	if (!m_iClip1 && pOwner->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
	{
		// HEV suit - indicate out of ammo condition
		pOwner->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponRailgun::SecondaryAttack(void)
{
	//NOTENOTE: The zooming is handled by the post/busy frames
}
