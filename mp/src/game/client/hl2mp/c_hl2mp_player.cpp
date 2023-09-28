//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Player for HL2.
//
//=============================================================================//

#include "cbase.h"
#include "vcollide_parse.h"
#include "c_hl2mp_player.h"
#include "view.h"
#include "takedamageinfo.h"
#include "hl2mp_gamerules.h"
#include "in_buttons.h"
#include "iviewrender_beams.h"			// flashlight beam
#include "r_efx.h"
#include "dlight.h"
#include "c_team.h"
#ifdef PONEDM
	#include "materialsystem/imaterialproxy.h"
	#include "materialsystem/imaterialvar.h"
	#include "materialsystem/itexture.h"
	#include "materialsystem/imaterialsystem.h"
	#include "functionproxy.h"
	#include "effect_dispatch_data.h"
	#include "c_te_effect_dispatch.h"
	#include "props_shared.h"
	#include "c_gib.h"
	#include "weapon_railgun.h"
#endif
#include <ai_debug_shared.h>

// Don't alias here
#if defined( CHL2MP_Player )
#undef CHL2MP_Player	
#endif
#include <bone_setup.h>

LINK_ENTITY_TO_CLASS( player, C_HL2MP_Player );

IMPLEMENT_CLIENTCLASS_DT(C_HL2MP_Player, DT_HL2MP_Player, CHL2MP_Player)
	RecvPropFloat( RECVINFO( m_angEyeAngles[0] ) ),
	RecvPropFloat( RECVINFO( m_angEyeAngles[1] ) ),
	RecvPropEHandle( RECVINFO( m_hRagdoll ) ),
	RecvPropInt( RECVINFO( m_iSpawnInterpCounter ) ),
	RecvPropBool( RECVINFO( m_fIsWalking ) ),

#ifdef PONEDM
	RecvPropVector(RECVINFO(m_vPrimaryColor)),
	RecvPropVector(RECVINFO(m_vSecondaryColor)),
	RecvPropVector(RECVINFO(m_vTertiaryColor)),
#endif

END_RECV_TABLE()

