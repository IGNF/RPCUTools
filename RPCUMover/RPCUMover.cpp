//-----------------------------------------------------------------------------
//								RPCUMover.cpp
//								=============
//
// Application de l'algorithme de Haunert sur un jeu de vecteurs
// 
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 21/10/2021
//-----------------------------------------------------------------------------

#include <iostream>
#include <algorithm>
#include "XGeoBase.h"
#include "XShapefile.h"
#include "XMifMid.h"
#include "XPath.h"
#include "XMatcher.h"
#include "XWaitConsole.h"
#include "XTrigMover.h"
#include "XBaryMover.h"
#include "XMifMidConverter.h"
#include "XGeoProjection.h"
#include "XGeoPref.h"

// Gestionnaire d'erreurs
class AppError : public XError {
protected:
	std::ostream* m_Out;

public:
	AppError() : XError() { m_Out = NULL; }
	virtual void Output(std::ostream* out) { m_Out = out; }				// Sortie d'un flux
	virtual std::ostream* Output() { return m_Out; }

	virtual void Error(const char* mes, Type t = eNull) { if (m_Out != NULL) *m_Out << mes << std::endl; }
	virtual void Alert(const char* mes, Type t = eNull) { if (m_Out != NULL) *m_Out << mes << std::endl; }
	virtual void Info(const char* mes, Type t = eNull) { if (m_Out != NULL) *m_Out << mes << std::endl; }
};

// Usage de l'application
int Usage()
{
	std::cout << "RPCUMover " << std::endl
		<< "\t--p fichier_parcelle " << std::endl
		<< "\t--f fichier_feuille " << std::endl
		<< "\t--i fichier_a_traiter " << std::endl
		<< "\t--o repertoire_resultat" << std::endl
		<< "\t--proj projection" << std::endl;
	std::cout << "Projections disponibles :" << std::endl;
	for (int i = XGeoProjection::RGF93; i <= XGeoProjection::NC_RGNC91_UTM59; i++) {
		std::string shortname = XGeoProjection::ProjectionShortName((XGeoProjection::XProjCode)i);
		if (shortname.size() < 1)
			continue;
		std::cout << "\t" << XGeoProjection::ProjectionName((XGeoProjection::XProjCode)i) << " : " << shortname << std::endl;
	}
	return -1;
}

// Import d'un fichier
XGeoClass* ImportFile(XGeoBase* base, std::string filename)
{
	XPath P;
	std::string ext = P.Ext(filename.c_str());
	std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
	if (ext == ".shp")
		return XShapefile::ImportShapefile(base, filename.c_str());
	if (ext == ".mif")
		return XMifMid::ImportMifMid(base, filename.c_str());
	return NULL;
}

