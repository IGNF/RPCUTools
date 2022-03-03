//-----------------------------------------------------------------------------
//								XGeoMemVector.cpp
//								=================
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 22/09/2004
//-----------------------------------------------------------------------------

#include <cstring>
#include <sstream>
#include "XGeoMemVector.h"
	
//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XGeoMemVector::XGeoMemVector()
{
	m_Type = Null;
	m_nNumPoints = 0;
	m_Pt = NULL;
	m_nNumParts = 0;
	m_Parts = NULL;
	m_Z = NULL;
	m_ZRange = NULL;
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XGeoMemVector::~XGeoMemVector()
{
	Clear();
}

//-----------------------------------------------------------------------------
// Re-initialisation de l'objet
//-----------------------------------------------------------------------------
void XGeoMemVector::Clear()
{
	if (m_Pt != NULL)
		delete[] m_Pt;
	m_Pt = NULL;
	if (m_Parts != NULL)
		delete[] m_Parts;
	m_Parts = NULL;
	if (m_Z != NULL)
		delete[] m_Z;
	m_Z = NULL;
	if (m_ZRange != NULL)
		delete[] m_ZRange;
  if (m_Repres != NULL)
    delete m_Repres;
  m_Repres = NULL;
	m_ZRange = NULL;
	m_Type = Null;
	m_nNumPoints = 0;
	m_nNumParts = 0;	
}

//-----------------------------------------------------------------------------
// Recuperation d'un point XY de la geometrie
//-----------------------------------------------------------------------------
/*
XPt2D XGeoMemVector::Pt(uint32 i)
{ 
  if (((int)i < m_nNumPoints)&&(m_Pt != NULL))
		return XPt2D(m_Pt[i].X, m_Pt[i].Y);
	return XPt2D(XGEO_NO_DATA, XGEO_NO_DATA);
}
*/
//-----------------------------------------------------------------------------
// Recuperation d'un Z de la geometrie
//-----------------------------------------------------------------------------
/*
double XGeoMemVector::Z(uint32 i)
{
  if (((int)i < m_nNumPoints)&&(m_Z != NULL))
		return m_Z[i];
	return XGEO_NO_DATA;
}
*/
//-----------------------------------------------------------------------------
// Clonage d'un vecteur
//-----------------------------------------------------------------------------
bool XGeoMemVector::Clone(XGeoVector* V)
{
  Clear();

  m_Frame = V->Frame();
  m_nNumPoints = V->NbPt();
  m_nNumParts = V->NbPart();

  // Allocation des tableaux
  m_Pt = new XPt[m_nNumPoints];
  m_Z = new double[m_nNumPoints];
  m_ZRange = new double[2];
  m_Parts = new int[m_nNumParts];
  if ((m_Pt == NULL)||(m_Z == NULL)||(m_Parts == NULL)||(m_ZRange == NULL)) {
    Clear();
    return false;
  }

  // Copie des donnees
  if (V->NbPt() > 1)
    ::memcpy(m_Pt, V->Pt(), sizeof(XPt) * m_nNumPoints);
  else {
    m_Pt[0].X = V->Pt(0).X;  m_Pt[0].Y = V->Pt(0).Y;
  }
  if (V->Parts() != NULL)
    ::memcpy(m_Parts, V->Parts(), sizeof(int) * m_nNumParts);
  else
    ::memset(m_Parts, 0, m_nNumParts * sizeof(int));

  if (V->Is3D()) {
    if (V->NbPt() > 1)
      ::memcpy(m_Z, V->Z(), sizeof(double) * m_nNumPoints);
    else
      m_Z[0] = V->Z(0);
    m_ZRange[0] = V->Zmin();
    m_ZRange[1] = V->Zmax();
  } else {
    ::memset(m_Z, 0, m_nNumPoints * sizeof(double));
    m_ZRange[0] = m_ZRange[1] = 0;
  }
  m_Attrib.clear();
  V->ReadAttributes(m_Attrib);

  if (V->NbPt() == 1) {
    m_Type = XGeoVector::PointZ;
    return true;
  }
  if (V->Is3D()) {
    m_Type = V->TypeVector();
    return true;
  }
  if (V->TypeVector() == XGeoVector::Line) m_Type = XGeoVector::LineZ;
  if (V->TypeVector() == XGeoVector::Poly) m_Type = XGeoVector::PolyZ;
  if (V->TypeVector() == XGeoVector::MLine) m_Type = XGeoVector::MLineZ;
  if (V->TypeVector() == XGeoVector::MPoly) m_Type = XGeoVector::MPolyZ;

  return true;
}
//-----------------------------------------------------------------------------
// Test si la geometrie est proche d'un point
//-----------------------------------------------------------------------------
/*
bool XGeoMemVector::IsNear2D(const XPt2D& P, double dist)
{
  if (!XGeoVector::IsNear2D(P, dist))
    return false;

  double d2 = dist * dist;
  double d, am, ab, prod;
  uint32 i, j, k;
  double minx, maxx, miny, maxy;
  if (!LoadGeom())
    return false;
  for (i = 0; i < m_nNumParts; i++) {
    if (i == m_nNumParts - 1)
      k = m_nNumPoints;
    else
      k = m_Parts[i + 1];
    for (j = m_Parts[i]; j < k - 1; j++) {
      minx = XMin(m_Pt[j].X, m_Pt[j + 1].X);
      if (P.X < minx - dist)
        continue;
      maxx = XMax(m_Pt[j].X, m_Pt[j + 1].X);
      if (P.X > maxx + dist)
        continue;
      miny = XMin(m_Pt[j].Y, m_Pt[j + 1].Y);
      if (P.Y < miny - dist)
        continue;
      maxy = XMax(m_Pt[j].Y, m_Pt[j + 1].Y);
      if (P.Y > maxy + dist)
        continue;

      prod = (m_Pt[j+1].X - m_Pt[j].X)*(P.X - m_Pt[j].X) + (m_Pt[j+1].Y - m_Pt[j].Y)*(P.Y - m_Pt[j].Y);
      am = (P.X - m_Pt[j].X)*(P.X - m_Pt[j].X) + (P.Y - m_Pt[j].Y)*(P.Y - m_Pt[j].Y);
      ab = (m_Pt[j+1].X - m_Pt[j].X)*(m_Pt[j+1].X - m_Pt[j].X) +
            (m_Pt[j+1].Y - m_Pt[j].Y)*(m_Pt[j+1].Y - m_Pt[j].Y);
      if (ab == 0)
        continue;
      d = am - (prod * prod) / ab;
      if (d < d2){
        Unload();
        return true;
        }
      }
    }
  Unload();
  return false;
}
*/
//-----------------------------------------------------------------------------
// Lecture XML
//-----------------------------------------------------------------------------
bool XGeoMemVector::XmlRead(XParserXML* parser, uint32 num, XError* error)
{
	Clear();
	XParserXML vec = parser->FindSubParser("/xgeovector", num);
	
	m_strLayer = vec.ReadNode("/xgeovector/layer");
	m_strClass = vec.ReadNode("/xgeovector/class");
	m_Type = (XGeoVector::eTypeVector)vec.ReadNodeAsInt("/xgeovector/type");
	m_nNumPoints = vec.ReadNodeAsInt("/xgeovector/nb_pt");
	m_nNumParts = vec.ReadNodeAsInt("/xgeovector/nb_part");

	XParserXML frame = parser->FindSubParser("/xgeovector/frame");
  m_Frame.XmlRead(&frame);

	// Recuperation des parties
	if (m_nNumParts > 0) {
		std::vector<int> Part;
		vec.ReadArrayNodeAsInt("/xgeovector/part", &Part);
		if (Part.size() != m_nNumParts)
			return false;

		if (m_Parts != NULL) 
			delete[] m_Parts;
		m_Parts = new int[m_nNumParts];
		if (m_Parts == NULL) {
			Clear();
			return false;
		}
		for (int i = 0; i < m_nNumParts; i++)
			m_Parts[i] = Part[i];
	}

	// Recuperation des points
	std::string Pt;
	Pt = vec.ReadNode("/xgeovector/pt");
	if (m_Pt != NULL) 
		delete[] m_Pt;
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Clear();
		return false;
	}
	std::istringstream in;
	in.str(Pt.c_str());
	for (int i = 0; i < m_nNumPoints; i++)
		in >> m_Pt[i].X >> m_Pt[i].Y;

	// Recuperation des attributs
	m_Attrib.clear();
	XParserXML att_list = parser->FindSubParser("/xgeovector/attribut_list", num);
	uint32 nb_att = 0;
	XParserXML att;
	std::string name, value;
	while(true){
		att = att_list.FindSubParser("/attribut_list/attribut", nb_att);
		if (att.IsEmpty())
			break;
		name = att.ReadNode("/attribut/name");
		value = att.ReadNode("/attribut/value");
		m_Attrib.push_back(name);
		m_Attrib.push_back(value);
		nb_att++;
	}

	// Recuperation de la representation
	XParserXML repres = parser->FindSubParser("/xgeovector/xgeorepres", num);
	if (!repres.IsEmpty()) {
    if (m_Repres != NULL)
      delete m_Repres;
    m_Repres = new XGeoRepres;
    if (m_Repres != NULL)
      m_Repres->XmlRead(&repres);
	}
	
	return true;
}

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XGeoMemPoint3D::XGeoMemPoint3D() : XGeoPoint3D() 
{ 
	Editable(true); 
	m_Attrib.push_back("Nom");
	m_Attrib.push_back("");
	m_Attrib.push_back("Importance");
	m_Attrib.push_back("10");
} 

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XGeoMemPoint3D::~XGeoMemPoint3D()
{
	if (m_Repres != NULL)
		delete m_Repres;
	m_Repres = NULL;	
}