BEGIN_PREDICTION_DATA( C_HL2MP_Player )
	DEFINE_PRED_FIELD( m_fIsWalking, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
END_PREDICTION_DATA()

static ConVar cl_defaultweapon( "cl_defaultweapon", "weapon_physcannon", FCVAR_USERINFO | FCVAR_ARCHIVE, "Default Spawn Weapon");

#ifdef PONEDM
static ConVar cl_ponedm_primarycolor_r("cl_ponedm_primarycolor_r", "255", FCVAR_USERINFO | FCVAR_ARCHIVE | FCVAR_SERVER_CAN_EXECUTE, "");
static ConVar cl_ponedm_primarycolor_g("cl_ponedm_primarycolor_g", "255", FCVAR_USERINFO | FCVAR_ARCHIVE | FCVAR_SERVER_CAN_EXECUTE, "");
static ConVar cl_ponedm_primarycolor_b("cl_ponedm_primarycolor_b", "255", FCVAR_USERINFO | FCVAR_ARCHIVE | FCVAR_SERVER_CAN_EXECUTE, "");

static ConVar cl_ponedm_secondarycolor_r("cl_ponedm_secondarycolor_r", "255", FCVAR_USERINFO | FCVAR_ARCHIVE | FCVAR_SERVER_CAN_EXECUTE, "");
static ConVar cl_ponedm_secondarycolor_g("cl_ponedm_secondarycolor_g", "255", FCVAR_USERINFO | FCVAR_ARCHIVE | FCVAR_SERVER_CAN_EXECUTE, "");
static ConVar cl_ponedm_secondarycolor_b("cl_ponedm_secondarycolor_b", "255", FCVAR_USERINFO | FCVAR_ARCHIVE | FCVAR_SERVER_CAN_EXECUTE, "");

static ConVar cl_ponedm_tertiarycolor_r("cl_ponedm_tertiarycolor_r", "255", FCVAR_USERINFO | FCVAR_ARCHIVE | FCVAR_SERVER_CAN_EXECUTE, "");
static ConVar cl_ponedm_tertiarycolor_g("cl_ponedm_tertiarycolor_g", "255", FCVAR_USERINFO | FCVAR_ARCHIVE | FCVAR_SERVER_CAN_EXECUTE, "");
static ConVar cl_ponedm_tertiarycolor_b("cl_ponedm_tertiarycolor_b", "255", FCVAR_USERINFO | FCVAR_ARCHIVE | FCVAR_SERVER_CAN_EXECUTE, "");

static ConVar cl_ponedm_uppermane("cl_ponedm_uppermane", "0", FCVAR_USERINFO | FCVAR_ARCHIVE | FCVAR_SERVER_CAN_EXECUTE, "");
static ConVar cl_ponedm_lowermane("cl_ponedm_lowermane", "0", FCVAR_USERINFO | FCVAR_ARCHIVE | FCVAR_SERVER_CAN_EXECUTE, "");
static ConVar cl_ponedm_tail("cl_ponedm_tail", "0", FCVAR_USERINFO | FCVAR_ARCHIVE | FCVAR_SERVER_CAN_EXECUTE, "");
static ConVar cl_ponedm_horn("cl_ponedm_horn", "0", FCVAR_USERINFO | FCVAR_ARCHIVE | FCVAR_SERVER_CAN_EXECUTE, "");
static ConVar cl_ponedm_wings("cl_ponedm_wings", "0", FCVAR_USERINFO | FCVAR_ARCHIVE | FCVAR_SERVER_CAN_EXECUTE, "");

static ConVar cl_ponedm_gibtime("cl_ponedm_gibtime", "30", FCVAR_USERINFO | FCVAR_ARCHIVE | FCVAR_SERVER_CAN_EXECUTE, "");

extern ConVar cl_ponedm_violencelevel;
#endif

void SpawnBlood (Vector vecSpot, const Vector &vecDir, int bloodColor, float flDamage);
extern CBaseEntity* BreakModelCreateSingle(CBaseEntity* pOwner, breakmodel_t* pModel, const Vector& position,
	const QAngle& angles, const Vector& velocity, const AngularImpulse& angVelocity, int nSkin, const breakablepropparams_t& params);

C_HL2MP_Player::C_HL2MP_Player() : m_PlayerAnimState( this ), m_iv_angEyeAngles( "C_HL2MP_Player::m_iv_angEyeAngles" )
{
	m_iIDEntIndex = 0;
	m_iSpawnInterpCounterCache = 0;

	m_angEyeAngles.Init();

	AddVar( &m_angEyeAngles, &m_iv_angEyeAngles, LATCH_SIMULATION_VAR );

	m_EntClientFlags |= ENTCLIENTFLAG_DONTUSEIK;
	m_blinkTimer.Invalidate();

	m_pFlashlightBeam = NULL;

#ifdef GLOWS_ENABLE
	m_pGlowEffect = NULL;
#endif // GLOWS_ENABLE
}

C_HL2MP_Player::~C_HL2MP_Player( void )
{
	ReleaseFlashlight();

	if ((!HL2MPRules()->IsTeamplay() && (sv_ponedm_gamemode.GetInt() == 3)) && GetTeamNumber() == TEAM_ZOMBIES)
	{
#ifdef GLOWS_ENABLE
		DestroyGlowEffect();
#endif // GLOWS_ENABLE
	}
}

int C_HL2MP_Player::GetIDTarget() const
{
	return m_iIDEntIndex;
}

//-----------------------------------------------------------------------------
// Purpose: Update this client's target entity
//-----------------------------------------------------------------------------
void C_HL2MP_Player::UpdateIDTarget()
{
	if ( !IsLocalPlayer() )
		return;

	// Clear old target and find a new one
	m_iIDEntIndex = 0;

	// don't show IDs in chase spec mode
	if ( GetObserverMode() == OBS_MODE_CHASE || 
		 GetObserverMode() == OBS_MODE_DEATHCAM )
		 return;

	trace_t tr;
	Vector vecStart, vecEnd;
	VectorMA( MainViewOrigin(), 1500, MainViewForward(), vecEnd );
	VectorMA( MainViewOrigin(), 10,   MainViewForward(), vecStart );
	UTIL_TraceLine( vecStart, vecEnd, MASK_SOLID, this, COLLISION_GROUP_NONE, &tr );

	if ( !tr.startsolid && tr.DidHitNonWorldEntity() )
	{
		C_BaseEntity *pEntity = tr.m_pEnt;

		if ( pEntity && (pEntity != this) )
		{
			m_iIDEntIndex = pEntity->entindex();
		}
	}
}

void C_HL2MP_Player::TraceAttack( const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr, CDmgAccumulator *pAccumulator )
{
	Vector vecOrigin = ptr->endpos - vecDir * 4;

	float flDistance = 0.0f;
	
	if ( info.GetAttacker() )
	{
		flDistance = (ptr->endpos - info.GetAttacker()->GetAbsOrigin()).Length();
	}

	if ( m_takedamage )
	{
		AddMultiDamage( info, this );
		
		CBaseEntity *pAttacker = info.GetAttacker();

		if ( pAttacker )
		{
			if (((!HL2MPRules()->IsTeamplay() && (sv_ponedm_gamemode.GetInt() == 3)) || 
				HL2MPRules()->IsTeamplay()) && pAttacker->InSameTeam(this) == true)
				return;
		}

		if (cl_ponedm_violencelevel.GetInt() >= 1)
		{
			int blood = BloodColor();

			if (blood != DONT_BLEED)
			{
				SpawnBlood(vecOrigin, vecDir, blood, flDistance);// a little surface blood.
				TraceBleed(flDistance, vecDir, ptr, info.GetDamageType());
			}
		}
		else if (cl_ponedm_violencelevel.GetInt() == 0)
		{
			SpawnBlood(vecOrigin, vecDir, 0, flDistance); // a little surface blood.
		}
	}
}

C_HL2MP_Player* C_HL2MP_Player::GetLocalHL2MPPlayer()
{
	return (C_HL2MP_Player*)C_BasePlayer::GetLocalPlayer();
}

void C_HL2MP_Player::Initialize( void )
{
	m_headYawPoseParam = LookupPoseParameter( "head_yaw" );
	GetPoseParameterRange( m_headYawPoseParam, m_headYawMin, m_headYawMax );

	m_headPitchPoseParam = LookupPoseParameter( "head_pitch" );
	GetPoseParameterRange( m_headPitchPoseParam, m_headPitchMin, m_headPitchMax );

	CStudioHdr *hdr = GetModelPtr();
	if (hdr)
	{
		for (int i = 0; i < hdr->GetNumPoseParameters(); i++)
		{
			SetPoseParameter(hdr, i, 0.0);
		}
	}
}

CStudioHdr *C_HL2MP_Player::OnNewModel( void )
{
	CStudioHdr *hdr = BaseClass::OnNewModel();
	
	Initialize( );

	return hdr;
}

//-----------------------------------------------------------------------------
/**
 * Orient head and eyes towards m_lookAt.
 */
void C_HL2MP_Player::UpdateLookAt( void )
{
	// head yaw
	if (m_headYawPoseParam < 0 || m_headPitchPoseParam < 0)
		return;

	// orient eyes
	m_viewtarget = m_vLookAtTarget;

	// blinking
	if (m_blinkTimer.IsElapsed())
	{
		m_blinktoggle = !m_blinktoggle;
		m_blinkTimer.Start( RandomFloat( 1.5f, 4.0f ) );
	}

	// Figure out where we want to look in world space.
	QAngle desiredAngles;
	Vector to = m_vLookAtTarget - EyePosition();
	VectorAngles( to, desiredAngles );

	// Figure out where our body is facing in world space.
	QAngle bodyAngles( 0, 0, 0 );
	bodyAngles[YAW] = GetLocalAngles()[YAW];


	float flBodyYawDiff = bodyAngles[YAW] - m_flLastBodyYaw;
	m_flLastBodyYaw = bodyAngles[YAW];
	

	// Set the head's yaw.
	float desired = AngleNormalize( desiredAngles[YAW] - bodyAngles[YAW] );
	desired = clamp( desired, m_headYawMin, m_headYawMax );
	m_flCurrentHeadYaw = ApproachAngle( desired, m_flCurrentHeadYaw, 130 * gpGlobals->frametime );

	// Counterrotate the head from the body rotation so it doesn't rotate past its target.
	m_flCurrentHeadYaw = AngleNormalize( m_flCurrentHeadYaw - flBodyYawDiff );
	desired = clamp( desired, m_headYawMin, m_headYawMax );
	
	SetPoseParameter( m_headYawPoseParam, m_flCurrentHeadYaw );

	
	// Set the head's yaw.
	desired = AngleNormalize( desiredAngles[PITCH] );
	desired = clamp( desired, m_headPitchMin, m_headPitchMax );
	
	m_flCurrentHeadPitch = ApproachAngle( desired, m_flCurrentHeadPitch, 130 * gpGlobals->frametime );
	m_flCurrentHeadPitch = AngleNormalize( m_flCurrentHeadPitch );
	SetPoseParameter( m_headPitchPoseParam, m_flCurrentHeadPitch );
}

void C_HL2MP_Player::ClientThink( void )
{
	bool bFoundViewTarget = false;
	
	Vector vForward;
	AngleVectors( GetLocalAngles(), &vForward );

	for( int iClient = 1; iClient <= gpGlobals->maxClients; ++iClient )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( iClient );
		if(!pEnt || !pEnt->IsPlayer())
			continue;

		if ( pEnt->entindex() == entindex() )
			continue;

		Vector vTargetOrigin = pEnt->GetAbsOrigin();
		Vector vMyOrigin =  GetAbsOrigin();

		Vector vDir = vTargetOrigin - vMyOrigin;
		
		if ( vDir.Length() > 128 ) 
			continue;

		VectorNormalize( vDir );

		if ( DotProduct( vForward, vDir ) < 0.0f )
			 continue;

		m_vLookAtTarget = pEnt->EyePosition();
		bFoundViewTarget = true;
		break;
	}

	if ( bFoundViewTarget == false )
	{
		m_vLookAtTarget = GetAbsOrigin() + vForward * 512;
	}

#ifdef GLOWS_ENABLE
	if ((!HL2MPRules()->IsTeamplay() && (sv_ponedm_gamemode.GetInt() == 3)) && GetTeamNumber() == TEAM_ZOMBIES)
	{
		if (!IsAlive())
		{
			DestroyGlowEffect();
		}
	}
#endif // GLOWS_ENABLE

	UpdateIDTarget();
}

