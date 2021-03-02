//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#ifndef HL2MP_PLAYER_H
#define HL2MP_PLAYER_H
#pragma once

class C_HL2MP_Player;
#include "c_basehlplayer.h"
#include "hl2mp_player_shared.h"
#include "beamdraw.h"
#ifdef PONEDM
#include "hl2_shareddefs.h"
#endif

//=============================================================================
// >> HL2MP_Player
//=============================================================================
class C_HL2MP_Player : public C_BaseHLPlayer
{
public:
	DECLARE_CLASS( C_HL2MP_Player, C_BaseHLPlayer );

	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_INTERPOLATION();


	C_HL2MP_Player();
	~C_HL2MP_Player( void );

	void ClientThink( void );

	static C_HL2MP_Player* GetLocalHL2MPPlayer();
	
	virtual int DrawModel( int flags );
	virtual void AddEntity( void );

	QAngle GetAnimEyeAngles( void ) { return m_angEyeAngles; }
	Vector GetAttackSpread( CBaseCombatWeapon *pWeapon, CBaseEntity *pTarget = NULL );


	// Should this object cast shadows?
	virtual ShadowType_t		ShadowCastType( void );
	virtual C_BaseAnimating *BecomeRagdollOnClient();
	virtual const QAngle& GetRenderAngles();
	virtual bool ShouldDraw( void );
	virtual void OnDataChanged( DataUpdateType_t type );
	virtual float GetFOV( void );
	virtual CStudioHdr *OnNewModel( void );
	virtual void TraceAttack( const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr, CDmgAccumulator *pAccumulator );
	virtual void ItemPreFrame( void );
	virtual void ItemPostFrame( void );
	virtual float GetMinFOV()	const { return 5.0f; }
	virtual Vector GetAutoaimVector( float flDelta );
	virtual void NotifyShouldTransmit( ShouldTransmitState_t state );
	virtual void CreateLightEffects( void ) {}
	virtual bool ShouldReceiveProjectedTextures( int flags );
	virtual void PostDataUpdate( DataUpdateType_t updateType );
	virtual void PlayStepSound( Vector &vecOrigin, surfacedata_t *psurface, float fvol, bool force );
	virtual void PreThink( void );
	virtual void DoImpactEffect( trace_t &tr, int nDamageType );
	IRagdoll* GetRepresentativeRagdoll() const;
	virtual void CalcView( Vector &eyeOrigin, QAngle &eyeAngles, float &zNear, float &zFar, float &fov );
	virtual const QAngle& EyeAngles( void );

#ifdef PONEDM
	virtual Vector GetPrimaryColor(void) { return m_vPrimaryColor; }
	virtual Vector GetSecondaryColor(void) { return m_vSecondaryColor; }
	virtual Vector GetTertiaryColor(void) { return m_vTertiaryColor; }
#endif

	void	UpdateLookAt( void );
	void	Initialize( void );
	int		GetIDTarget() const;
	void	UpdateIDTarget( void );
	void	PrecacheFootStepSounds( void );
	const char	*GetPlayerModelSoundPrefix( void );

	HL2MPPlayerState State_Get() const;

	virtual void PostThink( void );

#ifdef PONEDM
	CNetworkVector(m_vPrimaryColor);
	CNetworkVector(m_vSecondaryColor);
	CNetworkVector(m_vTertiaryColor);
#endif

private:
	
	C_HL2MP_Player( const C_HL2MP_Player & );

	CPlayerAnimState m_PlayerAnimState;

	QAngle	m_angEyeAngles;

	CInterpolatedVar< QAngle >	m_iv_angEyeAngles;

	EHANDLE	m_hRagdoll;

	int	m_headYawPoseParam;
	int	m_headPitchPoseParam;
	float m_headYawMin;
	float m_headYawMax;
	float m_headPitchMin;
	float m_headPitchMax;

	bool m_isInit;
	Vector m_vLookAtTarget;

	float m_flLastBodyYaw;
	float m_flCurrentHeadYaw;
	float m_flCurrentHeadPitch;

	int	  m_iIDEntIndex;

	CountdownTimer m_blinkTimer;

	int	  m_iSpawnInterpCounter;
	int	  m_iSpawnInterpCounterCache;

