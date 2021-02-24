//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "PlayerColorDialog.h"

#include <stdio.h>

using namespace vgui;

#include <vgui/ILocalize.h>
#include "filesystem.h"
#include <KeyValues.h>
#include "CvarSlider.h"
#include "EngineInterface.h"
#include "tier1/convar.h"
#include <vgui_controls/TextEntry.h>

// for SRC
#include <vstdlib/random.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

void LoadCommand(void)
{
	CPlayerColorDialog* pCPlayerColorDialog = new CPlayerColorDialog(NULL);
	pCPlayerColorDialog->Activate();
}

ConCommand PlayerColorDialog("PlayerColorDialog", LoadCommand, "", FCVAR_HIDDEN);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CPlayerColorDialog::CPlayerColorDialog(vgui::Panel *parent) : BaseClass(NULL, "PlayerColorDialog")
{
	SetSize(348, 460);
	
	//primary
	m_pPrimaryColorRSlider = new CCvarSlider( this, "PrimaryRedSlider", "#GameUI_PrimaryColor_R", 0, 255, "cl_ponedm_primarycolor_r", false );

    m_pPrimaryColorRLabel = new TextEntry(this, "PrimaryRedLabel");
    m_pPrimaryColorRLabel->AddActionSignalTarget(this);

	m_pPrimaryColorGSlider = new CCvarSlider(this, "PrimaryGreenSlider", "#GameUI_PrimaryColor_G", 0, 255, "cl_ponedm_primarycolor_g", false);

	m_pPrimaryColorGLabel = new TextEntry(this, "PrimaryGreenLabel");
	m_pPrimaryColorGLabel->AddActionSignalTarget(this);

	m_pPrimaryColorBSlider = new CCvarSlider(this, "PrimaryBlueSlider", "#GameUI_PrimaryColor_B", 0, 255, "cl_ponedm_primarycolor_b", false);

	m_pPrimaryColorBLabel = new TextEntry(this, "PrimaryBlueLabel");
	m_pPrimaryColorBLabel->AddActionSignalTarget(this);

	//secondary
	m_pSecondaryColorRSlider = new CCvarSlider(this, "SecondaryRedSlider", "#GameUI_SecondaryColor_R", 0, 255, "cl_ponedm_secondarycolor_r", false);

	m_pSecondaryColorRLabel = new TextEntry(this, "SecondaryRedLabel");
	m_pSecondaryColorRLabel->AddActionSignalTarget(this);

	m_pSecondaryColorGSlider = new CCvarSlider(this, "SecondaryGreenSlider", "#GameUI_SecondaryColor_G", 0, 255, "cl_ponedm_secondarycolor_g", false);

	m_pSecondaryColorGLabel = new TextEntry(this, "SecondaryGreenLabel");
	m_pSecondaryColorGLabel->AddActionSignalTarget(this);

	m_pSecondaryColorBSlider = new CCvarSlider(this, "SecondaryBlueSlider", "#GameUI_SecondaryColor_B", 0, 255, "cl_ponedm_secondarycolor_b", false);

	m_pSecondaryColorBLabel = new TextEntry(this, "SecondaryBlueLabel");
	m_pSecondaryColorBLabel->AddActionSignalTarget(this);

	LoadControlSettings("Resource/PlayerColorDialog.res");
	
	//primary
	ConVarRef primaryRed( "cl_ponedm_primarycolor_r" );
	if ( primaryRed.IsValid() )
	{
		float cl_ponedm_primarycolor_r = primaryRed.GetFloat();

		char buf[64];
		Q_snprintf(buf, sizeof(buf), " %.1f", cl_ponedm_primarycolor_r);
		m_pPrimaryColorRLabel->SetText(buf);
	}
	ConVarRef primaryGreen("cl_ponedm_primarycolor_g");
	if (primaryGreen.IsValid())
	{
		float cl_ponedm_primarycolor_g = primaryGreen.GetFloat();

		char buf[64];
		Q_snprintf(buf, sizeof(buf), " %.1f", cl_ponedm_primarycolor_g);
		m_pPrimaryColorGLabel->SetText(buf);
	}
	ConVarRef primaryBlue("cl_ponedm_primarycolor_b");
	if (primaryBlue.IsValid())
	{
		float cl_ponedm_primarycolor_b = primaryBlue.GetFloat();

		char buf[64];
		Q_snprintf(buf, sizeof(buf), " %.1f", cl_ponedm_primarycolor_b);
		m_pPrimaryColorBLabel->SetText(buf);
	}

	//secondary
	ConVarRef SecondaryRed("cl_ponedm_secondarycolor_r");
	if (SecondaryRed.IsValid())
	{
		float cl_ponedm_primarycolor_r = SecondaryRed.GetFloat();

		char buf[64];
		Q_snprintf(buf, sizeof(buf), " %.1f", cl_ponedm_primarycolor_r);
		m_pSecondaryColorRLabel->SetText(buf);
	}
	ConVarRef SecondaryGreen("cl_ponedm_secondarycolor_g");
	if (SecondaryGreen.IsValid())
	{
		float cl_ponedm_primarycolor_g = SecondaryGreen.GetFloat();

		char buf[64];
		Q_snprintf(buf, sizeof(buf), " %.1f", cl_ponedm_primarycolor_g);
		m_pSecondaryColorGLabel->SetText(buf);
	}
	ConVarRef SecondaryBlue("cl_ponedm_secondarycolor_b");
	if (SecondaryBlue.IsValid())
	{
		float cl_ponedm_primarycolor_b = SecondaryBlue.GetFloat();

		char buf[64];
		Q_snprintf(buf, sizeof(buf), " %.1f", cl_ponedm_primarycolor_b);
		m_pSecondaryColorBLabel->SetText(buf);
	}
	
	SetSizeable(false);
	SetDeleteSelfOnClose(true);
	MoveToCenterOfScreen();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CPlayerColorDialog::~CPlayerColorDialog()
{
}

void CPlayerColorDialog::DialogInit()
{
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPlayerColorDialog::OnClose()
{
	m_pPrimaryColorRSlider->ApplyChanges();
	m_pPrimaryColorGSlider->ApplyChanges();
	m_pPrimaryColorBSlider->ApplyChanges();

	m_pSecondaryColorRSlider->ApplyChanges();
	m_pSecondaryColorGSlider->ApplyChanges();
	m_pSecondaryColorBSlider->ApplyChanges();
	
	BaseClass::OnClose();
	MarkForDeletion();
}

void CPlayerColorDialog::OnKeyCodeTyped(KeyCode code)
{
	// force ourselves to be closed if the escape key it pressed
	if (code == KEY_ESCAPE)
	{
		Close();
	}
	else
	{
		BaseClass::OnKeyCodeTyped(code);
	}
}

void CPlayerColorDialog::OnControlModified(Panel *panel)
{
    if (panel == m_pPrimaryColorRSlider && m_pPrimaryColorRSlider->HasBeenModified())
    {
        char buf[64];
		Q_snprintf(buf, sizeof( buf ), " %.1f", m_pPrimaryColorRSlider->GetSliderValue());
		m_pPrimaryColorRLabel->SetText(buf);
		m_pPrimaryColorRSlider->ApplyChanges();
    }
	else if (panel == m_pPrimaryColorGSlider && m_pPrimaryColorGSlider->HasBeenModified())
	{
		char buf[64];
		Q_snprintf(buf, sizeof(buf), " %.1f", m_pPrimaryColorGSlider->GetSliderValue());
		m_pPrimaryColorGLabel->SetText(buf);
		m_pPrimaryColorGSlider->ApplyChanges();
	}
	else if (panel == m_pPrimaryColorBSlider && m_pPrimaryColorBSlider->HasBeenModified())
	{
		char buf[64];
		Q_snprintf(buf, sizeof(buf), " %.1f", m_pPrimaryColorBSlider->GetSliderValue());
		m_pPrimaryColorBLabel->SetText(buf);
		m_pPrimaryColorBSlider->ApplyChanges();
	}

	if (panel == m_pSecondaryColorRSlider && m_pSecondaryColorRSlider->HasBeenModified())
	{
		char buf[64];
		Q_snprintf(buf, sizeof(buf), " %.1f", m_pSecondaryColorRSlider->GetSliderValue());
		m_pSecondaryColorRLabel->SetText(buf);
		m_pSecondaryColorRSlider->ApplyChanges();
	}
	else if (panel == m_pSecondaryColorGSlider && m_pSecondaryColorGSlider->HasBeenModified())
	{
		char buf[64];
		Q_snprintf(buf, sizeof(buf), " %.1f", m_pSecondaryColorGSlider->GetSliderValue());
		m_pSecondaryColorGLabel->SetText(buf);
		m_pSecondaryColorGSlider->ApplyChanges();
	}
	else if (panel == m_pSecondaryColorBSlider && m_pSecondaryColorBSlider->HasBeenModified())
	{
		char buf[64];
		Q_snprintf(buf, sizeof(buf), " %.1f", m_pSecondaryColorBSlider->GetSliderValue());
		m_pSecondaryColorBLabel->SetText(buf);
		m_pSecondaryColorBSlider->ApplyChanges();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPlayerColorDialog::OnTextChanged(Panel *panel)
{
    if (panel == m_pPrimaryColorRLabel)
    {
        char buf[64];
        m_pPrimaryColorRLabel->GetText(buf, 64);

        float fValue = (float) atof(buf);
		if (fValue >= 0.0f && fValue <= 255.0f)
		{
            m_pPrimaryColorRSlider->SetSliderValue(fValue);
        }
    }
	else if (panel == m_pPrimaryColorGLabel)
	{
		char buf[64];
		m_pPrimaryColorGLabel->GetText(buf, 64);

		float fValue = (float)atof(buf);
		if (fValue >= 0.0f && fValue <= 255.0f)
		{
			m_pPrimaryColorGSlider->SetSliderValue(fValue);
		}
	}
	else if (panel == m_pPrimaryColorBLabel)
	{
		char buf[64];
		m_pPrimaryColorBLabel->GetText(buf, 64);

		float fValue = (float)atof(buf);
		if (fValue >= 0.0f && fValue <= 255.0f)
		{
			m_pPrimaryColorBSlider->SetSliderValue(fValue);
		}
	}

	if (panel == m_pSecondaryColorRLabel)
	{
		char buf[64];
		m_pSecondaryColorRLabel->GetText(buf, 64);

		float fValue = (float)atof(buf);
		if (fValue >= 0.0f && fValue <= 255.0f)
		{
			m_pSecondaryColorRSlider->SetSliderValue(fValue);
		}
	}
	else if (panel == m_pSecondaryColorGLabel)
	{
		char buf[64];
		m_pSecondaryColorGLabel->GetText(buf, 64);

		float fValue = (float)atof(buf);
		if (fValue >= 0.0f && fValue <= 255.0f)
		{
			m_pSecondaryColorGSlider->SetSliderValue(fValue);
		}
	}
	else if (panel == m_pSecondaryColorBLabel)
	{
		char buf[64];
		m_pSecondaryColorBLabel->GetText(buf, 64);

		float fValue = (float)atof(buf);
		if (fValue >= 0.0f && fValue <= 255.0f)
		{
			m_pSecondaryColorBSlider->SetSliderValue(fValue);
		}
	}
}