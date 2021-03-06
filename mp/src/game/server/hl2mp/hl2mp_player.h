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

class CHL2MP_Player;

#include "basemultiplayerplayer.h"
#include "hl2_playerlocaldata.h"
#include "hl2_player.h"
#include "simtimer.h"
#include "soundenvelope.h"
#include "hl2mp_player_shared.h"
#include "hl2mp_gamerules.h"
#include "utldict.h"

//=============================================================================
// >> HL2MP_Player
//=============================================================================
class CHL2MPPlayerStateInfo
{
public:
	HL2MPPlayerState m_iPlayerState;
	const char *m_pStateName;

	void (CHL2MP_Player::*pfnEnterState)();	// Init and deinit the state.
	void (CHL2MP_Player::*pfnLeaveState)();

	void (CHL2MP_Player::*pfnPreThink)();	// Do a PreThink() in this state.
};

class CHL2MP_Player : public CHL2_Player
{
public:
	DECLARE_CLASS( CHL2MP_Player, CHL2_Player );

	CHL2MP_Player();
	~CHL2MP_Player( void );
	
	static CHL2MP_Player *CreatePlayer( const char *className, edict_t *ed )
	{
		CHL2MP_Player::s_PlayerEdict = ed;
		return (CHL2MP_Player*)CreateEntityByName( className );
	}

	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	virtual void Precache( void );
	virtual void Spawn( void );
	virtual void PostThink( void );
	virtual void PreThink( void );
	virtual void PlayerDeathThink( void );
	virtual void SetAnimation( PLAYER_ANIM playerAnim );
	virtual void TraceAttack(const CTakeDamageInfo& info, const Vector& vecDir, trace_t* ptr, CDmgAccumulator* pAccumulator);
	void DismemberRandomLimbs(void);
	virtual bool HandleCommand_JoinTeam( int team );
	virtual bool ClientCommand( const CCommand &args );
	virtual void CreateViewModel( int viewmodelindex = 0 );
	virtual bool BecomeRagdollOnClient( const Vector &force );
	virtual void Event_Killed( const CTakeDamageInfo &info );
	virtual int OnTakeDamage( const CTakeDamageInfo &inputInfo );
	virtual bool WantsLagCompensationOnEntity( const CBasePlayer *pPlayer, const CUserCmd *pCmd, const CBitVec<MAX_EDICTS> *pEntityTransmitBits ) const;
	virtual void FireBullets ( const FireBulletsInfo_t &info );
	virtual bool Weapon_Switch( CBaseCombatWeapon *pWeapon, int viewmodelindex = 0);
	virtual bool BumpWeapon( CBaseCombatWeapon *pWeapon );
	virtual void ChangeTeam( int iTeam );
	virtual void PickupObject ( CBaseEntity *pObject, bool bLimitMassAndSize );
	virtual void PlayStepSound( Vector &vecOrigin, surfacedata_t *psurface, float fvol, bool force );
	virtual void Weapon_Drop( CBaseCombatWeapon *pWeapon, const Vector *pvecTarget = NULL, const Vector *pVelocity = NULL );
	virtual void UpdateOnRemove( void );
	virtual void DeathSound( const CTakeDamageInfo &info );
	virtual CBaseEntity* EntSelectSpawnPoint( void );
		
	int FlashlightIsOn( void );
	void FlashlightTurnOn( void );
	void FlashlightTurnOff( void );
	void	PrecacheFootStepSounds( void );

	QAngle GetAnimEyeAngles( void ) { return m_angEyeAngles.Get(); }

	Vector GetAttackSpread( CBaseCombatWeapon *pWeapon, CBaseEntity *pTarget = NULL );

	void CheatImpulseCommands( int iImpulse );
	void CreateRagdollEntity( void );
	void GiveDefaultItems( void );
	void GiveItems(bool bGiveAll = false);

	void NoteWeaponFired( void );

	void ResetAnimation( void );
	Activity TranslateTeamActivity( Activity ActToTranslate );
	
	float GetNextTeamChangeTime( void ) { return m_flNextTeamChangeTime; }
	void  PickDefaultSpawnTeam( void );
	
