//-----------------------------------------------------------------------------
//								XGeocon.cpp
//								===========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 14/08/2003
//-----------------------------------------------------------------------------

#include <cstring>
#include <sstream>

#include "XGeoBase.h"
#include "XGeocon.h"
#include "XPath.h"
#include "XLambert.h"

int			XGeocon::m_nSize;
char*		XGeocon::m_Line = NULL;
XPt*		XGeocon::m_Pt = NULL;
double*	XGeocon::m_Z = NULL;
int*		XGeocon::m_Parts = NULL;

#define GEOCON_MAX_POINT		120000

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XGeocon::XGeocon()
{
	PrepareGeocon();
	m_dZmin = m_dZmax = XGEO_NO_DATA;
	m_strName = "GeoConcept";
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XGeocon::~XGeocon()
{
	delete[] m_Line;
	m_Line = NULL;
	delete[] m_Pt;
	m_Pt = NULL;
	delete[] m_Z;
	m_Z = NULL;
	delete[] m_Parts;
	m_Parts = NULL;
}

//-----------------------------------------------------------------------------
// Import d'un fichier GeoConcept
//-----------------------------------------------------------------------------
bool XGeocon::ImportGeoconcept(XGeoBase* base, const char* filename, XGeoMap* map)
{
  XGeocon* file = new XGeocon;
  if (file->Read(filename, base)) {
    if (map == NULL)
      base->AddMap(file);
    else
      map->AddObject(file);
  } else {
    delete file;
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
// Preparation des buffers de lecture
//-----------------------------------------------------------------------------
void XGeocon::PrepareGeocon()
{
	m_nSize = GEOCON_MAX_POINT * 30;
	if (m_Pt == NULL)
		m_Pt = new XPt[GEOCON_MAX_POINT];
	if (m_Line == NULL)
		m_Line = new char[m_nSize];
	if (m_Z == NULL)
		m_Z = new double[GEOCON_MAX_POINT];
	if (m_Parts == NULL)
		m_Parts = new int[200];
}

//-----------------------------------------------------------------------------
// Lecture d'un fichier GeoConcept
//-----------------------------------------------------------------------------
bool XGeocon::Read(const char* filename, XGeoBase* base, XError* error)
{
	XPath P;
	m_strFilename = filename;
	m_strName = P.Name(filename);
	m_In.open(filename, std::ios_base::in| std::ios_base::binary);
	if (!m_In.good())
		return XErrorError(error, "XGeocon::Read", XError::eIOOpen);

	XGeoconPoint2D* point2D;
	XGeoconPoint3D* point3D;
	XGeoconLine2D* line2D;
	XGeoconLine3D* line3D;
	XGeoconPoly2D* poly2D;
	XGeoconPoly3D* poly3D;
	XGeoconMPoly2D* polyM2D;
	XGeoconMPoly3D* polyM3D;

	XGeoClass* C;
	m_b3D = false;

	while( (!m_In.eof()) && (m_In.good()) ) {
		uint32 pos = m_In.tellg();
		if (!AnalyseLine())
			XErrorError(error, "XGeocon::Read", XError::eBadFormat);
		if (m_Type == Null)
			continue;

		C = base->AddClass(m_strType.c_str(), m_strSousType.c_str());

		// Recherche du schema
		if (C->NbVector() < 1) {
			for (uint32 i = 0; i < m_Schema.size(); i++) {
				if (C->Name() == m_Schema[i].Name()) {
					C->SetSchema(m_Schema[i]);
					break;
				}
			}
		}

		m_F.Xmax = m_F.Xmin = m_Pt[0].X;
		m_F.Ymax = m_F.Ymin = m_Pt[0].Y;
		for (uint32 i = 1; i < m_nNbPt; i++) {
			m_F.Xmin = XMin(m_F.Xmin, m_Pt[i].X);
			m_F.Ymin = XMin(m_F.Ymin, m_Pt[i].Y);
			m_F.Xmax = XMax(m_F.Xmax, m_Pt[i].X);
			m_F.Ymax = XMax(m_F.Ymax, m_Pt[i].Y);
		}
		m_Frame += m_F;

		// Calcul du Zmin et du Zmax
		if (m_b3D) 
			ComputeZRange();

		// Creation des objets ponctuels
		if (m_Type == Point) {
			if (m_b3D) {
				point3D = new XGeoconPoint3D(this, pos);
				point3D->SetGeom(m_Pt, m_Z);
				m_Data.push_back(point3D);
				point3D->Class(C);
				C->Vector(point3D);
				m_b3D = false;
				point3D->Unload();
			} else {
				point2D = new XGeoconPoint2D(this, pos);
				point2D->SetGeom(m_Pt);
				m_Data.push_back(point2D);
				point2D->Class(C);
				C->Vector(point2D);
				point2D->Unload();
			}
		}

		// Creation des objets lineaires
		if (m_Type == Line) {
			if (m_b3D) {
				line3D = new XGeoconLine3D(this, pos);
				line3D->SetGeom(m_Pt, m_Z, m_nNbPt, &m_F, m_ZRange);
				m_Data.push_back(line3D);
				line3D->Class(C);
				C->Vector(line3D);
				m_b3D = false;
				line3D->Unload();
			} else {
				line2D = new XGeoconLine2D(this, pos);
				line2D->SetGeom(m_Pt, m_nNbPt, &m_F);
				m_Data.push_back(line2D);
				line2D->Class(C);
				C->Vector(line2D);
				line2D->Unload();
			}
		}

		// Creation des objets polygones simples
		if ((m_Type == Polygon)&&(m_nNumParts < 1)) {
			if (m_b3D) {
				poly3D = new XGeoconPoly3D(this, pos);
				poly3D->SetGeom(m_Pt, m_Z, m_nNbPt, &m_F, m_ZRange);
				m_Data.push_back(poly3D);
				poly3D->Class(C);
				C->Vector(poly3D);
				m_b3D = false;
				poly3D->Unload();
			} else {
				poly2D = new XGeoconPoly2D(this, pos);
				poly2D->SetGeom(m_Pt, m_nNbPt, &m_F);
				m_Data.push_back(poly2D);
				poly2D->Class(C);
				C->Vector(poly2D);
				poly2D->Unload();
			}
		}

		// Creation des objets polygones multiples
		if ((m_Type == Polygon)&&(m_nNumParts > 0)) {
			if (m_b3D) {
				polyM3D = new XGeoconMPoly3D(this, pos);
				polyM3D->SetGeom(m_Pt, m_Z, m_nNbPt, &m_F, m_ZRange, m_Parts, &m_nNumParts);
				m_Data.push_back(polyM3D);
				polyM3D->Class(C);
				C->Vector(polyM3D);
				m_b3D = false;
				polyM3D->Unload();
			} else {
				polyM2D = new XGeoconMPoly2D(this, pos);
				polyM2D->SetGeom(m_Pt, m_nNbPt, &m_F, m_Parts, &m_nNumParts);
				m_Data.push_back(polyM2D);
				polyM2D->Class(C);
				C->Vector(polyM2D);
				polyM2D->Unload();
			}
		}
		
	}
	m_In.clear();
	return true;
}

//-----------------------------------------------------------------------------
// Lecture des attributs
//-----------------------------------------------------------------------------
bool XGeocon::ReadAttributes(uint32 pos, std::vector<std::string>& V, XGeoClass* C)
{
	m_In.seekg(pos);
	m_In.getline(m_Line, m_nSize);
	
	V.clear();
	// Recherche des commentaires 
	if ((m_Line[0] == '/')&&(m_Line[1] == '/')) {
		if (strncmp(m_Line, "//$3DOBJECT", 11) == 0)
			m_b3D = true;
		m_Type = Null;
		return false;
	}
	if (strlen(m_Line) < 2) {
		m_Type = Null;
		return false;
	}

	std::istringstream S;
	S.str(m_Line);

	char sep;
	char buf[1024], att[64];
	std::string att_name;

	S >> m_nId;
	S.get(sep);

	S.getline(buf, 1024, sep);
	m_strType = buf;
	S.getline(buf, 1024, sep);
	m_strSousType = buf;
	S.getline(buf, 1024, sep);
	m_strObName = buf;

//	if (m_strObName.size() > 0) {
		V.push_back("Nom");
		V.push_back(m_strObName);
//	}

	// Lecture des attributs
	S.getline(buf, 1024, sep);
	sscanf(buf, "%d", &m_nNbAtt);
	bool header = true;
	for (int i = 0; i < m_nNbAtt; i++) {
		S.getline(buf, 1024, sep);
		if (strlen(buf) < 1) {
			if (header)
				continue;
			else
				strcpy(buf, "");
		}

		// Recherche des noms des attributs
		bool flag = false;
		for (uint32 j = 0; j < m_Schema.size(); j++) {
			if (m_Schema[j].Name() == m_strSousType) {
				V.push_back(m_Schema[j].AttributName(i));
				flag = true;
				break;
			}
		}
		if (!flag) {
			att_name = "";
			if (C != NULL)
				att_name = C->AttributName(V.size()/2);
			if ((att_name.size() < 1)||(att_name == " ")) {
				sprintf(att,"Att%02d", i);
				att_name = att;
				}
			V.push_back(att_name);
		}
		V.push_back(buf);
		header = false;
	}

	V.push_back("Identifiant GéoConcept");
	sprintf(buf, "%u", m_nId);
	V.push_back(buf);

	return true;
}

//-----------------------------------------------------------------------------
// Calcul de l'emprise en Z
//-----------------------------------------------------------------------------
bool XGeocon::ComputeZRange()
{
  uint32 i, j;
  for (i = 0; i < m_nNbPt; i++)
		if (m_Z[i] > XGEO_NO_DATA) {
			m_ZRange[0] = m_ZRange[1] = m_Z[i];
			break;
		}

  for (j = i; j < m_nNbPt; j++) {
		if (m_Z[j] > XGEO_NO_DATA) {
			m_ZRange[0] = XMin(m_ZRange[0], m_Z[j]);
			m_ZRange[1] = XMax(m_ZRange[1], m_Z[j]);
		}
	}

	if (m_ZRange[0] > XGEO_NO_DATA) {
		if (m_dZmin > XGEO_NO_DATA)
			m_dZmin = XMin(m_ZRange[0], m_dZmin);
		else
			m_dZmin = m_ZRange[0];
	}
					
	if (m_ZRange[1] > XGEO_NO_DATA) {
		if (m_dZmax > XGEO_NO_DATA)
			m_dZmax = XMax(m_ZRange[1], m_dZmax);
		else
			m_dZmax = m_ZRange[1];
	}
	return true;
}

//-----------------------------------------------------------------------------
// Lecture de la geometrie
//-----------------------------------------------------------------------------
bool XGeocon::LoadGeom(uint32 pos, XGeoconLine2D* V)
{
	m_In.seekg(pos);
	m_b3D = V->Is3D();
	if(!AnalyseLine())
		return false;
	V->SetGeom(m_Pt, m_nNbPt, NULL);
	return true;
}

bool XGeocon::LoadGeom(uint32 pos, XGeoconLine3D* V)
{
	m_In.seekg(pos);
	m_b3D = V->Is3D();
	if (!AnalyseLine())
		return false;
	ComputeZRange();
	V->SetGeom(m_Pt, m_Z, m_nNbPt, NULL, m_ZRange);
	return true;
}

bool XGeocon::LoadGeom(uint32 pos, XGeoconPoly2D* V)
{
	m_In.seekg(pos);
	m_b3D = V->Is3D();
	if (!AnalyseLine())
		return false;
	V->SetGeom(m_Pt, m_nNbPt, NULL);
	return true;
}

bool XGeocon::LoadGeom(uint32 pos, XGeoconPoly3D* V)
{
	m_In.seekg(pos);
	m_b3D = V->Is3D();
	if (!AnalyseLine())
		return false;
	ComputeZRange();
	V->SetGeom(m_Pt, m_Z, m_nNbPt, NULL, m_ZRange);
	return true;
}

bool XGeocon::LoadGeom(uint32 pos, XGeoconMPoly2D* V)
{
	m_In.seekg(pos);
	m_b3D = V->Is3D();
	if (!AnalyseLine())
		return false;
	V->SetGeom(m_Pt, m_nNbPt, NULL, m_Parts, &m_nNumParts);
	return true;
}

bool XGeocon::LoadGeom(uint32 pos, XGeoconMPoly3D* V)
{
	m_In.seekg(pos);
	m_b3D = V->Is3D();
	if (!AnalyseLine())
		return false;
	ComputeZRange();
	V->SetGeom(m_Pt, m_Z, m_nNbPt, NULL, m_ZRange, m_Parts, &m_nNumParts);
	return true;
}

//-----------------------------------------------------------------------------
// Analyse d'une ligne d'un fichier GeoConcept
//-----------------------------------------------------------------------------
bool XGeocon::AnalyseLine()
{
	m_In.getline(m_Line, m_nSize);
	
	// Recherche des commentaires 
	if ((m_Line[0] == '/')&&(m_Line[1] == '/')) {
		m_Type = Null;
		if (strncmp(m_Line, "//$3DOBJECT", 11) == 0)
			m_b3D = true;
		if (strncmp(m_Line, "//$FIELDS", 9) == 0)
			return AnalyseFields();
		return true;
	}
	if (strlen(m_Line) < 2) {
		m_Type = Null;
		return true;
	}

	std::istringstream S;
	S.str(m_Line);

	char sep;
	char buf[1024];
  int i;
  bool relative = true;

	S >> m_nId;
	S.get(sep);
	S.getline(buf, 1024, sep);
	m_strType = buf;
	S.getline(buf, 1024, sep);
	m_strSousType = buf;
	S.getline(buf, 1024, sep);
	m_strObName = buf;

	// Lecture des attributs
	S.getline(buf, 1024, sep);
	sscanf(buf, "%d", &m_nNbAtt);
	m_Att.clear();
  for (i = 0; i < m_nNbAtt; i++) {
		S.getline(buf, 1024, sep);
		m_Att.push_back(buf);
	}

	S.getline(buf, 1024, sep);
	sscanf(buf, "%lf", &m_Pt[0].X);
	S.getline(buf, 1024, sep);
	sscanf(buf, "%lf", &m_Pt[0].Y);
	if (m_b3D) {
		S.getline(buf, 1024, sep);
		if (strncmp(buf, "NOZ", 3) == 0)
			m_Z[0] = XGEO_NO_DATA;
		else
			sscanf(buf, "%lf", &m_Z[0]);
		}

	if (S.tellg() > strlen(m_Line) - 6) {
		m_Type = Point;
		m_nNbPt = 1;
		return true;
	}

	double data;
	S.getline(buf, 1024, sep);
	sscanf(buf, "%lf", &data);
	if ((data < GEOCON_MAX_POINT) && (floor(data) == ceil(data))) {
		m_nNbPt = (uint32)floor(data);
		m_Type = Polygon;
	} else {
		m_Type = Line;
		S.getline(buf, 1024, sep);	// Y'
		S.getline(buf, 1024, sep);
		sscanf(buf, "%u", &m_nNbPt);
	}
	m_nNbPt++;

	// Lecture des points
	for (i = 1; i < m_nNbPt; i++) {
		S.getline(buf, 1024, sep);
		sscanf(buf, "%lf", &m_Pt[i].X);
		// On regarde si les coordonnees sont absolues ou relatives
		if ( i == 1 ) {
			if ((m_Pt[i].X / XMax(m_Pt[i-1].X, 0.1)) > 0.3)
				relative = false;
		}
		if (relative)
			m_Pt[i].X += m_Pt[i-1].X;
		S.getline(buf, 1024, sep);
		sscanf(buf, "%lf", &m_Pt[i].Y);
		if (relative)
			m_Pt[i].Y += m_Pt[i-1].Y;
		if (m_b3D) {
			S.getline(buf, 1024, sep);
			if (strncmp(buf, "NOZ", 3) == 0)
				m_Z[i] = XGEO_NO_DATA;
			else
				sscanf(buf, "%lf", &m_Z[i]);
		}
	}

	S.getline(buf, 1024, sep);
	if (!S.good()) {	// On est a la fin de la ligne
		m_nNumParts = 0;
		return true;
	}
	sscanf(buf, "%lf", &data);
	if ((data > 200) || (floor(data) != ceil(data))) {
		m_nNumParts = 0;
		return true;
	}
	m_nNumParts = (uint32)floor(data);
	m_nNumParts++;
	if (m_nNumParts > 200) {
		m_nNumParts = 0;
		m_Type = Null;
		m_b3D = false;
		return false;
	}
	m_Parts[0] = 0;

	for (i = 1; i < m_nNumParts; i++) {
		// Premier point
		S.getline(buf, 1024, sep);
		sscanf(buf, "%lf", &m_Pt[m_nNbPt].X);
		S.getline(buf, 1024, sep);
		sscanf(buf, "%lf", &m_Pt[m_nNbPt].Y);
		if (m_b3D) {
			S.getline(buf, 1024, sep);
			if (strncmp(buf, "NOZ", 3) == 0)
				m_Z[m_nNbPt] = XGEO_NO_DATA;
			else
				sscanf(buf, "%lf", &m_Z[m_nNbPt]);
		}

		// Nombre de point
		int nb_pt;
		S.getline(buf, 1024, sep);
		sscanf(buf, "%d", &nb_pt);

		// Lecture des points
		for (int j = 0; j < nb_pt; j++) {
			S.getline(buf, 1024, sep);
			sscanf(buf, "%lf", &m_Pt[m_nNbPt+1+j].X);
			if (relative)
				m_Pt[m_nNbPt+1+j].X += m_Pt[m_nNbPt+j].X;
			S.getline(buf, 1024, sep);
			sscanf(buf, "%lf", &m_Pt[m_nNbPt+1+j].Y);
			if (relative)
				m_Pt[m_nNbPt+1+j].Y += m_Pt[m_nNbPt+j].Y;
			if (m_b3D) {
				S.getline(buf, 1024, sep);
				if (strncmp(buf, "NOZ", 3) == 0)
					m_Z[m_nNbPt+1+j] = XGEO_NO_DATA;
				else
					sscanf(buf, "%lf", &m_Z[m_nNbPt+1+j]);
			}
		}

		m_Parts[i] = m_nNbPt;
		m_nNbPt += (nb_pt + 1);
	}
	return true;
}

//-----------------------------------------------------------------------------
// Analyse des champs
//-----------------------------------------------------------------------------
bool XGeocon::AnalyseFields()
{
	std::string type, soustype, data;
	char buf[1024];
	char* ptr = m_Line;
  uint32 i;

  for(i = 0; i < m_nSize; i++) {
		if (strncmp(ptr, "Class=", 6) == 0) {
			type = &ptr[6];
			type = type.substr(0, type.find(';'));
			break;
		}
		ptr++;
	}
	if (type.size() < 1)
		return false;

	for(i = 0; i < m_nSize; i++) {
		if (strncmp(ptr, "Subclass=", 9) == 0) {
			soustype = &ptr[9];
			soustype = soustype.substr(0, soustype.find(';'));
			break;
		}
		ptr++;
	}
	if (soustype.size() < 1)
		return false;

	XGeoSchema schema(soustype.c_str());

	for(i = 0; i < m_nSize; i++) {
		if (strncmp(ptr, "Fields=", 7) == 0) {
			std::istringstream S;
			S.str(&ptr[7]);
			while(S.good()) {
				S.getline(buf, 1024,'\t');
				data = buf;
				if (data.find('#') != std::string::npos)
					continue;
				schema.AddAttribut(data,"","", XGeoAttribut::String,40);
			}
			m_Schema.push_back(schema);
			return true;
		}
		ptr++;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Conversion geodesique
//-----------------------------------------------------------------------------
bool XGeocon::Convert(const char* file_in, const char* file_out, XGeodConverter* L, XError* error)
{
	XPath P;
	m_strFilename = file_in;
	m_strName = P.Name(file_in);
	m_In.open(file_in, std::ios_base::in| std::ios_base::binary);
	if (!m_In.good())
		return XErrorError(error, "XGeocon::Convert", XError::eIOOpen);

	// Ouverture du fichier de sortie
	std::ofstream out;
	out.open(file_out);
	if (!out.good())
		return XErrorError(error, "XGeocon::Convert", XError::eIOOpen);
	out.setf(std::ios::fixed);
	out.precision(3);
	if (L->EndProjection() == XGeoProjection::RGF93)
		out.precision(9);

	m_b3D = false;

	while( (!m_In.eof()) && (m_In.good()) ) {
		uint32 pos = m_In.tellg();
		if (!AnalyseLine())
			XErrorError(error, "XGeocon::Read", XError::eBadFormat);
		if (m_Type == Null)
			continue;

		// Conversion
		double x, y, z;
    uint32 i, j;
    for (i = 0; i < m_nNbPt; i++) {
			if (m_b3D) z = m_Z[i]; else z = 0.;
			if (z <= XGEO_NO_DATA) z = 0;
			L->ConvertDeg(m_Pt[i].X, m_Pt[i].Y, x, y, z);
			m_Pt[i].X = x;
			m_Pt[i].Y = y;
		}
		
		// Ecriture de l'objet
		if (m_b3D)
			out << "//$3DOBJECT" << std::endl;
		else
			out << "//$2DOBJECT" << std::endl;

		// Entete de l'objet
		out << m_nId << "\t" << m_strType << "\t" << m_strSousType << "\t";

		// Attribut de l'objet
		out << m_strObName << "\t" << m_nNbAtt << "\t";
    for (j = 0; j < m_nNbAtt; j++)
			out << m_Att[j] << "\t";

		// Geometrie de l'objet
		out << m_Pt[0].X << "\t" << m_Pt[0].Y << "\t";
		if (m_b3D)
			if (m_Z[0] > XGEO_NO_DATA)
				out << m_Z[0] << "\t";
			else
				out << "NOZ" << "\t";


		// Objet ponctuel
		if (m_nNbPt < 2) {
			out << "0" << std::endl;
			continue;
		}

		// Objet lineaire en une partie
		if ((m_Type == Line)&&(m_nNumParts == 0)) {
			out << m_Pt[m_nNbPt - 1].X << "\t" << m_Pt[m_nNbPt - 1].Y << "\t";
			out << m_nNbPt - 1;
			for (j = 1; j < m_nNbPt; j++) {
				out << "\t" << m_Pt[j].X - m_Pt[j-1].X << "\t" << m_Pt[j].Y - m_Pt[j-1].Y;
				if (m_b3D)
					if (m_Z[j] > XGEO_NO_DATA)
						out << "\t" << m_Z[j];
					else
						out << "\t" << "NOZ";
			}
			out << std::endl;
			continue;
		}

		// Objet surfacique en une partie
		if ((m_Type == Polygon)&&(m_nNumParts == 0)) {
			out << m_nNbPt - 1;
			for (j = 1; j < m_nNbPt; j++) {
				out << "\t" << m_Pt[j].X - m_Pt[j-1].X << "\t" << m_Pt[j].Y - m_Pt[j-1].Y;
				if (m_b3D)
					if (m_Z[j] > XGEO_NO_DATA)
						out << "\t" << m_Z[j];
					else
						out << "\t" << "NOZ";
			}
			out << std::endl;
			continue;
		}

		// Objet surfacique en plusieurs parties
		if ((m_Type == Polygon)&&(m_nNumParts > 1)) {
			uint32 nb_pt = m_Parts[1];
			out << nb_pt - 1;
			for (j = 1; j < nb_pt; j++) {
				out << "\t" << m_Pt[j].X - m_Pt[j-1].X << "\t" << m_Pt[j].Y - m_Pt[j-1].Y;
				if (m_b3D)
					if (m_Z[j] > XGEO_NO_DATA)
						out << "\t" << m_Z[j];
					else
						out << "\t" << "NOZ";
			}
			out << "\t" << m_nNumParts - 1;
			for (uint32 k = 1; k < m_nNumParts; k++) {
				if (k < m_nNumParts - 1)
					nb_pt = m_Parts[k+1] - m_Parts[k];
				else
					nb_pt = m_nNbPt - m_Parts[k];
				out << "\t" << m_Pt[m_Parts[k]].X << "\t" << m_Pt[m_Parts[k]].Y;
				if (m_b3D)
					out << "\t" << m_Z[m_Parts[k]];
				out << "\t" << nb_pt - 1;
				for (j = m_Parts[k] + 1; j < m_Parts[k] + nb_pt; j++) {
					out << "\t" << m_Pt[j].X - m_Pt[j-1].X << "\t" << m_Pt[j].Y - m_Pt[j-1].Y;
					if (m_b3D)
						if (m_Z[j] > XGEO_NO_DATA)
							out << "\t" << m_Z[j];
						else
							out << "\t" << "NOZ";
				}
			}

			out << std::endl;
			continue;
		}

	}
	m_In.clear();
	return true;
}

//-----------------------------------------------------------------------------
// Fixe la geometrie
//-----------------------------------------------------------------------------
bool XGeoconPoint2D::SetGeom(XPt* P)
{
	m_Frame.Xmax = m_Frame.Xmin = P->X;
	m_Frame.Ymax = m_Frame.Ymin = P->Y;
	return true;
}

//-----------------------------------------------------------------------------
// Fixe la geometrie
//-----------------------------------------------------------------------------
bool XGeoconPoint3D::SetGeom(XPt* P, double* Z)
{
	m_Frame.Xmax = m_Frame.Xmin = P->X;
	m_Frame.Ymax = m_Frame.Ymin = P->Y;
	m_Z = *Z;
	return true;
}

//-----------------------------------------------------------------------------
// Fixe la geometrie
//-----------------------------------------------------------------------------
bool XGeoconLine2D::SetGeom(XPt* P, uint32 nbpt, XFrame* F)
{
	if (F != NULL)
		m_Frame = *F;
	m_nNumPoints = nbpt;
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}
	::memcpy(m_Pt, P, nbpt * sizeof(XPt));
	return true;
}

//-----------------------------------------------------------------------------
// Fixe la geometrie
//-----------------------------------------------------------------------------
bool XGeoconLine3D::SetGeom(XPt* P, double* Z, uint32 nbpt, XFrame* F, double* ZRange)
{
	if (F != NULL)
		m_Frame = *F;
	m_nNumPoints = nbpt;
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}
	::memcpy(m_Pt, P, nbpt * sizeof(XPt));

	m_Z = new double[m_nNumPoints];
	if (m_Z == NULL) {
		Unload();
		return false;
		}
	::memcpy(m_Z, Z, nbpt * sizeof(double));

	m_ZRange = new double[2];
	if (m_ZRange == NULL) {
		Unload();
		return false;
	}
	::memcpy(m_ZRange, ZRange, 2 * sizeof(double));

	return true;
}

//-----------------------------------------------------------------------------
// Fixe la geometrie
//-----------------------------------------------------------------------------
bool XGeoconPoly2D::SetGeom(XPt* P, uint32 nbpt, XFrame* F)
{
	if (F != NULL)
		m_Frame = *F;
	m_nNumPoints = nbpt;
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}
	::memcpy(m_Pt, P, nbpt * sizeof(XPt));
	return true;
}

//-----------------------------------------------------------------------------
// Fixe la geometrie
//-----------------------------------------------------------------------------
bool XGeoconPoly3D::SetGeom(XPt* P, double* Z, uint32 nbpt, XFrame* F, double* ZRange)
{
	if (F != NULL)
		m_Frame = *F;
	m_nNumPoints = nbpt;
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}
	::memcpy(m_Pt, P, nbpt * sizeof(XPt));

	m_Z = new double[m_nNumPoints];
	if (m_Z == NULL) {
		Unload();
		return false;
		}
	::memcpy(m_Z, Z, nbpt * sizeof(double));

	m_ZRange = new double[2];
	if (m_ZRange == NULL) {
		Unload();
		return false;
	}
	::memcpy(m_ZRange, ZRange, 2 * sizeof(double));

	return true;
}

//-----------------------------------------------------------------------------
// Fixe la geometrie
//-----------------------------------------------------------------------------
bool XGeoconMPoly2D::SetGeom(XPt* P, uint32 nbpt, XFrame* F, int* parts, int* nbpart)
{
	if (F != NULL)
		m_Frame = *F;
	m_nNumPoints = nbpt;
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}
	::memcpy(m_Pt, P, nbpt * sizeof(XPt));

	m_nNumParts = *nbpart;
	m_Parts = new int[m_nNumParts];
	if (m_Parts == NULL){
		Unload();
		return false;
	}
	::memcpy(m_Parts, parts, m_nNumParts * sizeof(int)); 

	return true;
}

//-----------------------------------------------------------------------------
// Fixe la geometrie
//-----------------------------------------------------------------------------
bool XGeoconMPoly3D::SetGeom(XPt* P, double* Z, uint32 nbpt, XFrame* F, double* ZRange,
														 int* parts, int* nbpart)
{
	if (F != NULL)
		m_Frame = *F;
	m_nNumPoints = nbpt;
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}
	::memcpy(m_Pt, P, nbpt * sizeof(XPt));

	m_Z = new double[m_nNumPoints];
	if (m_Z == NULL) {
		Unload();
		return false;
		}
	::memcpy(m_Z, Z, nbpt * sizeof(double));

	m_ZRange = new double[2];
	if (m_ZRange == NULL) {
		Unload();
		return false;
	}
	::memcpy(m_ZRange, ZRange, 2 * sizeof(double));

	m_nNumParts = *nbpart;
	m_Parts = new int[m_nNumParts];
	if (m_Parts == NULL){
		Unload();
		return false;
	}
	::memcpy(m_Parts, parts, m_nNumParts * sizeof(int)); 

	return true;
}
