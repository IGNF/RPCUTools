//-----------------------------------------------------------------------------
//								XDelaunay.h
//								===========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// Code base sur http://paulbourke.net/papers/triangulate/ et l'implementation C
//
// 07/08/2007
//-----------------------------------------------------------------------------

#ifndef _XDELAUNAY_H
#define _XDELAUNAY_H

#include <vector>
#include "XPt2D.h"
#include "XGeoVector.h"

#define EPSILON 0.00001

class XDelaunay {
private:
	typedef struct {
		int p1,p2,p3;
	} ITRIANGLE;
	typedef struct {
		int p1,p2;
	} IEDGE;
	typedef struct {
		double x,y,z;
	} XYZ;

protected:
	std::vector<XGeoVector*>		m_Geom;					// Geometries a trianguler
	XPt*												m_P;						// Points a trianguler
	ITRIANGLE*									m_V;						// Resultat de la triangulation
	int													m_nTri;					// Nombre de triangles

	void Clear();
	bool Prepare();
	bool CircumCircle(XPt& P, XPt& A1, XPt& A2, XPt& A3, XPt& C, double& rsqr);

public:
	XDelaunay();
	virtual ~XDelaunay();

	bool AddVector(XGeoVector* V);
	void ClearVector() { m_Geom.clear(); Clear(); }

	bool Triangulate();
	int NbTriangle() { return m_nTri;}
	bool Triangle(int i, XPt& A, XPt& B, XPt& C);
	bool Triangle(int i, XGeoVector** A, XGeoVector** B, XGeoVector** C);
	int Triangle(double X, double Y);

};

#endif //_XDELAUNAY_H
