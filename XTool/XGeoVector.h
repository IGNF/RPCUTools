//-----------------------------------------------------------------------------
//								XGeoVector.h
//								============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 05/03/2003
//-----------------------------------------------------------------------------

#ifndef _XGEOVECTOR_H
#define _XGEOVECTOR_H

#include "XGeoObject.h"
#include "XGeoRepres.h"
#include "XGeoClass.h"

class XGeoMap;

class XGeoVector : public XGeoObject {
public:
	enum eTypeVector { Null, Point, PointZ, MPoint, MPointZ, Line, LineZ, MLine, MLineZ,
												Poly, PolyZ, MPoly, MPolyZ, Raster, DTM};
protected :
	XGeoClass*	m_Class;		// Classe de l'objet
	XGeoRepres*	m_Repres;		// Representation particuliere du vecteur

public :
	XGeoVector();
  virtual ~XGeoVector() { if (m_Repres != NULL) if (m_Repres->Deletable()) delete m_Repres;}

	virtual inline eType Type() const { return Vector;}

	void Class(XGeoClass* c) { m_Class = c;}
	inline XGeoClass* Class() const { return m_Class;}
	std::string ClassName() { if (m_Class != NULL) return m_Class->Name(); return "";}
	virtual inline XGeoMap* Map() const { return NULL;}

	virtual XGeoRepres* Repres();
	virtual void Repres(XGeoRepres* repres);
	virtual bool IsSelectable() const;
	virtual bool IsNear2D(const XPt2D& P, double dist);
	virtual bool IsIn2D(const XPt2D& P);
	virtual bool IsIn2D(const XPt2D& P, double d);
	virtual bool IsIn2D(const XPt2D& M, const XPt2D& P);
	virtual bool IsIn2D(const XFrame& F);
	virtual bool Intersect(const XFrame& F);
	virtual bool IsConnected(const XPt2D& P);
  virtual int Intersect(const XPt2D& M, const XPt2D& P, double epsilon = 0.001);
  virtual bool IsSom2D(const XPt2D& P, int& index, double epsilon = 0.001);

	virtual eTypeVector TypeVector () const { return Null;}
	virtual uint32 NbPt() const { return 0;}
	virtual XPt2D Pt(uint32 i) { return XPt2D(XGEO_NO_DATA, XGEO_NO_DATA);}
	virtual inline XPt* Pt() { return NULL;}
	virtual double Z(uint32 i) { return XGEO_NO_DATA;}
	virtual inline double* Z() { return NULL;}
	virtual inline double* ZRange() { return NULL;}
	virtual uint32 NbPart() const { return 0;}
	virtual uint32 Part(uint32 i) { return 0;}
	virtual inline int* Parts() { return NULL;}
	virtual bool Is3D() const { return false;}
	virtual bool IsClosed() const { return false;}
	virtual inline double Resolution() const { return 10;}

	virtual std::string FindAttribute(std::string att_name, bool no_case = false);
	virtual bool UpdateAttribute(std::string att_name, std::string value, bool no_case = false);
	virtual	bool ReadAttributes(std::vector<std::string>& V) { return false;}
	virtual bool WriteAttributes(std::vector<std::string>& V) { return false;}
	virtual bool LoadGeom() { return false;}
	virtual bool LoadGeom2D() { return LoadGeom();}
	virtual void Unload() {;}
	virtual bool IsLoaded() { return false;}

	virtual std::string TypeVectorString();
	virtual bool WriteHtml(std::ostream* out);
  virtual bool WriteGeoJson(std::ostream* json, std::vector<std::string>* Att = NULL);

	virtual bool XmlWrite(std::ostream* out);

  virtual bool TestZSol(XPt3D& P, double noz = XGEO_NO_DATA);
	virtual bool TestZ0(std::string& source);
	virtual bool InterpolZ0();

	virtual bool AutoIntersect(XPt2D& M,  bool doublons = true);
	virtual bool Doublons(XPt2D& M);

	virtual double Length();
	virtual double Length3D();
	virtual double Length(double& min, double& max);
	virtual double Area();
	virtual double Slope(double& min, double& max);
	virtual double SlopeFilter(double& min, double& max);
	virtual double DeltaZ(double& pos, double& neg);
	virtual bool Altitude(std::vector<double>& T, double step, bool interpol = true);
	virtual bool Position(std::vector<XPt3D>& Pos, double step);
	virtual uint16 Importance() { return 10;}
	virtual XPt2D LabelPoint(uint16* rot = NULL);
	virtual XPt2D Barycentre2D();
	virtual XPt2D Center2D(double d);
	virtual XPt2D Centroide2D();
	virtual XPt2D Centroide2D(double d);
	virtual XPt2D Centroide() { return XPt2D(XGEO_NO_DATA, XGEO_NO_DATA);}
  virtual double Z(const XPt2D&) { return XGEO_NO_DATA;}
	virtual bool HasCentroide() { return false;}
  virtual uint32 ZFrame(const XFrame &F, double* zmax, double* zmin, double* zmean);
  virtual double ZMean();

	virtual bool IsNear3D(XPt2D M, double dist, XFrame* F, double zmin, double zmax,
												XPt3D& A, XPt3D& B, double Zfactor = 1.);
  virtual bool Generalize(std::vector<XPt2D>& T, uint32 nb_max_pt);
  virtual bool Generalize(std::vector<XPt2D>& T, double min_angle);

  virtual bool HasCustomData() { return false;}
  virtual uint32 Data(uint32) { return 0;}

};

// Predicats pour les XGeoVector
bool predXGeoVectorX(XGeoVector* A, XGeoVector* B);
bool predXGeoVectorLength(XGeoVector* A, XGeoVector* B);

#endif  //_XGEOVECTOR_H
