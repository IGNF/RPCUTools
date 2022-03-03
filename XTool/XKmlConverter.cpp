//-----------------------------------------------------------------------------
//								XKmlConverter.cpp
//								=================
//
// Auteur : F.Becirspahic - MODSP / IGN
//
// Date : 1/12/2011
//-----------------------------------------------------------------------------

#include "XKmlConverter.h"
#include "XKmlWriter.h"
#include "XGeoBase.h"
#include "XGeoLayer.h"
#include "XGeoClass.h"
#include "XGeoVector.h"

//-----------------------------------------------------------------------------
// Conversion d'une base
//-----------------------------------------------------------------------------
bool XKmlConverter::ConvertBase(XGeoBase* base, const char* folder, XWait* wait)
{
  std::string layer_folder;
  uint32 nb_class = base->NbClass();
  XWaitRange(wait, 0, nb_class);
  for (uint32 i = 0; i < base->NbLayer(); i++) {
    XGeoLayer* layer = base->Layer(i);
    if (!layer->Visible())
      continue;
    layer_folder = (std::string)folder + "/" + layer->Name();
    for (uint32 j = 0; j < layer->NbClass(); j++) {
      XGeoClass* classe = layer->Class(j);
      XWaitStatus(wait, ("Export de la classe " + classe->Name()).c_str());
      if ((!m_bVisibleOnly)||(classe->Visible()))
        ConvertClass(classe, layer_folder.c_str());
      XWaitStepIt(wait);
      if (XWaitCheckCancel(wait))
        return false;
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
// Conversion d'une classe
//-----------------------------------------------------------------------------
bool XKmlConverter::ConvertClass(XGeoClass* classe, const char* folder)
{
  XKmlWriter writer(m_bAttribut);
  XGeoVector* V;
  std::string filename = (std::string)folder + "/" + classe->Name() + ".kml";

  for (uint32 i = 0; i < classe->NbVector(); i++) {
    V = classe->Vector(i);
    if ((m_bVisibleOnly)&&(!V->Visible()))
      continue;
    writer.AddVector(V);
  }
  return writer.WriteFile(filename.c_str());
}

//-----------------------------------------------------------------------------
// Conversion d'une liste de vecteurs
//-----------------------------------------------------------------------------
bool XKmlConverter::ConvertVector(std::vector<XGeoVector*>* L, const char* filename)
{
  XKmlWriter writer(m_bAttribut);
  XGeoVector* V;

  for (uint32 i = 0; i < L->size(); i++) {
    V = (*L)[i];
    if ((m_bVisibleOnly)&&(!V->Visible()))
      continue;
    writer.AddVector(V);
  }
  return writer.WriteFile(filename);
}
