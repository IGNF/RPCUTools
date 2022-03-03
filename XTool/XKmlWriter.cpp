//-----------------------------------------------------------------------------
//								XKmlWriter.cpp
//								============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 09/09/2009
//-----------------------------------------------------------------------------

#include <cstring>
#include "XKmlWriter.h"
#include "XGeoVector.h"
#include "XGeoPref.h"
#include "XXml.h"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XKmlWriter::XKmlWriter(bool attribut)
{
  m_bAttribut = attribut;
}

//-----------------------------------------------------------------------------
// Ecriture d'un fichier
//-----------------------------------------------------------------------------
bool XKmlWriter::WriteFile(const char* filename)
{
  std::ofstream out(filename);
  if (!out.good())
    return false;
  out.setf(std::ios::fixed);
  out.precision(6);

  out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
  out << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">" << std::endl;
  out << "<Document>" << std::endl;

  // Ecriture des vecteurs
  for (uint32 k = 0; k < m_V.size(); k++) {
    XGeoVector* V = m_V[k];
    WriteVector(&out, V, k);
  }

  out << "</Document>" << std::endl;
  out << "</kml>" << std::endl;

  return out.good();
}

//-----------------------------------------------------------------------------
// Ecriture d'un vecteur
//-----------------------------------------------------------------------------
/*
bool XKmlWriter::WriteVector(std::ofstream* out, XGeoVector* V, uint32 id)
{
  if (!V->LoadGeom())
    return false;

  double lat, lon;
  XGeoPref Pref;

  // Representation
  byte r, g, b, t;
  char fill[16], pen[16];
  ::strcpy(fill, "7f00ff00");
  ::strcpy(pen, "7fff00ff");
  int size = 2;
  if (V->Repres() != NULL) {
    XGeoRepres* R = V->Repres();
    t = 255 - XRint((double)R->Transparency() * 2.55);
    R->Color(r, g, b);
    ::sprintf(pen, "%02x%02x%02x%02x", t, b, g, r);

    R->FillColor(r, g, b);
    if (R->FillColor() == 0xFFFFFFFF)
      t = 0;
    ::sprintf(fill, "%02x%02x%02x%02x", t, b, g, r);
    size = V->Repres()->Size();
    if (size == 0)
      size = 1;
  }
  *out << "<Style id=\"" << id << "\">" << std::endl;
  *out << "<LineStyle>" << std::endl;
  *out << "<color> " << pen << " </color>" << std::endl;
  *out << "<width> " << size << " </width>" << std::endl;
  *out << "</LineStyle>" << std::endl;
  if (V->IsClosed()) {
    *out << "<PolyStyle>" << std::endl;
    *out << "<color> " << fill << " </color>" << std::endl;
    *out << "</PolyStyle>" << std::endl;
  }
  *out << "</Style>" << std::endl;

  *out << "<Placemark>" << std::endl;

  if (V->NbPt() < 2) {	// Ponctuel
    Pref.ConvertDeg(Pref.Projection(), XGeoProjection::RGF93, V->Frame().Xmin, V->Frame().Ymax, lon, lat);
    *out << "<name>" << XXml::OemToXml(V->Name()) << "</name>" << std::endl;
    *out << "<Point>" << std::endl;
    *out << "<coordinates>" << lon << "," << lat << ",0" << "</coordinates>" << std::endl;
    *out << "</Point>" << std::endl;
  } else {
     *out << "<styleUrl>#" << id << "</styleUrl>" << std::endl;

    if (V->IsClosed())
      *out << "<Polygon>" << std::endl;
    else
      *out << "<LineString>" << std::endl;

    *out << "<extrude>1</extrude>" << std::endl;
    *out << "<tessellate>1</tessellate>" << std::endl;

    if (V->IsClosed()) {
      *out << "<outerBoundaryIs>" << std::endl;
      *out << "<LinearRing>" << std::endl;
    }

    XPt* P = V->Pt();
    uint32 nbpt = V->NbPt();
    if (V->NbPart() > 1)
      nbpt = V->Part(1) - 1;
    *out << "<coordinates>" << std::endl;
    for (uint32 i = 0; i < nbpt; i++) {
      Pref.ConvertDeg(Pref.Projection(), XGeoProjection::RGF93, P[i].X, P[i].Y, lon, lat);
      *out << lon << "," << lat << ",0 ";
    }
    *out << "</coordinates>" << std::endl;

    if (V->IsClosed()) {
      *out << "</LinearRing>" << std::endl;
      *out << "</outerBoundaryIs>" << std::endl;
    }

    if (V->IsClosed())
      *out << "</Polygon>" << std::endl;
    else
      *out << "</LineString>" << std::endl;
  }

  *out << "</Placemark>" << std::endl;

  V->Unload();
  return out->good();
}
*/
bool XKmlWriter::WriteVector(std::ofstream* out, XGeoVector* V, uint32 id)
{
  if (!V->LoadGeom())
    return false;

  double lat, lon;
  XGeoPref Pref;

  // Representation
  byte r, g, b, t;
  char fill[16], pen[16];
  ::strcpy(fill, "7f00ff00");
  ::strcpy(pen, "7fff00ff");
  int size = 2;
  if (V->Repres() != NULL) {
    XGeoRepres* R = V->Repres();
    t = 255 - XRint((double)R->Transparency() * 2.55);
    R->Color(r, g, b);
    ::sprintf(pen, "%02x%02x%02x%02x", t, b, g, r);

    R->FillColor(r, g, b);
    if (R->FillColor() == 0xFFFFFFFF)
      t = 0;
    ::sprintf(fill, "%02x%02x%02x%02x", t, b, g, r);
    size = V->Repres()->Size();
    if (size == 0)
      size = 1;
  }
  *out << "<Style id=\"" << id << "\">" << std::endl;
  *out << "<LineStyle>" << std::endl;
  *out << "<color> " << pen << " </color>" << std::endl;
  *out << "<width> " << size << " </width>" << std::endl;
  *out << "</LineStyle>" << std::endl;
  if (V->IsClosed()) {
    *out << "<PolyStyle>" << std::endl;
    *out << "<color> " << fill << " </color>" << std::endl;
    *out << "</PolyStyle>" << std::endl;
  }
  *out << "</Style>" << std::endl;

  *out << "<Placemark>" << std::endl;

  // Ecriture des attributs
  if (m_bAttribut) {
    std::vector<std::string> Att;
    if (V->ReadAttributes(Att)) {
      *out << "<ExtendedData>" << std::endl;
      for (uint32 i = 0; i < Att.size() / 2; i++) {
        *out << "<Data name=\"" << XXml::OemToXml(Att[2*i]) << "\"> "
             << "<value>" << XXml::OemToXml(Att[2*i+1]) << "</value>"
             << "</Data>" << std::endl;
      }
      *out << "</ExtendedData>" << std::endl;
    }
  }

  if (V->NbPt() < 2) {	// Ponctuel
    Pref.ConvertDeg(Pref.Projection(), XGeoProjection::RGF93, V->Frame().Xmin, V->Frame().Ymax, lon, lat);
    *out << "<name>" << XXml::OemToXml(V->Name()) << "</name>" << std::endl;
    *out << "<Point>" << std::endl;
    *out << "<coordinates>" << lon << "," << lat << ",0" << "</coordinates>" << std::endl;
    *out << "</Point>" << std::endl;
    *out << "</Placemark>" << std::endl;
    V->Unload();
    return out->good();
  }

  *out << "<styleUrl>#" << id << "</styleUrl>" << std::endl;

  // Polyligne
  if (!V->IsClosed()) {
    *out << "<LineString>" << std::endl;
    *out << "<extrude>1</extrude>" << std::endl;
    *out << "<tessellate>1</tessellate>" << std::endl;
    XPt* P = V->Pt();
    uint32 nbpt = V->NbPt();
    *out << "<coordinates>" << std::endl;
    for (uint32 i = 0; i < nbpt; i++) {
      Pref.ConvertDeg(Pref.Projection(), XGeoProjection::RGF93, P[i].X, P[i].Y, lon, lat);
      *out << lon << "," << lat << ",0 ";
    }
    *out << "</coordinates>" << std::endl;
    *out << "</LineString>" << std::endl;
    *out << "</Placemark>" << std::endl;
    V->Unload();
    return out->good();
  }

  // Polygone
  *out << "<Polygon>" << std::endl;
  *out << "<extrude>1</extrude>" << std::endl;
  *out << "<tessellate>1</tessellate>" << std::endl;

  XPt* P = V->Pt();
  uint32 nbpt = V->NbPt();
  uint32 nbpart = V->NbPart();

	for (uint32 i = 0; i < nbpart; i++) {
		if (i < nbpart - 1)
			nbpt = V->Part(i+1) - V->Part(i);
		else
			nbpt = V->NbPt() - V->Part(i);
		if (i > 0)
			*out << "<innerBoundaryIs>" << std::endl;
		else
			*out << "<outerBoundaryIs>" << std::endl;
		*out << "<LinearRing>" << std::endl;
		*out << "<coordinates>" << std::endl;
		for (uint32 j = V->Part(i); j < V->Part(i) + nbpt; j++) {
			Pref.ConvertDeg(Pref.Projection(), XGeoProjection::RGF93, P[j].X, P[j].Y, lon, lat);
			*out << lon << "," << lat << ",0" << std::endl;
		}
		*out << "</coordinates>" << std::endl;
		*out << "</LinearRing>" << std::endl;
		if (i > 0)
			*out << "</innerBoundaryIs>" << std::endl;
		else
			*out << "</outerBoundaryIs>" << std::endl;
	}

  *out << "</Polygon>" << std::endl;
  *out << "</Placemark>" << std::endl;

  V->Unload();
  return out->good();
}
