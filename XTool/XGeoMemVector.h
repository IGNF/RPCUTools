//-----------------------------------------------------------------------------
//								XGeoMemVector.h
//								===============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 22/09/2004
//-----------------------------------------------------------------------------

#ifndef _XGEOMEMVECTOR_H
#define _XGEOMEMVECTOR_H

#include "XGeoPoint.h"
#include "XGeoLine.h"
#include "XGeoPoly.h"

//-----------------------------------------------------------------------------
// Geometrie par defaut
//-----------------------------------------------------------------------------
class XGeoMemVector : public XGeoMLine3D {
protected :
	eTypeVector	m_Type;
//  int   	m_nNumPoints;
//	XPt*		m_Pt;
//  int   	m_nNumParts;
//	int*		m_Parts;
//	double*	m_Z;
//	double*	m_ZRange;
	std::vector<std::string>	m_Attrib;
	std::string	m_strClass;
	std::string	m_strLayer;

public:
	XGeoMemVector();
	~XGeoMemVector();

	void Clear();
	virtual eTypeVector TypeVector () const { return m_Type;}
//	virtual uint32 NbPt() const { return m_nNumPoints;}
//	virtual XPt2D Pt(uint32 i);
//	virtual inline XPt* Pt() { return m_Pt;}
//	virtual double Z(uint32 i);
//	virtual inline double* Z() { return m_Z;}
//	virtual uint32 NbPart() const { return m_nNumParts;}
//  virtual uint32 Part(uint32 i) { if (((int)i < m_nNumParts)&&(m_Parts != NULL)) return m_Parts[i]; return 0;}
//	virtual inline int* Parts() { return m_Parts;}
	/*
	virtual bool Is3D() const { return false;}
	virtual bool IsClosed() const { return false;}
	*/
  virtual	bool ReadAttributes(std::vector<std::string>& V) { V = m_Attrib; return true;}
	virtual bool WriteAttributes(std::vector<std::string>& V) { m_Attrib = V; return true;} 

  void ClassName(const char* name) { m_strClass = name;}
  void LayerName(const char* name) { m_strLayer = name;}
  std::string ClassName() { return m_strClass;}
	std::string LayerName() { return m_strLayer;}

  bool Clone(XGeoVector* V);
  virtual bool LoadGeom() { return true;}
  virtual void Unload() {;}
  virtual bool IsLoaded() { return true;}

//  virtual bool IsNear2D(const XPt2D& P, double dist);

  void SetRangeZ(double zmin, double zmax)
                { if (m_ZRange == NULL) return; m_ZRange[0] = zmin; m_ZRange[1] = zmax;}
//  virtual double Zmin() const { if (m_ZRange == NULL) return XGEO_NO_DATA; return m_ZRange[0];}
//  virtual double Zmax() const { if (m_ZRange == NULL) return XGEO_NO_DATA; return m_ZRange[1];}

	virtual bool XmlRead(XParserXML* parser, uint32 num = 0, XError* error = NULL);
};

//-----------------------------------------------------------------------------
// Geometrie Ponctuelle
//-----------------------------------------------------------------------------
class XGeoMemPoint3D : public XGeoPoint3D {
protected :
	std::vector<std::string>	m_Attrib;

public :
	XGeoMemPoint3D();
	virtual ~XGeoMemPoint3D();
	
	bool AddPoint(double x, double y, double z);
	virtual uint16 Rotation();

	bool Clone(XGeoVector* V);
	virtual	bool ReadAttributes(std::vector<std::string>& V) { V = m_Attrib; return true;}
	virtual bool WriteAttributes(std::vector<std::string>& V) { m_Attrib = V; return true;} 
};

//-----------------------------------------------------------------------------
// Geometrie Lineaire
//-----------------------------------------------------------------------------
class XGeoMemLine3D : public XGeoLine3D {
protected :
	bool m_bLock;
	std::vector<std::string>	m_Attrib;
	
public :
	XGeoMemLine3D() : XGeoLine3D() { m_bLock = true; Editable(true);}
	virtual ~XGeoMemLine3D();

	virtual bool LoadGeom() { return true;}
	virtual void Unload();

	bool Clear();
	bool AddPoint(double x, double y, double z);
	bool RemoveLastPoint();
	bool Clone(XGeoVector* V);
	virtual	bool ReadAttributes(std::vector<std::string>& V) { V = m_Attrib; return true;}
	virtual bool WriteAttributes(std::vector<std::string>& V) { m_Attrib = V; return true;} 
};

//-----------------------------------------------------------------------------
// Geometrie Polygonale 3D
//-----------------------------------------------------------------------------
class XGeoMemPoly3D : public XGeoPoly3D {
protected :
	bool m_bLock;
	std::vector<std::string>	m_Attrib;
	
public :
	XGeoMemPoly3D() : XGeoPoly3D() { m_bLock = true; Editable(true);}
	virtual ~XGeoMemPoly3D();

	virtual bool LoadGeom() { return true;}
  virtual void Unload();

	bool AddPoint(double x, double y, double z);
	bool Clone(XGeoVector* V);

	virtual	bool ReadAttributes(std::vector<std::string>& V) { V = m_Attrib; return true;}
	virtual bool WriteAttributes(std::vector<std::string>& V) { m_Attrib = V; return true;} 
};

//-----------------------------------------------------------------------------
// Geometrie Multi-Polygonale 2D
//-----------------------------------------------------------------------------
class XGeoMemMPoly2D : public XGeoMPoly2D {
protected :
	bool m_bLock;
	std::vector<std::string>	m_Attrib;

public :
	XGeoMemMPoly2D() : XGeoMPoly2D() { m_bLock = true; Editable(true);}
	virtual ~XGeoMemMPoly2D();

  virtual bool LoadGeom() { return true;}
	virtual void Unload();
	virtual bool PrepareGeom(uint32 nbPart, uint32 nbPt);

	bool SetPoint(double x, double y, uint32 pos);
	bool SetPart(int start, uint32 pos);

	virtual	bool ReadAttributes(std::vector<std::string>& V) { V = m_Attrib; return true;}
	virtual bool WriteAttributes(std::vector<std::string>& V) { m_Attrib = V; return true;} 
};

//-----------------------------------------------------------------------------
// Geometrie Multi-Polygonale 3D
//-----------------------------------------------------------------------------
class XGeoMemMPoly3D : public XGeoMPoly3D {
protected :
  bool m_bLock;
  std::vector<std::string>	m_Attrib;

public :
  XGeoMemMPoly3D() : XGeoMPoly3D() { m_bLock = true; Editable(true);}
  virtual ~XGeoMemMPoly3D();

  virtual bool LoadGeom() { return true;}
  virtual void Unload();

  virtual	bool ReadAttributes(std::vector<std::string>& V) { V = m_Attrib; return true;}
  virtual bool WriteAttributes(std::vector<std::string>& V) { m_Attrib = V; return true;}
};

#endif //_XGEOMEMVECTOR_H
