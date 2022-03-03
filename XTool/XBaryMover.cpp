//-----------------------------------------------------------------------------
//								XBaryMover.cpp
//								===============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 9/4/2019
//-----------------------------------------------------------------------------

#include "XBaryMover.h"
#include <algorithm>

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XBaryMover::XBaryMover(double epsilon)
{
  m_dEpsilon = epsilon;
}

//-----------------------------------------------------------------------------
// Deplacement d'un point par rapport a un polygone
//-----------------------------------------------------------------------------
XPt2D XBaryMover::Delta(std::vector<XPt2D>& Poly_ini, std::vector<XPt2D>& Poly_fin, XPt2D& T)
{
  if (Poly_ini.size() != Poly_fin.size())
    return XPt2D(0, 0);
  eDelta retour;

  m_TA.clear();
  m_TBX.clear();
  m_TBY.clear();

  XPt2D M_ini, P_ini, M_fin, P_fin;
  for (uint32 i = 0; i < Poly_ini.size(); i++) {
    M_ini = Poly_ini[i];
    M_fin = Poly_fin[i];
    if (i == Poly_ini.size() - 1) {
      P_ini = Poly_ini[0];
      P_fin = Poly_fin[0];
    } else {
      P_ini = Poly_ini[i+1];
      P_fin = Poly_fin[i+1];
    }
    retour = Delta(M_ini, M_fin, P_ini, P_fin, T);
    if (retour == None) // Pas de contribution du segment
      continue;
    if (retour == Interpol) // Interpolation sur le segment
      return m_Delta;
  }

  double SA = 0., SBX = 0., SBY = 0.;
  for (uint32 i = 0; i < m_TA.size(); i++)
    SA += m_TA[i];
  for (uint32 i = 0; i < m_TBX.size(); i++)
    SBX += m_TBX[i];
  for (uint32 i = 0; i < m_TBY.size(); i++)
    SBY += m_TBY[i];

  return XPt2D(SBX / SA, SBY / SA);
}

//-----------------------------------------------------------------------------
// Deplacement d'un point par rapport a un polygone
//-----------------------------------------------------------------------------
XPt2D XBaryMover::Delta(XPt* Poly_ini, XPt* Poly_fin, uint32 nbPt, XPt2D& T)
{
  eDelta retour;

  m_TA.clear();
  m_TBX.clear();
  m_TBY.clear();

  XPt2D M_ini, P_ini, M_fin, P_fin;
  for (uint32 i = 0; i < nbPt; i++) {
    M_ini = Poly_ini[i];
    M_fin = Poly_fin[i];
    if (i == nbPt - 1) {
      P_ini = Poly_ini[0];
      P_fin = Poly_fin[0];
    } else {
      P_ini = Poly_ini[i+1];
      P_fin = Poly_fin[i+1];
    }
    retour = Delta(M_ini, M_fin, P_ini, P_fin, T);
    if (retour == None) // Pas de contribution du segment
      continue;
    if (retour == Interpol) // Interpolation sur le segment
      return m_Delta;
  }

  double SA = 0., SBX = 0., SBY = 0.;
  for (uint32 i = 0; i < m_TA.size(); i++)
    SA += m_TA[i];
  for (uint32 i = 0; i < m_TBX.size(); i++)
    SBX += m_TBX[i];
  for (uint32 i = 0; i < m_TBY.size(); i++)
    SBY += m_TBY[i];

  return XPt2D(SBX / SA, SBY / SA);
}

