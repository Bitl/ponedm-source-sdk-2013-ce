#include "cbase.h"
#include "ponedm_bot.h"
#include "bot_manager.h"
#include "bots/in_utils.h"
#include "in_buttons.h"
#include "hl2_shareddefs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar sv_ponedm_bot_closestdistancetoplayer("sv_ponedm_bot_closestdistancetoplayer", "1800", FCVAR_CHEAT | FCVAR_NOTIFY, "");
ConVar sv_ponedm_bot_respawn("sv_ponedm_bot_respawn", "1", FCVAR_CHEAT | FCVAR_NOTIFY, "");
ConVar sv_ponedm_bot_ai("sv_ponedm_bot_ai", "1", FCVAR_NOTIFY, "");
ConVar sv_ponedm_bot_tag("sv_ponedm_bot_tag", "1", FCVAR_NOTIFY, "");

extern void respawn(CBaseEntity* pEdict, bool fCopyCorpse);

CUtlVector<string_t> m_botPonyNames;

void LoadPonyNames(void)
{
    m_botPonyNames.RemoveAll();

    KeyValues* pKV = new KeyValues("PonyNames");
    if (pKV->LoadFromFile(filesystem, "scripts/pony_names.txt", "GAME"))
    {
        FOR_EACH_VALUE(pKV, pSubData)
        {
            if (FStrEq(pSubData->GetString(), ""))
                continue;

            string_t iName = AllocPooledString(pSubData->GetString());
            if (m_botPonyNames.Find(iName) == m_botPonyNames.InvalidIndex())
                m_botPonyNames[m_botPonyNames.AddToTail()] = iName;
        }
    }

    pKV->deleteThis();
}

const char* NewPonyNameSelection(void)
{
    if (m_botPonyNames.Count() == 0)
        return "MISSINGNO";

    int nPoneNames = m_botPonyNames.Count();
    int randomChoice = rand() % nPoneNames;
    string_t iszName = m_botPonyNames[randomChoice];
    const char* pszName = STRING(iszName);

    return pszName;
}

//================================================================================
// It allows to create a bot with the name and position specified. Uses our custom name algorithm.
//================================================================================
CPlayer* CreatePoneDMBot(const char* pPlayername, const Vector* vecPosition, const QAngle* angles)
{
    LoadPonyNames();

    if (!pPlayername) {
        //HACK!!!!!
        int nameCount = RandomInt(1, 3);
        const char* pPlayername1 = NewPonyNameSelection();
        char string2[MAX_PLAYER_NAME_LENGTH];
        Q_snprintf(string2, sizeof(string2), " %s", NewPonyNameSelection());
        const char* pPlayername2 = (nameCount >= 2) ? string2 : "";
        char string3[MAX_PLAYER_NAME_LENGTH];
        Q_snprintf(string3, sizeof(string3), " %s", NewPonyNameSelection());
        const char* pPlayername3 = (nameCount == 3) ? string3 : "";
        const char* pBotNameFormat = sv_ponedm_bot_tag.GetBool() ? "[BOT] %s%s%s" : "%s%s%s";

        char combinedName[MAX_PLAYER_NAME_LENGTH];
        Q_snprintf(combinedName, sizeof(combinedName), pBotNameFormat, pPlayername1, pPlayername2, pPlayername3);
        pPlayername = combinedName;
    }

    edict_t* pSoul = engine->CreateFakeClient(pPlayername);
    Assert(pSoul);

    if (!pSoul) {
        Warning("There was a problem creating a bot. Maybe there is no more space for players on the server.");
        return NULL;
    }

    CPlayer* pPlayer = (CPlayer*)CBaseEntity::Instance(pSoul);
    Assert(pPlayer);

    pPlayer->ClearFlags();
    pPlayer->AddFlag(FL_CLIENT | FL_FAKECLIENT);

    // This is where we implement the Artificial Intelligence. 
    pPlayer->SetUpBot();
    Assert(pPlayer->GetBotController());

    if (!pPlayer->GetBotController()) {
        Warning("There was a problem creating a bot. The player was created but the controller could not be created.");
        return NULL;
    }

    pPlayer->Spawn();

    if (vecPosition) {
        pPlayer->Teleport(vecPosition, angles, NULL);
    }

    ++g_botID;
    return pPlayer;
}

CPoneDM_Bot::CPoneDM_Bot(CBasePlayer* parent) : BaseClass(parent)
{
}

//================================================================================
// Create the components that the bot will have
//================================================================================
void CPoneDM_Bot::SetUpComponents()
{
    ADD_COMPONENT(CBotVision);
    ADD_COMPONENT(CBotFollow);
    ADD_COMPONENT(CBotLocomotion);
    ADD_COMPONENT(CBotMemory);
    ADD_COMPONENT(CBotAttack);
    ADD_COMPONENT(CBotDecision); // This component is mandatory!
}

//================================================================================
// Create the schedules that the bot will have
//================================================================================
void CPoneDM_Bot::SetUpSchedules()
{
    ADD_COMPONENT(CHuntEnemySchedule);
    ADD_COMPONENT(CReloadSchedule);
    ADD_COMPONENT(CCoverSchedule);
    ADD_COMPONENT(CHideSchedule);
    ADD_COMPONENT(CChangeWeaponSchedule);
	//this is dumb. 1. it makes the bots super op as it allows us to run somewhere and heal ourselves. 2. there's no healing function.
    //ADD_COMPONENT(CHideAndHealSchedule);
    ADD_COMPONENT(CHideAndReloadSchedule);
    ADD_COMPONENT(CMoveAsideSchedule);
    ADD_COMPONENT(CCallBackupSchedule);
    ADD_COMPONENT(CDefendSpawnSchedule);
}

