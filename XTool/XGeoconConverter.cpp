//-----------------------------------------------------------------------------
//								XGeoconConverter.cpp
//								====================
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 09/05/2006
//-----------------------------------------------------------------------------

#include "XGeoconConverter.h"
#include "XGeoBase.h"
#include "XGeoLayer.h"
#include "XGeoVector.h"

//-----------------------------------------------------------------------------
// Conversion d'une base
//-----------------------------------------------------------------------------
bool XGeoconConverter::ConvertBase(XGeoBase* base, const char* folder, XWait* wait)
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
bool XGeoconConverter::ConvertClass(XGeoClass* classe, const char* folder)
{
	XGeoVector* vector;
	std::vector<std::string> V;
	XPt2D P;
  std::string filename = (std::string)folder + "/" + classe->Name();

	std::ofstream gxt, mid;
	gxt.open((filename + ".gxt").c_str());
	if (!gxt.good())
		return false;

	gxt.setf(std::ios::fixed);
	gxt.precision(2);

	if (classe->NbVector() < 1)
		return true;

	// Ecriture des donnees
	for (uint32 i = 0; i < classe->NbVector(); i++) {
		vector = classe->Vector(i);
		if (m_bVisibleOnly)
			if (!vector->Visible())
				continue;
		vector->LoadGeom();
		V.clear();
		vector->ReadAttributes(V);

		if (vector->Is3D())
			gxt << "//$3DOBJECT" << std::endl;
		else
			gxt << "//$2DOBJECT" << std::endl;

		// Entete de l'objet
		gxt << i << "\t" << classe->Layer()->Name() << "\t" << classe->Name() << "\t";

		// Attribut de l'objet
    if ((V.size() == 0)&&(vector->Name().size() > 0)) {
      gxt << "\t" << 2 << "\t" << vector->Name() << "\t" << vector->Importance() << "\t";
    } else {
      gxt << /*vector->Name() << */"\t" << V.size() / 2 << "\t";
      for (uint32 j = 0; j < V.size() / 2; j++)
        gxt << V[2*j + 1] << "\t";
    }
			
		// Geometrie de l'objet
		XPt* P = vector->Pt();
		double* Z = vector->Z();
    if ( (P == NULL)&&(vector->NbPt() > 1)) { // On ecrit le cadre
      gxt << vector->Frame().Xmin << "\t" << vector->Frame().Ymin << "\t"
          << "4\t" << 0. << "\t" << vector->Frame().Height() << "\t"
          << vector->Frame().Width() << "\t" << 0. << "\t"
          << 0. << "\t" << vector->Frame().Height() * (-1.) << "\t"
          << vector->Width() * (-1.) << "\t" << 0. << std::endl;
      continue;
    }
    if ((vector->Is3D())&&(Z==NULL)&&(vector->NbPt()>1))
			continue;
		gxt << vector->Pt(0).X << "\t" << vector->Pt(0).Y << "\t";
		if (vector->Is3D())
			if (vector->Z(0) > XGEO_NO_DATA)
				gxt << vector->Z(0) << "\t";
			else
				gxt << "NOZ" << "\t";

		// Objet ponctuel
		if (vector->NbPt() < 2) {
			gxt << "0" << std::endl;
			vector->Unload();
			continue;
		}

		// Objet lineaire en une partie
		if ((!vector->IsClosed())&&(vector->NbPart() == 1)) {
			gxt << vector->Pt(vector->NbPt() - 1).X << "\t" << vector->Pt(vector->NbPt() - 1).Y << "\t";
			gxt << vector->NbPt() - 1;
      for (uint32 j = 1; j < vector->NbPt(); j++) {
				gxt << "\t" << P[j].X - P[j-1].X << "\t" << P[j].Y - P[j-1].Y;
				if (vector->Is3D())
					if (Z[j] > XGEO_NO_DATA)
						gxt << "\t" << Z[j];
					else
						gxt << "\t" << "NOZ";
			}
			gxt << std::endl;
			vector->Unload();
			continue;
		}

		// Objet surfacique en une partie
		if ((vector->IsClosed())&&(vector->NbPart() == 1)) {
			gxt << vector->NbPt() - 1;
      for (uint32 j = 1; j < vector->NbPt(); j++) {
				gxt << "\t" << P[j].X - P[j-1].X << "\t" << P[j].Y - P[j-1].Y;
				if (vector->Is3D())
					if (Z[j] > XGEO_NO_DATA)
						gxt << "\t" << Z[j];
					else
						gxt << "\t" << "NOZ";
			}
			gxt << std::endl;
			vector->Unload();
			continue;
		}

		// Objet surfacique en plusieurs parties
		if ((vector->IsClosed())&&(vector->NbPart() > 1)) {
			uint32 nb_pt = vector->Part(1);
			gxt << nb_pt - 1;
      for (uint32 j = 1; j < nb_pt; j++) {
				gxt << "\t" << P[j].X - P[j-1].X << "\t" << P[j].Y - P[j-1].Y;
				if (vector->Is3D())
					if (Z[j] > XGEO_NO_DATA)
						gxt << "\t" << Z[j];
					else
						gxt << "\t" << "NOZ";
			}
			gxt << "\t" << vector->NbPart() - 1;
			for (uint32 k = 1; k < vector->NbPart(); k++) {
				if (k < vector->NbPart() - 1)
					nb_pt = vector->Part(k+1) - vector->Part(k);
				else
					nb_pt = vector->NbPt() - vector->Part(k);
				gxt << "\t" << P[vector->Part(k)].X << "\t" <<P[vector->Part(k)].Y;
				if (vector->Is3D())
					if (Z[vector->Part(k)] > XGEO_NO_DATA)
						gxt << "\t" << Z[vector->Part(k)];
					else
						gxt << "\t" << "NOZ";

				gxt << "\t" << nb_pt - 1;
        for (uint32 j = vector->Part(k) + 1; j < vector->Part(k) + nb_pt; j++) {
					gxt << "\t" << P[j].X - P[j-1].X << "\t" << P[j].Y - P[j-1].Y;
					if (vector->Is3D())
						if (Z[j] > XGEO_NO_DATA)
							gxt << "\t" << Z[j];
						else
							gxt << "\t" << "NOZ";
				}
			}

			gxt << std::endl;
			vector->Unload();
			continue;
		}


	}

	return true;
}
