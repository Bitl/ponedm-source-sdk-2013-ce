//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef HL2_SHAREDDEFS_H
#define HL2_SHAREDDEFS_H

#ifdef _WIN32
#pragma once
#endif

#include "const.h"


//--------------------------------------------------------------------------
// Collision groups
//--------------------------------------------------------------------------

enum
{
	HL2COLLISION_GROUP_PLASMANODE = LAST_SHARED_COLLISION_GROUP,
	HL2COLLISION_GROUP_SPIT,
	HL2COLLISION_GROUP_HOMING_MISSILE,
	HL2COLLISION_GROUP_COMBINE_BALL,

	HL2COLLISION_GROUP_FIRST_NPC,
	HL2COLLISION_GROUP_HOUNDEYE,
	HL2COLLISION_GROUP_CROW,
	HL2COLLISION_GROUP_HEADCRAB,
	HL2COLLISION_GROUP_STRIDER,
	HL2COLLISION_GROUP_GUNSHIP,
	HL2COLLISION_GROUP_ANTLION,
	HL2COLLISION_GROUP_LAST_NPC,
	HL2COLLISION_GROUP_COMBINE_BALL_NPC,
};


//--------------
// HL2 SPECIFIC
//--------------
#define DMG_SNIPER			(DMG_LASTGENERICFLAG<<1)	// This is sniper damage
#define DMG_MISSILEDEFENSE	(DMG_LASTGENERICFLAG<<2)	// The only kind of damage missiles take. (special missile defense)

#ifdef PONEDM
#define PONEDM_PLAYER_INDEX_NONE			( MAX_PLAYERS + 1 )
#define PONEDM_PLAYERSPEED 320
#define PONEDM_UPPERMANE_BODYGROUP 1
#define PONEDM_LOWERMANE_BODYGROUP 2
#define PONEDM_TAIL_BODYGROUP 3
#define PONEDM_HORN_BODYGROUP 4
#define PONEDM_WINGS_BODYGROUP 5
#define PONEDM_NOWINGSORHORN 0
#endif


#endif // HL2_SHAREDDEFS_H
