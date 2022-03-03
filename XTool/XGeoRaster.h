//-----------------------------------------------------------------------------
//								XGeoRaster.h
//								============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 23/08/2002
//-----------------------------------------------------------------------------

#ifndef _XGEORASTER_H
#define _XGEORASTER_H

#include "XGeoVector.h"

//-----------------------------------------------------------------------------
// Contexte d'un raster
//-----------------------------------------------------------------------------
class XGeoRasterContext {
protected:
	bool			m_bActif;			// Indique si le contexte est actif
	uint16		m_nNbChannel;
	double*		m_Gamma;			// Tableau de gamma pour les canaux
	byte*			m_Min;				// Tableau des niveaux minimums pour les canaux
	byte*			m_Max;				// Tableau des niveaux maximums pour les canaux
	byte**		m_Lut;				// Tableau des LUT pour les canaux
  bool      m_bMedian;    // Filtre median actif
  uint16    m_nWinMed;    // Fenetre du filtre median
  uint16    m_nCenMed;    // Centile du filtre median
  bool      m_bMatrix;    // Filtre matriciel actif
  int       m_Matrix[10];
  bool      m_bImageAdd;  // Indique que l'on a une image additive
  bool      m_bNormGradient;  // Norme du gradient actif
  bool      m_bDirGradient;
  byte*     m_Palette;    // Palette eventuelle

public:
	XGeoRasterContext();
	virtual ~XGeoRasterContext();

	void Clear();
  void Clone(XGeoRasterContext* clone);
	bool NbChannel(uint16 nb);
	inline uint16 NbChannel() { return m_nNbChannel;}
	bool Channel(uint16 nb, double gamma, byte min, byte max);
	bool Channel(uint16 nb, double* gamma, byte* min, byte* max, byte** lut);
  bool NeedLut();
	byte* Lut(uint16 nb);

  byte Min(uint16 nb) { if(nb < m_nNbChannel) return m_Min[nb]; return 0;}
  byte Max(uint16 nb) { if(nb < m_nNbChannel) return m_Max[nb]; return 255;}
  double Gamma(uint16 nb) { if(nb < m_nNbChannel) return m_Gamma[nb]; return 1.0;}

  void Median(bool flag, uint16 win, uint16 cen) { m_bMedian = flag; m_nWinMed = win; m_nCenMed = cen;}
  bool Median() { return m_bMedian;}
  uint16 WinMedian() { return m_nWinMed;}
  uint16 CenMedian() { return m_nCenMed;}

  void Matrix(bool flag, int matrix[10]) { m_bMatrix = flag; for (int i=0;i<10;i++) m_Matrix[i]=matrix[i];}
  bool Matrix() { return m_bMatrix;}
  int Denom() { return m_Matrix[0];}
  int* Matrix3x3() {return &m_Matrix[1];}

  void ImageAdd(bool flag) { m_bImageAdd = flag;}
  inline bool ImageAdd() { return m_bImageAdd;}

  void Gradient(bool norm, bool dir) { m_bNormGradient = norm; m_bDirGradient = dir;}
  inline bool NormGradient() { return m_bNormGradient;}
  inline bool DirGradient() { return m_bDirGradient;}

  void Palette(byte* pal);
  byte* Palette() { return m_Palette;}

	void Actif(bool flag) { m_bActif = flag;}
	inline bool Actif() const { return m_bActif;}
};

//-----------------------------------------------------------------------------
// Histogramme d'un raster
//-----------------------------------------------------------------------------
class XGeoRasterHisto {
protected:
	uint16		m_nNbChannel;
	double**	m_H;					// Tableau des histogrammes pour les canaux

public:
	XGeoRasterHisto();
	virtual ~XGeoRasterHisto();

	void Clear();
	bool NbChannel(uint16 nb);
	inline uint16 NbChannel() { return m_nNbChannel;}
	bool Channel(uint16 nb, double* H);

