//-----------------------------------------------------------------------------
//								XGeoconConverter.h
//								==================
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 09/05/2006
//-----------------------------------------------------------------------------

#ifndef _XGEOCONCONVERTER_H
#define _XGEOCONCONVERTER_H

#include "XBase.h" 

class XGeoBase;
class XGeoClass;

class XGeoconConverter {
protected:
	bool		m_bVisibleOnly;	// Copie des objets visible uniquement

public:
	XGeoconConverter() {	m_bVisibleOnly = false;}

	bool ConvertBase(XGeoBase* base, const char* folder, XWait* wait = NULL);
	bool ConvertClass(XGeoClass* classe, const char* folder);

	void ConvertVisibleOnly(bool flag) { m_bVisibleOnly = flag;}
};

#endif //_XGEOCONCONVERTER_H