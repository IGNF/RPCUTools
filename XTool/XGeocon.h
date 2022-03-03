//-----------------------------------------------------------------------------
//								XGeocon.h
//								=========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 14/08/2003
//-----------------------------------------------------------------------------

#ifndef _XGEOCON_H
#define _XGEOCON_H

#include "XGeoMap.h"
#include "XGeoVector.h"
#include "XGeoLine.h"
#include "XGeoPoint.h"
#include "XGeoPoly.h"

class XGeoBase;

class XGeoconPoint2D;
class XGeoconPoint3D;
class XGeoconLine2D;
class XGeoconLine3D;
class XGeoconPoly2D;
class XGeoconPoly3D;
class XGeoconMPoly2D;
class XGeoconMPoly3D;

class XGeodConverter;

class XGeocon : public XGeoMap {
protected:
	enum eType { Null, Point, Line, Polygon};

	std::string					m_strFilename;
	std::ifstream				m_In;

	static int					m_nSize;
	static char*				m_Line;

	bool								m_b3D;				// Indique si l'objet suivant dans le flux est 3D
	eType								m_Type;
	uint32							m_nId;				// Identifiant de l'objet en cours
	std::string					m_strType;		// Type de l'objet en cours
	std::string					m_strSousType;// Sous-Type de l'objet en cours
	std::string					m_strObName;
	uint16							m_nNbAtt;
	uint32							m_nNbPt;
	static XPt*					m_Pt;
	static double*			m_Z;
	XFrame							m_F;
	double							m_ZRange[2];
	int									m_nNumParts;
	static int*					m_Parts;
	std::vector<std::string>	m_Att;	// Attributs de l'objet en cours
	std::vector<XGeoSchema>		m_Schema;			// Schema present dans le fichier

	bool AnalyseLine();
	bool AnalyseFields();
	bool ComputeZRange();

public:
	XGeocon();
	virtual ~XGeocon();

	static void PrepareGeocon();

	bool Read(const char* filename, XGeoBase* base, XError* error = NULL);
	bool Write(const char* filename, XError* error = NULL);

	inline std::ifstream* IStream() { return &m_In;}

	bool ReadAttributes(uint32 pos, std::vector<std::string>& V, XGeoClass* C = NULL);
	bool LoadGeom(uint32 pos, XGeoconLine2D* V);
	bool LoadGeom(uint32 pos, XGeoconLine3D* V);
	bool LoadGeom(uint32 pos, XGeoconPoly2D* V);
	bool LoadGeom(uint32 pos, XGeoconPoly3D* V);
	bool LoadGeom(uint32 pos, XGeoconMPoly2D* V);
	bool LoadGeom(uint32 pos, XGeoconMPoly3D* V);

	bool Convert(const char* file_in, const char* file_out, XGeodConverter* L, XError* error = NULL);

  static bool ImportGeoconcept(XGeoBase* base, const char* filename, XGeoMap* map = NULL);
};

//-----------------------------------------------------------------------------
// Point
//-----------------------------------------------------------------------------
class XGeoconPoint2D : public XGeoPoint2D {
protected :
	XGeocon*			m_File;
	uint32				m_Pos;						// Position dans le fichier
public:
	XGeoconPoint2D(XGeocon* file = NULL, uint32 pos = 0) : XGeoPoint2D() { m_File = file; m_Pos = pos;}

	virtual inline XGeoMap* Map() const { return m_File;}
	virtual bool ReadAttributes(std::vector<std::string>& V)
												{ if (m_File!=NULL) return m_File->ReadAttributes(m_Pos, V, m_Class); return false;}

	bool SetGeom(XPt* P);
};

class XGeoconPoint3D : public XGeoPoint3D {
protected :
	XGeocon*			m_File;
	uint32				m_Pos;						// Position dans le fichier
public:
	XGeoconPoint3D(XGeocon* file = NULL, uint32 pos = 0) : XGeoPoint3D() { m_File = file; m_Pos = pos;}

	virtual inline XGeoMap* Map() const { return m_File;}
	virtual bool ReadAttributes(std::vector<std::string>& V)
												{ if (m_File!=NULL) return m_File->ReadAttributes(m_Pos, V, m_Class); return false;}
	bool SetGeom(XPt* P, double* Z);
};

//-----------------------------------------------------------------------------
// Ligne 2D
//-----------------------------------------------------------------------------
class XGeoconLine2D : public XGeoLine2D {
protected :
	XGeocon*			m_File;
	uint32				m_Pos;						// Position dans le fichier
public:
	XGeoconLine2D(XGeocon* file = NULL, uint32 pos = 0) : XGeoLine2D() { m_File = file; m_Pos = pos;}
	
