//-----------------------------------------------------------------------------
//								XDelaunay.cpp
//								=============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 07/08/2007
//-----------------------------------------------------------------------------

#include "XDelaunay.h"
#include <cfloat>
#include <cstring>
#include <algorithm>

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XDelaunay::XDelaunay()
{
	m_P = NULL;
	m_V = NULL;
	m_nTri = 0;
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XDelaunay::~XDelaunay()
{
	Clear();	
}

//-----------------------------------------------------------------------------
// Destruction des tableaux
//-----------------------------------------------------------------------------
void XDelaunay::Clear()
{
	if (m_P != NULL)
		delete[] m_P;
	m_P = NULL;
	if (m_V != NULL)
		delete[] m_V;
	m_V = NULL;
	m_nTri = 0;
}

//-----------------------------------------------------------------------------
// Ajout d'un vecteur
//-----------------------------------------------------------------------------
bool XDelaunay::AddVector(XGeoVector* V)
{
	Clear();
	double x0 = V->X0(), y0 = V->Y0();
	XGeoVector* P;
	for (uint32 i = 0; i < m_Geom.size(); i++) {
		P = m_Geom[i];
		if ((P->X0() == x0)&&(P->Y0() == y0))
			return false;
	}
	m_Geom.push_back(V);
	return true;
}

//-----------------------------------------------------------------------------
// Renvoie un triangle (sous forme de points)
//-----------------------------------------------------------------------------
bool XDelaunay::Triangle(int i, XPt& A, XPt& B, XPt& C)
{
	if (i >= m_nTri)
		return false;
	A = m_P[m_V[i].p1];
	B = m_P[m_V[i].p2];
	C = m_P[m_V[i].p3];
	return true;
}

//-----------------------------------------------------------------------------
// Renvoie un triangle (sous forme de vecteurs)
//-----------------------------------------------------------------------------
bool XDelaunay::Triangle(int i, XGeoVector** A, XGeoVector** B, XGeoVector** C)
{
	if (i >= m_nTri)
		return false;
	*A = m_Geom[m_V[i].p1];
	*B = m_Geom[m_V[i].p2];
	*C = m_Geom[m_V[i].p3];
	return true;
}

//-----------------------------------------------------------------------------
// Renvoie un triangle (sous forme de vecteurs) a partir d'une position
//-----------------------------------------------------------------------------
int XDelaunay::Triangle(double X, double Y)
{
	XPt A, B, C;
	double fAB, fBC, fCA;
	for (int i = 0; i < m_nTri; i++) {
		A = m_P[m_V[i].p1];
		B = m_P[m_V[i].p2];
		C = m_P[m_V[i].p3];
		if ((X < A.X)&&(X < B.X)&&(X < C.X))
			continue;
		if ((X > A.X)&&(X > B.X)&&(X > C.X))
			continue;
		if ((Y < A.Y)&&(Y < B.Y)&&(Y < C.Y))
			continue;
		if ((Y > A.Y)&&(Y > B.Y)&&(Y > C.Y))
			continue;
		
		fAB = (Y - A.Y)*(B.X - A.X) - (X - A.X)*(B.Y - A.Y);
		fCA = (Y - C.Y)*(A.X - C.X) - (X - C.X)*(A.Y - C.Y);
		fBC = (Y - B.Y)*(C.X - B.X) - (X - B.X)*(C.Y - B.Y);

		 if ( ((fAB * fBC)>0) && ((fBC * fCA) >0))
			 return i;
	}
	return -1;
}

//-----------------------------------------------------------------------------
// Preparation de la triangulation
//-----------------------------------------------------------------------------
bool XDelaunay::Prepare()
{
	Clear();
	// Allocation des tableaux
	m_P = new XPt[m_Geom.size() + 3];
	m_V = new ITRIANGLE[m_Geom.size() * 3];
	if ((m_V == NULL)||(m_P == NULL)) {
		Clear();
		return false;
	}

	std::sort(m_Geom.begin(), m_Geom.end(), predXGeoVectorX);
	// Creation du tableau de points
	XGeoVector* V;
	XPt2D P;
	for (uint32 i = 0; i < m_Geom.size(); i++) {
		V = m_Geom[i];
		if (V->NbPt() < 2) {
			m_P[i].X = V->X0();
			m_P[i].Y = V->Y0();
		} else {
			V->LoadGeom();
			P = V->Pt(0);
			m_P[i].X = P.X;
			m_P[i].Y = P.Y;
			V->Unload();
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
// Indique si le point P est contenue dans le triangle (A1, A2, A3)
//-----------------------------------------------------------------------------
bool XDelaunay::CircumCircle(XPt& P, XPt& A1, XPt& A2, XPt& A3, XPt& C, double& rsqr)
{
   double m1,m2,mx1,mx2,my1,my2;
   double dx,dy,drsqr;
   double fabsy1y2 = fabs(A1.Y - A2.Y);
   double fabsy2y3 = fabs(A2.Y - A3.Y);

   // Check for coincident points
   if (fabsy1y2 < DBL_EPSILON && fabsy2y3 < DBL_EPSILON)
       return(false);

   if (fabsy1y2 < DBL_EPSILON) {
      m2 = - (A3.X - A2.X) / (A3.Y - A2.Y);
      mx2 = (A2.X + A3.X) / 2.0;
      my2 = (A2.Y + A3.Y) / 2.0;
      C.X = (A2.X + A1.X) / 2.0;
      C.Y = m2 * (C.X - mx2) + my2;
   } else if (fabsy2y3 < DBL_EPSILON) {
      m1 = - (A2.X - A1.X) / (A2.Y - A1.Y);
      mx1 = (A1.X + A2.X) / 2.0;
      my1 = (A1.Y + A2.Y) / 2.0;
      C.X = (A3.X + A2.X) / 2.0;
      C.Y = m1 * (C.X - mx1) + my1;
   } else {
      m1 = - (A2.X - A1.X) / (A2.Y - A1.Y);
      m2 = - (A3.X - A2.X) / (A3.Y - A2.Y);
      mx1 = (A1.X + A2.X) / 2.0;
      mx2 = (A2.X + A3.X) / 2.0;
      my1 = (A1.Y + A2.Y) / 2.0;
      my2 = (A2.Y + A3.Y) / 2.0;
      C.X = (m1 * mx1 - m2 * mx2 + my2 - my1) / (m1 - m2);
      if (fabsy1y2 > fabsy2y3) {
         C.Y = m1 * (C.X - mx1) + my1;
      } else {
         C.Y = m2 * (C.X - mx2) + my2;
      }
   }

   dx = A2.X - C.X;
   dy = A2.Y - C.Y;
   rsqr = dx*dx + dy*dy;

   dx = P.X - C.X;
   dy = P.Y - C.Y;
   drsqr = dx*dx + dy*dy;

   return((drsqr <= rsqr) ? true : false);
}

//-----------------------------------------------------------------------------
// Triangulation
//-----------------------------------------------------------------------------
bool XDelaunay::Triangulate()
{
	uint32 nv = m_Geom.size();
  bool* complete = NULL;
  IEDGE* edges = NULL;
  int nedge = 0;
  int trimax,emax = 200;

  bool inside;
	XPt P, A1, A2, A3, C;
  double r;

	if (m_Geom.size() < 1)
		return false;
	if (!Prepare())
		return false;

  // Allocation du tableau de completude et du tableau des bords 
  trimax = 4 * nv;
	complete = new bool[trimax];
	edges = new IEDGE[emax];
	if ((edges == NULL)||(complete == NULL)) {
		delete[] edges;
		delete[] complete;
		return false;
	}

	// Find the maximum and minimum vertex bounds.
  // This is to allow calculation of the bounding triangle
  double xmin = m_P[0].X;
  double ymin = m_P[0].Y;
  double xmax = xmin;
  double ymax = ymin;
  for (int i = 1; i < nv; i++) {
		if (m_P[i].X < xmin) xmin = m_P[i].X;
    if (m_P[i].X > xmax) xmax = m_P[i].X;
    if (m_P[i].Y < ymin) ymin = m_P[i].Y;
    if (m_P[i].Y > ymax) ymax = m_P[i].Y;
  }
  double dx = xmax - xmin;
  double dy = ymax - ymin;
  double dmax = (dx > dy) ? dx : dy;
  double xmid = (xmax + xmin) / 2.0;
  double ymid = (ymax + ymin) / 2.0;

  // Set up the supertriangle
  // This is a triangle which encompasses all the sample points.
  // The supertriangle coordinates are added to the end of the
  // vertex list. The supertriangle is the first triangle in
  // the triangle list.
  m_P[nv+0].X = xmid - 20 * dmax;
  m_P[nv+0].Y = ymid - dmax;
  m_P[nv+1].X = xmid;
  m_P[nv+1].Y = ymid + 20 * dmax;
  m_P[nv+2].X = xmid + 20 * dmax;
	m_P[nv+2].Y = ymid - dmax;
  m_V[0].p1 = nv;
  m_V[0].p2 = nv+1;
  m_V[0].p3 = nv+2;
  complete[0] = false;
  m_nTri = 1;

  // Include each point one at a time into the existing mesh
  for (int i = 0;i < nv; i++) {
		P = m_P[i];
		nedge = 0;

    // Set up the edge buffer.
    // If the point (xp,yp) lies inside the circumcircle then the
    // three edges of that triangle are added to the edge buffer
    // and that triangle is removed.
    
    for (int j = 0; j < m_nTri; j++) {
			if (complete[j])
				continue;
			A1 = m_P[m_V[j].p1];
			A2 = m_P[m_V[j].p2];
			A3 = m_P[m_V[j].p3];

      inside = CircumCircle(P, A1, A2, A3, C, r);
      if (C.X < P.X && ((P.X-C.X)*(P.X-C.X)) > r)
				complete[j] = true;
      if (inside) {
				// Check that we haven't exceeded the edge list size
        if (nedge+3 >= emax) {
					emax += 100;
					IEDGE* ptr = new IEDGE[emax];
					if (ptr == NULL) {
						delete[] edges;
						delete[] complete;
						return false;
					}
					::memcpy(ptr, edges, (emax - 100)*(long)sizeof(IEDGE));
					delete[] edges;
					edges = ptr;
        }
        edges[nedge+0].p1 = m_V[j].p1;
        edges[nedge+0].p2 = m_V[j].p2;
        edges[nedge+1].p1 = m_V[j].p2;
        edges[nedge+1].p2 = m_V[j].p3;
        edges[nedge+2].p1 = m_V[j].p3;
        edges[nedge+2].p2 = m_V[j].p1;
        nedge += 3;
        m_V[j] = m_V[m_nTri-1];
        complete[j] = complete[m_nTri-1];
        m_nTri--;
        j--;
      }
    }

		// Tag multiple edges
    // Note: if all triangles are specified anticlockwise then all
    // interior edges are opposite pointing in direction.
    for (int j = 0; j < nedge-1; j++) {
			for (int k = j+1; k < nedge; k++) {
				if ((edges[j].p1 == edges[k].p2) && (edges[j].p2 == edges[k].p1)) {
					edges[j].p1 = -1;
					edges[j].p2 = -1;
          edges[k].p1 = -1;
          edges[k].p2 = -1;
        }
				// Shouldn't need the following, see note above
        if ((edges[j].p1 == edges[k].p1) && (edges[j].p2 == edges[k].p2)) {
					edges[j].p1 = -1;
          edges[j].p2 = -1;
          edges[k].p1 = -1;
          edges[k].p2 = -1;
        }
      } // endfor k
		} // endfor j

      
        // Form new triangles for the current point
        // Skipping over any tagged edges.
        // All edges are arranged in clockwise order.
      
      for (int j = 0; j < nedge; j++) {
         if (edges[j].p1 < 0 || edges[j].p2 < 0)
            continue;
         if (m_nTri >= trimax) {
					 delete[] edges;
					 delete[] complete;
					 return false;
         }
         m_V[m_nTri].p1 = edges[j].p1;
         m_V[m_nTri].p2 = edges[j].p2;
         m_V[m_nTri].p3 = i;
         complete[m_nTri] = false;
         m_nTri++;
      }
   }

   
	// Remove triangles with supertriangle vertices
  // These are triangles which have a vertex number greater than nv

	for (int i = 0; i < m_nTri; i++) {
		if (m_V[i].p1 >= nv || m_V[i].p2 >= nv || m_V[i].p3 >= nv) {
			m_V[i] = m_V[m_nTri-1];
      m_nTri--;
      i--;
    }
	}

  delete[] edges;
  delete[] complete;
  return true;
}
