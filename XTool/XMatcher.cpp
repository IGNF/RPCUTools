//-----------------------------------------------------------------------------
//								XMatcher.cpp
//								===============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 20/5/2019
//-----------------------------------------------------------------------------

#include "XMatcher.h"
#include "XGeoVector.h"
#include "XPt3D.h"
#include "XGeoPref.h"
#include "XBaryMover.h"

#include <algorithm>

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XMatcher::XMatcher(double max_angle, double diff_angle, double vector_size)
{
  m_Log = NULL;
  Max_angle_alignement = max_angle * XPI / 180.;
  Max_diff_angle = diff_angle * XPI / 180.;
  Min_vector_size = vector_size;
}

//-----------------------------------------------------------------------------
// Fixe le fichier de sortie
//-----------------------------------------------------------------------------
bool XMatcher::SetOutput(std::string filename, bool poly_out)
{
  m_bPoly_out = poly_out;

  std::string path_mif = filename + "_vec.mif";
  std::string path_mid = filename + "_vec.mid";

  m_Mif.open(path_mif.c_str());
  m_Mid.open(path_mid.c_str());
  if ((!m_Mid.good())||(!m_Mif.good()))
    return false;

  if (m_bPoly_out) {
    path_mif = filename + "_poly.mif";
    path_mid = filename + "_poly.mid";
    m_MifPoly.open(path_mif.c_str());
    m_MidPoly.open(path_mid.c_str());
    if ((!m_MidPoly.good()) || (!m_MifPoly.good()))
      return false;
  }

  XGeoPref pref;
  m_Mif.setf(std::ios::fixed);
  if (pref.Projection() == XGeoProjection::RGF93)
    m_Mif.precision(6);
  else
    m_Mif.precision(2);

  // Entete du MIF
  m_Mif << "VERSION 300" << std::endl;
  m_Mif << "Charset \"WindowsLatin1\"" << std::endl;
  m_Mif << XGeoProjection::MifProjection(pref.Projection()) << std::endl;
  m_Mif << "COLUMNS 3" << std::endl;
  m_Mif << "JOINT_VALUE " <<  "char (40)" << std::endl;
  m_Mif << "PART " <<  "Integer" << std::endl;
  m_Mif << "NUM " << "Integer" << std::endl;
  m_Mif << "DATA" << std::endl;

  if (m_bPoly_out) {
    m_MifPoly.setf(std::ios::fixed);
    if (pref.Projection() == XGeoProjection::RGF93)
      m_MifPoly.precision(6);
    else
      m_MifPoly.precision(2);

    // Entete du MIF
    m_MifPoly << "VERSION 300" << std::endl;
    m_MifPoly << "Charset \"WindowsLatin1\"" << std::endl;
    m_MifPoly << XGeoProjection::MifProjection(pref.Projection()) << std::endl;
    m_MifPoly << "COLUMNS 3" << std::endl;
    m_MifPoly << "JOINT_VALUE " << "char (40)" << std::endl;
    m_MifPoly << "PART " << "Integer" << std::endl;
    m_MifPoly << "NUM " << "Integer" << std::endl;
    m_MifPoly << "DATA" << std::endl;
  }

  return true;
}

