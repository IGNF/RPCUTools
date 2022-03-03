//-----------------------------------------------------------------------------
//								XTrigMover.h
//								============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 21/10/2021
//-----------------------------------------------------------------------------

#ifndef XTRIGMOVER_H
#define XTRIGMOVER_H

#include "XGeoBase.h"
#include "XGeoClass.h"
#include "XGeoVector.h"
#include "XDelaunay.h"

class XTrigVector : public XGeoVector {
protected:
	double  m_dXf;
	double  m_dYf;
public:
	XTrigVector(double Xi, double Yi, double Xf, double Yf) {
		m_Frame.Xmax = m_Frame.Xmin = Xi;
		m_Frame.Ymax = m_Frame.Ymin = Yi;
		m_dXf = Xf;
		m_dYf = Yf;
	}
	double Xf() { return m_dXf; }
	double Yf() { return m_dYf; }
	XPt2D Delta() { return XPt2D(m_dXf - m_Frame.Xmin, m_dYf - m_Frame.Ymin); }
	double Dist(const XPt2D& P) { XPt2D A(m_Frame.Xmin, m_Frame.Ymin);  return dist(A, P); }
};

class XTrigMover {
	protected:
		XError* m_Error;	// Gestionnaire d'erreurs
		std::vector<XGeoVector*> m_T;
	
	public:
		XTrigMover(XError* error = NULL) { m_Error = error;}

		void SetClasses(XGeoClass* parcelle, XGeoClass* feuille);

		bool MovePoint(XPt2D P, XPt2D& P_out, double max_dist = 25);

};


#endif // XTRIGMOVER_H
