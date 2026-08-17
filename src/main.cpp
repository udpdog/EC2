#include "cli.hpp"
#include "formats.hpp"
#include "imagemagick.hpp"
#include <filesystem>
#include <iostream>
namespace fs=std::filesystem;
int wmain(int argc,wchar_t* argv[]) {
	auto o=ec2::parseArguments(argc,argv);
	if(o.action==ec2::Action::Help) {
		ec2::printHelp();
		return 0;
	}
	if(o.action==ec2::Action::FormatHelp) {
		if(!ec2::printConversionOptionsHelp(o.helpFormat,std::wcout))
			std::wcout<<L"Le format "<<o.helpFormat<<L" ne possede pas d'options avancees.\n";
		return 0;
	}
	if(o.action==ec2::Action::Formats) {
		ec2::printFormats(std::wcout);
		return 0;
	}
	if(o.action==ec2::Action::Error) {
		std::wcerr<<L"Erreur : "<<o.error<<L'\n';
		return 2;
	}
	std::error_code e;
	o.input=fs::absolute(o.input,e);
	if(e||!fs::is_regular_file(o.input,e)) {
		std::wcerr<<L"Erreur : fichier introuvable : "<<o.input<<L'\n';
		return 3;
	}
	const std::wstring inputFormat=ec2::normalizeFormat(o.input.extension().wstring());
	if(!ec2::isSupportedInputFormat(inputFormat)) {
		std::wcerr<<L"Erreur : format d'entree non accepte '"<<inputFormat<<L"'.\n";
		return 3;
	}
	if(o.output.empty()) {
		o.output=o.input.parent_path()/(o.input.stem().wstring()+L"."+o.format);
		if(ec2::normalizeFormat(o.input.extension().wstring())==o.format)o.output=o.input.parent_path()/(o.input.stem().wstring()+L"_converted."+o.format);
	} else {
		o.output=fs::absolute(o.output,e);
		if(e) {
			std::wcerr<<L"Erreur : sortie invalide.\n";
			return 3;
		}
		o.output.replace_extension(L"."+o.format);
	}
	if(!fs::exists(o.output.parent_path(),e)) {
		std::wcerr<<L"Erreur : dossier de sortie introuvable.\n";
		return 3;
	}
	if(fs::exists(o.output,e)&&!o.force) {
		std::wcerr<<L"Erreur : le fichier existe deja. Ajoutez --force.\n";
		return 4;
	}
	std::wstring magick;
	if(!ec2::findImageMagick(magick)) {
		std::wcout<<L"Installation automatique d'ImageMagick...\n";
		if(!ec2::installImageMagick()||!ec2::findImageMagick(magick)) {
			std::wcerr<<L"Erreur : installation impossible. Verifiez winget.\n";
			return 5;
		}
	}
	std::wcout<<L"[EC2] Entree  : "<<o.input<<L" ("<<inputFormat<<L")\n"
	          <<L"[EC2] Sortie  : "<<o.output<<L" ("<<o.format<<L")\n"
	          <<L"[EC2] Moteur  : "<<magick<<L"\n"
	          <<L"[EC2] Conversion en cours...\n";
	ec2::printConversionOptions(o.conversion,std::wcout);
	auto code=ec2::convertImage(magick,o.input,o.output,o.format,o.conversion);
	if(code||!fs::is_regular_file(o.output,e)) {
		std::wcerr<<L"Erreur : conversion impossible (code "<<code<<L").\n";
		return 6;
	}
	std::wcout<<L"[EC2] Conversion terminee avec succes.\n";
	return 0;
}