	virtual inline XGeoMap* Map() const { return m_File;}
	virtual bool ReadAttributes(std::vector<std::string>& V)
												{ if (m_File!=NULL) return m_File->ReadAttributes(m_Pos, V, m_Class); return false;}
	virtual bool LoadGeom()
												{ if (m_File!=NULL) return m_File->LoadGeom(m_Pos, this); return false;}
	bool SetGeom(XPt* P, uint32 nbpt, XFrame* F);
};

//-----------------------------------------------------------------------------
// Polyligne 3D
//-----------------------------------------------------------------------------
class XGeoconLine3D : public XGeoLine3D {
protected :
	XGeocon*			m_File;
	uint32				m_Pos;						// Position dans le fichier
public:
	XGeoconLine3D(XGeocon* file = NULL, uint32 pos = 0) : XGeoLine3D() { m_File = file; m_Pos = pos;}
	
	virtual inline XGeoMap* Map() const { return m_File;}
	virtual bool ReadAttributes(std::vector<std::string>& V)
												{ if (m_File!=NULL) return m_File->ReadAttributes(m_Pos, V, m_Class); return false;}
	virtual bool LoadGeom()
												{ if (m_File!=NULL) return m_File->LoadGeom(m_Pos, this); return false;}
	bool SetGeom(XPt* P, double* Z, uint32 nbpt, XFrame* F, double* ZRange);
};

//-----------------------------------------------------------------------------
// Multi-Polygone 2D
//-----------------------------------------------------------------------------
class XGeoconMPoly2D : public XGeoMPoly2D {
protected :
	XGeocon*			m_File;
	uint32				m_Pos;						// Position dans le fichier
public:
	XGeoconMPoly2D(XGeocon* file = NULL, uint32 pos = 0) : XGeoMPoly2D() { m_File = file; m_Pos = pos;}

	virtual inline XGeoMap* Map() const { return m_File;}
	virtual bool ReadAttributes(std::vector<std::string>& V)
												{ if (m_File!=NULL) return m_File->ReadAttributes(m_Pos, V, m_Class); return false;}
	virtual bool LoadGeom()
												{ if (m_File!=NULL) return m_File->LoadGeom(m_Pos, this); return false;}
	bool SetGeom(XPt* P, uint32 nbpt, XFrame* F, int* parts, int* nbpart);
};

//-----------------------------------------------------------------------------
// Polygone 2D
//-----------------------------------------------------------------------------
class XGeoconPoly2D : public XGeoPoly2D {
protected :
	XGeocon*			m_File;
	uint32				m_Pos;						// Position dans le fichier
public:
	XGeoconPoly2D(XGeocon* file = NULL, uint32 pos = 0) : XGeoPoly2D() { m_File = file; m_Pos = pos;}

	virtual inline XGeoMap* Map() const { return m_File;}
	virtual bool ReadAttributes(std::vector<std::string>& V)
												{ if (m_File!=NULL) return m_File->ReadAttributes(m_Pos, V, m_Class); return false;}
	virtual bool LoadGeom()
												{ if (m_File!=NULL) return m_File->LoadGeom(m_Pos, this); return false;}
	bool SetGeom(XPt* P, uint32 nbpt, XFrame* F);
};

//-----------------------------------------------------------------------------
// Multi-Polygone 3D
//-----------------------------------------------------------------------------
class XGeoconMPoly3D : public XGeoMPoly3D {
protected :
	XGeocon*			m_File;
	uint32				m_Pos;						// Position dans le fichier
public:
	XGeoconMPoly3D(XGeocon* file = NULL, uint32 pos = 0) : XGeoMPoly3D() { m_File = file; m_Pos = pos;}

	virtual inline XGeoMap* Map() const { return m_File;}
	virtual bool ReadAttributes(std::vector<std::string>& V)
												{ if (m_File!=NULL) return m_File->ReadAttributes(m_Pos, V, m_Class); return false;}
	virtual bool LoadGeom()
												{ if (m_File!=NULL) return m_File->LoadGeom(m_Pos, this); return false;}
	bool SetGeom(XPt* P, double* Z, uint32 nbpt, XFrame* F, double* ZRange, int* parts, int* nbpart);
};

//-----------------------------------------------------------------------------
// Polygone 3D
//-----------------------------------------------------------------------------
class XGeoconPoly3D : public XGeoPoly3D {
protected :
	XGeocon*			m_File;
	uint32				m_Pos;						// Position dans le fichier
public:
	XGeoconPoly3D(XGeocon* file = NULL, uint32 pos = 0) : XGeoPoly3D() { m_File = file; m_Pos = pos;}
	
	virtual inline XGeoMap* Map() const { return m_File;}
	virtual bool ReadAttributes(std::vector<std::string>& V)
												{ if (m_File!=NULL) return m_File->ReadAttributes(m_Pos, V, m_Class); return false;}
	virtual bool LoadGeom()
												{ if (m_File!=NULL) return m_File->LoadGeom(m_Pos, this); return false;}
	bool SetGeom(XPt* P, double* Z, uint32 nbpt, XFrame* F, double* ZRange);
};

#endif
