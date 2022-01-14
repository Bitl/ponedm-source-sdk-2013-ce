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
#include <vgui_controls/ComboBox.h>
#include <vgui/ILocalize.h>

#include <hl2/hl2_shareddefs.h>

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

	//tertiary
	m_pTertiaryColorRSlider = new CCvarSlider(this, "TertiaryRedSlider", "#GameUI_TertiaryColor_R", 0, 255, "cl_ponedm_tertiarycolor_r", false);

	m_pTertiaryColorRLabel = new TextEntry(this, "TertiaryRedLabel");
	m_pTertiaryColorRLabel->AddActionSignalTarget(this);

	m_pTertiaryColorGSlider = new CCvarSlider(this, "TertiaryGreenSlider", "#GameUI_TertiaryColor_G", 0, 255, "cl_ponedm_tertiarycolor_g", false);

	m_pTertiaryColorGLabel = new TextEntry(this, "TertiaryGreenLabel");
	m_pTertiaryColorGLabel->AddActionSignalTarget(this);

	m_pTertiaryColorBSlider = new CCvarSlider(this, "TertiaryBlueSlider", "#GameUI_TertiaryColor_B", 0, 255, "cl_ponedm_tertiarycolor_b", false);

	m_pTertiaryColorBLabel = new TextEntry(this, "TertiaryBlueLabel");
	m_pTertiaryColorBLabel->AddActionSignalTarget(this);

	//customization
	m_pUpperManeList = new ComboBox(this, "UpperManeList", 12, false);
	m_pLowerManeList = new ComboBox(this, "LowerManeList", 12, false);
	m_pTailList = new ComboBox(this, "TailList", 12, false);

	//model
	m_pPonyModel = new CAdvModelPanel(this, "PonyModelPanel");

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

	//tertiary
	ConVarRef TertiaryRed("cl_ponedm_tertiarycolor_r");
	if (TertiaryRed.IsValid())
	{
		float cl_ponedm_tertiarycolor_r = TertiaryRed.GetFloat();

		char buf[64];
		Q_snprintf(buf, sizeof(buf), " %.1f", cl_ponedm_tertiarycolor_r);
		m_pTertiaryColorRLabel->SetText(buf);
	}
	ConVarRef TertiaryGreen("cl_ponedm_tertiarycolor_g");
	if (TertiaryGreen.IsValid())
	{
		float cl_ponedm_tertiarycolor_g = TertiaryGreen.GetFloat();

		char buf[64];
		Q_snprintf(buf, sizeof(buf), " %.1f", cl_ponedm_tertiarycolor_g);
		m_pTertiaryColorGLabel->SetText(buf);
	}
	ConVarRef TertiaryBlue("cl_ponedm_tertiarycolor_b");
	if (TertiaryBlue.IsValid())
	{
		float cl_ponedm_tertiarycolor_b = TertiaryBlue.GetFloat();

		char buf[64];
		Q_snprintf(buf, sizeof(buf), " %.1f", cl_ponedm_tertiarycolor_b);
		m_pTertiaryColorBLabel->SetText(buf);
	}

	LoadAppearanceOptions();
	SetSizeable(false);
	SetDeleteSelfOnClose(true);
	MoveToCenterOfScreen();
	Update();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CPlayerColorDialog::~CPlayerColorDialog()
{
}

void CPlayerColorDialog::DialogInit()
{
}

