//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		railgun access 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//

#ifndef	WEAPON_RAILGUN_H
#define	WEAPON_RAILGUN_H

#include "weapon_hl2mpbasehlmpcombatweapon.h"
#include "beam_shared.h"

#define GAUSS_BEAM_SPRITE "sprites/laserbeam.vmt"
#define RAIL_RECHARGE_TIME 0.0897f
#define RAIL_RECHARGE_BACKGROUND_TIME 0.055f
#define RAIL_RECHARGE_OVERCHARGE_TIME 0.16f
#define RAIL_AMMO 25
#define RAIL_AMMO_OVERCHARGE 50

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
	void	HolsterThink(void);
	void	ItemBusyFrame(void);
	void	PrimaryAttack( void );
	void	SecondaryAttack(void);
	void	Fire(void);
	void	RechargeAmmo(bool bIsHolstered);
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

	bool IsOvercharged()
	{
		return m_bJustOvercharged;
	}

private:
	void	CheckZoomToggle(void);
	void	ToggleZoom(void);

private:
	CBeam* m_pBeam;
	CNetworkVar(float, m_flNextCharge);
	CNetworkVar(bool, m_bInZoom);
	CNetworkVar(bool, m_bJustOvercharged);
	CNetworkVar(bool, m_bIsLowBattery);
	CNetworkVar(bool, m_bOverchargeDamageBenefits);
	
	CWeaponRailgun( const CWeaponRailgun & );
};
#endif