void CPoneDM_Bot::Spawn()
{
    BaseClass::Spawn();
}

void CPoneDM_Bot::Update()
{
    VPROF_BUDGET("CPoneDM_Bot::Update", VPROF_BUDGETGROUP_BOTS);

    //i should have made this a schedule but when i did it just didn't function so i'm doing this.

    if (GetHost()->IsAlive())
    {
        if (sv_ponedm_bot_ai.GetBool())
        {
            BaseClass::Update();

            if (TheNavAreas.Count() > 0)
            {
                //wander around.
                Vector vecFrom(GetAbsOrigin());
                float dist = sv_ponedm_bot_closestdistancetoplayer.GetFloat();
                CPlayer* closestPlayer = Utils::GetClosestPlayer(GetAbsOrigin(), &dist);

                CNavArea* pArea = closestPlayer->GetLastKnownArea();

                if (pArea == NULL)
                {
                    pArea = TheNavAreas[RandomInt(0, TheNavAreas.Count() - 1)];

                    if (pArea == NULL)
                        return;
                }

                Vector vecGoal(pArea->GetCenter());

                if (!GetLocomotion() || !GetLocomotion()->IsTraversable(vecFrom, vecGoal))
                    return;

                GetLocomotion()->DriveTo("Move to nearest player or randomly roam.", pArea);
            }
            else
            {
                Warning("No nav mesh detected. Kicking %s...\n", GetHost()->GetPlayerName());
                Kick();
                return;
            }
        }
    }
    //respawn if we die.
    else
    {
        if (sv_ponedm_bot_respawn.GetBool())
        {
            respawn(GetHost(), !IsObserver());
            Spawn();
        }
    }
}

extern ConVar bot_team;

CON_COMMAND_F(bot_add, "Adds a specified number of PoneDM bots", FCVAR_SERVER)
{
    SpawnPoneDMBots(Q_atoi(args.Arg(1)));
}

void SpawnPoneDMBots(int botCount)
{
    // Look at -count. Don't include ourselves.
    int count = Clamp(botCount, 1, gpGlobals->maxClients -1);
    // Ok, spawn all the bots.
    while (--count >= 0) {
        CPlayer* pPlayer = CreatePoneDMBot(NULL, NULL, NULL);
        Assert(pPlayer);

        if (!pPlayer || pPlayer == nullptr)
            return;

        if (pPlayer) {
            if (bot_team.GetInt() > 0) {
                pPlayer->ChangeTeam(bot_team.GetInt());
            }

            // pick a random color!
            Vector m_vPrimaryColor = vec3_origin;
            Vector m_vSecondaryColor = vec3_origin;
            Vector m_vTertiaryColor = vec3_origin;

            if (!TheGameRules->IsTeamplay())
            {
                float flColors[3];

                for (int i = 0; i < ARRAYSIZE(flColors); i++)
                    flColors[i] = RandomFloat(0, 255);

                m_vPrimaryColor.Init(flColors[0], flColors[1], flColors[2]);

                m_vPrimaryColor /= 255.0f;

                pPlayer->m_vPrimaryColor = m_vPrimaryColor;
            }

            float flColors2[3];

            for (int i = 0; i < ARRAYSIZE(flColors2); i++)
                flColors2[i] = RandomFloat(0, 255);

            m_vSecondaryColor.Init(flColors2[0], flColors2[1], flColors2[2]);

            m_vSecondaryColor /= 255.0f;

            pPlayer->m_vSecondaryColor = m_vSecondaryColor;

            float flColors3[3];

            for (int i = 0; i < ARRAYSIZE(flColors3); i++)
                flColors3[i] = RandomFloat(0, 255);

            m_vTertiaryColor.Init(flColors3[0], flColors3[1], flColors3[2]);

            m_vTertiaryColor /= 255.0f;

            pPlayer->m_vTertiaryColor = m_vTertiaryColor;

            //pick a random look!

            int iUpperMane = RandomInt(0, pPlayer->GetBodygroupCount(PONEDM_UPPERMANE_BODYGROUP));
            int iLowerMane = RandomInt(0, pPlayer->GetBodygroupCount(PONEDM_LOWERMANE_BODYGROUP));
            int iTail = RandomInt(0, pPlayer->GetBodygroupCount(PONEDM_TAIL_BODYGROUP));

            pPlayer->m_iUpperManeBodygroup = iUpperMane;
            pPlayer->m_iLowerManeBodygroup = iLowerMane;
            pPlayer->m_iTailBodygroup = iTail;

            pPlayer->SetBodygroup(PONEDM_UPPERMANE_BODYGROUP, pPlayer->m_iUpperManeBodygroup);
            pPlayer->SetBodygroup(PONEDM_LOWERMANE_BODYGROUP, pPlayer->m_iLowerManeBodygroup);
            pPlayer->SetBodygroup(PONEDM_TAIL_BODYGROUP, pPlayer->m_iTailBodygroup);
        }
    }
}