//-----------------------------------------------------------------------------
// Deplacement d'un point par rapport a un segment
//-----------------------------------------------------------------------------
XBaryMover::eDelta XBaryMover::Delta(XPt2D& M_ini, XPt2D& M_fin, XPt2D& P_ini, XPt2D& P_fin, XPt2D& T)
{
  double distMP = dist(P_ini, M_ini);
  if (distMP < m_dEpsilon)
    return None;

  // Coefficient A, B et C
  double a = dist2(P_ini, M_ini);
  double b = 2. * ( (M_ini.X - T.X)*(P_ini.X - M_ini.X) + (M_ini.Y - T.Y)*(P_ini.Y - M_ini.Y));
  double c = dist2(M_ini, T);

  double delta = 4*a*c - b*b;

  // T, M_ini et P_ini alignes
  if ((delta < m_dEpsilon)&&(delta > -m_dEpsilon)) {
    // Si T est en dehors du segment [M_ini, P_ini]
    if ((dist(T, M_ini) > distMP)||(dist(T, P_ini) > distMP))
      return None;
    // Sinon interpolation simple
    m_Delta = (distMP - dist(M_ini, T))*(M_fin - M_ini) + (distMP - dist(P_ini, T))*(P_fin - P_ini);
    m_Delta /= distMP;
    return Interpol;
  }

  double A0, A1, B0, B1;
  // Termes A0 et A1
  if (delta > m_dEpsilon) {
    double sqd = sqrt(delta);
    A0 = 2. * atan((b) / sqd) / sqd;
    A1 = 2. * atan((2.*a + b) / sqd) / sqd;
  }
  if (delta < -m_dEpsilon) {
    double sqd = sqrt(-delta);
    A0 = -2. * arctanh(b / sqd) / sqd;
    A1 = -2. * arctanh((2.*a + b) / sqd) / sqd;
  }
  m_TA.push_back(distMP * (A1 - A0));

  // Composante X pour B0 et B1
  double d = M_fin.X - M_ini.X;
  double e = (P_fin.X - P_ini.X) - d;

  B0 = e*log(c) / (2*a) + (d - (e*b)/(2*a))*A0;
  B1 = e*log(a+b+c) / (2*a) + ((d - (e*b)/(2*a)))*A1;

  m_TBX.push_back(distMP * (B1 - B0));

  // Composante Y pour B0 et B1
  d = M_fin.Y - M_ini.Y;
  e = (P_fin.Y - P_ini.Y) - d;

  B0 = e*log(c) / (2*a) + (d - (e*b)/(2*a))*A0;
  B1 = e*log(a+b+c) / (2*a) + ((d - (e*b)/(2*a)))*A1;

  m_TBY.push_back(distMP * (B1 - B0));

  return Standard;
}

//-----------------------------------------------------------------------------
// Densification d'un polygone en fonction des points homologues
//    M : polygone avant traitement, P : points homologues, D : densification
//-----------------------------------------------------------------------------
bool XBaryMover::Densifier(std::vector<XPt2D>& M_ini, std::vector<XPt2D>& M_fin,
                           std::vector<XPt2D>& P_ini, std::vector<XPt2D>& P_fin,
                           std::vector<XPt2D>& D_ini, std::vector<XPt2D>& D_fin)
{
  bool orientation = GetOrientation(P_ini);
  if (GetOrientation(M_ini) != orientation)
    ChangeOrientation(M_ini);
  if (GetOrientation(M_fin) != orientation)
    ChangeOrientation(M_fin);

  double epsilon = 1e-6;
  XPt2D A, B, C, U, V, W, T;
  int cmpt_a, cmpt_u;
  for (uint32 i = 0; i < P_ini.size()-1; i++) {
    A = P_ini[i]; U = P_fin[i];
    B = P_ini[i+1]; V = P_fin[i+1];
    cmpt_a = cmpt_u = -1;
    for (uint32 j = 0; j < M_ini.size(); j++) { // Correspondance A
      if (A.egal(M_ini[j], epsilon)) {
        cmpt_a = j;
        break;
      }
    }
    for (uint32 j = 0; j < M_fin.size(); j++) { // Correspondance U
      if (U.egal(M_fin[j], epsilon)) {
        cmpt_u = j;
        break;
      }
    }

    if ((cmpt_a < 0)||(cmpt_u < 0)) {
      continue; // A traiter
    }

    // Recopie
    std::vector<XPt2D> T_ini;
    std::vector<XPt2D> T_fin;
    T_ini.push_back(A); T_fin.push_back(U);
    int step_a = 1, step_u = 1;
    do {
      C = M_ini[(cmpt_a+step_a)%(M_ini.size())];
      W = M_fin[(cmpt_u+step_u)%(M_fin.size())];

      if ((C.egal(B, epsilon)) && (W.egal(V, epsilon)))
        break;
      if (C.egal(W, epsilon)) { // C == W
        /*
        if (!C.egal(B, epsilon)) {
          T_ini.push_back(C);
          A = C;
          step_a++;
        }
        if (!W.egal(V, epsilon)) {
          T_fin.push_back(W);
          U = W;
          step_u++;
        }
        */
        T_ini.push_back(C);
        if (!C.egal(B, epsilon)) {
          A = C;
          step_a++;
        }
        T_fin.push_back(W);
        if (!W.egal(V, epsilon)) {
          U = W;
          step_u++;
        }

        continue;
      }

      if (!C.egal(B, epsilon)) {
        if (U.egal(W, epsilon)) {
          step_u++;
          continue;
        }
        if (proj_in_seg(U, W, C)) {
          T_ini.push_back(C);
          T = proj_seg(U, W, C);
          T_fin.push_back(T);
          A = C;
          U = T;
          step_a++;
          continue;
        }
      }
      if (!W.egal(V, epsilon)) {
        if (A.egal(C, epsilon)) {
          step_a++;
          continue;
        }
        if (proj_in_seg(A, C, W)) {
          T = proj_seg(A, C, W);
          T_ini.push_back(T);
          T_fin.push_back(W);
          A = T;
          U = W;
          step_u++;
          continue;
        }
      }
      /*
      if (!C.egal(B, epsilon)) {
        T_ini.push_back(C);
        step_a++;
      }
      if (!W.egal(V, epsilon)) {
        T_fin.push_back(W);
        step_u++;
      }
      */
      T_ini.push_back(C);
      if (!C.egal(B, epsilon))
        step_a++;
      T_fin.push_back(W);
      if (!W.egal(V, epsilon))
        step_u++;
    } while(true);
    for (uint32 k = 0; k < T_ini.size(); k++)
      D_ini.push_back(T_ini[k]);
    for (uint32 k = 0; k < T_fin.size(); k++)
      D_fin.push_back(T_fin[k]);
  }
  D_ini.push_back(B); D_fin.push_back(V);
  return true;
}