// Fonction principale
int main(int argc, char* argv[])
{
	std::string version = "1.0";
	std::string file_par, file_feu, file_transfo_in, dir_result, proj = "L93";

	std::cout << "RPCUMover version " << version << std::endl;

	// Mode interactif
	if (argc <= 1) {
		std::cout << "Nom du fichier parcelle : ";
		std::cin >> file_par;
		std::cout << "Nom du fichier feuille : ";
		std::cin >> file_feu;

		std::cout << "Nom du fichier a tranformer : ";
		std::cin >> file_transfo_in;
		std::cout << "Nom du repertoire de sortie : ";
		std::cin >> dir_result;

		std::cout << "Projection (L93, CC42, CC43, RGR92...) : ";
		std::cin >> proj;
	}
	else {
		if ((argc % 2) != 1)
			return Usage();
		std::string token, value;
		for (int i = 1; i < argc; i += 2) {
			token = argv[i];
			value = argv[i + 1];
			if (token == "--p") file_par = value;
			if (token == "--f") file_feu = value;
			if (token == "--i") file_transfo_in = value;
			if (token == "--o") dir_result = value;
			if (token == "--proj") proj = value;
			if (token == "-h") return Usage();
			if (token == "--h") return Usage();
		}
	}

	if (file_par.empty() || file_feu.empty() || file_transfo_in.empty() || dir_result.empty())
		return Usage();

	// Recherche de la projection
	XGeoPref pref;
	for (int proj_code = XGeoProjection::RGF93; proj_code <= XGeoProjection::NC_RGNC91_UTM59; proj_code++) {
		std::string shortname = XGeoProjection::ProjectionShortName((XGeoProjection::XProjCode)proj_code);
		if (shortname.size() < 1)
			continue;
		if (shortname == proj) {
			pref.Projection((XGeoProjection::XProjCode)proj_code);
			break;
		}
	}

	// Import des fichiers de donnees
	XGeoBase base;
	XGeoClass* C_in_par, * C_in_feu, * C_in;

	C_in_par = ImportFile(&base, file_par.c_str());
	if (C_in_par == NULL) {
		std::cerr << "Ouverture impossible de " << file_par << std::endl;
		return -1;
	}
	C_in_feu = ImportFile(&base, file_feu.c_str());
	if (C_in_feu == NULL) {
		std::cerr << "Ouverture impossible de " << file_feu << std::endl;
		return -1;
	}

	C_in = ImportFile(&base, file_transfo_in.c_str());
	if (C_in == NULL) {
		std::cerr << "Ouverture impossible de " << file_transfo_in << std::endl;
		return -1;
	}

	// Creation des fichiers de sortie
	XPath path;
	std::ofstream log_file;
	log_file.open(path.FullName(dir_result.c_str(), "RCPUMover_log.csv"), std::ios_base::out);
	log_file << version << std::endl;
	log_file << "X,Y,Message" << std::endl;
	AppError error;
	error.Output(&log_file);

	// Creation du fichier MIF/MID
	XMifMidConverter Mif;
	Mif.BeginClass(C_in, dir_result.c_str());

	// Donnees parcelles a prendre en compte
	std::vector<XGeoVector*> T;
	for (uint32 i = 0; i < C_in_par->NbVector(); i++) {
		XGeoVector* V = C_in_par->Vector(i);
		if (V->NbPt() < 3)
			continue;
		T.push_back(V);
	}

	XWaitConsole wait;
	wait.SetStatus("Traitement");
	wait.SetRange(0, C_in->NbVector());
	std::vector<XPt2D> Result;
	XBaryMover bary(0.01);
	XTrigMover mover(&error);
	mover.SetClasses(C_in_par, C_in_feu);
	XPt2D P, D;

	// Traitement pour chacun des vecteurs en entree
	for (uint32 i = 0; i < C_in->NbVector(); i++) {
		wait.StepIt();
		XGeoVector* Vin = C_in->Vector(i);
		if (!Vin->LoadGeom())
			continue;
		Result.clear();

		// Recherche des geometries en intersection
		std::vector<XGeoVector*> L;
		for (uint32 j = 0; j < T.size() / 2; j++) {
			if (!Vin->Frame().Intersect(T[j * 2]->Frame()))
				continue;
			L.push_back(T[j * 2]);
			L.push_back(T[j * 2 + 1]);
			T[j * 2]->LoadGeom2D();
			T[j * 2 + 1]->LoadGeom2D();
		}
		if (L.size() < 1)	// Le vecteur en entree est hors zone
			continue;

		// Traitement pour chacun des points du vecteur en entree
		Result.clear();
		for (uint32 j = 0; j < Vin->NbPt(); j++) {
			P = Vin->Pt(j);
			int index = -1;
			// Test sur les sommets
			index = -1;
			for (uint32 k = 0; k < L.size() / 2; k++) {
				if (L[2 * k]->IsSom2D(P, index)) {
					D = L[2 * k + 1]->Pt(index);
					Result.push_back(D);
					break;
				}
			}
			if (index >= 0)	// Le point P est un sommet
				continue;

			// Recherche de la parcelle contenant le point P
			for (uint32 k = 0; k < L.size() / 2; k++) {
				if (L[2 * k]->IsIn2D(P)) {
					index = 2 * k;
					break;
				}
			}

			if (index >= 0) {	// Utilisation de Haunert
				D = bary.Delta(L[index]->Pt(), L[index + 1]->Pt(), L[index]->NbPt(), P);
				D += P;
				Result.push_back(D);
			}
			else {	// Le point est dans le DNC
				mover.MovePoint(P, D);
				Result.push_back(D);
			}
		}
		Mif.WriteObject(Vin, &Result);
		Vin->Unload();
	}	//endfor i

	std::cout << "Traitement termine de " << C_in->NbVector() << " objets" << std::endl;
	return 0;
}