//-----------------------------------------------------------------------------
// Appariement
//-----------------------------------------------------------------------------
bool XMatcher::Match(XGeoVector* Vin, XGeoVector* Vout, std::string joint_key)
{
  // Chargement des vecteurs
  Vin->LoadGeom2D();
  Vout->LoadGeom2D();
  m_Part.clear();
  m_Part_in.clear();
  m_Part_out.clear();

  // Recherche de la correspondance entre les parties
  uint32 last_in, last_out;
  for (uint32 part = 0; part < Vin->NbPart(); part++) {
    m_Pt_in.clear();
    XFrame Fin;
    if (part == Vin->NbPart() - 1)
      last_in = Vin->NbPt();
    else
      last_in = Vin->Part(part + 1);
    for (uint32 j = Vin->Part(part); j < last_in; j++) {
      m_Pt_in.push_back(Vin->Pt(j));
      Fin += Vin->Pt(j);
    }

    // Recherche de la partie correspondante
    double dmin = 0., d;
    uint32 part_min = 0;
    XFrame Fout_min;
    for (uint32 part_out = 0; part_out < Vout->NbPart(); part_out++) {
      m_Pt_out.clear();
      XFrame Fout;
      if (part_out == Vout->NbPart() - 1)
        last_out = Vout->NbPt();
      else
        last_out = Vout->Part(part_out + 1);
      for (uint32 k = Vout->Part(part_out); k < last_out; k++) {
        m_Pt_out.push_back(Vout->Pt(k));
        Fout += Vout->Pt(k);
      }
      d = dist(Fin, Fout);
      if (part_out == 0) {
        dmin = d;
        Fout_min = Fout;
      } else
        if (d < dmin) {
          dmin = d;
          part_min = part_out;
          Fout_min = Fout;
        }
    } // endfor part_out

    // Polygone correspondant
    if (part_min == Vout->NbPart() - 1)
      last_out = Vout->NbPt();
    else
      last_out = Vout->Part(part_min + 1);
    m_Pt_out.clear();
    for (uint32 k = Vout->Part(part_min); k < last_out; k++)
      m_Pt_out.push_back(Vout->Pt(k));

    // Generalisation par rectangle englobant
    std::vector<XPt2D> gen_in, gen_out;
    gen_in.push_back(XPt2D(Fin.Xmin, Fin.Ymin));
    gen_in.push_back(XPt2D(Fin.Xmin, Fin.Ymax));
    gen_in.push_back(XPt2D(Fin.Xmax, Fin.Ymax));
    gen_in.push_back(XPt2D(Fin.Xmax, Fin.Ymin));

    gen_out.push_back(XPt2D(Fout_min.Xmin, Fout_min.Ymin));
    gen_out.push_back(XPt2D(Fout_min.Xmin, Fout_min.Ymax));
    gen_out.push_back(XPt2D(Fout_min.Xmax, Fout_min.Ymax));
    gen_out.push_back(XPt2D(Fout_min.Xmax, Fout_min.Ymin));

    m_Affi.Compensation(gen_out, gen_in, 3);

    m_Homo_in.clear();
    m_Homo_out.clear();
    m_Idx_in.clear();
    m_Idx_out.clear();
    if (FindHomolog() < 1) {// Pas de points homologues trouves
      if (m_Log != NULL) {
        std::string mes = "Pas de points homologues pour " + joint_key;
        XErrorAlert(m_Log, mes);
      }
      continue;
    }
    
    uint32 nb_filter = 0;
    do {
      nb_filter++;
      if (nb_filter > 10) { // Sans doute un probleme ...
        std::string mes = "Filtrage impossible pour " + joint_key;
        XErrorAlert(m_Log, mes);
        break;
      }
    } while (!FilterHomolog());

    bool flag_homo = WriteHomolog(joint_key, part);
    if ((m_bPoly_out)&&(flag_homo)) {
      XBaryMover mover;
      m_Homo_in.push_back(m_Homo_in[0]);
      m_Homo_out.push_back(m_Homo_out[0]);
      mover.XDensifier(m_Pt_in, m_Pt_out, m_Homo_in, m_Homo_out, m_Poly_in, m_Poly_out);
      //WritePoly(joint_key, part);
      m_Part.push_back((int)m_Poly_in.size());
      m_Part_in.insert(m_Part_in.end(), m_Poly_in.begin(), m_Poly_in.end());
      m_Part_out.insert(m_Part_out.end(), m_Poly_out.begin(), m_Poly_out.end());
      m_Poly_in.clear();
      m_Poly_out.clear();
    }

  } // endfor part
  WritePoly(joint_key);

  // Dechargement des vecteurs
  Vin->Unload();
  Vout->Unload();

  return true;
}

bool XMatcher_predXPt3DX(XPt3D A, XPt3D B)
{
  if (A.X < B.X)
    return true;
  return false;
}