//-----------------------------------------------------------------------------
// Rotation du ponctuel
//-----------------------------------------------------------------------------
uint16 XGeoMemPoint3D::Rotation()
{
	std::string str = FindAttribute("rotation", true);
	if (str.size() < 1)
		return 0;
	int rot;
	sscanf(str.c_str(), "%d", &rot);
	return rot;
}

//-----------------------------------------------------------------------------
// Ajout d'un point
//-----------------------------------------------------------------------------
bool XGeoMemPoint3D::AddPoint(double x, double y, double z)
{
	m_Frame.Xmin = m_Frame.Xmax = x;
	m_Frame.Ymin = m_Frame.Ymax = y;
	m_Z = z;
	return true;
}

//-----------------------------------------------------------------------------
// Clonage a partir d'une geometrie
//-----------------------------------------------------------------------------
bool XGeoMemPoint3D::Clone(XGeoVector* V)
{
	if (V->NbPt() < 1)
		return false;
	XPt2D P = V->Pt(0);
	m_Frame.Xmin = m_Frame.Xmax = P.X;
	m_Frame.Ymin = m_Frame.Ymax = P.Y;
	m_Z = XGEO_NO_DATA;
	if (V->Is3D())
		m_Z = V->Z(0);
	m_Attrib.clear();
	V->ReadAttributes(m_Attrib);
	// Representation
	if (V->Repres() == NULL)
		return true;
	bool newRepres = false;
	if (V->Class() == NULL) 
		newRepres = true;
	if ((V->Class() != NULL))
		if (V->Repres() != V->Class()->Repres()) 
			newRepres = true;
	if (newRepres) {
		XGeoRepres* repres = new XGeoRepres;
		*repres = *(V->Repres());
		m_Repres = repres;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XGeoMemLine3D::~XGeoMemLine3D()
{
	m_bLock = false;
	Unload();
	if (m_Repres != NULL)
		delete m_Repres;
	m_Repres = NULL;
}

//-----------------------------------------------------------------------------
// Dechargement de la geometrie
//-----------------------------------------------------------------------------
void XGeoMemLine3D::Unload()
{
	if (m_bLock)
		return;
	XGeoLine3D::Unload();
}

//-----------------------------------------------------------------------------
// Reinitialisation de la geometrie
//-----------------------------------------------------------------------------
bool XGeoMemLine3D::Clear()
{
	m_bLock = false;
	Unload();
	m_nNumPoints = 0;
	m_bLock = true;
	return true;
}

//-----------------------------------------------------------------------------
// Ajout d'un point
//-----------------------------------------------------------------------------
bool XGeoMemLine3D::AddPoint(double x, double y, double z)
{
	if (m_Pt == NULL) {
		m_Pt = new XPt[1];
		m_Pt[0].X = x;
		m_Pt[0].Y = y;
		m_Z = new double[1];
		m_Z[0] = z;
		m_ZRange = new double[2];
		m_ZRange[0] = m_ZRange[1] = z;
		m_nNumPoints = 1;
		m_Frame.Xmin = m_Frame.Xmax = x;
		m_Frame.Ymin = m_Frame.Ymax = y;
		return true;
	}
	XPt* P = new XPt[m_nNumPoints + 1];
	if (P == NULL)
		return false;
	double* Z = new double[m_nNumPoints + 1];
	if (Z == NULL) {
		delete[] P;
		return false;
	}
	::memcpy(P, m_Pt, m_nNumPoints * sizeof(XPt));
	::memcpy(Z, m_Z, m_nNumPoints * sizeof(double));
	P[m_nNumPoints].X = x;
	P[m_nNumPoints].Y = y;
	Z[m_nNumPoints] = z;
	m_nNumPoints++;

	m_Frame += XPt2D(x, y);
	m_ZRange[0] = XMin(m_ZRange[0], z);
	m_ZRange[1] = XMax(m_ZRange[1], z);

	delete[] m_Pt;
	delete[] m_Z;

	m_Pt = P;
	m_Z = Z;

	return true;
}

//-----------------------------------------------------------------------------
// Destruction du dernier point
//-----------------------------------------------------------------------------
bool XGeoMemLine3D::RemoveLastPoint()
{
	if (m_nNumPoints > 1) {
		m_nNumPoints--;
		return true;
	}
	Clear();
	return true;
}

//-----------------------------------------------------------------------------
// Clonage a partir d'une geometrie
//-----------------------------------------------------------------------------
bool XGeoMemLine3D::Clone(XGeoVector* V)
{
	if (V->NbPt() < 2)
		return false;
	m_Frame = V->Frame();
	m_nNumPoints = V->NbPt();
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL)
		return false;
	m_Z = new double[m_nNumPoints];
	if (m_Z == NULL) {
		delete[] m_Pt;
		m_Pt = NULL;
		return false;
	}
	m_ZRange = new double[2];
	::memcpy(m_Pt, V->Pt(), sizeof(XPt) * m_nNumPoints);
	
	if (V->Is3D()) {
		::memcpy(m_Z, V->Z(), sizeof(double) * m_nNumPoints);
		m_ZRange[0] = V->Zmin();
		m_ZRange[1] = V->Zmax();
	} else {
		::memset(m_Z, 0, m_nNumPoints * sizeof(double));
		m_ZRange[0] = m_ZRange[1] = 0;
	}
	m_Attrib.clear();
	V->ReadAttributes(m_Attrib);
	// Representation
	if (V->Repres() == NULL)
		return true;
	bool newRepres = false;
	if (V->Class() == NULL) 
		newRepres = true;
	if ((V->Class() != NULL))
		if (V->Repres() != V->Class()->Repres()) 
			newRepres = true;
	if (newRepres) {
		XGeoRepres* repres = new XGeoRepres;
		*repres = *(V->Repres());
		m_Repres = repres;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XGeoMemPoly3D::~XGeoMemPoly3D()
{
	m_bLock = false;
	Unload();
	if (m_Repres != NULL)
		delete m_Repres;
	m_Repres = NULL;
}

//-----------------------------------------------------------------------------
// Dechargement de la geometrie
//-----------------------------------------------------------------------------
void XGeoMemPoly3D::Unload()
{
	if (m_bLock)
		return;
	XGeoPoly3D::Unload();
}

//-----------------------------------------------------------------------------
// Ajout d'un point
//-----------------------------------------------------------------------------
bool XGeoMemPoly3D::AddPoint(double x, double y, double z)
{
	if (m_Pt == NULL) {
		m_Pt = new XPt[1];
		m_Pt[0].X = x;
		m_Pt[0].Y = y;
		m_Z = new double[1];
		m_Z[0] = z;
		m_ZRange = new double[2];
		m_ZRange[0] = m_ZRange[1] = z;
		m_nNumPoints = 1;
		m_Frame.Xmin = m_Frame.Xmax = x;
		m_Frame.Ymin = m_Frame.Ymax = y;
		return true;
	}
	XPt* P = new XPt[m_nNumPoints + 1];
	if (P == NULL)
		return false;
	double* Z = new double[m_nNumPoints + 1];
	if (Z == NULL) {
		delete[] P;
		return false;
	}
	::memcpy(P, m_Pt, m_nNumPoints * sizeof(XPt));
	::memcpy(Z, m_Z, m_nNumPoints * sizeof(double));
	P[m_nNumPoints].X = x;
	P[m_nNumPoints].Y = y;
	Z[m_nNumPoints] = z;
	m_nNumPoints++;

	m_Frame += XPt2D(x, y);
	m_ZRange[0] = XMin(m_ZRange[0], z);
	m_ZRange[1] = XMax(m_ZRange[1], z);

	delete[] m_Pt;
	delete[] m_Z;

	m_Pt = P;
	m_Z = Z;

	return true;
}

//-----------------------------------------------------------------------------
// Clonage a partir d'une geometrie
//-----------------------------------------------------------------------------
bool XGeoMemPoly3D::Clone(XGeoVector* V)
{
	if (V->NbPt() < 3)
		return false;
	m_Frame = V->Frame();
	m_nNumPoints = V->NbPt();
	bool endpoint = true;
	if (V->Pt(0) != V->Pt(m_nNumPoints - 1)) {
		endpoint = false;
		m_nNumPoints++;
	}

	// Allocation des tableaux
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL)
		return false;
	m_Z = new double[m_nNumPoints];
	if (m_Z == NULL) {
		delete[] m_Pt;
		m_Pt = NULL;
		return false;
	}
	m_ZRange = new double[2];

	// Copie des donnees
	if (endpoint)
		::memcpy(m_Pt, V->Pt(), sizeof(XPt) * m_nNumPoints);
	else {
		::memcpy(m_Pt, V->Pt(), sizeof(XPt) * (m_nNumPoints - 1));
		::memcpy(&m_Pt[m_nNumPoints - 1], V->Pt(), sizeof(XPt));
	}

	
	if (V->Is3D()) {
		if (endpoint)
			::memcpy(m_Z, V->Z(), sizeof(double) * m_nNumPoints);
		else {
			::memcpy(m_Z, V->Z(), sizeof(double) * (m_nNumPoints - 1));
			::memcpy(&m_Z[m_nNumPoints - 1], V->Z(), sizeof(double));		
		}
		m_ZRange[0] = V->Zmin();
		m_ZRange[1] = V->Zmax();
	} else {
		::memset(m_Z, 0, m_nNumPoints * sizeof(double));
		m_ZRange[0] = m_ZRange[1] = 0;
	}
	m_Attrib.clear();
	V->ReadAttributes(m_Attrib);
	// Representation
	if (V->Repres() == NULL)
		return true;
	bool newRepres = false;
	if (V->Class() == NULL) 
		newRepres = true;
	if ((V->Class() != NULL))
		if (V->Repres() != V->Class()->Repres()) 
			newRepres = true;
	if (newRepres) {
		XGeoRepres* repres = new XGeoRepres;
		*repres = *(V->Repres());
		m_Repres = repres;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XGeoMemMPoly2D::~XGeoMemMPoly2D()
{
	m_bLock = false;
	Unload();
	if (m_Repres != NULL)
		delete m_Repres;
	m_Repres = NULL;
}

//-----------------------------------------------------------------------------
// Dechargement de la geometrie
//-----------------------------------------------------------------------------
void XGeoMemMPoly2D::Unload()
{
	if (m_bLock)
		return;
	XGeoMPoly2D::Unload();
}

//-----------------------------------------------------------------------------
// Preparation de la geometrie
//-----------------------------------------------------------------------------
bool XGeoMemMPoly2D::PrepareGeom(uint32 nbPart, uint32 nbPt)
{
	if (m_nNumPoints > 0)
    return false;
	m_Pt = new XPt[nbPt];
	if (m_Pt == NULL)
		return false;
	m_Parts = new int[nbPart];
	if (m_Parts == NULL) {
		delete m_Pt;
		return false;
	}
	m_nNumPoints = nbPt;
	m_nNumParts = nbPart;

	m_Frame = XFrame();

	return true;
}

//-----------------------------------------------------------------------------
// Ajout d'un point
//-----------------------------------------------------------------------------
bool XGeoMemMPoly2D::SetPoint(double x, double y, uint32 pos)
{
	if (pos >= m_nNumPoints)
		return false;
	m_Pt[pos].X = x;
	m_Pt[pos].Y = y;
	m_Frame += XPt2D(x, y);
	return true;
}

//-----------------------------------------------------------------------------
// Ajout d'une partie
//-----------------------------------------------------------------------------
bool XGeoMemMPoly2D::SetPart(int start, uint32 pos)
{
	if (pos >= m_nNumParts)
		return false;
	m_Parts[pos] = start;
	return true;
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XGeoMemMPoly3D::~XGeoMemMPoly3D()
{
  m_bLock = false;
  Unload();
  if (m_Repres != NULL)
    delete m_Repres;
  m_Repres = NULL;
}

//-----------------------------------------------------------------------------
// Dechargement de la geometrie
//-----------------------------------------------------------------------------
void XGeoMemMPoly3D::Unload()
{
  if (m_bLock)
    return;
  XGeoMPoly3D::Unload();
}