void CPlayerColorDialog::LoadAppearanceOptions()
{
	m_pUpperManeList->DeleteAllItems();
	m_pLowerManeList->DeleteAllItems();
	m_pTailList->DeleteAllItems();

	KeyValues* pKV = new KeyValues("UpperManeAppearance");
	if (pKV->LoadFromFile(filesystem, "scripts/appearance_uppermane.txt", "GAME"))
	{
		KeyValues* pNode = pKV->GetFirstSubKey();
		while (pNode)
		{
			const char *itemName = pNode->GetString("name", "");
			const char* itemID = pNode->GetString("id", 0);
			wchar_t text[128];
			wchar_t* tempString = g_pVGuiLocalize->Find(itemName);

			// setup our localized string
			if (tempString)
			{
#ifdef WIN32
				_snwprintf(text, sizeof(text) / sizeof(wchar_t) - 1, L"%s", tempString);
#else
				_snwprintf(text, sizeof(text) / sizeof(wchar_t) - 1, L"%S", tempString);
#endif
				text[sizeof(text) / sizeof(wchar_t) - 1] = 0;
			}
			else
			{
				// string wasn't found by g_pVGuiLocalize->Find()
				g_pVGuiLocalize->ConvertANSIToUnicode(itemName, text, sizeof(text));
			}

			m_pUpperManeList->AddItem(text, new KeyValues("data", "id", itemID));

			pNode = pNode->GetNextKey();
		}
	}

	pKV->deleteThis();

	KeyValues* pKV2 = new KeyValues("LowerManeAppearance");
	if (pKV2->LoadFromFile(filesystem, "scripts/appearance_lowermane.txt", "GAME"))
	{
		KeyValues* pNode = pKV2->GetFirstSubKey();
		while (pNode)
		{
			const char* itemName = pNode->GetString("name", "");
			const char* itemID = pNode->GetString("id", 0);

			wchar_t text[128];
			wchar_t* tempString = g_pVGuiLocalize->Find(itemName);

			// setup our localized string
			if (tempString)
			{
#ifdef WIN32
				_snwprintf(text, sizeof(text) / sizeof(wchar_t) - 1, L"%s", tempString);
#else
				_snwprintf(text, sizeof(text) / sizeof(wchar_t) - 1, L"%S", tempString);
#endif
				text[sizeof(text) / sizeof(wchar_t) - 1] = 0;
			}
			else
			{
				// string wasn't found by g_pVGuiLocalize->Find()
				g_pVGuiLocalize->ConvertANSIToUnicode(itemName, text, sizeof(text));
			}

			m_pLowerManeList->AddItem(text, new KeyValues("data", "id", itemID));

			pNode = pNode->GetNextKey();
		}
	}

	pKV2->deleteThis();

	KeyValues* pKV3 = new KeyValues("TailAppearance");
	if (pKV3->LoadFromFile(filesystem, "scripts/appearance_tail.txt", "GAME"))
	{
		KeyValues* pNode = pKV3->GetFirstSubKey();
		while (pNode)
		{
			const char* itemName = pNode->GetString("name", "");
			const char* itemID = pNode->GetString("id", 0);

			wchar_t text[128];
			wchar_t* tempString = g_pVGuiLocalize->Find(itemName);

			// setup our localized string
			if (tempString)
			{
#ifdef WIN32
				_snwprintf(text, sizeof(text) / sizeof(wchar_t) - 1, L"%s", tempString);
#else
				_snwprintf(text, sizeof(text) / sizeof(wchar_t) - 1, L"%S", tempString);
#endif
				text[sizeof(text) / sizeof(wchar_t) - 1] = 0;
			}
			else
			{
				// string wasn't found by g_pVGuiLocalize->Find()
				g_pVGuiLocalize->ConvertANSIToUnicode(itemName, text, sizeof(text));
			}

			m_pTailList->AddItem(text, new KeyValues("data", "id", itemID));

			pNode = pNode->GetNextKey();
		}
	}

	pKV3->deleteThis();

	ConVarRef upperMane("cl_ponedm_uppermane");
	ConVarRef lowerMane("cl_ponedm_lowermane");
	ConVarRef tail("cl_ponedm_tail");
	m_pUpperManeList->ActivateItem(upperMane.GetInt());
	m_pLowerManeList->ActivateItem(lowerMane.GetInt());
	m_pTailList->ActivateItem(tail.GetInt());
}

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

	m_pTertiaryColorRSlider->ApplyChanges();
	m_pTertiaryColorGSlider->ApplyChanges();
	m_pTertiaryColorBSlider->ApplyChanges();

	ConVarRef upperMane("cl_ponedm_uppermane");
	ConVarRef lowerMane("cl_ponedm_lowermane");
	ConVarRef tail("cl_ponedm_tail");

	int selectedUpperMane = m_pUpperManeList->GetActiveItemUserData()->GetInt("id");
	upperMane.SetValue(selectedUpperMane);

	int selectedLowerMane = m_pLowerManeList->GetActiveItemUserData()->GetInt("id");
	lowerMane.SetValue(selectedLowerMane);

	int selectedTail = m_pTailList->GetActiveItemUserData()->GetInt("id");
	tail.SetValue(selectedTail);
	
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

	if (panel == m_pTertiaryColorRSlider && m_pTertiaryColorRSlider->HasBeenModified())
	{
		char buf[64];
		Q_snprintf(buf, sizeof(buf), " %.1f", m_pTertiaryColorRSlider->GetSliderValue());
		m_pTertiaryColorRLabel->SetText(buf);
		m_pTertiaryColorRSlider->ApplyChanges();
	}
	else if (panel == m_pTertiaryColorGSlider && m_pTertiaryColorGSlider->HasBeenModified())
	{
		char buf[64];
		Q_snprintf(buf, sizeof(buf), " %.1f", m_pTertiaryColorGSlider->GetSliderValue());
		m_pTertiaryColorGLabel->SetText(buf);
		m_pTertiaryColorGSlider->ApplyChanges();
	}
	else if (panel == m_pTertiaryColorBSlider && m_pTertiaryColorBSlider->HasBeenModified())
	{
		char buf[64];
		Q_snprintf(buf, sizeof(buf), " %.1f", m_pTertiaryColorBSlider->GetSliderValue());
		m_pTertiaryColorBLabel->SetText(buf);
		m_pTertiaryColorBSlider->ApplyChanges();
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
			m_pPrimaryColorRSlider->ApplyChanges();
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
			m_pPrimaryColorGSlider->ApplyChanges();
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
			m_pPrimaryColorBSlider->ApplyChanges();
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
			m_pSecondaryColorRSlider->ApplyChanges();
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
			m_pSecondaryColorGSlider->ApplyChanges();
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
			m_pSecondaryColorBSlider->ApplyChanges();
		}
	}

	if (panel == m_pTertiaryColorRLabel)
	{
		char buf[64];
		m_pTertiaryColorRLabel->GetText(buf, 64);

		float fValue = (float)atof(buf);
		if (fValue >= 0.0f && fValue <= 255.0f)
		{
			m_pTertiaryColorRSlider->SetSliderValue(fValue);
			m_pTertiaryColorRSlider->ApplyChanges();
		}
	}
	else if (panel == m_pTertiaryColorGLabel)
	{
		char buf[64];
		m_pTertiaryColorGLabel->GetText(buf, 64);

		float fValue = (float)atof(buf);
		if (fValue >= 0.0f && fValue <= 255.0f)
		{
			m_pTertiaryColorGSlider->SetSliderValue(fValue);
			m_pTertiaryColorGSlider->ApplyChanges();
		}
	}
	else if (panel == m_pTertiaryColorBLabel)
	{
		char buf[64];
		m_pTertiaryColorBLabel->GetText(buf, 64);

		float fValue = (float)atof(buf);
		if (fValue >= 0.0f && fValue <= 255.0f)
		{
			m_pTertiaryColorBSlider->SetSliderValue(fValue);
			m_pTertiaryColorBSlider->ApplyChanges();
		}
	}

	ConVarRef upperMane("cl_ponedm_uppermane");
	ConVarRef lowerMane("cl_ponedm_lowermane");
	ConVarRef tail("cl_ponedm_tail");

	if (panel == m_pUpperManeList)
	{
		int selectedUpperMane = m_pUpperManeList->GetActiveItemUserData()->GetInt("id");
		upperMane.SetValue(selectedUpperMane);
		Update();
	}
	else if (panel == m_pLowerManeList)
	{
		int selectedLowerMane = m_pLowerManeList->GetActiveItemUserData()->GetInt("id");
		lowerMane.SetValue(selectedLowerMane);
		Update();
	}
	else if (panel == m_pTailList)
	{
		int selectedTail = m_pTailList->GetActiveItemUserData()->GetInt("id");
		tail.SetValue(selectedTail);
		Update();
	}
}

void CPlayerColorDialog::Update(void)
{
	if (m_pPonyModel)
	{
		Vector origpos;
		QAngle origang;
		m_pPonyModel->GetCameraPositionAndAngles(origpos, origang);
		m_pPonyModel->SetMergeMDL("models/weapons/w_crowbar.mdl", NULL, 0);

		//HACK.
		ConVarRef upperMane("cl_ponedm_uppermane");
		ConVarRef lowerMane("cl_ponedm_lowermane");
		ConVarRef tail("cl_ponedm_tail");
		m_pPonyModel->SetBodygroup(PONEDM_UPPERMANE_BODYGROUP, upperMane.GetInt());
		m_pPonyModel->SetBodygroup(PONEDM_LOWERMANE_BODYGROUP, lowerMane.GetInt());
		m_pPonyModel->SetBodygroup(PONEDM_TAIL_BODYGROUP, tail.GetInt());

		m_pPonyModel->Update();
		m_pPonyModel->SetCameraPositionAndAngles(origpos, origang);
	}
}