//-----------------------------------------------------------------------------
//								XKml.cpp
//								========
//
// Auteur : F.Becirspahic - DPC / SP
//
// 23/05/2018
//-----------------------------------------------------------------------------

#include "XKml.h"
#include <sstream>
#include "XGeoPref.h"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XKml::XKml()
{
  m_strFilename = "Inconnu";
  m_bUTF8 = true;
}

//-----------------------------------------------------------------------------
// Lecture d'un fichier KML
//-----------------------------------------------------------------------------
bool XKml::Read(const char* filename, XError* error)
{
  if (!m_Parser.Parse(filename))
    return XErrorError(error, "XKml::Read", XError::eBadFormat);

  // Recherche des documents
  std::vector<XParserXML> document;
  m_Parser.FindAllSubParsers("/kml/Document", &document);
  for (uint32 i = 0; i < document.size(); i++) {
    ReadDocument(&document[i]);
  }

  // Recherche des folders
  std::vector<XParserXML> folder;
  m_Parser.FindAllSubParsers("/kml/Folder", &folder);
  for (uint32 i = 0; i < folder.size(); i++) {
    ReadFolder(&folder[i]);
  }

  // Placemark directement dans le document
  m_Parser.FindAllSubParsers("/kml/Placemark", &m_Placemark);

  // Analyse des Placemarks
  for (uint32 i = 0; i < m_Placemark.size(); i++) {
    AnalyzePlacemark(&m_Placemark[i]);
  }

  return true;
}

//-----------------------------------------------------------------------------
// Lecture d'un document
//-----------------------------------------------------------------------------
bool XKml::ReadDocument(XParserXML* document)
{
  document->FindAllSubParsers("/Document/Placemark", &m_Placemark);

  // Recherche des documents
  std::vector<XParserXML> sub_document;
  document->FindAllSubParsers("/Document/Document", &sub_document);
  for (uint32 i = 0; i < sub_document.size(); i++) {
    ReadDocument(&sub_document[i]);
  }

  // Recherche des folders
  std::vector<XParserXML> folder;
  document->FindAllSubParsers("/Document/Folder", &folder);
  for (uint32 i = 0; i < folder.size(); i++) {
    ReadFolder(&folder[i]);
  }

  return true;
}

//-----------------------------------------------------------------------------
// Lecture d'un folder
//-----------------------------------------------------------------------------
bool XKml::ReadFolder(XParserXML* folder)
{
  folder->FindAllSubParsers("/Folder/Placemark", &m_Placemark);

  // Recherche des documents
  std::vector<XParserXML> document;
  folder->FindAllSubParsers("/Folder/Document", &document);
  for (uint32 i = 0; i < document.size(); i++) {
    ReadDocument(&document[i]);
  }

  // Recherche des folders
  std::vector<XParserXML> sub_folder;
  folder->FindAllSubParsers("/Folder/Folder", &sub_folder);
  for (uint32 i = 0; i < sub_folder.size(); i++) {
    ReadFolder(&sub_folder[i]);
  }

  return true;
}

