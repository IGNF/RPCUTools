//-----------------------------------------------------------------------------
//								XMifMidConverter.cpp
//								====================
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 06/11/2003
//-----------------------------------------------------------------------------

#include "XMifMidConverter.h"
#include "XGeoBase.h"
#include "XGeoLayer.h"
#include "XGeoVector.h"
#include "XGeoPref.h"
#include "XPath.h"

//-----------------------------------------------------------------------------
// Conversion d'une base
//-----------------------------------------------------------------------------
bool XMifMidConverter::ConvertBase(XGeoBase* base, const char* folder, XWait* wait)
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
bool XMifMidConverter::ConvertClass(XGeoClass* classe, const char* folder)
{
	XGeoVector* vector;
	XGeoRepres* repres;
	std::vector<std::string> V;
	XPt2D P;
  std::string filename = (std::string)folder + "/" + classe->Name();

	std::ofstream mif, mid;
	mif.open((filename + ".mif").c_str());
	mid.open((filename + ".mid").c_str());
	if ((!mid.good())||(!mif.good()))
		return false;

	XGeoPref pref;
	mif.setf(std::ios::fixed);
	if (pref.Projection() == XGeoProjection::RGF93)
		mif.precision(6);
	else
		mif.precision(2);

	// Entete du MIF
	mif << "VERSION 300" << std::endl;
	mif << "Charset \"WindowsLatin1\"" << std::endl;
	mif << XGeoProjection::MifProjection(pref.Projection()) << std::endl;

	if (classe->NbVector() < 1)
		return true;

	// Ecriture des colonnes
	uint32 i, nb_att;
	bool label = false;
	XGeoSchema* schema = classe->Schema();
	if (schema->NbAttribut() < 1) {
		vector = classe->Vector((uint32)0);
		vector->ReadAttributes(V);
		nb_att = V.size() / 2;
		if (nb_att > 0) {
			mif << "COLUMNS " << nb_att << std::endl;
			for (i = 0; i < nb_att; i++)
				mif << V[i* 2] << " char (40)" << std::endl;
		} else {
			if (vector->Name().size() > 0) {
				mif << "COLUMNS " << "2" << std::endl;
				mif << "NOM" << " char (40)" << std::endl;
				mif << "IMPORTANCE" << " smallint" << std::endl;
				label = true;
			} else
				mif << "COLUMNS " << "0" << std::endl;
		}

	} else {
		nb_att = schema->NbAttribut();
		mif << "COLUMNS " << nb_att << std::endl;
		for (i = 0; i < nb_att; i++) {
			mif << schema->AttributShortName(i);
			switch(schema->AttributType(i)) {
				case XGeoAttribut::Bool : mif << " char (20)"; break;
				case XGeoAttribut::Int16 : mif << " smallint"; break;
				case XGeoAttribut::Int32 : mif << " integer"; break;
				case XGeoAttribut::Double : mif << " decimal (10,2)"; break;
				case XGeoAttribut::String : mif << " char (" << schema->AttributLength(i) << ")"; break;
				case XGeoAttribut::List : mif << " char (40)"; break;
				case XGeoAttribut::NumericN : mif << " decimal (" << schema->AttributLength(i) << "," 
																					<< schema->AttributDecCount(i) << ")"; break;
				case XGeoAttribut::NumericF : mif << " decimal (" << schema->AttributLength(i) << ","
																					<< schema->AttributDecCount(i) << ")"; break;
			}
			mif << std::endl;
		}
	}
	
	// Ecriture des donnees
	mif << "DATA" << std::endl;
	for (i = 0; i < classe->NbVector(); i++) {
		vector = classe->Vector(i);
		if (m_bVisibleOnly)
			if (!vector->Visible())
				continue;
		vector->LoadGeom();
		V.clear();
		vector->ReadAttributes(V);
		repres = vector->Repres();

		if (schema->NbAttribut() > 0)
			schema->SortAttribut(&V);
		for (uint32 i = 0; i < nb_att; i++){
			mid << V[i* 2 + 1];
			if (i == nb_att - 1)
				mid << std::endl;
			else
				mid << "\t";
		}
		if (label) 
				mid << vector->Name() << "\t" << vector->Importance() << std::endl;

		if (vector->NbPt() < 2){
			mif << "POINT " << vector->X0() << " " << vector->Y0() << std::endl;
			mif << "\tSYMBOL (" << repres->Symbol() << ", " << MifColor(repres->Color()) << ", "
				<< (int)repres->Size() << ")" << std::endl;
			continue;
		}
			
		if (vector->IsClosed())
			mif << "REGION " << vector->NbPart() << std::endl;
		else {
			if (vector->NbPart() > 1)
				mif << "PLINE MULTIPLE " << vector->NbPart() << std::endl;
			else
				mif << "PLINE" << std::endl;
		}
			
		for (uint32 j = 0; j < vector->NbPart() - 1; j++) {
			mif << vector->Part(j + 1) - vector->Part(j) << std::endl;
			for (uint32 k = vector->Part(j); k < vector->Part(j + 1); k++) {
				P = vector->Pt(k);
				mif << P.X << " " << P.Y << std::endl;
			}
		}
		mif << vector->NbPt() - vector->Part(vector->NbPart() - 1) << std::endl;
		for (uint32 k = vector->Part(vector->NbPart() - 1); k < vector->NbPt(); k++) {
			P = vector->Pt(k);
			mif << P.X << " " << P.Y << std::endl;
		}
		mif << "\tPEN (" << (int)repres->Size() << ", " << repres->Symbol() << ", "
				<< MifColor(repres->Color()) << ")" << std::endl;
		if (vector->IsClosed())
			mif << "\tBRUSH (" << 2 << ", " << MifColor(repres->FillColor()) << ", "
					<< MifColor(repres->FillColor()) << ")" << std::endl;
	
		vector->Unload();
	}
	return mif.good();
}

