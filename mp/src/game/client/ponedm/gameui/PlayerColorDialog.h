//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef PLAYERCOLORDIALOG_H
#define PLAYERCOLORDIALOG_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Frame.h>
#include <ponedm/gameui/advmodelpanel.h>

class CCvarSlider;

//-----------------------------------------------------------------------------
// Purpose: dialog for launching a listenserver
//-----------------------------------------------------------------------------
class CPlayerColorDialog : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE( CPlayerColorDialog,  vgui::Frame );

public:
	CPlayerColorDialog(vgui::Panel *parent);
	~CPlayerColorDialog();
	
	// returns currently entered information about the server
	void DialogInit();
	void LoadAppearanceOptions();
	void Update();
	KeyValues *LoadAppearanceFile(const char *kvName, const char* scriptPath, vgui::ComboBox* comboBox);
	virtual void OnClose();
	virtual void OnKeyCodeTyped(vgui::KeyCode code);
	MESSAGE_FUNC_PTR(OnControlModified, "ControlModified", panel);
	MESSAGE_FUNC_PTR(OnTextChanged, "TextChanged", panel);

private:
	CCvarSlider			*m_pPrimaryColorRSlider;
    vgui::TextEntry		*m_pPrimaryColorRLabel;
	CCvarSlider			*m_pPrimaryColorGSlider;
	vgui::TextEntry		*m_pPrimaryColorGLabel;
	CCvarSlider			*m_pPrimaryColorBSlider;
	vgui::TextEntry		*m_pPrimaryColorBLabel;

	CCvarSlider			*m_pSecondaryColorRSlider;
    vgui::TextEntry		*m_pSecondaryColorRLabel;
	CCvarSlider			*m_pSecondaryColorGSlider;
	vgui::TextEntry		*m_pSecondaryColorGLabel;
	CCvarSlider			*m_pSecondaryColorBSlider;
	vgui::TextEntry		*m_pSecondaryColorBLabel;

	CCvarSlider			*m_pTertiaryColorRSlider;
	vgui::TextEntry		*m_pTertiaryColorRLabel;
	CCvarSlider			*m_pTertiaryColorGSlider;
	vgui::TextEntry		*m_pTertiaryColorGLabel;
	CCvarSlider			*m_pTertiaryColorBSlider;
	vgui::TextEntry		*m_pTertiaryColorBLabel;

	vgui::ComboBox		*m_pUpperManeList;
	vgui::ComboBox		*m_pLowerManeList;
	vgui::ComboBox		*m_pTailList;
	vgui::ComboBox		*m_pHornList;
	vgui::ComboBox		*m_pWingsList;

	CAdvModelPanel	*m_pPonyModel;
};
#endif