//-----------------------------------------------------------------------------
// Donne le sens d'un polygone (true = sens trigonometrique)
//-----------------------------------------------------------------------------
bool XBaryMover::GetOrientation(std::vector<XPt2D>& V)
{
  // Recherche du point Bottom Left
  double epsilon = 1e-6;
  uint32 botleft = 0;
  XPt2D BL = V[0];
  for (uint32 i = 1; i < V.size()-1; i++){
    if (V[i].Y < BL.Y) {
      BL = V[i];
      botleft = i;
      continue;
    }
    if (fabs(V[i].Y - BL.Y) < epsilon)
      if (V[i].X < BL.X) {
        BL = V[i];
        botleft = i;
      }
  }
  XPt2D A, C;
  if (botleft == 0) {
    A = V[V.size() - 1];
    if (A.egal(BL, epsilon))
      A = V[V.size() - 2];
  } else
    A = V[botleft - 1];
  C = V[(botleft + 1)%V.size()];
  if (C.egal(BL, epsilon))
    C = V[(botleft + 2)%V.size()];
  if (((C.X - BL.X)/dist(C, BL))>((A.X - BL.X)/dist(A, BL)))
    return true;
  return false;
}

//-----------------------------------------------------------------------------
// Change le sens d'un polygone
//-----------------------------------------------------------------------------
void XBaryMover::ChangeOrientation(std::vector<XPt2D>& V)
{
  std::reverse(V.begin(), V.end());
}

//-----------------------------------------------------------------------------
// Donne la densite normalisee des points [0 ; 1]
//-----------------------------------------------------------------------------
double XBaryMover::GetNormDensity(std::vector<XPt2D>& P, std::vector<double>& X)
{
  if (P.size() < 2)
    return 0.;
  double d, tot = 0.;
  X.clear();
  X.push_back(0.);
  for(int i = 0; i < P.size()-1; i++) {
    d = dist(P[i], P[i+1]);
    X.push_back(tot + d);
    tot += d;
  }
  if (tot == 0.)
    return 0.;
  for(int i = 0; i < X.size(); i++)
    X[i] = X[i] / tot;
  X[X.size()-1] = 1.0;
  return tot;
}