//-----------------------------------------------------------------------------
// Couleur Mif/Mid : R * 256 * 256 + G * 256 + B
//-----------------------------------------------------------------------------
uint32 XMifMidConverter::MifColor(uint32 c)
{
	uint32 r, g, b;
	byte* ptr = (byte*)&c;
	r = ptr[0];
	g = ptr[1];
	b = ptr[2];
	return (r * 65536 + g * 256 + b);
}

//-----------------------------------------------------------------------------
// Ecriture de l'entete d'un fichier MIF/MID
//-----------------------------------------------------------------------------
bool XMifMidConverter::WriteMifMidHeader(const char* filename, std::ofstream* mif, std::ofstream* mid)
{
  XPath P;
  std::string name = P.PathName(filename);

  mif->open((name + ".mif").c_str());
  mid->open((name + ".mid").c_str());
  if ((!mid->good())||(!mif->good()))
    return false;

  XGeoPref pref;
  mif->setf(std::ios::fixed);
  if (pref.Projection() == XGeoProjection::RGF93)
    mif->precision(6);
  else
    mif->precision(2);

  // Entete du MIF
  *mif << "VERSION 300" << std::endl;
  *mif << "Charset \"WindowsLatin1\"" << std::endl;
  *mif << XGeoProjection::MifProjection(pref.Projection()) << std::endl;

  return true;

}

//-----------------------------------------------------------------------------
// Commence la conversion d'une classe
//-----------------------------------------------------------------------------
bool XMifMidConverter::BeginClass(XGeoClass* classe, const char* folder)
{
  XGeoVector* vector;
  std::vector<std::string> V;
  std::string filename = (std::string)folder + "/" + classe->Name();

  m_Mif.open((filename + ".mif").c_str());
  m_Mid.open((filename + ".mid").c_str());
  if ((!m_Mif.good())||(!m_Mid.good()))
    return false;

  XGeoPref pref;
  m_Mif.setf(std::ios::fixed);
  if (pref.Projection() == XGeoProjection::RGF93)
    m_Mif.precision(6);
  else
    m_Mif.precision(2);

  m_Class = classe;
  // Entete du MIF
  m_Mif << "VERSION 300" << std::endl;
  m_Mif << "Charset \"WindowsLatin1\"" << std::endl;
  m_Mif << XGeoProjection::MifProjection(pref.Projection()) << std::endl;

  if (classe->NbVector() < 1)
    return true;

  // Ecriture des colonnes
  uint32 i, nb_att;
  bool label = false;
  XGeoSchema* schema = classe->Schema();
  if (schema->NbAttribut() < 1) {
    vector = classe->Vector((uint32)0);
    vector->ReadAttributes(V);
    nb_att = V.size() / 2;
    if (nb_att > 0) {
      m_Mif << "COLUMNS " << nb_att << std::endl;
      for (i = 0; i < nb_att; i++)
        m_Mif << V[i* 2] << " char (40)" << std::endl;
    } else {
      if (vector->Name().size() > 0) {
        m_Mif << "COLUMNS " << "2" << std::endl;
        m_Mif << "NOM" << " char (40)" << std::endl;
        m_Mif << "IMPORTANCE" << " smallint" << std::endl;
        label = true;
      } else
        m_Mif << "COLUMNS " << "0" << std::endl;
    }

  } else {
    nb_att = schema->NbAttribut();
    m_Mif << "COLUMNS " << nb_att << std::endl;
    for (i = 0; i < nb_att; i++) {
      m_Mif << schema->AttributShortName(i);
      switch(schema->AttributType(i)) {
        case XGeoAttribut::Bool : m_Mif << " char (20)"; break;
        case XGeoAttribut::Int16 : m_Mif << " smallint"; break;
        case XGeoAttribut::Int32 : m_Mif << " integer"; break;
        case XGeoAttribut::Double : m_Mif << " decimal (10,2)"; break;
        case XGeoAttribut::String : m_Mif << " char (" << schema->AttributLength(i) << ")"; break;
        case XGeoAttribut::List : m_Mif << " char (40)"; break;
        case XGeoAttribut::NumericN : m_Mif << " decimal (" << schema->AttributLength(i) << ","
                                          << schema->AttributDecCount(i) << ")"; break;
        case XGeoAttribut::NumericF : m_Mif << " decimal (" << schema->AttributLength(i) << ","
                                          << schema->AttributDecCount(i) << ")"; break;
      }
      m_Mif << std::endl;
    }
  }

  // Ecriture des donnees
  m_Mif << "DATA" << std::endl;
}

