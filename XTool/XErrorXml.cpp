//-----------------------------------------------------------------------------
//								XErrorXml.cpp
//								=============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 22/08/00
//-----------------------------------------------------------------------------

#include "XErrorXml.h"
#include "XXml.h"

//-----------------------------------------------------------------------------
// Renvoi d'une chaine de caractere explicitant le type d'erreur
//-----------------------------------------------------------------------------
std::string XErrorXml::TypeString(Type t)
{
	switch(t) {
	case eNull:
		return "";
	case eAllocation:
		return "Erreur d'allocation";
	case eRange:
		return "Erreur d'intervale";
	case eIOOpen:
		return "Erreur d'ouverture de fichier";
	case eIOSeek:
		return "Erreur de positionnement dans un fichier";
	case eIORead:
		return "Erreur de lecture dans un fichier";
	case eIOWrite:
		return "Erreur d'écriture dans un fichier";
	case eBadFormat:
		return "Erreur de format de fichier";
	case eUnsupported:
		return "Format de fichier non supporté";
	case eIllegal:
		return "Action illégale";
	case eBadData:
		return "Erreur de cohérence des données";
	default:
		return "Erreur inconnue";
	}
}

//-----------------------------------------------------------------------------
// Message d'erreur
//-----------------------------------------------------------------------------
void XErrorXml::Error(const char* mes, Type t)
{
	m_nError++;
	m_log->seekp((uint32)m_log->tellp() - (uint32)m_tagEnd.length());
	std::string s = TypeString(t) + "\n" + mes;
	*m_log << "<Error>" << XXml::OemToXml(s) << "</Error>" << m_tagEnd;
}

//-----------------------------------------------------------------------------
// Message d'information
//-----------------------------------------------------------------------------
void XErrorXml::Info(const char* mes, Type t)	
{
	m_log->seekp((uint32)m_log->tellp() - (uint32)m_tagEnd.length());
	std::string s = TypeString(t) + "\n" + mes;
	*m_log << "<Info>" << XXml::OemToXml(s) << "</Info>" << m_tagEnd;
}

//-----------------------------------------------------------------------------
// Message d'alerte
//-----------------------------------------------------------------------------
void XErrorXml::Alert(const char* mes, Type t)
{
	m_nAlert++;
	m_log->seekp((uint32)m_log->tellp() - (uint32)m_tagEnd.length());
	std::string s = TypeString(t) + "\n" + mes;
	*m_log << "<Alert>" << XXml::OemToXml(s) << "</Alert>" << m_tagEnd;
}

//-----------------------------------------------------------------------------
// Sortie d'un flux
//-----------------------------------------------------------------------------
void XErrorXml::Output(std::ostream* out)
{
	m_log->seekp((uint32)m_log->tellp() - (uint32)m_tagEnd.length());
	*m_log << out << m_tagEnd;
}

void XErrorXml::BeginOutput()
{
	m_log->seekp((uint32)m_log->tellp() - (uint32)m_tagEnd.length());
}

void XErrorXml::EndOutput()
{
	*m_log << m_tagEnd;
}

//-----------------------------------------------------------------------------
// Debut et fin de tag XML
//-----------------------------------------------------------------------------
void XErrorXml::StartTag(std::string& s)
{
	m_log->seekp((uint32)m_log->tellp() - (uint32)m_tagEnd.length());
	m_tagEnd = "</" + s + ">" + m_tagEnd;
	*m_log << "<" << s << ">" << m_tagEnd;
}

void XErrorXml::EndTag()
{
	int pos = m_tagEnd.find('>');
	m_tagEnd = m_tagEnd.substr(pos+1);
}