#ifdef GLOWS_ENABLE
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_HL2MP_Player::UpdateGlowEffect()
{
	if (sv_ponedm_gamemode.GetInt() != 3)
	{
		DestroyGlowEffect();
		return;
	}

	if (GetTeamNumber() > TEAM_UNASSIGNED)
	{
		DestroyGlowEffect();
		return;
	}

	// destroy the existing effect
	if (m_pGlowEffect)
	{
		DestroyGlowEffect();
	}

	// create a new effect if we have a cart
	float brightness = (180 / 255);
	m_pGlowEffect = new CGlowObject(this, Vector(m_vPrimaryColor.GetX() + brightness, m_vPrimaryColor.GetY() + brightness, m_vPrimaryColor.GetZ() + brightness), 1.0, true);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_HL2MP_Player::DestroyGlowEffect(void)
{
	if (m_pGlowEffect)
	{
		delete m_pGlowEffect;
		m_pGlowEffect = NULL;
	}
}
#endif // GLOWS_ENABLE

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int C_HL2MP_Player::DrawModel( int flags )
{
	if ( !m_bReadyToDraw )
		return 0;

    return BaseClass::DrawModel(flags);
}

//-----------------------------------------------------------------------------
// Should this object receive shadows?
//-----------------------------------------------------------------------------
bool C_HL2MP_Player::ShouldReceiveProjectedTextures( int flags )
{
	Assert( flags & SHADOW_FLAGS_PROJECTED_TEXTURE_TYPE_MASK );

	if ( IsEffectActive( EF_NODRAW ) )
		 return false;

	if( flags & SHADOW_FLAGS_FLASHLIGHT )
	{
		return true;
	}

	return BaseClass::ShouldReceiveProjectedTextures( flags );
}

void C_HL2MP_Player::DoImpactEffect( trace_t &tr, int nDamageType )
{
	if ( GetActiveWeapon() )
	{
		GetActiveWeapon()->DoImpactEffect( tr, nDamageType );
		return;
	}

	BaseClass::DoImpactEffect( tr, nDamageType );
}

void C_HL2MP_Player::PreThink( void )
{
	QAngle vTempAngles = GetLocalAngles();

	if ( GetLocalPlayer() == this )
	{
		vTempAngles[PITCH] = EyeAngles()[PITCH];
	}
	else
	{
		vTempAngles[PITCH] = m_angEyeAngles[PITCH];
	}

	if ( vTempAngles[YAW] < 0.0f )
	{
		vTempAngles[YAW] += 360.0f;
	}

	SetLocalAngles( vTempAngles );

	BaseClass::PreThink();

	if ((!HL2MPRules()->IsTeamplay() && (sv_ponedm_gamemode.GetInt() == 3)) && GetTeamNumber() == TEAM_UNASSIGNED)
	{
		SetMaxSpeed(PONEDM_PLAYERSPEED / 2);
	}
	else
	{
		SetMaxSpeed(PONEDM_PLAYERSPEED);
	}
}

const QAngle &C_HL2MP_Player::EyeAngles()
{
	if( IsLocalPlayer() )
	{
		return BaseClass::EyeAngles();
	}
	else
	{
		return m_angEyeAngles;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_HL2MP_Player::AddEntity( void )
{
	BaseClass::AddEntity();

	QAngle vTempAngles = GetLocalAngles();
	vTempAngles[PITCH] = m_angEyeAngles[PITCH];

	SetLocalAngles( vTempAngles );
		
	m_PlayerAnimState.Update();

	// Zero out model pitch, blending takes care of all of it.
	SetLocalAnglesDim( X_INDEX, 0 );

	if( this != C_BasePlayer::GetLocalPlayer() )
	{
		if ( IsEffectActive( EF_DIMLIGHT ) )
		{
			int iAttachment = LookupAttachment( "anim_attachment_RH" );

			if ( iAttachment < 0 )
				return;

			Vector vecOrigin;
			QAngle eyeAngles = m_angEyeAngles;
	
			GetAttachment( iAttachment, vecOrigin, eyeAngles );

			Vector vForward;
			AngleVectors( eyeAngles, &vForward );
				
			trace_t tr;
			UTIL_TraceLine( vecOrigin, vecOrigin + (vForward * 200), MASK_SHOT, this, COLLISION_GROUP_NONE, &tr );

			if( !m_pFlashlightBeam )
			{
				BeamInfo_t beamInfo;
				beamInfo.m_nType = TE_BEAMPOINTS;
				beamInfo.m_vecStart = tr.startpos;
				beamInfo.m_vecEnd = tr.endpos;
				beamInfo.m_pszModelName = "sprites/glow01.vmt";
				beamInfo.m_pszHaloName = "sprites/glow01.vmt";
				beamInfo.m_flHaloScale = 3.0;
				beamInfo.m_flWidth = 8.0f;
				beamInfo.m_flEndWidth = 35.0f;
				beamInfo.m_flFadeLength = 300.0f;
				beamInfo.m_flAmplitude = 0;
				beamInfo.m_flBrightness = 60.0;
				beamInfo.m_flSpeed = 0.0f;
				beamInfo.m_nStartFrame = 0.0;
				beamInfo.m_flFrameRate = 0.0;
				beamInfo.m_flRed = 255.0;
				beamInfo.m_flGreen = 255.0;
				beamInfo.m_flBlue = 255.0;
				beamInfo.m_nSegments = 8;
				beamInfo.m_bRenderable = true;
				beamInfo.m_flLife = 0.5;
				beamInfo.m_nFlags = FBEAM_FOREVER | FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;
				
				m_pFlashlightBeam = beams->CreateBeamPoints( beamInfo );
			}

			if( m_pFlashlightBeam )
			{
				BeamInfo_t beamInfo;
				beamInfo.m_vecStart = tr.startpos;
				beamInfo.m_vecEnd = tr.endpos;
				beamInfo.m_flRed = 255.0;
				beamInfo.m_flGreen = 255.0;
				beamInfo.m_flBlue = 255.0;

				beams->UpdateBeamInfo( m_pFlashlightBeam, beamInfo );

				dlight_t *el = effects->CL_AllocDlight( 0 );
				el->origin = tr.endpos;
				el->radius = 50; 
				el->color.r = 200;
				el->color.g = 200;
				el->color.b = 200;
				el->die = gpGlobals->curtime + 0.1;
			}
		}
		else if ( m_pFlashlightBeam )
		{
			ReleaseFlashlight();
		}
	}
}

ShadowType_t C_HL2MP_Player::ShadowCastType( void ) 
{
	if ( !IsVisible() )
		 return SHADOWS_NONE;

	return SHADOWS_RENDER_TO_TEXTURE_DYNAMIC;
}


const QAngle& C_HL2MP_Player::GetRenderAngles()
{
	if ( IsRagdoll() )
	{
		return vec3_angle;
	}
	else
	{
		return m_PlayerAnimState.GetRenderAngles();
	}
}

bool C_HL2MP_Player::ShouldDraw( void )
{
	// If we're dead, our ragdoll will be drawn for us instead.
	if ( !IsAlive() )
		return false;

//	if( GetTeamNumber() == TEAM_SPECTATOR )
//		return false;

	if( IsLocalPlayer() && IsRagdoll() )
		return true;
	
	if ( IsRagdoll() )
		return false;

	return BaseClass::ShouldDraw();
}

void C_HL2MP_Player::NotifyShouldTransmit( ShouldTransmitState_t state )
{
	if ( state == SHOULDTRANSMIT_END )
	{
		if( m_pFlashlightBeam != NULL )
		{
			ReleaseFlashlight();
		}
	}

	BaseClass::NotifyShouldTransmit( state );
}

void C_HL2MP_Player::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if ( type == DATA_UPDATE_CREATED )
	{
		SetNextClientThink( CLIENT_THINK_ALWAYS );
	}

	UpdateVisibility();

	UpdateGlowEffect();
}

void C_HL2MP_Player::PostDataUpdate( DataUpdateType_t updateType )
{
	if ( m_iSpawnInterpCounter != m_iSpawnInterpCounterCache )
	{
		MoveToLastReceivedPosition( true );
		ResetLatched();
		m_iSpawnInterpCounterCache = m_iSpawnInterpCounter;
	}

	BaseClass::PostDataUpdate( updateType );
}

void C_HL2MP_Player::ReleaseFlashlight( void )
{
	if( m_pFlashlightBeam )
	{
		m_pFlashlightBeam->flags = 0;
		m_pFlashlightBeam->die = gpGlobals->curtime - 1;

		m_pFlashlightBeam = NULL;
	}
}

float C_HL2MP_Player::GetFOV( void )
{
	//Find our FOV with offset zoom value
	float flFOVOffset = C_BasePlayer::GetFOV() + GetZoom();

	// Clamp FOV in MP
	int min_fov = GetMinFOV();
	
	// Don't let it go too low
	flFOVOffset = MAX( min_fov, flFOVOffset );

	return flFOVOffset;
}

//=========================================================
// Autoaim
// set crosshair position to point to enemey
//=========================================================
Vector C_HL2MP_Player::GetAutoaimVector( float flDelta )
{
	// Never autoaim a predicted weapon (for now)
	Vector	forward;
	AngleVectors( EyeAngles() + m_Local.m_vecPunchAngle, &forward );
	return	forward;
}

void C_HL2MP_Player::ItemPreFrame( void )
{
	if ( GetFlags() & FL_FROZEN )
		 return;

	// Disallow shooting while zooming
	if ( m_nButtons & IN_ZOOM )
	{
		//FIXME: Held weapons like the grenade get sad when this happens
		m_nButtons &= ~(IN_ATTACK|IN_ATTACK2);
	}

	BaseClass::ItemPreFrame();

}
	
void C_HL2MP_Player::ItemPostFrame( void )
{
	if ( GetFlags() & FL_FROZEN )
		 return;

	BaseClass::ItemPostFrame();
}

C_BaseAnimating *C_HL2MP_Player::BecomeRagdollOnClient()
{
	// Let the C_CSRagdoll entity do this.
	// m_builtRagdoll = true;
	return NULL;
}

void C_HL2MP_Player::CalcView( Vector &eyeOrigin, QAngle &eyeAngles, float &zNear, float &zFar, float &fov )
{
	if ( m_lifeState != LIFE_ALIVE && !IsObserver() )
	{
		Vector origin = EyePosition();			

		IRagdoll *pRagdoll = GetRepresentativeRagdoll();

		if ( pRagdoll )
		{
			origin = pRagdoll->GetRagdollOrigin();
			origin.z += VEC_DEAD_VIEWHEIGHT_SCALED( this ).z; // look over ragdoll, not through
		}

		BaseClass::CalcView( eyeOrigin, eyeAngles, zNear, zFar, fov );

		eyeOrigin = origin;
		
		Vector vForward; 
		AngleVectors( eyeAngles, &vForward );

		VectorNormalize( vForward );
		VectorMA( origin, -CHASE_CAM_DISTANCE_MAX, vForward, eyeOrigin );

		Vector WALL_MIN( -WALL_OFFSET, -WALL_OFFSET, -WALL_OFFSET );
		Vector WALL_MAX( WALL_OFFSET, WALL_OFFSET, WALL_OFFSET );

		trace_t trace; // clip against world
		C_BaseEntity::PushEnableAbsRecomputations( false ); // HACK don't recompute positions while doing RayTrace
		UTIL_TraceHull( origin, eyeOrigin, WALL_MIN, WALL_MAX, MASK_SOLID_BRUSHONLY, this, COLLISION_GROUP_NONE, &trace );
		C_BaseEntity::PopEnableAbsRecomputations();

		if (trace.fraction < 1.0)
		{
			eyeOrigin = trace.endpos;
		}
		
		return;
	}

	BaseClass::CalcView( eyeOrigin, eyeAngles, zNear, zFar, fov );
}

IRagdoll* C_HL2MP_Player::GetRepresentativeRagdoll() const
{
	if ( m_hRagdoll.Get() )
	{
		C_HL2MPRagdoll *pRagdoll = (C_HL2MPRagdoll*)m_hRagdoll.Get();

		return pRagdoll->GetIRagdoll();
	}
	else
	{
		return NULL;
	}
}

//HL2MPRAGDOLL

//-----------------------------------------------------------------------------
// Purpose: Gore!
//-----------------------------------------------------------------------------

// Scale head to nothing
static void ScaleGoreHead(C_BaseAnimating* pAnimating)
{
	if (pAnimating)
	{
		int iBone = -1;

		const char* boneNames[] = { "LrigScull", "LrigNeck2", "LrigNeck3",
									"Jaw", "Ear_R", "Ear_L", 
									"Mane01", "Mane02", "Mane03", "Mane03_tip",
									"Mane04", "Mane05", "Mane06", "Mane07" 
		};

		for (int i = 0; i < 14; i++)
		{
			iBone = pAnimating->LookupBone(boneNames[i]);
			if (iBone != -1)
				MatrixScaleBy(0.001f, pAnimating->GetBoneForWrite(iBone));
		}
	}

}

// Scale left arm to nothing
static void ScaleGoreFrontLeftKnee(C_BaseAnimating* pAnimating)
{
	if (pAnimating)
	{
		int iBone = -1;

		const char* boneNames[] = { "Lrig_LEG_FL_FrontHoof", "Lrig_LEG_FL_PhalangesManus", "Lrig_LEG_FL_Metacarpus", "Lrig_LEG_FL_Radius"};

		for (int i = 0; i < 4; i++)
		{
			iBone = pAnimating->LookupBone(boneNames[i]);
			if (iBone != -1)
				MatrixScaleBy(0.001f, pAnimating->GetBoneForWrite(iBone));
		}
	}
}

// Scale right arm to nothing
static void ScaleGoreFrontRightKnee(C_BaseAnimating* pAnimating)
{
	if (pAnimating)
	{
		int iBone = -1;

		const char* boneNames[] = { "Lrig_LEG_FR_FrontHoof", "Lrig_LEG_FR_PhalangesManus", "Lrig_LEG_FR_Metacarpus", "Lrig_LEG_FR_Radius" };

		for (int i = 0; i < 4; i++)
		{
			iBone = pAnimating->LookupBone(boneNames[i]);
			if (iBone != -1)
				MatrixScaleBy(0.001f, pAnimating->GetBoneForWrite(iBone));
		}
	}
}

// Scale left knee to nothing
static void ScaleGoreRearLeftKnee(C_BaseAnimating* pAnimating)
{
	if (pAnimating)
	{
		int iBone = -1;

		const char* boneNames[] = { "Lrig_LEG_BL_RearHoof", "Lrig_LEG_BL_PhalanxPrima", "Lrig_LEG_BL_LargeCannon", "Lrig_LEG_BL_Tibia" };

		for (int i = 0; i < 4; i++)
		{
			iBone = pAnimating->LookupBone(boneNames[i]);
			if (iBone != -1)
				MatrixScaleBy(0.001f, pAnimating->GetBoneForWrite(iBone));
		}
	}
}

// Scale right knee to nothing
static void ScaleGoreRearRightKnee(C_BaseAnimating* pAnimating)
{
	if (pAnimating)
	{
		int iBone = -1;

		const char* boneNames[] = { "Lrig_LEG_BR_RearHoof", "Lrig_LEG_BR_PhalanxPrima", "Lrig_LEG_BR_LargeCannon", "Lrig_LEG_BR_Tibia" };

		for (int i = 0; i < 4; i++)
		{
			iBone = pAnimating->LookupBone(boneNames[i]);
			if (iBone != -1)
				MatrixScaleBy(0.001f, pAnimating->GetBoneForWrite(iBone));
		}
	}
}

IMPLEMENT_CLIENTCLASS_DT_NOBASE( C_HL2MPRagdoll, DT_HL2MPRagdoll, CHL2MPRagdoll )
	RecvPropVector( RECVINFO(m_vecRagdollOrigin) ),
#ifdef PONEDM
	RecvPropInt(RECVINFO(m_iPlayerIndex)),
	RecvPropInt(RECVINFO(m_iGoreHead)),
	RecvPropInt(RECVINFO(m_iGoreFrontLeftLeg)),
	RecvPropInt(RECVINFO(m_iGoreFrontRightLeg)),
	RecvPropInt(RECVINFO(m_iGoreRearLeftLeg)),
	RecvPropInt(RECVINFO(m_iGoreRearRightLeg)),
	RecvPropInt(RECVINFO(m_iUpperManeBodygroup)),
	RecvPropInt(RECVINFO(m_iLowerManeBodygroup)),
	RecvPropInt(RECVINFO(m_iTailBodygroup)),
	RecvPropInt(RECVINFO(m_iHornBodygroup)),
	RecvPropInt(RECVINFO(m_iWingsBodygroup)),
#endif
	RecvPropEHandle(RECVINFO(m_hPlayer)),
	RecvPropInt(RECVINFO(m_nModelIndex)),
	RecvPropInt( RECVINFO(m_nForceBone) ),
	RecvPropVector( RECVINFO(m_vecForce) ),
	RecvPropVector( RECVINFO( m_vecRagdollVelocity ) )
END_RECV_TABLE()



C_HL2MPRagdoll::C_HL2MPRagdoll()
{
#ifdef PONEDM
	m_iPlayerIndex = PONEDM_PLAYER_INDEX_NONE;

	m_pDizzyEffect = NULL;

	m_bGoreEnabled = false;

	m_iGoreDecalAmount = 0;
	m_iGoreDecalBone = 0;
	m_fGoreDecalTime = -1;

	m_iGoreHead = 0;
	m_iGoreFrontLeftLeg = 0;
	m_iGoreFrontRightLeg = 0;
	m_iGoreRearLeftLeg = 0;
	m_iGoreRearRightLeg = 0;

	m_iUpperManeBodygroup = 0;
	m_iLowerManeBodygroup = 0;
	m_iTailBodygroup = 0;
	m_iHornBodygroup = 0;
	m_iWingsBodygroup = 0;
#endif
}

C_HL2MPRagdoll::~C_HL2MPRagdoll()
{
	PhysCleanupFrictionSounds( this );

	if ( m_hPlayer )
	{
		m_hPlayer->CreateModelInstance();
	}
}

void C_HL2MPRagdoll::Interp_Copy( C_BaseAnimatingOverlay *pSourceEntity )
{
	if ( !pSourceEntity )
		return;
	
	VarMapping_t *pSrc = pSourceEntity->GetVarMapping();
	VarMapping_t *pDest = GetVarMapping();
    	
	// Find all the VarMapEntry_t's that represent the same variable.
	for ( int i = 0; i < pDest->m_Entries.Count(); i++ )
	{
		VarMapEntry_t *pDestEntry = &pDest->m_Entries[i];
		const char *pszName = pDestEntry->watcher->GetDebugName();
		for ( int j=0; j < pSrc->m_Entries.Count(); j++ )
		{
			VarMapEntry_t *pSrcEntry = &pSrc->m_Entries[j];
			if ( !Q_strcmp( pSrcEntry->watcher->GetDebugName(), pszName ) )
			{
				pDestEntry->watcher->Copy( pSrcEntry->watcher );
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Scale the bones that need to be scaled for gore
//-----------------------------------------------------------------------------
void C_HL2MPRagdoll::BuildTransformations(CStudioHdr* pStudioHdr, Vector* pos, Quaternion q[], const matrix3x4_t& cameraTransform, int boneMask, CBoneBitList& boneComputed)
{
	BaseClass::BuildTransformations(pStudioHdr, pos, q, cameraTransform, boneMask, boneComputed);

	if (m_bGoreEnabled)
		ScaleGoreBones();
}

void C_HL2MPRagdoll::ScaleGoreBones()
{
	if (m_iGoreHead > 1)
		ScaleGoreHead(this);

	if (m_iGoreFrontLeftLeg > 1)
		ScaleGoreFrontLeftKnee(this);

	if (m_iGoreFrontRightLeg > 1)
		ScaleGoreFrontRightKnee(this);

	if (m_iGoreRearLeftLeg > 1)
		ScaleGoreRearLeftKnee(this);

	if (m_iGoreRearRightLeg > 1)
		ScaleGoreRearRightKnee(this);
}

//HACK
void C_HL2MPRagdoll::ResetScaledBones()
{
	int iBone = -1;

	const char* boneNames[] = {
								"LrigScull", "LrigNeck2", "LrigNeck3",
								"Jaw", "Ear_R", "Ear_L",
								"Mane01", "Mane02", "Mane03", "Mane03_tip",
								"Mane04", "Mane05", "Mane06", "Mane07",
								"Lrig_LEG_FL_FrontHoof", "Lrig_LEG_FL_PhalangesManus", "Lrig_LEG_FL_Metacarpus", "Lrig_LEG_FL_Radius",
								"Lrig_LEG_FR_FrontHoof", "Lrig_LEG_FR_PhalangesManus", "Lrig_LEG_FR_Metacarpus", "Lrig_LEG_FR_Radius",
								"Lrig_LEG_BL_RearHoof", "Lrig_LEG_BL_PhalanxPrima", "Lrig_LEG_BL_LargeCannon", "Lrig_LEG_BL_Tibia",
								"Lrig_LEG_BR_RearHoof", "Lrig_LEG_BR_PhalanxPrima", "Lrig_LEG_BR_LargeCannon", "Lrig_LEG_BR_Tibia"
	};

	for (int i = 0; i < 30; i++)
	{
		iBone = LookupBone(boneNames[i]);
		if (iBone != -1)
			MatrixScaleBy(1.0f, GetBoneForWrite(iBone));
	}
}

void C_HL2MPRagdoll::DismemberBase(bool bBloodEffects, char const* szParticleBone)
{
	if (cl_ponedm_violencelevel.GetInt() == 2)
	{
		int iAttach = LookupBone(szParticleBone);

		int iBloodAmount = 4;
		int iGibAmount = 4;

		if (iAttach != -1)
		{
			if (FStrEq(szParticleBone, "LrigNeck1"))
			{
				ParticleProp()->Create("smod_headshot_r", PATTACH_BONE_FOLLOW, szParticleBone);
				ParticleProp()->Create("smod_blood_decap_r", PATTACH_BONE_FOLLOW, szParticleBone);
				ParticleProp()->Create("blood_zombie_split_spray", PATTACH_BONE_FOLLOW, szParticleBone);
				iBloodAmount = 15;
				iGibAmount = 15;
			}
			else
			{
				ParticleProp()->Create("smod_blood_gib_r", PATTACH_BONE_FOLLOW, szParticleBone);
			}

			for (int i = 0; i <= iGibAmount; i++)
			{
				int randModel = RandomInt(0, 1);
				const char* model = "models/gibs/pgib_p3.mdl";

				if (randModel == 1)
				{
					model = "models/gibs/pgib_p4.mdl";
				}

				Vector vecOrigin;
				int iAttachment = Studio_BoneIndexByName(GetBaseAnimating()->GetModelPtr(), szParticleBone);
				if (iAttachment != -1)
				{
					const matrix3x4_t mBone = GetBaseAnimating()->GetBone(iAttachment);
					MatrixPosition(mBone, vecOrigin);
					Vector offset = RandomVector(-32, 32) + vecOrigin;
					C_Gib::CreateClientsideGib(model, offset, RandomVector(-25.0f, 25.0f), RandomAngularImpulse(-32, 32), cl_ponedm_gibtime.GetFloat());
				}
			}

			m_iGoreDecalAmount += iBloodAmount;
			m_iGoreDecalBone = iAttach;

			EmitSound("Gore.Headshot");
		}
	}
}

void C_HL2MPRagdoll::DismemberHead()
{
	DismemberBase(true, "LrigNeck1");
	m_iGoreHead = 3;
}

void C_HL2MPRagdoll::DismemberFrontLeftLeg()
{
	DismemberBase(true, "Lrig_LEG_FL_Radius");

	m_iGoreFrontLeftLeg = 3;
}

void C_HL2MPRagdoll::DismemberFrontRightLeg()
{
	DismemberBase(true, "Lrig_LEG_FR_Radius");

	m_iGoreFrontRightLeg = 3;
}

void C_HL2MPRagdoll::DismemberRearLeftLeg()
{
	DismemberBase(true, "Lrig_LEG_BL_Tibia");

	m_iGoreRearLeftLeg = 3;
}

void C_HL2MPRagdoll::DismemberRearRightLeg()
{
	DismemberBase(true, "Lrig_LEG_BR_Tibia");

	m_iGoreRearRightLeg = 3;
}

void C_HL2MPRagdoll::InitDismember()
{
	// head does not have two levels of dismemberment, only one
	if (m_iGoreHead > 1)
		DismemberHead();

	if (m_iGoreFrontLeftLeg > 1)
		DismemberFrontLeftLeg();

	if (m_iGoreFrontRightLeg > 1)
		DismemberFrontRightLeg();

	if (m_iGoreRearLeftLeg > 1)
		DismemberRearLeftLeg();

	if (m_iGoreRearRightLeg > 1)
		DismemberRearRightLeg();
}

void C_HL2MPRagdoll::DismemberRandomLimbs(void)
{
	int iGore = 0;

	if (m_iGoreHead < 3)
	{
		iGore = random->RandomInt(0, 2);

		if (iGore == 2)
			DismemberHead();
	}

	if (m_iGoreFrontLeftLeg < 3)
	{
		iGore = random->RandomInt(0, 2);

		if (iGore == 2)
			DismemberFrontLeftLeg();
	}

	if (m_iGoreFrontRightLeg < 3)
	{
		iGore = random->RandomInt(0, 2);

		if (iGore == 2)
			DismemberFrontRightLeg();
	}

	if (m_iGoreRearLeftLeg < 3)
	{
		iGore = random->RandomInt(0, 2);

		if (iGore == 2)
			DismemberRearLeftLeg();
	}

	if (m_iGoreRearRightLeg < 3)
	{
		iGore = random->RandomInt(0, 2);

		if (iGore == 2)
			DismemberRearRightLeg();
	}
}

void C_HL2MPRagdoll::ImpactTrace( trace_t *pTrace, int iDamageType, const char *pCustomImpactName )
{
	IPhysicsObject *pPhysicsObject = VPhysicsGetObject();

	if( !pPhysicsObject )
		return;

	Vector dir = pTrace->endpos - pTrace->startpos;
	Vector vecOrigin = pTrace->endpos - dir * 4;

	// m_iGore<limb> has a level, from 0 to 3
	// 1 is unused (reserved for normal TF bodygroups like pyro's head)
	// 2 means the lower limb is marked for removal, 3 means the upper limb is marked for removal, the head is an exception as it only has level 2
	// if our current level is at level 3, that means we can't dismember this limb anymore
	// if our current level is at level 2, that means we can dismember this limb once more up to level 3
	// if our current level is at level 0/1, that means we can dismember this limb up to level 2
	// Dismember<limb> function accepts true or false, true means this limb will be dismembered to level 3, false means dismembered to level 2

	switch (pTrace->hitgroup)
	{
	case HITGROUP_HEAD:
		if (m_iGoreHead == 3)
		{
			break;
		}
		else if (m_iGoreHead == 2)
		{
			break;
		}
		else
		{
			DismemberHead();
			break;
		}
	case HITGROUP_LEFTARM:
		if (m_iGoreFrontLeftLeg == 3)
		{
			break;
		}
		else if (m_iGoreFrontLeftLeg == 2)
		{
			break;
		}
		else
		{
			DismemberFrontLeftLeg();
			break;
		}
	case HITGROUP_RIGHTARM:
		if (m_iGoreFrontRightLeg == 3)
		{
			break;
		}
		else if (m_iGoreFrontRightLeg == 2)
		{
			break;
		}
		else
		{
			DismemberFrontRightLeg();
			break;
		}
	case HITGROUP_LEFTLEG:
		if (m_iGoreRearLeftLeg == 3)
		{
			break;
		}
		else if (m_iGoreRearLeftLeg == 2)
		{
			break;
		}
		else
		{
			DismemberRearLeftLeg();
			break;
		}
	case HITGROUP_RIGHTLEG:
		if (m_iGoreRearRightLeg == 3)
		{
			break;
		}
		else if (m_iGoreRearRightLeg == 2)
		{
			break;
		}
		else
		{
			DismemberRearRightLeg();
			break;
		}
	default:
		break;
	}

	if ( iDamageType == DMG_BLAST || iDamageType == DMG_NERVEGAS)
	{
		dir *= 4000;  // adjust impact strenght
				
		// apply force at object mass center
		pPhysicsObject->ApplyForceCenter( dir );

		DismemberRandomLimbs();
	}
	else
	{
		Vector hitpos;  
	
		VectorMA( pTrace->startpos, pTrace->fraction, dir, hitpos );
		VectorNormalize( dir );

		dir *= 4000;  // adjust impact strenght

		// apply force where we hit it
		pPhysicsObject->ApplyForceOffset( dir, hitpos );	

		if (cl_ponedm_violencelevel.GetInt() >= 1)
		{
			// Blood spray!
	//		FX_CS_BloodSpray( hitpos, dir, 10 );

			// make ragdolls emit blood decals
			// not a great way to do it, but it only works if all of it is defined here for some reason

			// make blood decal on the wall!
			trace_t Bloodtr;
			Vector vecTraceDir;
			float flNoise;
			int i;

			int cCount = 3;

			flNoise = 0.3;

			float flTraceDist = 172;

			for (i = 0; i < cCount; i++)
			{
				vecTraceDir = dir * -1;// trace in the opposite direction the shot came from (the direction the shot is going)

				vecTraceDir.x += random->RandomFloat(-flNoise, flNoise);
				vecTraceDir.y += random->RandomFloat(-flNoise, flNoise);
				vecTraceDir.z += random->RandomFloat(-flNoise, flNoise);

				// Don't bleed on grates.
				AI_TraceLine(pTrace->endpos, pTrace->endpos + vecTraceDir * -flTraceDist, MASK_SOLID_BRUSHONLY & ~CONTENTS_GRATE, this, COLLISION_GROUP_NONE, &Bloodtr);

				if (Bloodtr.fraction != 1.0)
				{
					UTIL_BloodDecalTrace(&Bloodtr, BLOOD_COLOR_RED);
				}
			}
		}
	}

	m_pRagdoll->ResetRagdollSleepAfterTime();
}

void C_HL2MPRagdoll::ClientThink(void)
{
	SetNextClientThink(CLIENT_THINK_ALWAYS);

	//make sure we check to make sure gore is enabled.
	int iBone = LookupBone("LrigScull");

	// enable dismemberment if we find LrigScull, as this is likely a pone model.
	if (iBone != -1)
	{
		if (cl_ponedm_violencelevel.GetInt() >= 2)
			m_bGoreEnabled = true;
		else
			m_bGoreEnabled = false;
	}
	else
		m_bGoreEnabled = false;

	if (m_bGoreEnabled == false)
	{
		ResetScaledBones();
	}

	if (cl_ponedm_violencelevel.GetInt() == 0)
	{
		m_nSkin = 2;

		Vector vecOrigin, vForward, vRight, vUp;
		QAngle vecAngles;
		// Get the muzzle origin
		if (!GetAttachment("forward", vecOrigin, vecAngles))
		{
			vecOrigin = GetAbsOrigin();
			vecAngles = GetAbsAngles();
		}

		AngleVectors(vecAngles, &vForward, &vRight, &vUp);

		if (!m_pDizzyEffect)
		{
			m_pDizzyEffect = ParticleProp()->Create("conc_stars", PATTACH_POINT_FOLLOW, LookupAttachment("forward"));

			if (!m_pDizzyEffect)
			{
				m_pDizzyEffect->SetControlPointOrientation(0, vForward, vRight, vUp);
			}
		}
	}
	else if (cl_ponedm_violencelevel.GetInt() >= 1)
	{
		//set the ragdoll's skin to the player dead skin.
		m_nSkin = 1;

		if (m_pDizzyEffect)
		{
			m_pDizzyEffect->StopEmission();
			m_pDizzyEffect = NULL;
		}
	}


	EHANDLE hPlayer = GetPlayerHandle();

	if (hPlayer)
	{
		C_HL2MP_Player* hl2mppPlayer = ToHL2MPPlayer(hPlayer);

		if (hl2mppPlayer)
		{
			if ((!HL2MPRules()->IsTeamplay() && (sv_ponedm_gamemode.GetInt() == 3)) && hl2mppPlayer->GetTeamNumber() == TEAM_ZOMBIES)
			{
				hl2mppPlayer->disableBlink = true;
				m_nSkin = 1;
			}
		}
	}

	// emit some blood decals if necessary
	if (m_iGoreDecalAmount > 0 && m_fGoreDecalTime < gpGlobals->curtime)
	{
		// emit another decal again after 0.1 seconds
		m_fGoreDecalTime = gpGlobals->curtime + 0.1f;
		m_iGoreDecalAmount--;

		if (m_iGoreDecalBone != -1)
		{
			Vector direction;
			Vector start;
			QAngle dummy;
			trace_t	tr;

			GetBonePosition(m_iGoreDecalBone, start, dummy);

			// any random direction
			direction.x = random->RandomFloat(-32, 32);
			direction.y = random->RandomFloat(-32, 32);
			direction.z = random->RandomFloat(-32, 32);

			UTIL_TraceLine(start, start + direction, MASK_SOLID_BRUSHONLY, this, COLLISION_GROUP_NONE, &tr);
			UTIL_BloodDecalTrace(&tr, BLOOD_COLOR_RED);

			//debugoverlay->AddLineOverlay( start, start + direction, 0, 255, 0, true, 1 ); 
		}
	}
}

void C_HL2MPRagdoll::CreateHL2MPRagdoll( void )
{
	// First, initialize all our data. If we have the player's entity on our client,
	// then we can make ourselves start out exactly where the player is.
	C_HL2MP_Player *pPlayer = dynamic_cast< C_HL2MP_Player* >( m_hPlayer.Get() );
	
	if ( pPlayer && !pPlayer->IsDormant() )
	{
		// move my current model instance to the ragdoll's so decals are preserved.
		pPlayer->SnatchModelInstance( this );

		VarMapping_t *varMap = GetVarMapping();

		// Copy all the interpolated vars from the player entity.
		// The entity uses the interpolated history to get bone velocity.
		bool bRemotePlayer = (pPlayer != C_BasePlayer::GetLocalPlayer());			
		if ( bRemotePlayer )
		{
			Interp_Copy( pPlayer );

			SetAbsAngles( pPlayer->GetRenderAngles() );
			GetRotationInterpolator().Reset();

			m_flAnimTime = pPlayer->m_flAnimTime;
			SetSequence( pPlayer->GetSequence() );
			m_flPlaybackRate = pPlayer->GetPlaybackRate();
		}
		else
		{
			// This is the local player, so set them in a default
			// pose and slam their velocity, angles and origin
			SetAbsOrigin( m_vecRagdollOrigin );
			
			SetAbsAngles( pPlayer->GetRenderAngles() );

			SetAbsVelocity( m_vecRagdollVelocity );

			int iSeq = pPlayer->GetSequence();
			if ( iSeq == -1 )
			{
				Assert( false );	// missing walk_lower?
				iSeq = 0;
			}
			
			SetSequence( iSeq );	// walk_lower, basic pose
			SetCycle( 0.0 );

			Interp_Reset( varMap );
		}		
	}
	else
	{
		// overwrite network origin so later interpolation will
		// use this position
		SetNetworkOrigin( m_vecRagdollOrigin );

		SetAbsOrigin( m_vecRagdollOrigin );
		SetAbsVelocity( m_vecRagdollVelocity );

		Interp_Reset( GetVarMapping() );
		
	}

	SetModelIndex( m_nModelIndex );

	// Make us a ragdoll..
	m_nRenderFX = kRenderFxRagdoll;

#ifdef PONEDM
	int iBone = LookupBone("LrigScull");

	// enable dismemberment if we find LrigScull, as this is likely a pone model.
	if (iBone != -1)
	{
		if (cl_ponedm_violencelevel.GetInt() >= 2)
			m_bGoreEnabled = true;
	}
	else
		m_bGoreEnabled = false;

	if (m_bGoreEnabled)
		m_BoneAccessor.SetWritableBones(BONE_USED_BY_ANYTHING);

	if (m_bGoreEnabled == false)
	{
		ResetScaledBones();
	}

	// must think immediately for dismemberment
	SetNextClientThink(gpGlobals->curtime + 0.1f);
#endif

	matrix3x4_t boneDelta0[MAXSTUDIOBONES];
	matrix3x4_t boneDelta1[MAXSTUDIOBONES];
	matrix3x4_t currentBones[MAXSTUDIOBONES];
	const float boneDt = 0.05f;

	if ( pPlayer && !pPlayer->IsDormant() )
	{
		pPlayer->GetRagdollInitBoneArrays( boneDelta0, boneDelta1, currentBones, boneDt );
	}
	else
	{
		GetRagdollInitBoneArrays( boneDelta0, boneDelta1, currentBones, boneDt );
	}

	InitAsClientRagdoll( boneDelta0, boneDelta1, currentBones, boneDt );
}


void C_HL2MPRagdoll::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if ( type == DATA_UPDATE_CREATED )
	{
		bool bCreateRagdoll = true;

		// Get the player.
		EHANDLE hPlayer = GetPlayerHandle();
		if (hPlayer)
		{
			// If we're getting the initial update for this player (e.g., after resetting entities after
			//  lots of packet loss, then don't create gibs, ragdolls if the player and it's gib/ragdoll
			//  both show up on same frame.
			if (abs(hPlayer->GetCreationTick() - gpGlobals->tickcount) < TIME_TO_TICKS(1.0f))
			{
				bCreateRagdoll = false;
			}
		}
		else if (C_BasePlayer::GetLocalPlayer())
		{
			// Ditto for recreation of the local player
			if (abs(C_BasePlayer::GetLocalPlayer()->GetCreationTick() - gpGlobals->tickcount) < TIME_TO_TICKS(1.0f))
			{
				bCreateRagdoll = false;
			}
		}

		if (bCreateRagdoll)
		{
			/*if (m_bGib)
			{
				if (cl_ponedm_violencelevel.GetInt() >= 2)
				{
					CreatePoneDMGibs();
				}
			}
			else
			{*/
				CreateHL2MPRagdoll();

				if (cl_ponedm_violencelevel.GetInt() >= 2)
				{
					InitDismember();
				}

				SetBodygroup(PONEDM_UPPERMANE_BODYGROUP, m_iUpperManeBodygroup);
				SetBodygroup(PONEDM_LOWERMANE_BODYGROUP, m_iLowerManeBodygroup);
				SetBodygroup(PONEDM_TAIL_BODYGROUP, m_iTailBodygroup);
				SetBodygroup(PONEDM_HORN_BODYGROUP, m_iHornBodygroup);
				SetBodygroup(PONEDM_WINGS_BODYGROUP, m_iWingsBodygroup);
			//}
		}
	}
}

IRagdoll* C_HL2MPRagdoll::GetIRagdoll() const
{
	return m_pRagdoll;
}

void C_HL2MPRagdoll::UpdateOnRemove( void )
{
	VPhysicsSetObject( NULL );

	BaseClass::UpdateOnRemove();
}

//-----------------------------------------------------------------------------
// Purpose: clear out any face/eye values stored in the material system
//-----------------------------------------------------------------------------
void C_HL2MPRagdoll::SetupWeights( const matrix3x4_t *pBoneToWorld, int nFlexWeightCount, float *pFlexWeights, float *pFlexDelayedWeights )
{
	BaseClass::SetupWeights( pBoneToWorld, nFlexWeightCount, pFlexWeights, pFlexDelayedWeights );

	static float destweight[128];
	static bool bIsInited = false;

	CStudioHdr *hdr = GetModelPtr();
	if ( !hdr )
		return;

	int nFlexDescCount = hdr->numflexdesc();
	if ( nFlexDescCount )
	{
		Assert( !pFlexDelayedWeights );
		memset( pFlexWeights, 0, nFlexWeightCount * sizeof(float) );
	}

	if ( m_iEyeAttachment > 0 )
	{
		matrix3x4_t attToWorld;
		if (GetAttachment( m_iEyeAttachment, attToWorld ))
		{
			Vector local, tmp;
			local.Init( 1000.0f, 0.0f, 0.0f );
			VectorTransform( local, attToWorld, tmp );
			modelrender->SetViewTarget( GetModelPtr(), GetBody(), tmp );
		}
	}
}

void C_HL2MP_Player::PostThink( void )
{
	BaseClass::PostThink();

	// Store the eye angles pitch so the client can compute its animation state correctly.
	m_angEyeAngles = EyeAngles();
}

#ifdef PONEDM
//-----------------------------------------------------------------------------
// Returns the player's primary color
//-----------------------------------------------------------------------------
class CPrimaryPlayerColorProxy : public CResultProxy
{
public:
	void OnBind(void* pC_BaseEntity)
	{
		Assert(m_pResult);

		if (!pC_BaseEntity)
		{
			float r = floorf(cl_ponedm_primarycolor_r.GetFloat()) / 255.0f;
			float g = floorf(cl_ponedm_primarycolor_g.GetFloat()) / 255.0f;
			float b = floorf(cl_ponedm_primarycolor_b.GetFloat()) / 255.0f;
			m_pResult->SetVecValue(r, g, b);
			return;
		}

		C_BaseEntity* pEntity = BindArgToEntity(pC_BaseEntity);
		if (!pEntity)
			return;

		Vector vecColor = pEntity->GetPrimaryColor();

		if (vecColor == vec3_origin)
		{
			C_BaseEntity* pOwner = pEntity->GetPrimaryColorOwner();
			while (pOwner && pOwner->GetPrimaryColorOwner() != NULL)
			{
				pOwner = pOwner->GetPrimaryColorOwner();
				if (pOwner == pOwner->GetPrimaryColorOwner())
					break;
			}
			if (pOwner)
			{
				vecColor = pOwner->GetPrimaryColor();
			}
		}
		m_pResult->SetVecValue(vecColor.x, vecColor.y, vecColor.z);
		return;

		m_pResult->SetVecValue(1, 1, 1);
	}
};

EXPOSE_INTERFACE(CPrimaryPlayerColorProxy, IMaterialProxy, "PrimaryPlayerColor" IMATERIAL_PROXY_INTERFACE_VERSION);

//-----------------------------------------------------------------------------
// Returns the player's secondary color
//-----------------------------------------------------------------------------
class CSecondaryPlayerColorProxy : public CResultProxy
{
public:
	void OnBind(void* pC_BaseEntity)
	{
		Assert(m_pResult);

		if (!pC_BaseEntity)
		{
			float r = floorf(cl_ponedm_secondarycolor_r.GetFloat()) / 255.0f;
			float g = floorf(cl_ponedm_secondarycolor_g.GetFloat()) / 255.0f;
			float b = floorf(cl_ponedm_secondarycolor_b.GetFloat()) / 255.0f;
			m_pResult->SetVecValue(r, g, b);
			return;
		}

		C_BaseEntity* pEntity = BindArgToEntity(pC_BaseEntity);
		if (!pEntity)
			return;

		Vector vecColor = pEntity->GetSecondaryColor();

		if (vecColor == vec3_origin)
		{
			C_BaseEntity* pOwner = pEntity->GetSecondaryColorOwner();
			while (pOwner && pOwner->GetSecondaryColorOwner() != NULL)
			{
				pOwner = pOwner->GetSecondaryColorOwner();
				if (pOwner == pOwner->GetSecondaryColorOwner())
					break;
			}
			if (pOwner)
			{
				vecColor = pOwner->GetSecondaryColor();
			}
		}
		m_pResult->SetVecValue(vecColor.x, vecColor.y, vecColor.z);
		return;

		m_pResult->SetVecValue(1, 1, 1);
	}
};

EXPOSE_INTERFACE(CSecondaryPlayerColorProxy, IMaterialProxy, "SecondaryPlayerColor" IMATERIAL_PROXY_INTERFACE_VERSION);

//-----------------------------------------------------------------------------
// Returns the player's tertiary color
//-----------------------------------------------------------------------------
class CTertiaryPlayerColorProxy : public CResultProxy
{
public:
	void OnBind(void* pC_BaseEntity)
	{
		Assert(m_pResult);

		if (!pC_BaseEntity)
		{
			float r = floorf(cl_ponedm_tertiarycolor_r.GetFloat()) / 255.0f;
			float g = floorf(cl_ponedm_tertiarycolor_g.GetFloat()) / 255.0f;
			float b = floorf(cl_ponedm_tertiarycolor_b.GetFloat()) / 255.0f;
			m_pResult->SetVecValue(r, g, b);
			return;
		}

		C_BaseEntity* pEntity = BindArgToEntity(pC_BaseEntity);
		if (!pEntity)
			return;

		Vector vecColor = pEntity->GetTertiaryColor();

		if (vecColor == vec3_origin)
		{
			C_BaseEntity* pOwner = pEntity->GetTertiaryColorOwner();
			while (pOwner && pOwner->GetTertiaryColorOwner() != NULL)
			{
				pOwner = pOwner->GetTertiaryColorOwner();
				if (pOwner == pOwner->GetTertiaryColorOwner())
					break;
			}
			if (pOwner)
			{
				vecColor = pOwner->GetTertiaryColor();
			}
		}
		m_pResult->SetVecValue(vecColor.x, vecColor.y, vecColor.z);
		return;

		m_pResult->SetVecValue(1, 1, 1);
	}
};

EXPOSE_INTERFACE(CTertiaryPlayerColorProxy, IMaterialProxy, "TertiaryPlayerColor" IMATERIAL_PROXY_INTERFACE_VERSION);

//-----------------------------------------------------------------------------
// Returns the local player's primary color
//-----------------------------------------------------------------------------
class CPrimaryPlayerColorProxyLocal : public CResultProxy
{
public:
	void OnBind(void* pC_BaseEntity)
	{
		Assert(m_pResult);
		float r = floorf(cl_ponedm_primarycolor_r.GetFloat()) / 255.0f;
		float g = floorf(cl_ponedm_primarycolor_g.GetFloat()) / 255.0f;
		float b = floorf(cl_ponedm_primarycolor_b.GetFloat()) / 255.0f;
		m_pResult->SetVecValue(r, g, b);
	}
};

EXPOSE_INTERFACE(CPrimaryPlayerColorProxyLocal, IMaterialProxy, "PrimaryPlayerColorLocal" IMATERIAL_PROXY_INTERFACE_VERSION);

//-----------------------------------------------------------------------------
// Returns the local player's secondary color
//-----------------------------------------------------------------------------
class CSecondaryPlayerColorProxyLocal : public CResultProxy
{
public:
	void OnBind(void* pC_BaseEntity)
	{
		Assert(m_pResult);
		float r = floorf(cl_ponedm_secondarycolor_r.GetFloat()) / 255.0f;
		float g = floorf(cl_ponedm_secondarycolor_g.GetFloat()) / 255.0f;
		float b = floorf(cl_ponedm_secondarycolor_b.GetFloat()) / 255.0f;
		m_pResult->SetVecValue(r, g, b);
	}
};

EXPOSE_INTERFACE(CSecondaryPlayerColorProxyLocal, IMaterialProxy, "SecondaryPlayerColorLocal" IMATERIAL_PROXY_INTERFACE_VERSION);

//-----------------------------------------------------------------------------
// Returns the local player's tertiary color
//-----------------------------------------------------------------------------
class CTertiaryPlayerColorProxyLocal : public CResultProxy
{
public:
	void OnBind(void* pC_BaseEntity)
	{
		Assert(m_pResult);
		float r = floorf(cl_ponedm_tertiarycolor_r.GetFloat()) / 255.0f;
		float g = floorf(cl_ponedm_tertiarycolor_g.GetFloat()) / 255.0f;
		float b = floorf(cl_ponedm_tertiarycolor_b.GetFloat()) / 255.0f;
		m_pResult->SetVecValue(r, g, b);
	}
};

EXPOSE_INTERFACE(CTertiaryPlayerColorProxyLocal, IMaterialProxy, "TertiaryPlayerColorLocal" IMATERIAL_PROXY_INTERFACE_VERSION);
#endif