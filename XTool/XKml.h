//-----------------------------------------------------------------------------
//								XKml.h
//								======
//
// Auteur : F.Becirspahic - DPC / SP
//
// 23/05/2018
//-----------------------------------------------------------------------------

#ifndef _XKML_H
#define _XKML_H

#include "XGeoMap.h"
#include "XParserXML.h"
#include "XGeoMemVector.h"


// Geometrie ponctuelle
class XKmlPt : public XGeoMemPoint3D {
protected:
  XGeoMap*  m_Map;
public:
  XKmlPt(XGeoMap* map) {m_Map = map;}

  virtual inline XGeoMap* Map() const { return m_Map;}
  bool Read(XParserXML* parser);
};

// Geometrie lineaire
class XKmlLine : public XGeoMemLine3D {
protected:
  XGeoMap*  m_Map;
public:
  XKmlLine(XGeoMap* map) {m_Map = map;}

  virtual inline XGeoMap* Map() const { return m_Map;}
  bool Read(XParserXML* parser);
};

// Geometrie polygonale
class XKmlPolygon : public XGeoMemMPoly3D {
protected:
  XGeoMap*  m_Map;
public:
  XKmlPolygon(XGeoMap* map) {m_Map = map;}

  virtual inline XGeoMap* Map() const { return m_Map;}
  bool Read(XParserXML* parser);
  bool ReadLinearRing(XParserXML* parser, std::vector<XPt3D> *V);
};

// Fichier KML
class XKml : public XGeoMap {
protected:
  std::string								m_strFilename;
  std::ifstream							m_In;
  XParserXML                m_Parser;
  std::vector<XParserXML>   m_Placemark;

  bool ReadDocument(XParserXML* document);
  bool ReadFolder(XParserXML* folder);
  bool AnalyzePlacemark(XParserXML* placemark);
  bool AnalyzeMultiGeometry(XParserXML* multi, std::vector<std::string>* V);

public:
  XKml();
  virtual ~XKml() {;}

  inline std::ifstream* IStream() { return &m_In;}

  void Class(XGeoClass* C);
  //XGeoClass* Class();

  bool Read(const char* filename, XError* error = NULL);

};

#endif // _XKML_H