//-----------------------------------------------------------------------------
// Densifie une polyligne
//-----------------------------------------------------------------------------
bool XBaryMover::Densify(std::vector<XPt2D>& P_in, std::vector<XPt2D>& P_out, std::vector<double>& X_out)
{
  double x, tot = 0., alpha;
  XPt2D A, B, C;
  std::vector<double> X_in;
  tot = GetNormDensity(P_in, X_in);

  P_out.clear();
  P_out.push_back(P_in[0]);
  int index;
  for(int i = 0; i < X_out.size(); i++) {
    if (fabs(X_out[i]) < m_dEpsilon)
      continue;
    index = 0;
    x = X_out[i];
    for(int j = 0; j < X_in.size(); j++) {
      if (x <= X_in[j] + m_dEpsilon) {
        index = j;
        break;
      }
    }
    if (index == 0)
      return false;
    if (fabs(x - X_in[index]) < m_dEpsilon) {
      P_out.push_back(P_in[index]);
      continue;
    }
    A = P_in[index - 1];
    B = P_in[index];

    alpha = (x - X_in[index-1]) * tot;
    C = A + (B - A)*alpha / dist(A, B);
    P_out.push_back(C);
  }
  return true;
}

//-----------------------------------------------------------------------------
// Densification d'un polygone en fonction des points homologues
//    M : polygone avant traitement, P : points homologues, D : densification
//-----------------------------------------------------------------------------
bool XBaryMover::XDensifier(std::vector<XPt2D>& M_ini, std::vector<XPt2D>& M_fin,
                            std::vector<XPt2D>& P_ini, std::vector<XPt2D>& P_fin,
                            std::vector<XPt2D>& D_ini, std::vector<XPt2D>& D_fin)
{
  bool orientation = GetOrientation(P_ini);
  if (GetOrientation(M_ini) != orientation)
    ChangeOrientation(M_ini);
  if (GetOrientation(M_fin) != orientation)
    ChangeOrientation(M_fin);

  XPt2D A, B, C, U, V, W, T;
  int cmpt_a, cmpt_u;
  std::vector<XPt2D> Poly_ini, Poly_fin;
  std::vector<double> X_ini, X_fin, X_tot;
  for (uint32 i = 0; i < P_ini.size()-1; i++) {
    Poly_ini.clear();
    Poly_fin.clear();
    X_ini.clear();
    X_fin.clear();
    X_tot.clear();

    // Creation des polylignes
    A = P_ini[i]; U = P_fin[i];
    B = P_ini[i+1]; V = P_fin[i+1];
    cmpt_a = cmpt_u = -1;
    for (uint32 j = 0; j < M_ini.size(); j++) { // Correspondance A
      if (A.egal(M_ini[j], m_dEpsilon)) {
        cmpt_a = j;
        break;
      }
    }
    for (uint32 j = 0; j < M_fin.size(); j++) { // Correspondance U
      if (U.egal(M_fin[j], m_dEpsilon)) {
        cmpt_u = j;
        break;
      }
    }

    if ((cmpt_a < 0)||(cmpt_u < 0)) {
      continue; // A traiter
    }

    int step_a = 1, step_u = 1;
    Poly_ini.push_back(A);
    do {
      C = M_ini[(cmpt_a+step_a)%(M_ini.size())];
      Poly_ini.push_back(C);
      if (C.egal(B, m_dEpsilon))
        break;
      step_a++;
    } while(true);

    Poly_fin.push_back(U);
    do {
      W = M_fin[(cmpt_u+step_u)%(M_fin.size())];
      Poly_fin.push_back(W);
      if (W.egal(V, m_dEpsilon))
        break;
      step_u++;
    } while(true);

    // Calcul des densites
    double tot_ini = GetNormDensity(Poly_ini, X_ini);
    double tot_fin = GetNormDensity(Poly_fin, X_fin);
    X_tot.resize(X_ini.size() + X_fin.size(), 0.);
    std::merge(X_ini.begin(), X_ini.end(), X_fin.begin(), X_fin.end(), X_tot.begin());
    std::vector<double>::iterator iter = std::unique(X_tot.begin(), X_tot.end());
    X_tot.resize( std::distance(X_tot.begin(),iter) );

    std::vector<XPt2D> T_ini, T_fin;
    Densify(Poly_ini, T_ini, X_tot);
    Densify(Poly_fin, T_fin, X_tot);

    if (T_ini.size() != T_fin.size()) // Probleme ...
      continue;

    // Premier point
    if (D_ini.size() < 1)
      D_ini.push_back(T_ini[0]);
    if (D_fin.size() < 1)
      D_fin.push_back(T_fin[0]);

    for (uint32 j = 1; j < T_ini.size(); j++) {
      D_ini.push_back(T_ini[j]);
      D_fin.push_back(T_fin[j]);
    }


  }

  return true;
}
