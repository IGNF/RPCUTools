//-----------------------------------------------------------------------------
//								XGpx.cpp
//								========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 14/02/2006
//-----------------------------------------------------------------------------

#include "XGpx.h"
#include "XPath.h"
#include "XParserXML.h"
#include "XGeoPref.h"
#include "XGeoBase.h"
#include "XXml.h"

//-----------------------------------------------------------------------------
// Lecture d'un fichier GPX
//-----------------------------------------------------------------------------
bool XGpxFile::Read(const char* filename, XGeoBase* base, XError* error)
{
	XPath P;
	m_strFilename = filename;
	m_strName = P.Name(filename);

	XParserXML parser;
	if (!parser.Parse(m_strFilename))
		return XErrorError(error, "XGpxFile::Read", XError::eIORead);

	if (!parser.FindNode("/gpx"))
		return XErrorError(error, "XGpxFile::Read", XError::eBadFormat);

	// Creation des classes GPX dans la base
	XGeoClass* Wpt = base->AddClass("GPX", "WayPoint");
	XGeoClass* Rte = base->AddClass("GPX", "Route");
	XGeoClass* Trk = base->AddClass("GPX", "Track");

	if ((Wpt == NULL)||(Rte == NULL)||(Trk == NULL))
		return false;

	Wpt->Repres()->ZOrder(10000);
	Wpt->Repres()->Symbol(126);
	Rte->Repres()->ZOrder(1000);
	Rte->Repres()->Color(0, 200, 120);
	Trk->Repres()->ZOrder(1000);
	Trk->Repres()->Color(150, 0, 120);

	m_Frame = XFrame();

	// Lecture des WayPoints
	uint32 nb_wpt = 0;
	XParserXML wpt;
	while(true){
		wpt = parser.FindSubParser("/gpx/wpt", nb_wpt);
		if (wpt.IsEmpty())
			break;

		XGpxWayPt* P = new XGpxWayPt;
		if (!P->XmlRead(&wpt)) {
			delete P;
			break;
		}
		if (!AddObject(P)) {
			delete P;
			break;
		}
		P->Class(Wpt);
		Wpt->Vector(P);
		nb_wpt++;
	}
	
	// Lecture des Routes
	uint32 nb_rte = 0;
	XParserXML rte;
	while(true){
		rte = parser.FindSubParser("/gpx/rte", nb_rte);
		if (rte.IsEmpty())
			break;

		XGpxRoute* R = new XGpxRoute;
		if (!R->XmlRead(&rte)) {
			delete R;
			break;
		}
		if (!AddObject(R)) {
			delete R;
			break;
		}
		R->Class(Rte);
		Rte->Vector(R);
		nb_rte++;
	}

	// Lecture des Tracks
	uint32 nb_trk = 0;
	XParserXML trk;
	while(true){
		trk = parser.FindSubParser("/gpx/trk", nb_trk);
		if (trk.IsEmpty())
			break;

		XGpxTrack* T = new XGpxTrack;
		if (!T->XmlRead(&trk)) {
			delete T;
			break;
		}
		if (!AddObject(T)) {
			delete T;
			break;
		}
		T->Class(Trk);
		Trk->Vector(T);
		nb_trk++;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Ecriture d'un fichier GPX
//-----------------------------------------------------------------------------
bool XGpxFile::Write(const char* filename, XGeoClass* C, XError* error)
{
	std::ofstream out;
	out.open(filename);
	if (!out.good())
		return XErrorError(error, "XGpxFile::Write", XError::eIOOpen);

	out.setf(std::ios::fixed);
	out.precision(6);

	XXml::WriteXMLHeader(&out, "");
	out << "<gpx version=\"1.1\" creator=\"IGNMap\">" << std::endl;

	XGeoVector* V;
	double lat, lon;
	XPt2D P;
	// Transformation geodesique
	XGeoPref Pref;

	for (uint32 i = 0; i < C->NbVector(); i++) {
		V = C->Vector(i);
		V->LoadGeom();
		if ((V->TypeVector() == XGeoVector::Raster) || (V->TypeVector() == XGeoVector::DTM))
			continue;

		// Cas des WayPoints
		if (V->NbPt() == 1) {
			P = V->Pt(0);
			Pref.ConvertDeg(Pref.Projection(), XGeoProjection::RGF93, P.X, P.Y, lon, lat);
			out << "<wpt lat=\"" << lat << "\" lon=\"" << lon << "\">" << std::endl;
			out << "<ele> " << 0. << " </ele>" << std::endl;
			out << "<name> " << V->Name() << " </name>" << std::endl;
			out << "</wpt>" << std::endl;
		}

		// Cas des Routes
		if (V->NbPt() > 1) {
			out << "<rte>" << std::endl;
			out << "<name> " << V->Name() << " </name>" << std::endl;
			for (uint32 j = 0; j < V->NbPt(); j++) {
				P = V->Pt(j);
				Pref.ConvertDeg(Pref.Projection(), XGeoProjection::RGF93, P.X, P.Y, lon, lat);
				out << "<rtept lat=\"" << lat << "\" lon=\"" << lon << "\">" << std::endl;
				out << "<ele> " << 0. << " </ele>" << std::endl;
				out << "</rtept>" << std::endl;
			}
			out << "</rte>" << std::endl;
		}

		V->Unload();
	}

	out <<"</gpx>";
	return false;
}

//-----------------------------------------------------------------------------
// Ouverture d'un fichier pour l'ecriture par morceaux
//-----------------------------------------------------------------------------
bool XGpxFile::OpenFile(const char* filename, XError* error)
{
	m_Out.open(filename);
	if (!m_Out.good())
		return XErrorError(error, "XGpxFile::OpenFile", XError::eIOOpen);

	m_Out.setf(std::ios::fixed);
	m_Out.precision(6);

	XXml::WriteXMLHeader(&m_Out, "");
	m_Out << "<gpx version=\"1.1\" creator=\"IGNMap\">" << std::endl;
	return m_Out.good();
}

//-----------------------------------------------------------------------------
// Fermeture du fichier pour l'ecriture par morceaux
//-----------------------------------------------------------------------------
bool XGpxFile::CloseFile()
{
	m_Out <<"</gpx>";
	m_Out.close();
	return true;
}

//-----------------------------------------------------------------------------
// Ecriture d'une classe sous forme de WayPoint
//-----------------------------------------------------------------------------	
bool XGpxFile::WriteWayPoint(XGeoClass* C, std::string att)
{
	XGeoVector* V;
	double lat, lon;
	XPt2D P;
	// Transformation geodesique
	XGeoPref Pref;

	for (uint32 i = 0; i < C->NbVector(); i++) {
		V = C->Vector(i);
		V->LoadGeom();
		if ((V->TypeVector() == XGeoVector::Raster) || (V->TypeVector() == XGeoVector::DTM))
			continue;

		// Cas des WayPoints
		if (V->NbPt() == 1) {
			P = V->Pt(0);
			Pref.ConvertDeg(Pref.Projection(), XGeoProjection::RGF93, P.X, P.Y, lon, lat);
			m_Out << "<wpt lat=\"" << lat << "\" lon=\"" << lon << "\">" << std::endl;
			m_Out << "<ele> " << 0. << " </ele>" << std::endl;
			m_Out << "<name> " << V->FindAttribute(att) << " </name>" << std::endl;
			m_Out << "</wpt>" << std::endl;
		}

		V->Unload();
	}
	return m_Out.good();
}

//-----------------------------------------------------------------------------
// Ecriture d'une classe sous forme de Route
//-----------------------------------------------------------------------------
bool XGpxFile::WriteRoute(XGeoClass* C, std::string att)
{
	XGeoVector* V;
	double lat, lon;
	XPt2D P;
	std::string name;
	// Transformation geodesique
	XGeoPref Pref;

	for (uint32 i = 0; i < C->NbVector(); i++) {
		V = C->Vector(i);
		V->LoadGeom();
		if ((V->TypeVector() == XGeoVector::Raster) || (V->TypeVector() == XGeoVector::DTM))
			continue;

		m_Out << "<rte>" << std::endl;
		if (V->NbPt() < 2) {
			P = V->Pt(0);
			name = V->FindAttribute(att);
			Pref.ConvertDeg(Pref.Projection(), XGeoProjection::RGF93, P.X, P.Y, lon, lat);
			m_Out << "<rtept lat=\"" << lat << "\" lon=\"" << lon << "\">" << std::endl;
			// m_Out << "<ele> " << ele << " </ele>" << std::endl;
			if (name.size() > 0)
				m_Out << "<name> " << name << " </name>" << std::endl;
			m_Out << "</rtept>" << std::endl;
			m_Out << "<rtept lat=\"" << lat << "\" lon=\"" << lon << "\">" << std::endl;
			// m_Out << "<ele> " << ele << " </ele>" << std::endl;
			if (name.size() > 0)
				m_Out << "<name> " << name << " </name>" << std::endl;
			m_Out << "</rtept>" << std::endl;
		} else {
			m_Out << "<name> " << V->FindAttribute(att) << " </name>" << std::endl;
			for (uint32 j = 0; j < V->NbPt(); j++) {
				P = V->Pt(j);
				Pref.ConvertDeg(Pref.Projection(), XGeoProjection::RGF93, P.X, P.Y, lon, lat);
				m_Out << "<rtept lat=\"" << lat << "\" lon=\"" << lon << "\">" << std::endl;
				// m_Out << "<ele> " << ele << " </ele>" << std::endl;
				m_Out << "</rtept>" << std::endl;
			}
		}
		m_Out << "</rte>" << std::endl;

		V->Unload();
	}

	return m_Out.good();
}

//-----------------------------------------------------------------------------
// Ecriture de l'ensemble d'une classe sous forme d'une route
//-----------------------------------------------------------------------------
bool XGpxFile::WriteRouteAll(XGeoClass* C, std::string att)
{
	XGeoVector* V;
	double lat, lon;
	XPt2D P;
	std::string name;
	// Transformation geodesique
	XGeoPref Pref;

	m_Out << "<rte>" << std::endl;
	for (uint32 i = 0; i < C->NbVector(); i++) {
		V = C->Vector(i);
		V->LoadGeom();
		if ((V->TypeVector() == XGeoVector::Raster) || (V->TypeVector() == XGeoVector::DTM))
			continue;

		if (V->NbPt() < 2) {
			P = V->Pt(0);
			name = V->FindAttribute(att);
			Pref.ConvertDeg(Pref.Projection(), XGeoProjection::RGF93, P.X, P.Y, lon, lat);
			m_Out << "<rtept lat=\"" << lat << "\" lon=\"" << lon << "\">" << std::endl;
			// m_Out << "<ele> " << ele << " </ele>" << std::endl;
			if (name.size() > 0)
				m_Out << "<name> " << name << " </name>" << std::endl;
			m_Out << "</rtept>" << std::endl;
		} else {
			for (uint32 j = 0; j < V->NbPt(); j++) {
				P = V->Pt(j);
			Pref.ConvertDeg(Pref.Projection(), XGeoProjection::RGF93, P.X, P.Y, lon, lat);
				m_Out << "<rtept lat=\"" << lat << "\" lon=\"" << lon << "\">" << std::endl;
				// m_Out << "<ele> " << ele << " </ele>" << std::endl;
				m_Out << "</rtept>" << std::endl;
			}
		}

		V->Unload();
	}
	m_Out << "</rte>" << std::endl;

	return m_Out.good();
}

//-----------------------------------------------------------------------------
// Ecriture d'une classe sous forme de Track
//-----------------------------------------------------------------------------
bool XGpxFile::WriteTrack(XGeoClass* C, std::string att)
{
	XGeoVector* V;
	double lat, lon;
	XPt2D P;
	std::string name;
	// Transformation geodesique
	XGeoPref Pref;

	m_Out << "<trk>" << std::endl;
	for (uint32 i = 0; i < C->NbVector(); i++) {
		V = C->Vector(i);
		V->LoadGeom();
		if ((V->TypeVector() == XGeoVector::Raster) || (V->TypeVector() == XGeoVector::DTM))
			continue;

		m_Out << "<trkseg>" << std::endl;
		m_Out << "<name> " << V->FindAttribute(att) << " </name>" << std::endl;
		for (uint32 j = 0; j < V->NbPt(); j++) {
			P = V->Pt(j);
			Pref.ConvertDeg(Pref.Projection(), XGeoProjection::RGF93, P.X, P.Y, lon, lat);
			m_Out << "<trkpt lat=\"" << lat << "\" lon=\"" << lon << "\">" << std::endl;
			// m_Out << "<ele> " << ele << " </ele>" << std::endl;
			m_Out << "</trkpt>" << std::endl;
		}
		m_Out << "</trkseg>" << std::endl;

		V->Unload();
	}
	m_Out << "</trk>" << std::endl;

	return m_Out.good();
}


//-----------------------------------------------------------------------------
// Lecture des attributs
//-----------------------------------------------------------------------------
bool XGpxRoute::ReadAttributes(std::vector<std::string>& V)
{
	char data[80];
	V.clear();
	V.push_back("Numero");
	sprintf(data,"%u", m_nNum);
	V.push_back(data);
	V.push_back("Nom");
	V.push_back(m_strName);
	V.push_back("Commentaire");
	V.push_back(m_strCmt);
	V.push_back("Description");
	V.push_back(m_strDesc);

	return true;
}

//-----------------------------------------------------------------------------
// Lecture dans un fichier XML
//-----------------------------------------------------------------------------
bool XGpxRoute::XmlRead(XParserXML* parser, uint32 num)
{
	XParserXML rte = parser->FindSubParser("/rte", num);
	if (rte.IsEmpty())
		return false;

	m_strName = rte.ReadNode("/rte/name");
	m_strCmt = rte.ReadNode("/rte/cmt");			// Commentaire
	m_strDesc = rte.ReadNode("/rte/desc");
	m_strSrc = rte.ReadNode("/rte/src");
	m_strType = rte.ReadNode("/rte/type");
	m_nNum = rte.ReadNodeAsUInt32("/rte/number");

	// Lecture des points
	m_Frame = XFrame();
	uint32 nb_wpt = 0;
	XParserXML wpt;
	while(true){
		wpt = rte.FindSubParser("/rte/rtept", nb_wpt);
		if (wpt.IsEmpty())
			break;

		XGpxWayPt P;
		if (!P.XmlRead(&wpt, 0, "/rtept"))
			break;

		m_WayPt.push_back(P);
		m_Frame += P.Frame();
		nb_wpt++;
	}

	// Creation des tableaux de points
	m_nNumPoints = m_WayPt.size();
	if (m_nNumPoints < 2)
		return false;
	if (m_Pt != NULL)
		delete m_Pt;
	if (m_Z != NULL)
		delete[] m_Z;
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL)
		return false;
	m_Z = new double[m_nNumPoints];
	if (m_Z == NULL) {
		delete[] m_Pt;
		return false;
	}
	for (uint32 i = 0; i < m_nNumPoints; i++) {
		m_Pt[i].X = m_WayPt[i].Pt(0).X;
		m_Pt[i].Y = m_WayPt[i].Pt(0).Y;
		m_Z[i] = m_WayPt[i].Z(0);
	}

	return true;
}

//-----------------------------------------------------------------------------
// Lecture des attributs
//-----------------------------------------------------------------------------
bool XGpxTrack::ReadAttributes(std::vector<std::string>& V)
{
	char data[80];
	V.clear();
	V.push_back("Numero");
	sprintf(data,"%u", m_nNum);
	V.push_back(data);
	V.push_back("Nom");
	V.push_back(m_strName);
	V.push_back("Commentaire");
	V.push_back(m_strCmt);
	V.push_back("Description");
	V.push_back(m_strDesc);

	return true;
}

//-----------------------------------------------------------------------------
// Lecture dans un fichier XML
//-----------------------------------------------------------------------------
bool XGpxTrack::XmlRead(XParserXML* parser, uint32 num)
{
	XParserXML trk = parser->FindSubParser("/trk", num);
	if (trk.IsEmpty())
		return false;

	m_strName = trk.ReadNode("/trk/name");
	m_strCmt = trk.ReadNode("/trk/cmt");			// Commentaire
	m_strDesc = trk.ReadNode("/trk/desc");
	m_strSrc = trk.ReadNode("/trk/src");
	m_strType = trk.ReadNode("/trk/type");
	m_nNum = trk.ReadNodeAsUInt32("/trk/number");
	m_Frame = XFrame();

	// Lecture des segments
	uint32 nb_seg = 0;
	XParserXML trkseg;
	while(true){
		trkseg = trk.FindSubParser("/trk/trkseg", nb_seg);
		if (trkseg.IsEmpty())
			break;

		XGpxTrackSeg S;
		if (!S.XmlRead(&trkseg))
			break;

		m_Seg.push_back(S);
		m_Frame += S.Frame();
		nb_seg++;
	}
	if (m_Seg.size() < 1)
		return false;

	// Creation des tableaux de points
	m_nNumParts = m_Seg.size();
	m_Parts = new int[m_nNumParts];
	if (m_Parts == NULL)
		return false;
	m_nNumPoints = 0;
	for (uint32 i = 0; i < m_Seg.size(); i++) {
		m_Parts[i] = m_nNumPoints;
		m_nNumPoints += m_Seg[i].NbPt();
	}
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		delete[] m_Parts;
		return false;
	}
	m_Z = new double[m_nNumPoints];
	if (m_Z == NULL) {
		delete[] m_Parts;
		delete[] m_Pt;
		return false;
	}
	m_ZRange = new double[2];
	XPt* ptr = m_Pt;
	double* zptr = m_Z;
	XGpxWayPt P = m_Seg[0].Pt(0);
	m_ZRange[0] = m_ZRange[1] = P.Z(0);
  for (uint32 i = 0; i < m_Seg.size(); i++)
		for (uint32 j = 0; j < m_Seg[i].NbPt(); j++) {
			P = m_Seg[i].Pt(j);
			ptr->X = P.Frame().Xmin;
			ptr->Y = P.Frame().Ymin;
			ptr++;
			*zptr = P.Z(0);
			m_ZRange[0] = XMin(m_ZRange[0], *zptr);
			m_ZRange[1] = XMax(m_ZRange[1], *zptr);
			zptr++;
		}

	return true;
}

