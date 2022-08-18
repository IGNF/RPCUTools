//-----------------------------------------------------------------------------
//								XMatcher.h
//								===============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 20/5/2019
//-----------------------------------------------------------------------------

#ifndef XMATCHER_H
#define XMATCHER_H

#include <vector>
#include <fstream>
#include "XPt2D.h"
#include "XAffinity2D.h"

class XGeoVector;

class XMatcher {
public:
  XMatcher(double max_angle = 10., double diff_angle = 10., double vector_size = 0.1);
  void SetLog(XError* error) { m_Log = error;}

  bool SetOutput(std::string filename, bool poly_out = true);

  bool Match(XGeoVector* Vin, XGeoVector* Vout, std::string joint_key) ;

protected:
  XError*   m_Log;
  double    Max_angle_alignement;
  double    Max_diff_angle;
  double    Min_vector_size;

  XGeoVector*   m_Vin;
  XGeoVector*   m_Vout;

  std::vector<XPt2D>  m_Pt_in;
  std::vector<XPt2D>  m_Pt_out;
  std::vector<XPt2D>  m_Homo_in;    // Points homologues
  std::vector<XPt2D>  m_Homo_out;
  std::vector<int>    m_Idx_in;     // Index des points homologues dans les polygones
  std::vector<int>    m_Idx_out;
  std::vector<XPt2D>  m_Poly_in;
  std::vector<XPt2D>  m_Poly_out;
  std::vector<int>    m_Part;
  std::vector<XPt2D>  m_Part_in;
  std::vector<XPt2D>  m_Part_out;

  XAffinity2D         m_Affi;

  std::ofstream       m_Mif;
  std::ofstream       m_Mid;
  bool                m_bPoly_out;
  std::ofstream       m_MifPoly;
  std::ofstream       m_MidPoly;

  bool CleanPolygons(std::vector<XPt2D>& T);
  uint32 FindHomolog();
  bool FilterHomolog();
  bool WriteHomolog(std::string id, uint32 part);
  bool WritePoly(std::string id, uint32 part);
  bool WritePoly(std::string id);
};

#endif // XMATCHER_H
