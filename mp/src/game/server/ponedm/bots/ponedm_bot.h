#ifndef PONEDM_BOT_H
#define PONEDM_BOT_H

#ifdef _WIN32
#pragma once
#endif

#include "bots\bot.h"
#include "ai_hint.h"

//================================================================================
// Artificial intelligence for PoneDM
//================================================================================
class CPoneDM_Bot : public CBot
{
public:
    DECLARE_CLASS_GAMEROOT(CPoneDM_Bot, CBot);

    CPoneDM_Bot(CBasePlayer* parent);

public:
    virtual void Spawn();
    virtual void Update();
    virtual void SetUpComponents();
    virtual void SetUpSchedules();
};

extern CPlayer* CreatePoneDMBot(const char* pPlayername, const Vector* vecPosition, const QAngle* angles);
void SpawnPoneDMBots(int botCount);

#endif // PONEDM_BOT_H