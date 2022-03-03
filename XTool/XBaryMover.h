//-----------------------------------------------------------------------------
//								XBaryMover.h
//								===============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 9/4/2019
//
// Deplacement d'un vecteur d'apres l'algorithme de Jan-Henrik Haunert
//-----------------------------------------------------------------------------

#ifndef XBARYMOVER_H
#define XBARYMOVER_H

#include "XGeoVector.h"

class XBaryMover {
public:
  XBaryMover(double epsilon = 0.001);

  XPt2D Delta(std::vector<XPt2D>& Poly_ini, std::vector<XPt2D>& Poly_fin, XPt2D& T);
  XPt2D Delta(XPt* Poly_ini, XPt* Poly_fin, uint32 nbPt, XPt2D& T);
  bool Densifier(std::vector<XPt2D>& M_ini, std::vector<XPt2D>& M_fin,
                 std::vector<XPt2D>& P_ini, std::vector<XPt2D>& P_fin,
                 std::vector<XPt2D>& D_ini, std::vector<XPt2D>& D_fin);

private:
  double              m_dEpsilon;
  std::vector<double> m_TA;
  std::vector<double> m_TBX;
  std::vector<double> m_TBY;
  XPt2D               m_Delta;

  enum eDelta { None, Standard, Interpol};

  eDelta Delta(XPt2D& M_ini, XPt2D& M_fin, XPt2D& P_ini, XPt2D& P_fin, XPt2D& T);
  double arctanh(double x) { return 0.5 * log((1.+x)/(1.-x));}

  bool GetOrientation(std::vector<XPt2D>& V);
  void ChangeOrientation(std::vector<XPt2D>& V);

public:
  double GetNormDensity(std::vector<XPt2D>& P, std::vector<double>& X);
  bool Densify(std::vector<XPt2D>& P_in, std::vector<XPt2D>& P_out, std::vector<double>& X_out);
  bool XDensifier(std::vector<XPt2D>& M_ini, std::vector<XPt2D>& M_fin,
                  std::vector<XPt2D>& P_ini, std::vector<XPt2D>& P_fin,
                  std::vector<XPt2D>& D_ini, std::vector<XPt2D>& D_fin);
};

#endif // XBARYMOVER_H