//-----------------------------------------------------------------------------
// Recherche des vertex homologues
//-----------------------------------------------------------------------------
uint32 XMatcher::FindHomolog()
{
  XPt2D Tr, D, J, K, L, R, S, T;
  int idx_K, idx_D;
  double d, alpha, beta;
  bool found = false;
  std::vector<XPt3D> Result;

  for (uint32 j = 0; j < m_Pt_in.size() - 1; j++) {
    found = false;
    Result.clear();
    K = m_Pt_in[j];
    idx_K = j;
    if (j > 0)
      J = m_Pt_in[j-1];
    else
      J = m_Pt_in[m_Pt_in.size()-2];
    L = m_Pt_in[j+1];
    //alpha = prodScal((K-J), (K-L)) / (dist(K,J)*dist(K,L));
    alpha = acos(prodScal((J-K), (L-K)) / (dist(K,J)*dist(K,L)));
    if (prodCross(J, K, L) < 0)
      alpha = 2*XPI - alpha;
    if (fabs(alpha - XPI) < Max_angle_alignement) continue;  // Point aligne
    alpha = cap(J, K, L);

    //Tr = K + Gout - Gin; // Translation globale
    Tr = m_Affi.Direct(K);

    for (uint32 k = 0; k < m_Pt_out.size() - 1; k++) {
      S = m_Pt_out[k];
      if (k > 0)
        R = m_Pt_out[k-1];
      else
        R = m_Pt_out[m_Pt_out.size()-2];
      T = m_Pt_out[k+1];
      //beta = prodScal((S-R), (S-T)) / (dist(S,R)*dist(S,T));
      beta = acos(prodScal((R-S), (T-S)) / (dist(S,R)*dist(S,T)));
      if (prodCross(R, S, T) < 0)
        beta = 2*XPI - beta;
      d = dist(Tr, S);
      beta = cap(R, S, T);
      Result.push_back(XPt3D(d, beta, k));
    }

    std::sort(Result.begin(), Result.end(), XMatcher_predXPt3DX);
    //if (dist(A, D) < 1.)
    //  break;
    for(uint32 k = 0; k < Result.size(); k++) {
      if (Result[k].X > XMax(dist(Tr, K)*3. , 1.))
        break;
      if ((fabs(alpha - Result[k].Y)) < Max_diff_angle) {
        found = true;
        D = m_Pt_out[(int)Result[k].Z];
        idx_D = (int)Result[k].Z;
        break;
      }
    }
    if (found) {
      m_Homo_in.push_back(K);
      m_Homo_out.push_back(D);
      m_Idx_in.push_back(idx_K);
      m_Idx_out.push_back(idx_D);
    }
  } // endfor j
  return m_Homo_in.size();
}

//-----------------------------------------------------------------------------
// Filtrage des vertex homologues
//-----------------------------------------------------------------------------
bool XMatcher::FilterHomolog()
{
  int positive = 0, negative = 0, min_index = 0;
  for (uint32 i = 1; i < m_Idx_out.size(); i++) {
    if (m_Idx_out[i] < m_Idx_out[min_index])
      min_index = i;
  }
  for (uint32 i = 0; i < m_Idx_out.size() - 1; i++) {
    if (m_Idx_out[(i + 1 + min_index)%m_Idx_out.size()] > m_Idx_out[(i + min_index)%m_Idx_out.size()])
      positive++;
    else
      negative++;
  }
  if ((negative == 0)||(positive == 0)) // Pas de filtrage necessaire
    return true;

  std::vector<int> bad_index;
  bool flag_pos = true;
  if (negative > positive) flag_pos = false;
  for (uint32 i = 0; i < m_Idx_out.size() - 1; i++) {
    if (m_Idx_out[(i + 1 + min_index)%m_Idx_out.size()] > m_Idx_out[(i + min_index)%m_Idx_out.size()]) {
      if (!flag_pos)
        bad_index.push_back((i + 1 + min_index)%m_Idx_out.size());
    } else {
      if (flag_pos)
        bad_index.push_back((i + 1 + min_index)%m_Idx_out.size());
    }
  }

  std::sort(bad_index.begin(), bad_index.end());
  for (int i = 0; i < bad_index.size(); i++) {
    m_Homo_in.erase(m_Homo_in.begin() + bad_index[bad_index.size() - 1 - i]);
    m_Homo_out.erase(m_Homo_out.begin() + bad_index[bad_index.size() - 1 - i]);
    m_Idx_in.erase(m_Idx_in.begin() + bad_index[bad_index.size() - 1 - i]);
    m_Idx_out.erase(m_Idx_out.begin() + bad_index[bad_index.size() - 1 - i]);
  }
  return false;
}

