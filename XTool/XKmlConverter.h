//-----------------------------------------------------------------------------
//								XKmlConverter.h
//								===============
//
// Auteur : F.Becirspahic - MODSP / IGN
//
// Date : 1/12/2011
//-----------------------------------------------------------------------------

#ifndef XKMLCONVERTER_H
#define XKMLCONVERTER_H

#include "XBase.h"
#include <vector>

class XGeoBase;
class XGeoClass;
class XGeoVector;

class XKmlConverter {
protected:
  bool		m_bVisibleOnly;	// Copie des objets visible uniquement
  bool		m_bAttribut;    // Copie des objets visible uniquement

public:
  XKmlConverter() {	m_bVisibleOnly = false; m_bAttribut = true;}

  bool ConvertBase(XGeoBase* base, const char* folder, XWait* wait = NULL);
  bool ConvertClass(XGeoClass* classe, const char* folder);
  bool ConvertVector(std::vector<XGeoVector*>* L, const char* filename);

  void ConvertVisibleOnly(bool flag) { m_bVisibleOnly = flag;}
  void ConvertAttribut(bool flag) { m_bAttribut = flag;}

};

#endif // XKMLCONVERTER_H
