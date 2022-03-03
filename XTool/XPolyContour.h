//*----------------------------------------------------------------------------
//								XPolyContour.h
//								==============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// Code base sur la GPC – General Polygon Clipper library d'Alan Murta
// http://www.cs.man.ac.uk/~toby/gpc/
//
// 09/10/2001
//*----------------------------------------------------------------------------

#ifndef _XPOLYCONTOUR_H
#define _XPOLYCONTOUR_H

#include "XPolygone2D.h"
class XGeoVector;

#define GPC_EPSILON (DBL_EPSILON)

typedef enum                        /* Set operation type                */
{
	GPC_DIFF,                         /* Difference                        */
	GPC_INT,                          /* Intersection                      */
	GPC_XOR,                          /* Exclusive or                      */
	GPC_UNION                         /* Union                             */
} gpc_op;

typedef struct                      /* Polygon vertex structure          */
{
	double              x;            /* Vertex x component                */
	double              y;            /* vertex y component                */
} gpc_vertex;

typedef struct                      /* Vertex list structure             */
{
	int                 num_vertices; /* Number of vertices in list        */
	gpc_vertex         *vertex;       /* Vertex array pointer              */
} gpc_vertex_list;

typedef struct                      /* Polygon set structure             */
{
	int                 num_contours; /* Number of contours in polygon     */
	int                *hole;         /* Hole / external contour flags     */
	gpc_vertex_list    *contour;      /* Contour array pointer             */
} gpc_polygon;

typedef struct                      /* Tristrip set structure            */
{
	int                 num_strips;   /* Number of tristrips               */
	gpc_vertex_list    *strip;        /* Tristrip array pointer            */
} gpc_tristrip;


//-----------------------------------------------------------------------------
// La classe XPolyContour permet de gerer un polygone multiple.
// Le polygone peut etre disjoint, troue.
//-----------------------------------------------------------------------------
class XPolyContour {
protected:
	void gpc_add_contour         (gpc_polygon     *polygon,
															  gpc_vertex_list *contour,
																int              hole);

	void gpc_polygon_clip        (gpc_op           set_operation,
																gpc_polygon     *subject_polygon,
																gpc_polygon     *clip_polygon,
																gpc_polygon     *result_polygon);

	void gpc_tristrip_clip       (gpc_op           set_operation,
																gpc_polygon     *subject_polygon,
																gpc_polygon     *clip_polygon,
																gpc_tristrip    *result_tristrip);

	void gpc_polygon_to_tristrip (gpc_polygon     *polygon,
																gpc_tristrip    *tristrip);

	void gpc_free_polygon        (gpc_polygon     *polygon);

	void gpc_free_tristrip       (gpc_tristrip    *tristrip);

protected:
	gpc_polygon		m_P;	// Polygone GPC

	void CreateGpcPolygon(XPolygone2D* M, gpc_polygon* poly);

public:
	XPolyContour() { m_P.contour = NULL; m_P.num_contours = 0; m_P.hole = NULL;}
	XPolyContour(const XPolyContour& P);

	virtual ~XPolyContour() { Clear();}
	virtual void Clear() { gpc_free_polygon(&m_P);}
	virtual bool Build(XGeoVector* V);

	virtual void AddPolygon(XPolygone2D* M);
	virtual void SubPolygon(XPolygone2D* M);
	virtual void XorPolygon(XPolygone2D* M);
	virtual void IntPolygon(XPolygone2D* M);

	inline bool IsEmpty() { if (NbContour() < 1) return true; return false;}
	inline uint32 NbContour() { return m_P.num_contours;}
	bool GetContour(uint32 num, uint32* nbPt, XPt2D** Pt, bool* hole);

	void Add(XPolyContour* M) {gpc_polygon_clip(GPC_UNION, &m_P, &(M->m_P), &m_P);}
	void Sub(XPolyContour* M) {gpc_polygon_clip(GPC_DIFF, &m_P, &(M->m_P), &m_P);}
	void Xor(XPolyContour* M) {gpc_polygon_clip(GPC_XOR, &m_P, &(M->m_P), &m_P);}
	void Int(XPolyContour* M) {gpc_polygon_clip(GPC_INT, &m_P, &(M->m_P), &m_P);}

	XPolyContour& operator=(const XPolyContour& P);

	double Area() const;											// Surface du poly-contour
	double GetContourArea(uint32 num) const;	// Surface d'un contour
	XFrame Frame() const;											// Rectangle englobant du poly-contour

	void Sort();

	virtual bool IsIn(XPolyContour* M);
	virtual bool IsIn(XPolygone2D* M);
	virtual bool IsIn(XPt2D* P, double d);

	virtual bool XmlRead(XParserXML* parser, uint32 num = 0, XError* error = NULL);
	virtual bool XmlWrite(std::ostream* out);

	virtual bool ExportGeoconcept(std::ostream* out);
	virtual bool ExportMifMid(std::ostream* out);
	virtual bool ExportDatabase(std::ostream* out, const char* header, char sep);
};

// Operateurs logiques
bool operator==(XPolyContour, XPolyContour);
bool operator!=(XPolyContour, XPolyContour);

#endif //_XPOLYCONTOUR_H