	void  DetonateTripmines( void );

	void Reset();

	bool IsReady();
	void SetReady( bool bReady );

	void CheckChatText( char *p, int bufsize );

	void State_Transition( HL2MPPlayerState newState );
	void State_Enter( HL2MPPlayerState newState );
	void State_Leave();
	void State_PreThink();
	CHL2MPPlayerStateInfo *State_LookupInfo( HL2MPPlayerState state );

	void State_Enter_ACTIVE();
	void State_PreThink_ACTIVE();
	void State_Enter_OBSERVER_MODE();
	void State_PreThink_OBSERVER_MODE();


	virtual bool StartObserverMode( int mode );
	virtual void StopObserverMode( void );


	Vector m_vecTotalBulletForce;	//Accumulator for bullet force in a single frame

	// Tracks our ragdoll entity.
	CNetworkHandle( CBaseEntity, m_hRagdoll );	// networked entity handle 

	virtual bool	CanHearAndReadChatFrom( CBasePlayer *pPlayer );

#ifdef PONEDM
	void UpdatePlayerColor(void);
	void UpdatePlayerAppearance(void);
	CNetworkVector(m_vPrimaryColor);
	CNetworkVector(m_vSecondaryColor);
	CNetworkVector(m_vTertiaryColor);
	CNetworkVar(int, m_iUpperManeBodygroup);
	CNetworkVar(int, m_iLowerManeBodygroup);
	CNetworkVar(int, m_iTailBodygroup);

	// Gore
	unsigned short m_iGoreHead;
	unsigned short m_iGoreFrontLeftLeg;
	unsigned short m_iGoreFrontRightLeg;
	unsigned short m_iGoreRearLeftLeg;
	unsigned short m_iGoreRearRightLeg;
#endif

	bool m_bEnterObserver;

private:

#ifdef PONEDM
	void				CustomizationUpdateThink();
#endif

	CNetworkQAngle( m_angEyeAngles );
	CPlayerAnimState   m_PlayerAnimState;

	int m_iLastWeaponFireUsercmd;
	int m_iModelType;
	CNetworkVar( int, m_iSpawnInterpCounter );

	float m_flNextTeamChangeTime;

	float m_flSlamProtectTime;	

	HL2MPPlayerState m_iPlayerState;
	CHL2MPPlayerStateInfo *m_pCurStateInfo;

	bool ShouldRunRateLimitedCommand( const CCommand &args );

	// This lets us rate limit the commands the players can execute so they don't overflow things like reliable buffers.
	CUtlDict<float,int>	m_RateLimitLastCommandTimes;

	bool m_bReady;

protected:
	IBot* m_pBotController;
	CAI_Senses* m_pSenses;

public:
	// Bot
	virtual IBot* GetBotController() {
		return m_pBotController;
	}

	virtual void SetBotController(IBot* pBot);
	virtual void SetUpBot();

	// Senses
	virtual CAI_Senses* GetSenses() {
		return m_pSenses;
	}

	virtual const CAI_Senses* GetSenses() const {
		return m_pSenses;
	}

	virtual void CreateSenses();

	virtual void SetDistLook(float flDistLook);

	virtual int GetSoundInterests();
	virtual int GetSoundPriority(CSound* pSound);

	virtual bool QueryHearSound(CSound* pSound);
	virtual bool QuerySeeEntity(CBaseEntity* pEntity, bool bOnlyHateOrFearIfNPC = false);

	virtual void OnLooked(int iDistance);
	virtual void OnListened();

	virtual CSound* GetLoudestSoundOfType(int iType);
	virtual bool SoundIsVisible(CSound* pSound);

	virtual CSound* GetBestSound(int validTypes = ALL_SOUNDS);
	virtual CSound* GetBestScent(void);
};

inline CHL2MP_Player *ToHL2MPPlayer( CBaseEntity *pEntity )
{
	if ( !pEntity || !pEntity->IsPlayer() )
		return NULL;

	return dynamic_cast<CHL2MP_Player*>( pEntity );
}

#endif //HL2MP_PLAYER_H
