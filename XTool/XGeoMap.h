//-----------------------------------------------------------------------------
//								XGeoMap.h
//								=========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 23/08/2002
//-----------------------------------------------------------------------------

#ifndef _XGEOMAP_H
#define _XGEOMAP_H

#include "XGeoObject.h"
#include "XGeoRaster.h"
#include "XGeoVector.h"

class XGeoMap : public XGeoObject {
protected:
	std::vector<XGeoObject*> m_Data;
	std::string		m_strName;

	double		m_dZmin;
	double		m_dZmax;
  bool      m_bUTF8;

public:
  XGeoMap() {m_dZmin = m_dZmax = XGEO_NO_DATA; m_bUTF8 = false;}
	virtual ~XGeoMap();

	virtual inline eType Type() const { return Map;}

	virtual inline double Zmin() const { return m_dZmin;}
	virtual inline double Zmax() const { return m_dZmax;}

	virtual void Visible(bool flag);
	virtual void Selectable(bool flag);

	bool UpdateFrame();

	bool AddObject(XGeoObject* obj);
	bool FindObject(XGeoObject* obj);
	bool RemoveObject(XGeoObject* obj);
	bool RemoveAllObjects();
  bool RemoveClass(XGeoClass* C);

	inline uint32 NbObject() const { return m_Data.size();}
	XGeoObject* Object(uint32 i) { if (i < m_Data.size()) return m_Data[i]; return NULL;}
  uint32 NbMap();
  XGeoMap* GeoMap(uint32 num);

	virtual void Name(std::string name) { m_strName = name;}
	virtual std::string Name() { return m_strName;}

	virtual void WriteInfo(const char* filename) {;}
  bool UTF8() { return m_bUTF8;}

	virtual bool XmlRead(XParserXML* parser, uint32 num = 0, XError* error = NULL);
	virtual bool XmlWrite(std::ostream* out);
};

#endif //_XGEOMAP_H
