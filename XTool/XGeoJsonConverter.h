//-----------------------------------------------------------------------------
//								XGeoJsonConverter.h
//								===================
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 11/06/2019
//-----------------------------------------------------------------------------

#ifndef _XGEOJSONCONVERTER_H
#define _XGEOJSONCONVERTER_H

#include "XBase.h"

class XGeoBase;
class XGeoClass;
class XGeoVector;

class XGeoJsonConverter {
protected:
	bool		m_bVisibleOnly;	// Copie des objets visible uniquement

public:
	XGeoJsonConverter() { m_bVisibleOnly = false;}

	bool ConvertBase(XGeoBase* base, const char* folder, XWait* wait = NULL);
	bool ConvertClass(XGeoClass* classe, const char* folder);
	bool ConvertVector(XGeoVector* V, std::ostream* json);

	void ConvertVisibleOnly(bool flag) { m_bVisibleOnly = flag;}
};

#endif // XGEOJSONCONVERTER_H
