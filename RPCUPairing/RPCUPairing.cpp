//-----------------------------------------------------------------------------
//                       RPCUPairing.cpp
//                       ===============
//
// Programme principal pour l'appariement entre deux jeux de vecteurs
// Un attribut permet de relier de maniere bijective les vecteurs des deux jeux de vecteurs
// En sortie : on obtient des vecteurs de deplacement et en option des polygones avant / apres de transformation
//
// Auteur : FBecirspahic - F.Becirspahic - IGN / DSTI / SIMV
//-----------------------------------------------------------------------------


#include <iostream>
#include <algorithm>
#include "XGeoBase.h"
#include "XShapefile.h"
#include "XMifMid.h"
#include "XPath.h"
#include "XMatcher.h"
#include "XWaitConsole.h"
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
  std::cout << "RPCUPairing " << std::endl
    << "\t--i fichier_avant " << std::endl
    << "\t--o fichier_apres " << std::endl
    << "\t--r fichier_resultat (sans extension) " << std::endl
    << "\t--a attribut_lien_avant" << std::endl
    << "\t--b attribut_lien_apres" << std::endl
    << "\t--w ecriture_polygones" << std::endl
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

//
int main(int argc, char* argv[])
{
  std::string version = "1.2";
  std::string file_in, file_out, file_result, attname_in, attname_out, poly, proj = "L93";
  double max_angle_alignement = 10.;
  double max_diff_angle = 10.;
  double min_vector_size = 0.1;
  bool write_poly = false;

  std::cout << "RPCUPairing version " << version << std::endl;

  // Mode interactif
  if (argc <= 1) {
    std::cout << "Nom du fichier avant : ";
    std::cin >> file_in;
    std::cout << "Nom de l'attribut lien : ";
    std::cin >> attname_in;

    std::cout << "Nom du fichier apres : ";
    std::cin >> file_out;
    std::cout << "Nom de l'attribut lien : ";
    std::cin >> attname_out;

    std::cout << "Nom du fichier de sortie (sans point ni extension) : ";
    std::cin >> file_result;

    std::cout << "Ecriture des polygones [o/n] : ";
    std::cin >> poly;

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
      if (token == "--i") file_in = value;
      if (token == "--o") file_out = value;
      if (token == "--r") file_result = value;
      if (token == "-h") return Usage();
      if (token == "--h") return Usage();
      if (token == "--a") attname_in = value;
      if (token == "--b") attname_out = value;
      if (token == "--w") poly = value;
      if (token == "--proj") proj = value;
    }
  }

  if (file_in.empty() || file_out.empty() || file_result.empty() || attname_in.empty() || attname_out.empty())
    return Usage();
  if ((poly == "O") || (poly == "o") || (poly == "y") || (poly == "Y"))
    write_poly = true;

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

  // Traitement
  XGeoBase base;
  XGeoClass* C_in, * C_out;

  C_in = ImportFile(&base, file_in.c_str());
  if (C_in == NULL) {
    std::cerr << "Ouverture impossible de " << file_in << std::endl;
    return -1;
  }

  C_out = ImportFile(&base, file_out.c_str());
  if (C_out == NULL) {
    std::cerr << "Ouverture impossible de " << file_out << std::endl;
    return -1;
  }

  XMatcher matcher(max_angle_alignement, max_diff_angle, min_vector_size);
  matcher.SetOutput(file_result, write_poly);

  std::ofstream log_file;
  log_file.open(file_result + ".log", std::ios_base::out);
  AppError error;
  error.Output(&log_file);
  matcher.SetLog(&error);
  XErrorInfo(&error, version);
  std::string command = "RPCUPairing --i " + file_in + " --o " + file_out + " --r " + file_result +
    " --a " + attname_in + " --b " + attname_out + " --w " + poly;
  XErrorInfo(&error, command);

  XWaitConsole wait;
  wait.SetStatus("Traitement");
  wait.SetRange(0, C_in->NbVector());
  for (uint32 i = 0; i < C_in->NbVector(); i++) {
    wait.StepIt();
    XGeoVector* Vin = C_in->Vector(i);
    if (!Vin->Visible())
      continue;

    std::string id = Vin->FindAttribute(attname_in);
    XFrame F_in = Vin->Frame();
    F_in *= 2.;
    XGeoVector* Vout = NULL;
    bool flag = false;
    for (uint32 j = 0; j < C_out->NbVector(); j++) {
      Vout = C_out->Vector(j);
      if (!Vout->Visible()) // Vecteur deja utilise
        continue;
      if (!F_in.Intersect(Vout->Frame()))
        continue;
      if (id.compare(Vout->FindAttribute(attname_out)) == 0) {
        flag = true;
        Vout->Visible(false); // Le vecteur ne sera plus utilise ensuite
        break;
      }
    }
    if (!flag) {
      std::string mes = id + " sans correspondance dans le fichier apres";
      XErrorAlert(&error, mes);
      continue;
    }

    matcher.Match(Vin, Vout, id);

  } // endfor i

  std::cout << "Traitement termine de " << C_in->NbVector() << " objets" << std::endl;
  return 0;
}
