//-----------------------------------------------------------------------------
//								XMifMidConverter.h
//								==================
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 06/11/2003
//-----------------------------------------------------------------------------

#ifndef _XMIFMIDCONVERTER_H
#define _XMIFMIDCONVERTER_H

#include "XBase.h"
#include "XPt2D.h"
#include <vector>

class XGeoBase;
class XGeoClass;
class XGeoVector;

class XMifMidConverter {
protected:
	bool		m_bVisibleOnly;	// Copie des objets visible uniquement
  std::ofstream m_Mif;
  std::ofstream m_Mid;
  XGeoClass* m_Class;

	uint32 MifColor(uint32 c);

public:
  XMifMidConverter() { m_bVisibleOnly = false; m_Class = NULL;}

  bool ConvertBase(XGeoBase* base, const char* folder, XWait* wait = NULL);
	bool ConvertClass(XGeoClass* classe, const char* folder);

	void ConvertVisibleOnly(bool flag) { m_bVisibleOnly = flag;}

  bool WriteMifMidHeader(const char* filename, std::ofstream* mif, std::ofstream* mid);

  bool BeginClass(XGeoClass* classe, const char* folder);
  bool WriteObject(XGeoVector* vector, std::vector<XPt2D>* Pt = NULL, std::vector<int>* Part = NULL);
  bool EndClass();
};


#endif
