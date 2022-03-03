//-----------------------------------------------------------------------------
//								XKmlWriter.h
//								============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 09/09/2009
//-----------------------------------------------------------------------------

#ifndef XKMLWRITER_H
#define XKMLWRITER_H

#include <vector>
#include <fstream>

#include "XPt2D.h"

class XGeoVector;

class XKmlWriter {
protected:
  std::vector<XGeoVector*>  m_V;
  bool                      m_bAttribut;   // Ecriture des attributs

public:
  XKmlWriter(bool attribut = false);

  bool WriteFile(const char* filename);

  void AddVector(XGeoVector* V) { m_V.push_back(V);}
  void Clear() { m_V.clear();}

protected :
  bool WriteVector(std::ofstream* out, XGeoVector* V, uint32 id);
};

#endif // XKMLWRITER_H
