#include "cbase.h"
#include "basemodelpanel.h"
#include "advmodelpanel.h"
#include "renderparm.h"
#include "animation.h"

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

DECLARE_BUILD_FACTORY( CAdvModelPanel );

static ConVar cl_advpanel_design("cl_advpanel_design", "0", FCVAR_DEVELOPMENTONLY, "");

CAdvModelPanel::CAdvModelPanel(vgui::Panel *parent, const char *name) : CBaseModelPanel(parent, name)
{
	SetParent(parent);
	SetScheme("ClientScheme");
	SetProportional(true);
	SetVisible(true);
	m_bShouldPaint = true;
	m_bAutoRotate = false;
	m_iAnimationIndex = 0;
	m_pStudioHdr = NULL;
	m_pData = NULL;
}

CAdvModelPanel::~CAdvModelPanel()
{

}

//-----------------------------------------------------------------------------
// Purpose: Load in the model portion of the panel's resource file.
//-----------------------------------------------------------------------------
void CAdvModelPanel::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	m_bAutoRotate = inResourceData->GetBool("autorotate", false);
}


void CAdvModelPanel::PerformLayout()
{
	BaseClass::PerformLayout();
}


void CAdvModelPanel::SetModelName(const char* name, int skin)
{
	m_BMPResData.m_pszModelName = name;
	m_BMPResData.m_nSkin = skin;
}

void CAdvModelPanel::SetParticleName(const char* name)
{
	m_bUseParticle = true;

	if (m_pData)
	{
		SafeDeleteParticleData(&m_pData);
	}
	m_pData = CreateParticleData(name);

	// We failed at creating that particle for whatever reason, bail (!)
	if (!m_pData) return;

	studiohdr_t *pStudioHdr = m_RootMDL.m_MDL.GetStudioHdr();
	if (!pStudioHdr)
		return;

	CStudioHdr studioHdr(pStudioHdr, g_pMDLCache);
	CUtlVector<int> vecAttachments;

	m_pData->UpdateControlPoints(&studioHdr, &m_RootMDL.m_MDLToWorld, vecAttachments);
	m_pData->m_bIsUpdateToDate = true;
}


void CAdvModelPanel::Update()
{
	if (!cl_advpanel_design.GetBool())
	{
		MDLHandle_t hSelectedMDL = g_pMDLCache->FindMDL(m_BMPResData.m_pszModelName);
		g_pMDLCache->PreloadModel(hSelectedMDL);
		SetMDL(hSelectedMDL);

		if (m_iAnimationIndex < m_BMPResData.m_aAnimations.Size())
		{
			SetModelAnim(m_iAnimationIndex);
		}

		SetSkin(m_BMPResData.m_nSkin);
	}
}

void CAdvModelPanel::OnThink()
{
	BaseClass::OnThink();

	if (m_bAutoRotate)
	{
		Vector vecPos = Vector(-275.0, 0.0, 170.0);
		QAngle angRot = QAngle(32.0, 0.0, 0.0);
		//vecPos.z += pWeaponData->m_flModelPanelZOffset;

		Vector vecBoundsMins, vecBoundsMax;
		GetBoundingBox(vecBoundsMins, vecBoundsMax);
		int iMaxBounds = -vecBoundsMins.x + vecBoundsMax.x;
		iMaxBounds = MAX(iMaxBounds, -vecBoundsMins.y + vecBoundsMax.y);
		iMaxBounds = MAX(iMaxBounds, -vecBoundsMins.z + vecBoundsMax.z);
		vecPos *= (float)iMaxBounds / 64.0f;

		SetCameraPositionAndAngles(vecPos, angRot);
		SetModelAnglesAndPosition(m_BMPResData.m_angModelPoseRot + QAngle(0.0f, gpGlobals->curtime * 45.0f, 0.0f), m_BMPResData.m_vecOriginOffset);
	}
}


void CAdvModelPanel::Paint()
{
	CMatRenderContextPtr pRenderContext( materials );

	// Turn off depth-write to dest alpha so that we get white there instead. The code that uses
	// the render target needs a mask of where stuff was rendered.
	pRenderContext->SetIntRenderingParameter( INT_RENDERPARM_WRITE_DEPTH_TO_DESTALPHA, false );

	// Disable flashlights when drawing our model
	pRenderContext->SetFlashlightMode(false);

	if ( m_bShouldPaint )
	{
		SetupLights();

		BaseClass::Paint();
	}
}

void CAdvModelPanel::SetBodygroup( int iGroup, int iValue )
{
	studiohdr_t *pStudioHdr = m_RootMDL.m_MDL.GetStudioHdr();
	if ( !pStudioHdr )
		return;

	CStudioHdr studioHdr( pStudioHdr, g_pMDLCache );

	::SetBodygroup( &studioHdr, m_RootMDL.m_MDL.m_nBody, iGroup, iValue );
}


int CAdvModelPanel::FindBodygroupByName( const char *name )
{
	studiohdr_t *pStudioHdr = m_RootMDL.m_MDL.GetStudioHdr();
	if ( !pStudioHdr )
		return -1;

	CStudioHdr studioHdr( pStudioHdr, g_pMDLCache );

	return ::FindBodygroupByName( &studioHdr, name );
}

int CAdvModelPanel::GetNumBodyGroups( void )
{
	studiohdr_t *pStudioHdr = m_RootMDL.m_MDL.GetStudioHdr();
	if ( !pStudioHdr )
		return 0;

	CStudioHdr studioHdr( pStudioHdr, g_pMDLCache );

	return ::GetNumBodyGroups( &studioHdr );
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAdvModelPanel::PostPaint3D(IMatRenderContext *pRenderContext)
{
	if (!m_bUseParticle)
		return;

	// This needs calling to reset various counters.
	g_pParticleSystemMgr->SetLastSimulationTime(gpGlobals->curtime);

	// Render Particles
	pRenderContext->MatrixMode(MATERIAL_MODEL);
	pRenderContext->PushMatrix();
	pRenderContext->LoadIdentity();

	FOR_EACH_VEC(m_particleList, i)
	{
		m_particleList[i]->m_pParticleSystem->Simulate(gpGlobals->frametime, false);
		m_particleList[i]->m_pParticleSystem->Render(pRenderContext);
		m_particleList[i]->m_bIsUpdateToDate = false;
	}

	pRenderContext->MatrixMode(MATERIAL_MODEL);
	pRenderContext->PopMatrix();
}