//-----------------------------------------------------------------------------
// Analyse d'un placemark
//-----------------------------------------------------------------------------
bool XKml::AnalyzePlacemark(XParserXML* placemark)
{
  std::string name = placemark->ReadNode("/Placemark/name");
  std::string address = placemark->ReadNode("/Placemark/address");
  std::string phoneNumber = placemark->ReadNode("/Placemark/phoneNumber");
  std::string snippet = placemark->ReadNode("/Placemark/Snippet");
  std::string description = placemark->ReadNode("/Placemark/description");

  std::vector<std::string> V;
  if (name.size() > 0) { V.push_back("Nom"); V.push_back(name);}
  if (address.size() > 0) { V.push_back("Adresse"); V.push_back(address);}
  if (phoneNumber.size() > 0) { V.push_back("Téléphone"); V.push_back(phoneNumber);}
  if (snippet.size() > 0) { V.push_back("Snippet"); V.push_back(snippet);}
  if (description.size() > 0) { V.push_back("Description"); V.push_back(description);}


  XParserXML point = placemark->FindSubParser("/Placemark/Point");
  if (!point.IsEmpty()) {
    XKmlPt* P = new XKmlPt(this);
    P->WriteAttributes(V);
    if (P->Read(&point))
      AddObject(P);
    else
      delete P;
    return true;
  }
  XParserXML line = placemark->FindSubParser("/Placemark/LineString");
  if (!line.IsEmpty()) {
    XKmlLine* L = new XKmlLine(this);
    L->WriteAttributes(V);
    if (L->Read(&line))
      AddObject(L);
    else
      delete L;
    return true;
  }
  XParserXML poly = placemark->FindSubParser("/Placemark/Polygon");
  if (!poly.IsEmpty()) {
    XKmlPolygon* P = new XKmlPolygon(this);
    P->WriteAttributes(V);
    if (P->Read(&poly))
      AddObject(P);
    else
      delete P;
    return true;
  }
  XParserXML multi = placemark->FindSubParser("/Placemark/MultiGeometry");
  if (!multi.IsEmpty())
    return AnalyzeMultiGeometry(&multi, &V);

  return false;
}

//-----------------------------------------------------------------------------
// Analyse d'une multi-geometrie
//-----------------------------------------------------------------------------
bool XKml::AnalyzeMultiGeometry(XParserXML* multi, std::vector<std::string>* V)
{
  std::vector<XParserXML> point;
  multi->FindAllSubParsers("/MultiGeometry/Point", &point);
  for (uint32 i = 0; i < point.size(); i++) {
    XKmlPt* P = new XKmlPt(this);
    P->WriteAttributes(*V);
    if (P->Read(&point[i]))
      AddObject(P);
    else
      delete P;
  }
  std::vector<XParserXML> line;
  multi->FindAllSubParsers("/MultiGeometry/LineString", &line);
  for (uint32 i = 0; i < line.size(); i++) {
    XKmlLine* L = new XKmlLine(this);
    L->WriteAttributes(*V);
    if (L->Read(&line[i]))
      AddObject(L);
    else
      delete L;
  }
  std::vector<XParserXML> poly;
  multi->FindAllSubParsers("/MultiGeometry/Polygon", &poly);
  for (uint32 i = 0; i < poly.size(); i++) {
    XKmlPolygon* P = new XKmlPolygon(this);
    P->WriteAttributes(*V);
    if (P->Read(&poly[i]))
      AddObject(P);
    else
      delete P;
  }
  return true;
}


//-----------------------------------------------------------------------------
// Fixe la classe d'objets
//-----------------------------------------------------------------------------
void XKml::Class(XGeoClass* C)
{
  uint32 i;
  for (i = 0; i < m_Data.size(); i++){
    XGeoVector* record = (XGeoVector*)m_Data[i];
    record->Class(C);
    C->Vector(record);
  }
}

//-----------------------------------------------------------------------------
// Lecture d'une geometrie ponctuelle
//-----------------------------------------------------------------------------
bool XKmlPt::Read(XParserXML *parser)
{
  XGeoPref Pref;
  double lon, lat, x, y, z = 0.;
  char sep;

  std::string coord = parser->ReadNode("/Point/coordinates");
  std::istringstream in(coord);
  in >> lon >> sep >> lat;
  if (in.peek() == sep)
    in >> sep >> z;
  Pref.ConvertDeg(XGeoProjection::RGF93,Pref.Projection(), lon, lat, x, y);
  m_Frame.Xmin = m_Frame.Xmax = x;
  m_Frame.Ymin = m_Frame.Ymax = y;
  m_Z = z;

  return true;
}

