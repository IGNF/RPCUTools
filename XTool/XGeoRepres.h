//-----------------------------------------------------------------------------
//								XGeoRepres.h
//								============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 11/03/2003
//-----------------------------------------------------------------------------

#ifndef _XGEOREPRES_H
#define _XGEOREPRES_H

#include "XBase.h"

class XParserXML;

class XGeoRepres {
protected:
	std::string	m_strName;		// Nom de la representation
	std::string	m_strFont;		// Nom de la police
	uint32			m_nColor;			// Couleur du trait
	uint32			m_nFillColor;	// Couleur de remplissage
	uint32			m_nSymbol;		// Numero du symbole ponctuelle ou du pattern
	uint32			m_nZOrder;		// Ordre d'affichage
	byte				m_nSize;			// Dimension du symbole ou epaisseur du trait
	byte				m_nFontSize;	// Taille de la police
	byte				m_nTrans;			// Pourcentage de transparence
  bool        m_bDeletable; // Indique si la representation peut etre detruite
  uint32      m_nMinScale;  // Echelle minimum d'affichage
  uint32      m_nMaxScale;  // Echelle minimum d'affichage
public:
  XGeoRepres();
  XGeoRepres(uint32 color, uint32 fill, uint32 symbol, uint32 zorder, byte size, byte font, byte trans)
  { m_nColor = color; m_nFillColor = fill; m_nSymbol = symbol; m_nZOrder = zorder;
    m_nSize = size; m_nFontSize = font; m_nTrans = trans; m_bDeletable = false;
    m_nMinScale = 0; m_nMaxScale = 0;}
	virtual ~XGeoRepres() {;}

	inline uint32 Color() const { return m_nColor;}
	inline uint32 FillColor() const { return m_nFillColor;}
	inline uint32 Symbol() const { return m_nSymbol;}
	inline uint32 ZOrder() const { return m_nZOrder;}
	inline byte Size() const { return m_nSize;}
	inline byte FontSize() const { return m_nFontSize;}
	inline byte Transparency() const { return m_nTrans;}
  inline bool Deletable() const { return m_bDeletable;}
  inline uint32 MinScale() const { return m_nMinScale;}
  inline uint32 MaxScale() const { return m_nMaxScale;}
	std::string Name() { return m_strName;}
	std::string Font() { return m_strFont;}
	void Color(byte& r, byte& g, byte& b);
	void FillColor(byte& r, byte& g, byte& b);

  //static inline uint32 RGBColor(uint32 r, uint32 g, uint32 b)
  //															{uint32 m = 255; return XMin(b,m)*256*256 + XMin(g,m)*256 + XMin(r, m);}
  // Modification pour QT
  static inline uint32 RGBColor(uint32 r, uint32 g, uint32 b)
                                {uint32 m = 255; return XMin(r,m)*256*256 + XMin(g,m)*256 + XMin(b, m);}
  static inline uint32 DPBCSymbol(uint32 d, uint32 p, uint32 b, uint32 s)
          {uint32 m = 255; return XMin(d,m)*256*256*256 + XMin(p,m)*256*256 + XMin(b,m)*256 + XMin(s, m);}

  byte Data() { byte* ptr = (byte*)&m_nSymbol; return ptr[3];}
  byte Pen() { byte* ptr = (byte*)&m_nSymbol; return ptr[2];}
	byte Brush() { byte* ptr = (byte*)&m_nSymbol; return ptr[1];}
	byte Cell() { byte* ptr = (byte*)&m_nSymbol; return ptr[0];}

	void Color(uint32 c) { m_nColor = c;}
	void Color(uint32 r, uint32 g, uint32 b) { m_nColor = RGBColor(r, g, b);}
	void FillColor(uint32 c) { m_nFillColor = c;}
	void FillColor(uint32 r, uint32 g, uint32 b) { m_nFillColor = RGBColor(r, g, b);}
	void Symbol(uint32 s) { m_nSymbol = s;}
	void Size(byte s) { m_nSize = s;}
	void FontSize(byte s) { m_nFontSize = s;}
	void ZOrder(uint32 z) { m_nZOrder = z;}
	void Name(const char* name) { m_strName = name;}
	void Font(const char* name) { m_strFont = name;}
	void Transparency(byte t) { if (t > 100) m_nTrans = 100; else m_nTrans = t;}
  void Deletable(bool flag) { m_bDeletable = flag;}
  void Scale(uint32 min, uint32 max) { m_nMinScale = min; m_nMaxScale = max;}

	virtual bool XmlRead(XParserXML* parser, uint32 num = 0, XError* error = NULL);
	virtual bool XmlWrite(std::ostream* out);

	// Operateurs logiques
	friend bool operator==(XGeoRepres, XGeoRepres);
	friend bool operator!=(XGeoRepres, XGeoRepres);
};

extern XGeoRepres   gDefaultRepres;

#endif //_XGEOREPRES_H
