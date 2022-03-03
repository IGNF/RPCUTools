//-----------------------------------------------------------------------------
//								XDBase.h
//								========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 17/03/2003
//-----------------------------------------------------------------------------

#ifndef _XDBASE_H
#define _XDBASE_H

#include "XBase.h"
#include "XFile.h"
#include <vector>

//-----------------------------------------------------------------------------
// Entete d'un fichier DBase
//-----------------------------------------------------------------------------
class XDBaseFileHeader {
protected:
	byte		m_cVersion;
	byte		m_Date[3];			// Date YYMMDD
	uint32	m_nNbRecord;		// Nombre de records dans le fichier
	uint16	m_nHeaderSize;	// Taille de l'entete en octets
	uint16	m_nRecordSize;	// Taille d'un record en octets
	byte		m_cTransaction;
	byte		m_cEncryption;
	byte		m_MultiUser[12];
	byte		m_cIndex;
	byte		m_cLanguage;

public:
	XDBaseFileHeader(uint32 nbRecord = 0, uint32 recordSize = 0,  uint32 nb_field = 0);
	virtual ~XDBaseFileHeader() {;}

	inline uint32	NbRecord() const { return m_nNbRecord;}
	inline uint16	HeaderSize() const { return m_nHeaderSize;}
	inline uint16	RecordSize() const { return m_nRecordSize;}

	bool Read(std::ifstream* in, XError* error = NULL);
	bool Write(std::ofstream* out, XError* error = NULL);
};

//-----------------------------------------------------------------------------
// Descripteur de champs DBase
//-----------------------------------------------------------------------------
class XDBaseFieldDescriptor {
public :
	enum eType { String, Date, NumericF, NumericN, Logical, Memo, Variable, Picture, Binary, General,
							 Short, Long, Double};
protected:
	std::string		m_strName;
	eType					m_Type;
	byte					m_nLength;
	byte					m_nDecCount;

	bool Char2Type(char t);

public:
	XDBaseFieldDescriptor(const char* name, char t = 'C', byte size = 254, byte dec = 0);
	virtual ~XDBaseFieldDescriptor() {;}

	bool Read(std::ifstream* in, XError* error = NULL);
	bool Write(std::ofstream* out, XError* error = NULL);

	inline uint16 Length() const { return (uint16)m_nLength;}
	inline uint16 DecCount() const { return (uint16)m_nDecCount;}
	inline std::string Name() const { return m_strName;}
	inline eType Type() const { return m_Type;}
};

//-----------------------------------------------------------------------------
// Fichier DBase
//-----------------------------------------------------------------------------
class XDBaseFile {
protected:
	std::string	m_strFilename;
	XDBaseFileHeader	m_Header;
	std::vector<XDBaseFieldDescriptor>	m_FieldDesc;
  //std::ifstream			m_In;
  XFile             m_In;
  std::ofstream			m_Out;

public:
	XDBaseFile() { m_strFilename = "<sans nom>";}
	virtual ~XDBaseFile() {;}

	bool ReadHeader(const char* filename, XError* error = NULL);
	bool ReadRecord(uint32 num, std::vector<std::string>& V, XError* error = NULL);

	uint32 NbField() const { return m_FieldDesc.size();}
	uint32 NbRecord() const { return m_Header.NbRecord();}
	bool GetFieldDesc(uint32 index, std::string& name, XDBaseFieldDescriptor::eType& type, uint16& length, byte& dec);

	int FindField(std::string fieldname, bool no_case = false);

	bool AddField(const char* name, char type, byte size, byte dec = 0);
	bool SetNbRecord(uint32 nb);

	bool WriteHeader(const char* filename, XError* error = NULL);
	bool WriteRecord(std::vector<std::string>& V, XError* error = NULL);
	bool UpdateRecord(uint32 num, std::vector<std::string>& V, XError* error = NULL);
	bool AddRecord(std::vector<std::string>& V, XError* error = NULL);

};

#endif //_XDBASE_H
