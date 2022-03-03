//-----------------------------------------------------------------------------
//								XErrorXml.h
//								===========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 22/08/00
//-----------------------------------------------------------------------------

#ifndef _XERRORXML
#define _XERRORXML

#include "XBase.h"
#include <fstream>

class XErrorXml : public XError {
protected:
	std::ofstream*	m_log;		// Fichier LOG
	std::string		m_tagEnd;	// Champ XML en cours
public:
	XErrorXml(std::ofstream* log = NULL) { m_log = log; m_tagEnd = "</log>";}
	virtual ~XErrorXml() {;}

	std::string TypeString(Type t);

	virtual void Error(const char* mes, Type t = eNull);	// Message d'erreur
	virtual void Info(const char* mes, Type t = eNull);		// Message d'information
	virtual void Alert(const char* mes, Type t = eNull); 	// Message d'alerte
	virtual void Output(std::ostream* out);					// Sortie d'un flux
	virtual std::ostream* Output() { return m_log;}
	virtual void BeginOutput();
	virtual void EndOutput();

	virtual void StartTag(const char* s) { std::string str = s; StartTag(str);}
	virtual void StartTag(std::string& s);
	virtual void EndTag();
};

#endif //_XERRORXML