	bool Analyze(uint16 nb, uint16& min, uint16& max, uint16& nblevel,
								uint16& nbhole, uint16& holemax, double& satur,
								double holesize = 0.);
	bool Write(uint16 nb, std::ostream& out);
};

//-----------------------------------------------------------------------------
// Gestion des objets raster
//-----------------------------------------------------------------------------
class XGeoRaster : public XGeoVector {
protected:
	uint32				m_nW;
	uint32				m_nH;
	uint16				m_nByte;
	double				m_dResolution;
	XGeoVector*		m_Mask;
	std::string		m_strName;
	std::string		m_strFilename;
	std::string		m_strPath;
	uint16				m_nRot;						// Indique la rotation de l'image
//  XGeoClass*		m_Class;					// Classe de l'objet
	XGeoRasterContext*	m_Context;
	XGeoRasterHisto*		m_Histo;

public:
	XGeoRaster() : m_dResolution(0.) {m_Histo = NULL; m_Context = NULL; m_nRot = 0; m_Mask = NULL; m_nW = m_nH = 0; m_nByte = 0;}
	virtual ~XGeoRaster() {if (m_Context != NULL) delete m_Context; if (m_Histo != NULL) delete m_Histo;}

	virtual eTypeVector TypeVector () const { return Raster;}

	// Geometrie de l'image
	virtual uint32 ImageWidth() { return m_nW;}
	virtual uint32 ImageHeight() { return m_nH;}
	virtual uint16 NbChannel() { return m_nByte;}

	// Georeferencement de l'image
	virtual void UpdateGeoref(double xmin, double ymax, double resol)
										{ m_Frame.Xmin = xmin; m_Frame.Ymax = ymax; m_dResolution = resol;}
  bool Pix2Ground(uint32 u, uint32 v, double& x, double& y);
  bool Ground2Pix(double x, double y, uint32& u, uint32& v);

	virtual inline double Resolution() const { return m_dResolution;}
	void Resolution(double d) { m_dResolution = d;}

  virtual std::string Filename() { return m_strFilename;}
	std::string Path() { return m_strPath;}

  virtual void Filename(std::string s) { m_strFilename = s;}
	void Path(std::string s) { m_strPath = s;}

	virtual void Name(std::string name) { m_strName = name;}
	virtual std::string Name() { return m_strName;}

	virtual	bool ReadAttributes(std::vector<std::string>& V)
											{ V.clear(); V.push_back("Nom"); V.push_back(m_strName);return true;}
	virtual bool GetHisto(double* HR, double* HG, double* HB)
											{ return false;}

	inline uint16 Rotation() { return m_nRot;}
	virtual void Rotation(uint16 rot) { m_nRot = rot;}

	virtual uint32 NbPt() const { return 5;}
	virtual uint32 NbPart() const { return 1;}
	virtual bool IsClosed() const { return true;}
	virtual XPt2D Pt(uint32 i);
  virtual bool Intersect(const XFrame& F) { return m_Frame.Intersect(F);}

	virtual bool CreateContext() {return false;}
	virtual void ClearContext() {if (m_Context != NULL) delete m_Context; m_Context = NULL;}
	virtual XGeoRasterContext* Context() { return m_Context;}
  virtual void ContextChange() {;}
	virtual bool ComputeAutoLevel(double lost = 0.02) { return false;}
	virtual bool ComputeAutoLevelBalance(double lost = 0.02) { return false;}
	virtual bool Gamma(double gamma);

	virtual bool CreateHisto() {return false;}
	virtual bool CreateHisto(XFrame& F) { return false;}
	virtual void ClearHisto() {if (m_Histo != NULL) delete m_Histo; m_Histo = NULL;}
	virtual bool WriteHisto(uint16 nb, std::ostream& out);
	virtual bool AnalyzeHisto(uint16 nb, uint16& min, uint16& max, uint16& nblevel,
														uint16& nbhole, uint16& holemax, double& satur,
														double holesize = 0.);

};

#endif //_XGEORASTER_H
