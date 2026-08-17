#include "cli.hpp"
#include "formats.hpp"
#include <iostream>
namespace ec2 {
void printHelp(){std::wcout
 <<L"EC2 - convertisseur d'images\n\nUtilisation :\n"
 <<L"  ec2.exe <image> -format <format> [-o <sortie>] [--force]\n\n"
 <<L"Options :\n  -format, -f   Format de destination\n  -o            Fichier de sortie\n"
 <<L"  --force       Autoriser l'ecrasement\n  --formats     Lister les formats\n  -h, --help    Aide\n";}
Options parseArguments(int argc,wchar_t* argv[]){
 Options o; if(argc==1){o.action=Action::Help;return o;}
 for(int i=1;i<argc;++i){std::wstring a=argv[i];
  if(a==L"-h"||a==L"--help"){o.action=Action::Help;return o;}
  if(a==L"--formats"){o.action=Action::Formats;return o;}
  if(a==L"--force") o.force=true;
  else if(a==L"-format"||a==L"--format"||a==L"-f"){
   if(++i>=argc){o.error=L"format manquant.";return o;} o.format=normalizeFormat(argv[i]);}
  else if(a==L"-o"||a==L"--output"){
   if(++i>=argc){o.error=L"chemin de sortie manquant.";return o;} o.output=argv[i];}
  else if(!a.empty()&&a.front()==L'-'){o.error=L"option inconnue : "+a;return o;}
  else if(o.input.empty()) o.input=a; else{o.error=L"un seul fichier d'entree est accepte.";return o;}
 }
 if(o.input.empty()||o.format.empty()) o.error=L"indiquez une image et -format <format>.";
 else if(!isSupportedOutputFormat(o.format)) o.error=L"format de sortie non accepte '"+o.format+L"'. Utilisez --formats.";
 else o.action=Action::Convert;
 return o;
}
}
