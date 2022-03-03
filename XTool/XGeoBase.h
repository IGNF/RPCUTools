//-----------------------------------------------------------------------------
//								XGeoBase.h
//								==========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 20/03/2003
//-----------------------------------------------------------------------------

#ifndef _XGEOBASE_H
#define _XGEOBASE_H

#include "XGeoObject.h"
#include <vector>

class XGeoMap;
class XGeoLayer;
class XGeoClass;
class XGeoVector;

class XGeoBase : public XGeoObject {
protected:
	std::vector<XGeoMap*>			m_Map;
	std::vector<XGeoLayer*>		m_Layer;
	std::vector<XGeoObject*>	m_Selection;
	std::vector<XGeoClass*>		m_Class;			// Liste des classes dans l'ordre d'affichage
	double	m_dZmin;
	double	m_dZmax;
	uint32	m_nVersion;			// Version de la base

public:
	XGeoBase() {m_dZmin = m_dZmax = XGEO_NO_DATA; m_nVersion = 0;}
	virtual ~XGeoBase();

  bool UpdateFrame(XFrame* initF = NULL);
	virtual void Clear();

	uint32 Version() { return m_nVersion;}
	void UpdateVersion() { m_nVersion++;}

	inline double ZMin() const { return m_dZmin;}
	inline double ZMax() const { return m_dZmax;}
  double Z(const XPt2D& P);
  bool Z(const XPt2D& TL, float* T, double delta, uint32 w, uint32 h);

	uint32 NbMap() { return m_Map.size();}
	XGeoMap* Map(uint32 i) { if (i < m_Map.size()) return m_Map[i]; return NULL;}
	XGeoMap* Map(const char* mapname);
	bool AddMap(XGeoMap* map);
	bool RemoveMap(XGeoMap* map);

	uint32 NbLayer() { return m_Layer.size();}
	XGeoLayer* Layer(uint32 i) { if (i < m_Layer.size()) return m_Layer[i]; return NULL;}
	XGeoLayer* Layer(const char* layername);
	XGeoLayer* AddLayer(const char* name);
	XGeoClass* AddClass(const char* layer_name, const char* class_name);

	uint32 NbClass() { return m_Class.size();}
	uint32 SortClass();
	XGeoClass* Class(uint32 i);
	XGeoClass* Class(const char* layername, const char* classname);
	void RemoveClass(const char* layername, const char* classname);

	uint32 Find(std::vector<XGeoVector*>* T, XPt2D& P, double dist);
	uint32 Find(std::vector<XGeoVector*>* T, XFrame& F);
	XGeoVector* Find(const char* classname, const char* att_name, const char* att_value);
  XGeoVector* FindClosest(XPt2D& P);
  void ShowAll();
	void HideOut(XFrame* F);
	void HideOut(XGeoVector* V);
	void HideOutStrict(XGeoVector* V);

	double ComputeMinResol();
	std::string FindTopRasterClass();
	XGeoVector* FindTopRasterObject(XFrame* F);
	uint32 NbObjectFrame(XFrame* F, bool only_visible);
  bool IntersectFrame(XFrame* F, bool only_visible);
};

#endif //_XGEOBASE_H
