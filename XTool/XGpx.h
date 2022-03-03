//-----------------------------------------------------------------------------
//								XGpx.h
//								======
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 14/02/2006
//-----------------------------------------------------------------------------

#ifndef _XGPX_H
#define _XGPX_H

#include "XGeoMap.h"
#include "XGeoMemVector.h"

class XGeoBase;

//-----------------------------------------------------------------------------
// WayPoint GPX
//-----------------------------------------------------------------------------
class XGpxWayPt : public XGeoPoint3D {
protected:
	double					m_dLat;
	double					m_dLon;
	double					m_dEle;				// Elevation
	std::string			m_strTime;		// Date
	double					m_dMagVar;		// Variation magnetique
	double					m_dGeoidH;		// Hauteur de geoide
	std::string			m_strName;
	std::string			m_strCmt;			// Commentaire
	std::string			m_strDesc;
	std::string			m_strSrc;
	std::string			m_strType;
	std::string			m_strSym;			// Symbol
	std::string			m_strFixType;
	uint32					m_nSat;				// Nombre de satellites
	double					m_dHdop;
	double					m_dVdop;
	double					m_dPdop;
	double					m_dAge;				// Nombre de secondes
	uint32					m_nIdStation;	// Identifiant de la station fixe

public:
	XGpxWayPt();

	inline double Elevation() { return m_dEle;}

	virtual uint16 Importance() { return 0;}

	virtual	bool ReadAttributes(std::vector<std::string>& V);
	virtual bool XmlRead(XParserXML* parser, uint32 num = 0, std::string type = "/wpt");
};


//-----------------------------------------------------------------------------
// Route GPX
//-----------------------------------------------------------------------------
class XGpxRoute : public XGeoMemLine3D {
protected:
	std::string			m_strName;
	std::string			m_strCmt;
	std::string			m_strDesc;
	std::string			m_strSrc;
	std::string			m_strType;
	uint32					m_nNum;
	std::vector<XGpxWayPt>	m_WayPt;

public:
	XGpxRoute() { m_nNum = 0;}

	virtual	bool ReadAttributes(std::vector<std::string>& V);
	virtual bool XmlRead(XParserXML* parser, uint32 num = 0);
};

//-----------------------------------------------------------------------------
// Segment GPX
//-----------------------------------------------------------------------------
class XGpxTrackSeg {
protected:
	XFrame									m_Frame;
	std::vector<XGpxWayPt>	m_WayPt;

public:
	XGpxTrackSeg() {;}

	uint32 NbPt() { return m_WayPt.size();}
	XGpxWayPt Pt(uint32 i) { if (i < NbPt()) return m_WayPt[i]; return XGpxWayPt();}

	XFrame Frame() { return m_Frame;}

	virtual bool XmlRead(XParserXML* parser, uint32 num = 0);
};

//-----------------------------------------------------------------------------
// Track GPX
//-----------------------------------------------------------------------------
class XGpxTrack : public XGeoMLine3D {
protected:
	std::string			m_strName;
	std::string			m_strCmt;
	std::string			m_strDesc;
	std::string			m_strSrc;
	std::string			m_strType;
	uint32					m_nNum;
	std::vector<XGpxTrackSeg>	m_Seg;
public:
	XGpxTrack() : XGeoMLine3D() { m_nNum = 0;}
	virtual ~XGpxTrack() { XGeoMLine3D::Unload();}

	virtual bool LoadGeom() { return true;}
	virtual void Unload() {;}

	virtual	bool ReadAttributes(std::vector<std::string>& V);
	virtual bool XmlRead(XParserXML* parser, uint32 num = 0);
};

//-----------------------------------------------------------------------------
// Fichier GPX
//-----------------------------------------------------------------------------
class XGpxFile : public XGeoMap {
protected:
	std::string											m_strFilename;
	std::ofstream										m_Out;			// Fichier de sortie pour l'ecriture par morceaux

public:
	XGpxFile() {;}
	virtual ~XGpxFile() {;}

	bool Read(const char* filename, XGeoBase* base, XError* error = NULL);
	bool Write(const char* filename, XGeoClass* C, XError* error = NULL);

	bool OpenFile(const char* filename, XError* error = NULL);
	bool CloseFile();

	bool WriteWayPoint(XGeoClass* C, std::string att);
	bool WriteRoute(XGeoClass* C, std::string att);
	bool WriteTrack(XGeoClass* C, std::string att);

	bool WriteRouteAll(XGeoClass* C, std::string att);
};

#endif //_XGPX_H
