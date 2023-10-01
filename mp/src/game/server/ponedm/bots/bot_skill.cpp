//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
// Authors: 
// Iván Bravo Bravo (linkedin.com/in/ivanbravobravo), 2017

#include "cbase.h"
#include "bots/bot.h"

#ifdef INSOURCE_DLL
#include "in_utils.h"
#include "in_gamerules.h"
#else
#include "bots/in_utils.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

DECLARE_COMMAND(bot_difficulty, 0, "", FCVAR_NOTIFY)

//================================================================================
//================================================================================
CBotProfile::CBotProfile()
{
    if (bot_difficulty.GetInt() <= 0)
    {
        SetSkill(RandomInt(SKILL_EASY, SKILL_HARD));
    }
    else
    {
        SetSkill(bot_difficulty.GetInt());
    }
}

//================================================================================
//================================================================================
CBotProfile::CBotProfile( int skill )
{
    SetSkill( skill );
}

//================================================================================
// Sets the level of difficulty
//================================================================================
void CBotProfile::SetSkill( int skill )
{
    skill = clamp( skill, SKILL_EASIEST, SKILL_HARDEST );

    // TODO: Put all this in a script file

    switch ( skill ) {
        case SKILL_EASY:
            SetMemoryDuration(7.0f);
            SetReactionDelay(RandomFloat(0.0f, 0.01f));
            SetAlertDuration(RandomFloat(6.0f, 10.0f));
            SetAimSpeed(AIM_SPEED_NORMAL, AIM_SPEED_FAST);
            SetAttackDelay(RandomFloat(0.01f, 0.05f));
            SetFavoriteHitbox(HITGROUP_STOMACH);
            SetAggression(60.0f);
            break;

        case SKILL_MEDIUM:
        default:
            SetMemoryDuration(7.0f);
            SetReactionDelay(RandomFloat(0.0f, 0.01f));
            SetAlertDuration(RandomFloat(6.0f, 10.0f));
            SetAimSpeed(AIM_SPEED_FAST, AIM_SPEED_VERYFAST);
            SetAttackDelay(RandomFloat(0.005f, 0.01f));
            SetFavoriteHitbox(RandomInt(HITGROUP_CHEST, HITGROUP_STOMACH));
            SetAggression(90.0f);
            break;

        case SKILL_HARD:
            SetMemoryDuration(12.0f);
            SetReactionDelay(RandomFloat(0.0f, 0.01f));
            SetAlertDuration(RandomFloat(6.0f, 10.0f));
            SetAimSpeed(AIM_SPEED_VERYFAST, AIM_SPEED_INSTANT);
            SetAttackDelay(RandomFloat(0.0f, 0.05f));
            SetFavoriteHitbox(RandomInt(HITGROUP_CHEST, HITGROUP_STOMACH));
            SetAggression(150.0f);
            break;
    }

    m_iSkillLevel = skill;
}

//================================================================================
// Returns the name of the difficulty level
//================================================================================
const char *CBotProfile::GeSkillName()
{
    switch ( m_iSkillLevel ) {
        case SKILL_EASY:
            return "EASY";
            break;

        case SKILL_MEDIUM:
            return "MEDIUM";
            break;

        case SKILL_HARD:
            return "HARD";
            break;
    }

    return "UNKNOWN";
}
