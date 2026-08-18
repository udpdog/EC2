#include "cli.hpp"
#include "formats.hpp"
#include "imagemagick.hpp"
#include <filesystem>
#include <iostream>
#ifndef _WIN32
#include <clocale>
#include <codecvt>
#include <locale>
#include <stdexcept>
#include <vector>
#endif
namespace fs=std::filesystem;
namespace {

std::wstring displayPath(const fs::path& path) {
#ifdef _WIN32
	return path.wstring();
#else
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.from_bytes(path.string());
#endif
}

std::wstring pathExtension(const fs::path& path) {
	const std::string extension=path.extension().string();
	return std::wstring(extension.begin(),extension.end());
}

fs::path outputFilename(const fs::path& input,const std::wstring& format,bool converted) {
	fs::path filename=input.stem();
#ifdef _WIN32
	filename+=converted?L"_converted.":L".";
	filename+=format;
#else
	const std::string narrowFormat(format.begin(),format.end());
	filename+=converted?"_converted.":".";
	filename+=narrowFormat;
#endif
	return input.parent_path()/filename;
}

int runApplication(int argc,wchar_t* argv[]) {
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
		std::wcerr<<L"Erreur : fichier introuvable : "<<displayPath(o.input)<<L'\n';
		return 3;
	}
	const std::wstring inputFormat=ec2::normalizeFormat(pathExtension(o.input));
	if(!ec2::isSupportedInputFormat(inputFormat)) {
		std::wcerr<<L"Erreur : format d'entree non accepte '"<<inputFormat<<L"'.\n";
		return 3;
	}
	if(o.output.empty()) {
		const bool sameFormat=ec2::normalizeFormat(pathExtension(o.input))==o.format;
		o.output=outputFilename(o.input,o.format,sameFormat);
	} else {
		o.output=fs::absolute(o.output,e);
		if(e) {
			std::wcerr<<L"Erreur : sortie invalide.\n";
			return 3;
		}
#ifdef _WIN32
		o.output.replace_extension(L"."+o.format);
#else
		const std::string extension="."+std::string(o.format.begin(),o.format.end());
		o.output.replace_extension(extension);
#endif
	}
	if(!fs::exists(o.output.parent_path(),e)) {
		std::wcerr<<L"Erreur : dossier de sortie introuvable.\n";
		return 3;
	}
	if(fs::exists(o.output,e)&&!o.force) {
		std::wcerr<<L"Erreur : le fichier existe deja. Ajoutez --force.\n";
		return 4;
	}
	fs::path magick;
	if(!ec2::findImageMagick(magick)) {
		std::wcout<<L"Installation automatique d'ImageMagick...\n";
		ec2::installImageMagick();
		if(!ec2::findImageMagick(magick)) {
			std::wcerr<<L"Erreur : installation impossible. "<<ec2::imageMagickInstallHint()<<L'\n';
			return 5;
		}
	}
	std::wcout<<L"[EC2] Entree  : "<<displayPath(o.input)<<L" ("<<inputFormat<<L")\n"
	          <<L"[EC2] Sortie  : "<<displayPath(o.output)<<L" ("<<o.format<<L")\n"
	          <<L"[EC2] Moteur  : "<<displayPath(magick)<<L"\n"
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
}

#ifdef _WIN32
int wmain(int argc,wchar_t* argv[]) {
	return runApplication(argc,argv);
}
#else
int main(int argc,char* argv[]) {
	std::setlocale(LC_ALL,"");
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::vector<std::wstring> arguments;
	arguments.reserve(static_cast<std::size_t>(argc));
	try {
		for(int index=0;index<argc;++index) arguments.push_back(converter.from_bytes(argv[index]));
	} catch(const std::range_error&) {
		std::cerr<<"Erreur : argument UTF-8 invalide.\n";
		return 2;
	}
	std::vector<wchar_t*> pointers;
	pointers.reserve(arguments.size());
	for(std::wstring& argument:arguments) pointers.push_back(argument.data());
	return runApplication(argc,pointers.data());
}
#endif
