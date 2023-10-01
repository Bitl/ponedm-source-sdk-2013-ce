//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
// Authors: 
// Iván Bravo Bravo (linkedin.com/in/ivanbravobravo), 2017

#include "cbase.h"
#include "bots/bot.h"

#include "bots/bot_manager.h"

#ifdef INSOURCE_DLL
#include "in_utils.h"
#else
#include "bots/in_utils.h"
#endif

#include "in_buttons.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//================================================================================
// Called on each frame to update component operation.
//================================================================================
void CBotAttack::Update()
{
    VPROF_BUDGET( "CBotAttack::Update", VPROF_BUDGETGROUP_BOTS );

    CEntityMemory* memory = GetMemory()->GetPrimaryThreat();

    if (GetVision() && memory)
    {
        CBasePlayer* pAsshole = (CBasePlayer*)memory->GetEntity();

        if (pAsshole)
        {
            // We aim at the enemy
            GetVision()->LookAt("Enemy", pAsshole->EyePosition(), PRIORITY_CRITICAL, 1.0f);
        }
    }

    if (!HasCondition(BCOND_ENEMY_OCCLUDED) && !HasCondition(BCOND_ENEMY_OCCLUDED_BY_FRIEND))
    {
        if (HasCondition(BCOND_CAN_MELEE_ATTACK1) || HasCondition(BCOND_CAN_MELEE_ATTACK2))
        {
            if (!GetDecision()->IsUsingFiregun())
            {
                MeleeWeaponAttack();
            }
        }
        else if (HasCondition(BCOND_CAN_RANGE_ATTACK1) || HasCondition(BCOND_CAN_RANGE_ATTACK2))
        {
            if (GetDecision()->IsUsingFiregun())
            {
                FiregunAttack();
            }
        }
    }
}

//================================================================================
// Activates the attack of a firearm
//================================================================================
void CBotAttack::FiregunAttack()
{
    // Check to see if we can crouch for accuracy
    if ( GetDecision()->CanCrouchAttack() ) {
        if ( GetDecision()->ShouldCrouchAttack() ) {
            GetLocomotion()->Crouch();
        }
        else {
            GetLocomotion()->StandUp();
        }
    }

    if ( HasCondition( BCOND_CAN_RANGE_ATTACK1 ) ) {
        GetBot()->Combat();
        InjectButton( IN_ATTACK );
        
    }
    else if ( HasCondition( BCOND_CAN_RANGE_ATTACK2 ) ) {
        GetBot()->Combat();
        InjectButton( IN_ATTACK2 );
    }
}

//================================================================================
//================================================================================
void CBotAttack::MeleeWeaponAttack()
{
    if ( HasCondition( BCOND_CAN_MELEE_ATTACK1 ) ) {
        GetBot()->Combat();
        InjectButton( IN_ATTACK );
    }
    else if ( HasCondition( BCOND_CAN_MELEE_ATTACK2 ) ) {
        GetBot()->Combat();
        InjectButton( IN_ATTACK2 );
    }
}
