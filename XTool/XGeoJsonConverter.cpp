//-----------------------------------------------------------------------------
//								XGeoJsonConverter.cpp
//								=====================
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 11/06/2019
//-----------------------------------------------------------------------------

#include "XGeoJsonConverter.h"
#include "XGeoBase.h"
#include "XGeoLayer.h"
#include "XGeoVector.h"
#include "XGeoPref.h"
#include "XPath.h"

#include <ctime>
#include <cstdlib>
#include <sstream>

//-----------------------------------------------------------------------------
// Conversion d'une base
//-----------------------------------------------------------------------------
bool XGeoJsonConverter::ConvertBase(XGeoBase* base, const char* folder, XWait* wait)
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
bool XGeoJsonConverter::ConvertClass(XGeoClass* classe, const char* folder)
{
	XGeoVector* V;
	XPt2D P;
	std::string filename = (std::string)folder + "/" + classe->Name();

	std::ofstream geojson;
	geojson.open((filename + ".json").c_str());
	if (!geojson.good())
		return false;

	geojson.setf(std::ios::fixed);
	geojson.precision(6);

	// Entete du GeoJson
	geojson << "{\"type\":\"FeatureCollection\",\"features\":[" << std::endl;

	// Ecriture des objets
	for (uint32 i = 0; i < classe->NbVector(); i++) {
		V = classe->Vector(i);
		if (m_bVisibleOnly)
			if (!V->Visible())
				continue;
		ConvertVector(V, &geojson);
		if (i < classe->NbVector() - 1)
			geojson << "," << std::endl;
	}

	// Fin du fichier
	geojson << "]}" << std::endl;
	return geojson.good();
}

//-----------------------------------------------------------------------------
// Conversion d'un vecteur
//-----------------------------------------------------------------------------
bool XGeoJsonConverter::ConvertVector(XGeoVector* V, std::ostream* json)
{
	if (!V->LoadGeom())
		return false;
	XGeoPref pref;
	double x, y, lon, lat;
	std::vector<std::string> Att;
	V->ReadAttributes(Att);

  /* Creation d'un profil temporel
  Att.push_back("profil");
  srand((uint32)V);
  int nb_profil = rand() % 30 + 1;
  std::ostringstream out;
  int year = 2017, month = 1, day = 1, num;
  out << "[";
  for (int k = 0; k < nb_profil; k++) {
    num = rand() % 10 + 1;
    day += num;
    if (day > 28) {
      month++;
      day = rand() % 5 + 1;
    }
    if (month > 12)
      break;
    out << year << "-";
    if (month < 10) out << "0";
    out << month << "-";
    if (day < 10) out << "0";
    out  << day << ":" << (double)(rand() % 100) * 0.01;
    if (k < nb_profil - 1)
      out << ",";
  }
  out << "]";
  Att.push_back(out.str());*/

	*json << "{\"type\":\"Feature\",";
	*json << "\"geometry\":{";
	*json << "\"type\":";

	switch(V->TypeVector()) {
		case XGeoVector::Point : *json << "\"Point\","; break;
		case XGeoVector::PointZ : *json << "\"Point\","; break;
		case XGeoVector::MPoint : *json << "\"MultiPoint\","; break;
		case XGeoVector::MPointZ : *json << "\"MultiPoint\","; break;
		case XGeoVector::Line : *json << "\"LineString\","; break;
		case XGeoVector::LineZ : *json << "\"LineString\","; break;
		case XGeoVector::MLine : *json << "\"MultiLineString\","; break;
		case XGeoVector::MLineZ : *json << "\"MultiLineString\","; break;
		case XGeoVector::Poly : *json << "\"Polygon\","; break;
		case XGeoVector::PolyZ : *json << "\"Polygon\","; break;
		case XGeoVector::MPoly : *json << "\"MultiPolygon\","; break;
		case XGeoVector::MPolyZ : *json << "\"MultiPolygon\","; break;
		default : V->Unload(); return false;
	}

	*json << "\"coordinates\":";
	if (V->NbPt() == 1) {
		x = V->Frame().Xmin;
		y = V->Frame().Ymax;
		pref.ConvertDeg(pref.Projection(), XGeoProjection::RGF93, x, y, lon, lat);
		*json << "[" << lon << "," << lat;
		if (V->Z(0) > XGEO_NO_DATA)
			*json << "," << V->Z(0) << "]";
		else
			*json << "]";
	} else {
		XPt2D P;
		double z;
		uint32 part = 1;
		if (V->NbPart() > 1)
			*json << "[[[";
		else
			*json << "[[";
		for (uint32 i = 0; i < V->NbPt(); i++) {
			P = V->Pt(i);
			pref.ConvertDeg(pref.Projection(), XGeoProjection::RGF93, P.X, P.Y, lon, lat);
			z = V->Z(i);
			for (uint32 j = part; j < V->NbPart(); j++)
				if (i == V->Part(j)) {
					part ++;
					*json << "],[";
				}
			*json << "[" << lon << "," << lat;
			if (z > XGEO_NO_DATA)
				*json << "," << z << "]";
			else
				*json << "]";
			if (i < (V->NbPt() - 1)) {
				if (part == V->NbPart())
					*json << ",";
				else
					if (i != V->Part(part)-1)
						*json << ",";
			}
		}
		if (V->NbPart() > 1)
			*json << "]]]";
		else
			*json << "]]";
	}

	V->Unload();

	*json << "}";  // Fin de la geometrie


	if (Att.size() < 2) {
		*json << "}";   // Fin de la feature
		return true;
	}

	if (Att.size() > 0) {
		*json << ",\"properties\": {";
		for (uint32 i = 0; i < Att.size() / 2; i++) {
			*json << "\"" << Att[2*i] << "\":\"" << Att[2*i+1] << "\"";
			if (i != (Att.size() / 2 - 1))
				*json << ",";
		}
		*json << "}";
	}

	*json << "}";   // Fin de la feature


	return true;
}