//-----------------------------------------------------------------------------
// Lecture dans un fichier XML
//-----------------------------------------------------------------------------
bool XGpxTrackSeg::XmlRead(XParserXML* parser, uint32 num)
{
	XParserXML seg = parser->FindSubParser("/trkseg", num);
	if (seg.IsEmpty())
		return false;

	m_Frame = XFrame();
	// Lecture des WayPoints
	uint32 nb_wpt = 0;
	XParserXML wpt;
	while(true){
		wpt = seg.FindSubParser("/trkseg/trkpt", nb_wpt);
		if (wpt.IsEmpty())
			break;

		XGpxWayPt P;
		if (!P.XmlRead(&wpt, 0, "/trkpt"))
			break;

		m_WayPt.push_back(P);
		m_Frame += P.Frame();
		nb_wpt++;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XGpxWayPt::XGpxWayPt()
{
	m_dLat = m_dLon = m_dEle = XGEO_NO_DATA;
	m_dMagVar = m_dGeoidH = 0.;
	m_nSat = 0;
	m_dHdop = m_dVdop = m_dPdop = 0.;
	m_dAge = 0;
	m_nIdStation = 0;
}

//-----------------------------------------------------------------------------
// Lecture des attributs
//-----------------------------------------------------------------------------
bool XGpxWayPt::ReadAttributes(std::vector<std::string>& V)
{
	char data[80];
	V.clear();
	V.push_back("Latitude");
	sprintf(data,"%.6lf", m_dLat);
	V.push_back(data);
	V.push_back("Longitude");
	sprintf(data,"%.6lf", m_dLon);
	V.push_back(data);
	V.push_back("Elevation");
	sprintf(data,"%.6lf", m_dEle);
	V.push_back(data);
	V.push_back("Date");
	V.push_back(m_strTime);
	V.push_back("Nom");
	V.push_back(m_strName);
	V.push_back("Commentaire");
	V.push_back(m_strCmt);
	V.push_back("Description");
	V.push_back(m_strDesc);

	return true;
}

//-----------------------------------------------------------------------------
// Lecture dans un fichier XML
//-----------------------------------------------------------------------------
bool XGpxWayPt::XmlRead(XParserXML* parser, uint32 num, std::string type)
{
	XParserXML wpt = parser->FindSubParser(type, num);
	if (wpt.IsEmpty())
		return false;

	m_dLat = wpt.ReadNodeAsDouble(type + "/lat");
	m_dLon = wpt.ReadNodeAsDouble(type + "/lon");
	m_dEle = wpt.ReadNodeAsDouble(type + "/ele");				// Elevation
	m_strTime = wpt.ReadNode(type + "/time");						// Date
	m_dMagVar = wpt.ReadNodeAsDouble(type + "/magvar");				// Variation magnetique
	m_dGeoidH = wpt.ReadNodeAsDouble(type + "/geoidheight");	// Hauteur de geoide
	m_strName = wpt.ReadNode(type + "/name");
	m_strCmt = wpt.ReadNode(type + "/cmt");			// Commentaire
	m_strDesc = wpt.ReadNode(type + "/desc");
	m_strSrc = wpt.ReadNode(type + "/src");
	m_strType = wpt.ReadNode(type + "/type");
	m_strSym = wpt.ReadNode(type + "/sym");			// Symbol
	m_strFixType = wpt.ReadNode(type + "/fix");
	m_nSat = wpt.ReadNodeAsUInt32(type + "/sat");				// Nombre de satellites
	m_dHdop = wpt.ReadNodeAsDouble(type + "/hdop");
	m_dVdop = wpt.ReadNodeAsDouble(type + "/vdop");
	m_dPdop = wpt.ReadNodeAsDouble(type + "/pdop");
	m_dAge = wpt.ReadNodeAsDouble(type + "/ageofdgpsdata");		// Nombre de secondes
	m_nIdStation = wpt.ReadNodeAsUInt32(type + "/dgpsid");		// Identifiant de la station fixe

	// Transformation geodesique
	XGeoPref Pref;
	double x, y;
	Pref.ConvertDeg(XGeoProjection::RGF93,Pref.Projection(), m_dLon, m_dLat, x, y);

	m_Frame.Xmin = m_Frame.Xmax = x;
	m_Frame.Ymin = m_Frame.Ymax = y;
	m_Z = m_dEle;

	return true;
}