//-----------------------------------------------------------------------------
// Ecriture des vertex homologues
//-----------------------------------------------------------------------------
bool XMatcher::WriteHomolog(std::string id, uint32 part)
{
  if (m_Homo_in.size() != m_Homo_out.size())
    return false;
  XPt2D K, S;
  uint32 nb_vec = 0;
  for (uint32 i = 0; i < m_Homo_in.size(); i++) {
    K = m_Homo_in[i];
    S = m_Homo_out[i];
    if (dist(S, K) > Min_vector_size){
      nb_vec++;
      m_Mif << "PLINE" << std::endl;
      m_Mif << "2" << std::endl;
      m_Mif << K.X << " " << K.Y << std::endl;
      m_Mif << S.X << " " << S.Y << std::endl;
      m_Mid << id << "\t" << part << "\t" << i << std::endl;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
// Ecriture des polygones
//-----------------------------------------------------------------------------
bool XMatcher::WritePoly(std::string id, uint32 part)
{
  m_MifPoly << "REGION 1" << std::endl;
  m_MifPoly << m_Poly_in.size() << std::endl;
  for (uint32 i = 0; i < m_Poly_in.size(); i++)
    m_MifPoly << m_Poly_in[i].X << " " << m_Poly_in[i].Y << std::endl;
  //m_Mif << m_Poly_in[0].X << " " << m_Poly_in[0].Y << std::endl;
  m_MidPoly << id << "\t" << part << "\t" << "0" << std::endl;

  m_MifPoly << "REGION 1" << std::endl;
  m_MifPoly << m_Poly_out.size() << std::endl;
  for (uint32 i = 0; i < m_Poly_in.size(); i++)
    m_MifPoly << m_Poly_out[i].X << " " << m_Poly_out[i].Y << std::endl;
  //m_Mif << m_Poly_out[0].X << " " << m_Poly_out[0].Y << std::endl;
  m_MidPoly << id << "\t" << part << "\t" << "1" << std::endl;

  return true;
}

//-----------------------------------------------------------------------------
// Ecriture des polygones
//-----------------------------------------------------------------------------
bool XMatcher::WritePoly(std::string id)
{
  uint32 start = 0;
  m_MifPoly << "REGION " << m_Part.size() << std::endl;
  for (uint32 i = 0; i < m_Part.size(); i++) {
    m_MifPoly << m_Part[i] << std::endl;
    for (uint32 j = start; j < (start + m_Part[i]); j++) {
      m_MifPoly << m_Part_in[j].X << " " << m_Part_in[j].Y << std::endl;
    }
    start += m_Part[i];
  }
  m_MidPoly << id << "\t" << "0" << "\t" << "0" << std::endl;

  start = 0;
  m_MifPoly << "REGION " << m_Part.size() << std::endl;
  for (uint32 i = 0; i < m_Part.size(); i++) {
    m_MifPoly << m_Part[i] << std::endl;
    for (uint32 j = start; j < (start + m_Part[i]); j++) {
      m_MifPoly << m_Part_out[j].X << " " << m_Part_out[j].Y << std::endl;
    }
    start += m_Part[i];
  }
  m_MidPoly << id << "\t" << "0" << "\t" << "1" << std::endl;

  return true;
}