//-----------------------------------------------------------------------------
// Ecriture d'un objet
//-----------------------------------------------------------------------------
bool XMifMidConverter::WriteObject(XGeoVector* vector, std::vector<XPt2D>* Pt, std::vector<int>* Part)
{
  if (Pt == NULL)
    vector->LoadGeom();
  std::vector<std::string> V;
  vector->ReadAttributes(V);
  XGeoRepres* repres = vector->Repres();

  XGeoSchema* schema = m_Class->Schema();
  if (schema->NbAttribut() > 0)
    schema->SortAttribut(&V);
  for (uint32 i = 0; i < V.size() / 2; i++){
    m_Mid << V[i* 2 + 1];
    if (i == (V.size() / 2 - 1))
      m_Mid << std::endl;
    else
      m_Mid << "\t";
  }

  XPt2D P;
  uint32 nb_part = vector->NbPart(), partJ, partK, lastPart, nbPt = vector->NbPt();
  if (Part != NULL)
    nb_part = Part->size();
  if (Pt != NULL)
    nbPt = Pt->size();
  lastPart = vector->Part(vector->NbPart() - 1);
  if (Part != NULL)
    lastPart = (*Part)[nb_part - 1];

  // Ponctuel
  if (nbPt < 2){
    if (Pt == NULL)
      P = XPt2D(vector->X0(), vector->Y0());
    else
      P = (*Pt)[0];
    m_Mif << "POINT " << P.X << " " << P.Y << std::endl;

    m_Mif << "\tSYMBOL (" << repres->Symbol() << ", " << MifColor(repres->Color()) << ", "
      << (int)repres->Size() << ")" << std::endl;
    return true;
  }

  // Lineaire ou surfacique
  if (vector->IsClosed())
    m_Mif << "REGION " << nb_part << std::endl;
  else {
    if (nb_part > 1)
      m_Mif << "PLINE MULTIPLE " << nb_part << std::endl;
    else
      m_Mif << "PLINE" << std::endl;
  }

  for (uint32 j = 0; j < nb_part - 1; j++) {
    partJ = vector->Part(j);
    partK = vector->Part(j + 1);
    if (Part != NULL) {
      partJ = (*Part)[j];
      partK = (*Part)[j+1];
    }

    m_Mif << partK - partJ << std::endl;
    for (uint32 k = partJ; k < partK; k++) {
      if (Pt == NULL)
        P = vector->Pt(k);
      else
        P = (*Pt)[k];
      m_Mif << P.X << " " << P.Y << std::endl;
    }
  }
  m_Mif << nbPt - lastPart << std::endl;
  for (uint32 k = lastPart; k < nbPt; k++) {
    if (Pt == NULL)
      P = vector->Pt(k);
    else
      P = (*Pt)[k];
    m_Mif << P.X << " " << P.Y << std::endl;
  }

  // Representation
  m_Mif << "\tPEN (" << (int)repres->Size() << ", " << repres->Symbol() << ", "
      << MifColor(repres->Color()) << ")" << std::endl;
  if (vector->IsClosed())
    m_Mif << "\tBRUSH (" << 2 << ", " << MifColor(repres->FillColor()) << ", "
        << MifColor(repres->FillColor()) << ")" << std::endl;

  if (Pt == NULL)
    vector->Unload();
}

//-----------------------------------------------------------------------------
// Fin de la conversion d'une classe
//-----------------------------------------------------------------------------
bool XMifMidConverter::EndClass()
{
  m_Mif.close();
  m_Mid.close();
  return true;
}