//-----------------------------------------------------------------------------
// Lecture d'une geometrie lineaire
//-----------------------------------------------------------------------------
bool XKmlLine::Read(XParserXML *line)
{
  XGeoPref Pref;
  double lon, lat;
  XPt3D P;
  std::vector<XPt3D> V;
  char sep;

  std::string coord = line->ReadNode("/LineString/coordinates");
  std::istringstream in(coord);
  while(in.good()) {
    in >> lon >> sep >> lat;
    if (in.peek() == sep)
      in >> sep >> P.Z;
    Pref.ConvertDeg(XGeoProjection::RGF93,Pref.Projection(), lon, lat, P.X, P.Y);
    V.push_back(P);
  }
  m_nNumPoints = V.size();
  m_Pt = new XPt[m_nNumPoints];
  if (m_Pt == NULL)
    return false;
  m_Z = new double[m_nNumPoints];
  if (m_Z == NULL) {
    delete[] m_Pt;
    return false;
  }
  m_ZRange = new double[2];
  m_ZRange[0] =  m_ZRange[1] = V[0].Z;
  m_Frame.Xmin = m_Frame.Xmax = V[0].X;
  m_Frame.Ymin = m_Frame.Ymax = V[0].Y;

  for (uint32 i = 0; i < m_nNumPoints; i++) {
    m_Pt[i].X = V[i].X;
    m_Pt[i].Y = V[i].Y;
    m_Z[i] = V[i].Z;
    m_ZRange[0] = XMin(m_ZRange[0], V[i].Z);
    m_ZRange[1] = XMax(m_ZRange[1], V[i].Z);
    m_Frame += V[i];
  }

  return true;
}

//-----------------------------------------------------------------------------
// Lecture d'une geometrie polygonale
//-----------------------------------------------------------------------------
bool XKmlPolygon::Read(XParserXML *poly)
{
  // Contour exterieur
  XParserXML outer = poly->FindSubParser("/Polygon/outerBoundaryIs");
  if (outer.IsEmpty())
    return false;

  std::vector<XPt3D> V;
  XParserXML ring = outer.FindSubParser("/outerBoundaryIs/LinearRing");
  ReadLinearRing(&ring, &V);

  // Contours interieurs
  std::vector<XParserXML> inner;
  poly->FindAllSubParsers("/Polygon/innerBoundaryIs", &inner);
  m_nNumParts = inner.size() + 1;
  m_Parts = new int[m_nNumParts];
  m_Parts[0] = 0;
  if (inner.size() > 0) {
    for (uint32 i = 0; i < inner.size(); i++) {
      m_Parts[i+1] = V.size();
      ring = inner[i].FindSubParser("/innerBoundaryIs/LinearRing");
      ReadLinearRing(&ring, &V);
    }
  }

  m_nNumPoints = V.size();
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
  m_ZRange[0] =  m_ZRange[1] = V[0].Z;
  m_Frame.Xmin = m_Frame.Xmax = V[0].X;
  m_Frame.Ymin = m_Frame.Ymax = V[0].Y;

  for (uint32 i = 0; i < m_nNumPoints; i++) {
    m_Pt[i].X = V[i].X;
    m_Pt[i].Y = V[i].Y;
    m_Z[i] = V[i].Z;
    m_ZRange[0] = XMin(m_ZRange[0], V[i].Z);
    m_ZRange[1] = XMax(m_ZRange[1], V[i].Z);
    m_Frame += V[i];
  }

  return true;
}

//-----------------------------------------------------------------------------
// Lecture d'un LinearRing
//-----------------------------------------------------------------------------
bool XKmlPolygon::ReadLinearRing(XParserXML* parser, std::vector<XPt3D>* V)
{
  XGeoPref Pref;
  double lon, lat;
  XPt3D P;
  char sep;

  std::string coord = parser->ReadNode("/LinearRing/coordinates");
  std::istringstream in(coord);
  while(in.good()) {
    in >> lon >> sep >> lat;
    if (in.peek() == sep)
      in >> sep >> P.Z;
    Pref.ConvertDeg(XGeoProjection::RGF93,Pref.Projection(), lon, lat, P.X, P.Y);
    V->push_back(P);
  }
  return true;
}