	void ReleaseFlashlight( void );
	Beam_t	*m_pFlashlightBeam;

	CNetworkVar( HL2MPPlayerState, m_iPlayerState );	

	bool m_fIsWalking;
};

inline C_HL2MP_Player *ToHL2MPPlayer( CBaseEntity *pEntity )
{
	if ( !pEntity || !pEntity->IsPlayer() )
		return NULL;

	return dynamic_cast<C_HL2MP_Player*>( pEntity );
}


class C_HL2MPRagdoll : public C_BaseAnimatingOverlay
{
public:
	DECLARE_CLASS( C_HL2MPRagdoll, C_BaseAnimatingOverlay );
	DECLARE_CLIENTCLASS();
	
	C_HL2MPRagdoll();
	~C_HL2MPRagdoll();

	virtual void OnDataChanged( DataUpdateType_t type );

	int GetPlayerEntIndex() const;
	IRagdoll* GetIRagdoll() const;

#ifdef PONEDM
	EHANDLE GetPlayerHandle(void)
	{
		if (m_iPlayerIndex == PONEDM_PLAYER_INDEX_NONE)
			return NULL;
		return cl_entitylist->GetNetworkableHandle(m_iPlayerIndex);
	}
#endif

	void ClientThink(void);
	void ImpactTrace( trace_t *pTrace, int iDamageType, const char *pCustomImpactName );
	void UpdateOnRemove( void );
	virtual void SetupWeights( const matrix3x4_t *pBoneToWorld, int nFlexWeightCount, float *pFlexWeights, float *pFlexDelayedWeights );

#ifdef PONEDM
	// c_baseanimating functions
	virtual void BuildTransformations(CStudioHdr* pStudioHdr, Vector* pos, Quaternion q[], const matrix3x4_t& cameraTransform, int boneMask, CBoneBitList& boneComputed);

	// GORE
	void ScaleGoreBones(void);
	void ResetScaledBones(void);
	void InitDismember(void);

	void DismemberHead();
	void DismemberBase(bool bBloodEffects, char const* szParticleBone);
	void DismemberFrontLeftLeg();
	void DismemberFrontRightLeg();
	void DismemberRearLeftLeg();
	void DismemberRearRightLeg();

	void DismemberRandomLimbs(void);

	virtual C_BaseEntity* GetPrimaryColorOwner(void) 
	{ 
		EHANDLE hPlayer = GetPlayerHandle();
		return hPlayer.Get();
	}
	virtual C_BaseEntity* GetSecondaryColorOwner(void) 
	{ 
		EHANDLE hPlayer = GetPlayerHandle();
		return hPlayer.Get();
	}
	virtual C_BaseEntity* GetTertiaryColorOwner(void) 
	{ 
		EHANDLE hPlayer = GetPlayerHandle();
		return hPlayer.Get();
	}
#endif
	
private:
	
	C_HL2MPRagdoll( const C_HL2MPRagdoll & ) {}

	void Interp_Copy( C_BaseAnimatingOverlay *pDestinationEntity );
	void CreateHL2MPRagdoll( void );

private:

	EHANDLE	m_hPlayer;
	CNetworkVector( m_vecRagdollVelocity );
	CNetworkVector( m_vecRagdollOrigin );
#ifdef PONEDM
	int	  m_iPlayerIndex;

	int m_iUpperManeBodygroup;
	int m_iLowerManeBodygroup;
	int m_iTailBodygroup;

	// gore stuff
	int m_iGoreHead;
	int m_iGoreFrontLeftLeg;
	int m_iGoreFrontRightLeg;
	int m_iGoreRearLeftLeg;
	int m_iGoreRearRightLeg;

	// checks if this model can utilise gore
	bool m_bGoreEnabled;
	// how many blood decals to spray out when we dismember a limb overtime
	int m_iGoreDecalAmount;
	// the index of the bone we should spray blood decals out from
	int m_iGoreDecalBone;
	// time when blood decal was sprayed so that blood decals sprays are delayed in bursts for ClientThink
	float m_fGoreDecalTime;

	CNewParticleEffect* m_pDizzyEffect;
#endif
};

#endif //HL2MP_PLAYER_H
