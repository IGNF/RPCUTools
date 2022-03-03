//-----------------------------------------------------------------------------
//								XTrigMover.cpp
//								==============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 21/10/2021
//-----------------------------------------------------------------------------

#include "XTrigMover.h"

//-----------------------------------------------------------------------------
// Preparation des donnees en entree
//-----------------------------------------------------------------------------
void XTrigMover::SetClasses(XGeoClass* parcelle, XGeoClass* feuille)
{
	XGeoVector* V;
	for (uint32 i = 0; i < parcelle->NbVector(); i++) {
		V = parcelle->Vector(i);
		if (V->NbPt() < 3)
			continue;
		m_T.push_back(V);
	}
	for (uint32 i = 0; i < feuille->NbVector(); i++) {
		V = feuille->Vector(i);
		if (V->NbPt() < 3)
			continue;
		m_T.push_back(V);
	}
}

//-----------------------------------------------------------------------------
// Deplacement d'un point
//-----------------------------------------------------------------------------
bool XTrigMover::MovePoint(XPt2D P_in, XPt2D& P_out, double max_dist)
{
	if (m_T.size() < 2)
		return false;
	
	P_out = P_in;
	XDelaunay delaunay;
	std::vector<XTrigVector*> L;
	double epsilon = 0.01;

	for (uint32 i = 0; i < m_T.size() / 2; i++) {
		XGeoVector* V_in = m_T[2 * i];	// Les elements paires sont les elements en entree
		XGeoVector* V_out = m_T[2 * i + 1];	// Les elements impaires sont les elements en sortie

		if (V_in->NbPt() != V_out->NbPt()) 
			continue;

		XFrame F = V_in->Frame();
		F += max_dist;
		if (!F.IsIn(P_in))
			continue;

		// Chargement des geometries si ce n'est pas deja fait
		bool vin_loaded = true, vout_loaded = true;
		if (!V_in->IsLoaded()) {
			if (!V_in->LoadGeom2D())
				continue;
			vin_loaded = false;
		}
		if (!V_out->IsLoaded()) {
			if (!V_out->LoadGeom2D()) {
				if (!vin_loaded)
					V_in->Unload();
				continue;
			}
			vout_loaded = false;
		}

		// Creation des points de triangulation
		XPt2D A, B;
		for (uint32 j = 0; j < V_in->NbPt(); j++) {
			A = V_in->Pt(j);
			B = V_out->Pt(j);

			if (dist(A, P_in) < epsilon) {	// On est sur un sommet, donc pas la peine de trianguler
				P_out = B;
				V_in->Unload();
				V_out->Unload();
				for (uint32 j = 0; j < L.size(); j++)
					delete L[j];
				delaunay.ClearVector();
				return true;
			}

			XTrigVector* U = new XTrigVector(A.X, A.Y, B.X, B.Y);
			L.push_back(U);
			delaunay.AddVector(U);
		}
		// Dechargement des geometries si necessaire
		if (!vin_loaded)
			V_in->Unload();
		if (!vout_loaded)
			V_out->Unload();
	} // endfor i

	// Calcul de la triangulation
	delaunay.Triangulate();

	int num_tri = delaunay.Triangle(P_in.X, P_in.Y);
	if (num_tri >= 0) {
		XTrigVector *A1, *A2, *A3;
		delaunay.Triangle(num_tri, (XGeoVector**)&A1, (XGeoVector**)&A2, (XGeoVector**)&A3);
		XPt2D D1 = A1->Delta(), D2 = A2->Delta(), D3 = A3->Delta();
		double dist1 = A1->Dist(P_in), dist2 = A2->Dist(P_in), dist3 = A3->Dist(P_in);
		if (dist1 < epsilon)
			P_out = XPt2D(A1->Xf(), A1->Yf());
		if (dist2 < epsilon)
			P_out = XPt2D(A2->Xf(), A2->Yf());
		if (dist3 < epsilon)
			P_out = XPt2D(A3->Xf(), A3->Yf());
		if ((dist1 >= epsilon) && (dist2 >= epsilon) && (dist3 >= epsilon)) {
			P_out = (1. / dist1) * D1 + (1. / dist2) * D2 + (1. / dist3) * D3;
			P_out /= ((1. / dist1) + (1. / dist2) + (1. / dist3));
			P_out += P_in;
		}
	}

	for (uint32 j = 0; j < L.size(); j++)
		delete L[j];
	L.clear();

	delaunay.ClearVector();
	return true;
}